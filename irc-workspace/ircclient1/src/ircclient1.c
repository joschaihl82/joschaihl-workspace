// ws_irc_bridge_dynamic.c
// WebSocket <-> IRC bridge with hardcoded config and dynamic linked lists
// Compile: gcc -O2 -Wall -pthread ws_irc_bridge_dynamic.c -o ws_irc_bridge_dynamic
// Run: ./ws_irc_bridge_dynamic
//
// Notes:
// - Hardcoded IRC servers and channels are at the top of this file.
// - No external crypto library required (SHA1 + Base64 implemented).
// - Plaintext only (no TLS). Use a reverse proxy for TLS in production.
// - Minimal RFC6455 support: text frames, ping/pong, close; no fragmentation.

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <endian.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define RECV_BUF 8192
#define NICK_MAX 64
#define MAX_CLIENTS 128

const char *WS_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

/* ---------------- Hardcoded configuration (edit here) ---------------- */
/* Provide servers as "host:port" strings. The code will iterate them on failure. */
static const char *HARDCODED_SERVERS[] = {
    "irc.libera.chat:6667",
    "irc.oftc.net:6667",
    "irc.efnet.org:6667"
};
static const int HARDCODED_SERVER_COUNT = 3;

/* Provide channels to join. The bridge will join all of them. */
static const char *HARDCODED_CHANNELS[] = {
    "#testchannel",
    "#anotherchannel"
};
static const int HARDCODED_CHANNEL_COUNT = 2;

/* Bridge nick and listen port */
static const char *HARDCODED_NICK = "WSBridgeDyn";
static const int HARDCODED_LISTEN_PORT = 8888;

/* ---------------- Embedded HTML UI ---------------- */
static const char *INDEX_HTML =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=utf-8\r\n"
"Connection: close\r\n\r\n"
"<!doctype html>\n"
"<html>\n<head>\n<meta charset='utf-8'>\n<title>WebSocket IRC Bridge</title>\n<style>body{font-family:Arial;margin:20px}#log{height:60vh;border:1px solid #ccc;padding:8px;overflow:auto}#msg{width:80%}</style>\n</head>\n<body>\n<h2>WebSocket IRC Bridge</h2>\n<div id='log'></div>\n<input id='msg' placeholder='Type message or /nick /who /ircstatus' autofocus>\n<button id='send'>Send</button>\n<script>\n(function(){\n  var log=document.getElementById('log');\n  var ws=new WebSocket((location.protocol==='https:'?'wss://':'ws://')+location.host+location.pathname);\n  ws.onopen=function(){ log.innerHTML+='<div><em>Connected</em></div>'; };\n  ws.onmessage=function(e){ var d=document.createElement('div'); d.textContent=e.data; log.appendChild(d); log.scrollTop=log.scrollHeight; };\n  ws.onclose=function(){ log.innerHTML+='<div><em>Disconnected</em></div>'; };\n  document.getElementById('send').onclick=function(){ var m=document.getElementById('msg').value; if(!m) return; ws.send(m); document.getElementById('msg').value=''; };\n  document.getElementById('msg').addEventListener('keydown',function(e){ if(e.key==='Enter'){ document.getElementById('send').click(); } });\n})();\n</script>\n</body>\n</html>\r\n";

/* ---------------- Small SHA1 implementation ---------------- */
typedef struct { uint32_t h[5]; uint8_t buf[64]; uint64_t len; } SHA1_CTX;
static void sha1_init(SHA1_CTX *c) {
    c->h[0]=0x67452301; c->h[1]=0xEFCDAB89; c->h[2]=0x98BADCFE;
    c->h[3]=0x10325476; c->h[4]=0xC3D2E1F0; c->len=0;
}
static uint32_t rol(uint32_t x,int n){ return (x<<n)|(x>>(32-n)); }
static void sha1_transform(SHA1_CTX *c,const uint8_t *b){
    uint32_t w[80],a,bv,cc,d,e,t;
    for(int i=0;i<16;i++) w[i]= (b[4*i]<<24)|(b[4*i+1]<<16)|(b[4*i+2]<<8)|b[4*i+3];
    for(int i=16;i<80;i++) w[i]=rol(w[i-3]^w[i-8]^w[i-14]^w[i-16],1);
    a=c->h[0]; bv=c->h[1]; cc=c->h[2]; d=c->h[3]; e=c->h[4];
    for(int i=0;i<80;i++){
        if(i<20) t=rol(a,5)+((bv&cc)|((~bv)&d))+e+0x5A827999+w[i];
        else if(i<40) t=rol(a,5)+(bv^cc^d)+e+0x6ED9EBA1+w[i];
        else if(i<60) t=rol(a,5)+((bv&cc)|(bv&d)|(cc&d))+e+0x8F1BBCDC+w[i];
        else t=rol(a,5)+(bv^cc^d)+e+0xCA62C1D6+w[i];
        e=d; d=cc; cc=rol(bv,30); bv=a; a=t;
    }
    c->h[0]+=a; c->h[1]+=bv; c->h[2]+=cc; c->h[3]+=d; c->h[4]+=e;
}
static void sha1_update(SHA1_CTX *c,const uint8_t *data,size_t len){
    size_t i=0,off=c->len%64;
    c->len += len;
    if(off){
        size_t need=64-off;
        if(len<need){ memcpy(c->buf+off,data,len); return; }
        memcpy(c->buf+off,data,need); sha1_transform(c,c->buf); i+=need;
    }
    for(; i+64<=len; i+=64) sha1_transform(c,data+i);
    if(i<len) memcpy(c->buf,data+i,len-i);
}
static void sha1_final(SHA1_CTX *c,uint8_t out[20]){
    uint64_t bits = c->len*8;
    size_t off = c->len%64;
    c->buf[off++]=0x80;
    if(off>56){ memset(c->buf+off,0,64-off); sha1_transform(c,c->buf); off=0; }
    memset(c->buf+off,0,56-off);
    for(int i=0;i<8;i++) c->buf[63-i]=bits>>(8*i);
    sha1_transform(c,c->buf);
    for(int i=0;i<5;i++){
        out[4*i]=(c->h[i]>>24)&0xFF; out[4*i+1]=(c->h[i]>>16)&0xFF;
        out[4*i+2]=(c->h[i]>>8)&0xFF; out[4*i+3]=c->h[i]&0xFF;
    }
}

/* ---------------- Base64 encode ---------------- */
static char *base64_encode(const unsigned char *in,size_t len){
    static const char T[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t outlen = 4*((len+2)/3);
    char *out = malloc(outlen+1);
    if(!out) return NULL;
    char *p=out;
    for(size_t i=0;i<len;i+=3){
        uint32_t v = in[i]<<16;
        if(i+1<len) v |= in[i+1]<<8;
        if(i+2<len) v |= in[i+2];
        *p++ = T[(v>>18)&0x3F];
        *p++ = T[(v>>12)&0x3F];
        *p++ = (i+1<len)?T[(v>>6)&0x3F]:'=';
        *p++ = (i+2<len)?T[v&0x3F]:'=';
    }
    *p=0; return out;
}

/* ---------------- Logging ---------------- */
static void log_info(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    char buf[512]; vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    time_t t = time(NULL);
    struct tm tm; localtime_r(&t, &tm);
    char ts[64]; strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm);
    fprintf(stderr, "[%s] %s\n", ts, buf);
}

/* ---------------- Linked list types for servers and channels ---------------- */
typedef struct server_node {
    char *host;           // host string
    int port;             // port
    struct server_node *next;
} server_node_t;

typedef struct channel_node {
    char *name;           // channel name like "#room"
    struct channel_node *next;
} channel_node_t;

/* ---------------- Client structure & management ---------------- */
typedef struct {
    int fd;
    char nick[NICK_MAX];
    int id;
} client_t;

static client_t clients[MAX_CLIENTS];
static pthread_mutex_t clients_mtx = PTHREAD_MUTEX_INITIALIZER;

/* ---------------- IRC client state ---------------- */
typedef struct {
    server_node_t *servers;      // linked list of servers
    channel_node_t *channels;    // linked list of channels
    char nick[NICK_MAX];
    int sock;
    pthread_mutex_t sock_mtx;
    int connected;
    server_node_t *current_server; // pointer to current server node
} irc_client_t;

static irc_client_t irc;

/* ---------------- Utility helpers ---------------- */
static char *strdup_safe(const char *s){
    if (!s) return NULL;
    char *p = malloc(strlen(s)+1);
    if (p) strcpy(p, s);
    return p;
}

/* ---------------- Build linked lists from hardcoded arrays ---------------- */
static server_node_t *servers_from_hardcoded(const char **arr, int count){
    server_node_t *head = NULL, *tail = NULL;
    for (int i=0;i<count;i++){
        const char *s = arr[i];
        char *copy = strdup_safe(s);
        if (!copy) continue;
        // parse host:port
        char *colon = strchr(copy, ':');
        char *host = copy;
        int port = 6667;
        if (colon){
            *colon = 0;
            port = atoi(colon+1);
        }
        server_node_t *n = malloc(sizeof(*n));
        n->host = strdup_safe(host);
        n->port = port;
        n->next = NULL;
        if (!head) head = tail = n; else { tail->next = n; tail = n; }
        free(copy);
    }
    return head;
}
static channel_node_t *channels_from_hardcoded(const char **arr, int count){
    channel_node_t *head = NULL, *tail = NULL;
    for (int i=0;i<count;i++){
        channel_node_t *n = malloc(sizeof(*n));
        n->name = strdup_safe(arr[i]);
        n->next = NULL;
        if (!head) head = tail = n; else { tail->next = n; tail = n; }
    }
    return head;
}

/* ---------------- Networking helpers ---------------- */
static int create_listen_socket(int port){
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return -1; }
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(port), .sin_addr.s_addr = INADDR_ANY };
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(s); return -1; }
    if (listen(s, 64) < 0) { perror("listen"); close(s); return -1; }
    return s;
}

/* ---------------- HTTP helpers ---------------- */
static int is_get_root(const char *req){ return (strncmp(req, "GET /", 5) == 0); }
static int is_ws_upgrade(const char *req){
    return (strcasestr(req, "Upgrade: websocket") != NULL);
}
static char *get_header_value(const char *req, const char *header){
    char *p = strcasestr(req, header);
    if (!p) return NULL;
    p += strlen(header);
    while (*p == ' ' || *p == '\t') p++;
    const char *end = strstr(p, "\r\n");
    if (!end) return NULL;
    size_t len = end - p;
    char *val = malloc(len + 1);
    if (!val) return NULL;
    memcpy(val, p, len);
    val[len] = 0;
    return val;
}

/* ---------------- WebSocket handshake ---------------- */
static int compute_accept_key(const char *client_key, char **out_accept){
    if (!client_key) return -1;
    char src[512];
    snprintf(src, sizeof(src), "%s%s", client_key, WS_GUID);
    SHA1_CTX ctx; sha1_init(&ctx); sha1_update(&ctx, (uint8_t*)src, strlen(src));
    uint8_t sha[20]; sha1_final(&ctx, sha);
    char *b64 = base64_encode(sha, 20);
    if (!b64) return -1;
    *out_accept = b64;
    return 0;
}
static int send_handshake_response(int client_fd, const char *accept_key){
    char resp[512];
    int n = snprintf(resp, sizeof(resp),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n\r\n", accept_key);
    if (n <= 0) return -1;
    return send(client_fd, resp, n, 0) == n ? 0 : -1;
}
static int perform_handshake(int client_fd, const char *req){
    char *key = get_header_value(req, "Sec-WebSocket-Key:");
    if (!key) return -1;
    char *accept = NULL;
    int rc = compute_accept_key(key, &accept);
    free(key);
    if (rc != 0) return -1;
    rc = send_handshake_response(client_fd, accept);
    free(accept);
    return rc;
}

/* ---------------- WebSocket framing ---------------- */
static ssize_t recv_all(int fd, void *buf, size_t len){
    size_t off = 0;
    while (off < len) {
        ssize_t r = recv(fd, (char*)buf + off, len - off, 0);
        if (r <= 0) return r;
        off += r;
    }
    return (ssize_t)off;
}

/* Return codes:
   >0 : payload length (text)
   -2 : close frame
   -3 : ping frame
   -4 : pong frame
   -1 : error
*/
static int recv_ws_frame(int fd, char *out, size_t outsz){
    unsigned char hdr[2];
    if (recv_all(fd, hdr, 2) != 2) return -1;
    int fin = hdr[0] & 0x80;
    int opcode = hdr[0] & 0x0F;
    int masked = hdr[1] & 0x80;
    uint64_t payload_len = hdr[1] & 0x7F;

    if (payload_len == 126) {
        uint16_t v;
        if (recv_all(fd, &v, 2) != 2) return -1;
        payload_len = ntohs(v);
    } else if (payload_len == 127) {
        uint64_t v;
        if (recv_all(fd, &v, 8) != 8) return -1;
        payload_len = be64toh(v);
    }

    unsigned char mask[4] = {0,0,0,0};
    if (masked) {
        if (recv_all(fd, mask, 4) != 4) return -1;
    }

    if (payload_len > outsz - 1) return -1;
    if (recv_all(fd, out, payload_len) != (ssize_t)payload_len) return -1;

    if (masked) for (uint64_t i=0;i<payload_len;i++) out[i] ^= mask[i%4];
    out[payload_len] = 0;

    if (opcode == 0x8) return -2; // close
    if (opcode == 0x9) return -3; // ping
    if (opcode == 0xA) return -4; // pong
    (void)fin;
    return (int)payload_len;
}

static int send_ws_control(int fd, int opcode, const char *payload, size_t payload_len){
    unsigned char hdr[10];
    int hlen = 0;
    hdr[0] = 0x80 | (opcode & 0x0F); // FIN + opcode
    hdr[1] = (unsigned char)payload_len;
    hlen = 2;
    if (send(fd, hdr, hlen, 0) != hlen) return -1;
    if (payload_len > 0 && send(fd, payload, payload_len, 0) != (ssize_t)payload_len) return -1;
    return 0;
}

static int send_ws_text(int fd, const char *msg){
    size_t len = strlen(msg);
    unsigned char hdr[10];
    int hlen = 0;
    hdr[0] = 0x81; // FIN + text
    if (len < 126) {
        hdr[1] = (unsigned char)len; hlen = 2;
    } else if (len < 65536) {
        hdr[1] = 126; uint16_t v = htons((uint16_t)len); memcpy(hdr+2,&v,2); hlen = 4;
    } else {
        hdr[1] = 127; uint64_t be = htobe64(len); memcpy(hdr+2,&be,8); hlen = 10;
    }
    if (send(fd, hdr, hlen, 0) != hlen) return -1;
    if (send(fd, msg, len, 0) != (ssize_t)len) return -1;
    return 0;
}

/* ---------------- Client management ---------------- */
static void init_clients(void){
    for (int i=0;i<MAX_CLIENTS;i++){ clients[i].fd = 0; clients[i].nick[0]=0; clients[i].id = -1; }
}
static int add_client(int fd){
    pthread_mutex_lock(&clients_mtx);
    for (int i=0;i<MAX_CLIENTS;i++){
        if (clients[i].fd == 0){
            clients[i].fd = fd;
            clients[i].id = i;
            snprintf(clients[i].nick, NICK_MAX, "User%d", i+1);
            pthread_mutex_unlock(&clients_mtx);
            return i;
        }
    }
    pthread_mutex_unlock(&clients_mtx);
    return -1;
}
static void remove_client_by_index(int idx){
    pthread_mutex_lock(&clients_mtx);
    if (clients[idx].fd){
        close(clients[idx].fd);
        log_info("Client %s (fd=%d) disconnected", clients[idx].nick, clients[idx].fd);
        clients[idx].fd = 0;
        clients[idx].nick[0] = 0;
        clients[idx].id = -1;
    }
    pthread_mutex_unlock(&clients_mtx);
}
static void broadcast_message_from_ws(int sender_idx, const char *msg){
    char out[RECV_BUF];
    snprintf(out, sizeof(out), "%s: %s", clients[sender_idx].nick, msg);
    pthread_mutex_lock(&clients_mtx);
    for (int i=0;i<MAX_CLIENTS;i++){
        if (clients[i].fd && i != sender_idx){
            if (send_ws_text(clients[i].fd, out) != 0){
                log_info("Failed to send to %s (fd=%d), closing", clients[i].nick, clients[i].fd);
                close(clients[i].fd);
                clients[i].fd = 0;
                clients[i].nick[0] = 0;
                clients[i].id = -1;
            }
        }
    }
    pthread_mutex_unlock(&clients_mtx);
}
static void broadcast_message_from_irc(const char *msg){
    char out[RECV_BUF];
    snprintf(out, sizeof(out), "[IRC] %s", msg);
    pthread_mutex_lock(&clients_mtx);
    for (int i=0;i<MAX_CLIENTS;i++){
        if (clients[i].fd){
            if (send_ws_text(clients[i].fd, out) != 0){
                log_info("Failed to send to %s (fd=%d), closing", clients[i].nick, clients[i].fd);
                close(clients[i].fd);
                clients[i].fd = 0;
                clients[i].nick[0] = 0;
                clients[i].id = -1;
            }
        }
    }
    pthread_mutex_unlock(&clients_mtx);
}

/* ---------------- IRC helpers ---------------- */
static int irc_connect_to_server(const char *host, int port){
    struct addrinfo hints, *res, *rp;
    char portstr[16];
    snprintf(portstr, sizeof(portstr), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, portstr, &hints, &res) != 0) return -1;
    int s = -1;
    for (rp = res; rp != NULL; rp = rp->ai_next){
        s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (s < 0) continue;
        if (connect(s, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(s); s = -1;
    }
    freeaddrinfo(res);
    return s;
}

static int irc_send_raw(const char *fmt, ...){
    pthread_mutex_lock(&irc.sock_mtx);
    if (!irc.connected || irc.sock <= 0) { pthread_mutex_unlock(&irc.sock_mtx); return -1; }
    char buf[1024];
    va_list ap; va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n <= 0) { pthread_mutex_unlock(&irc.sock_mtx); return -1; }
    if (n+2 >= (int)sizeof(buf)) { pthread_mutex_unlock(&irc.sock_mtx); return -1; }
    buf[n++] = '\r'; buf[n++] = '\n';
    ssize_t sent = send(irc.sock, buf, n, 0);
    pthread_mutex_unlock(&irc.sock_mtx);
    return (sent == n) ? 0 : -1;
}

static void irc_join_channels(void){
    channel_node_t *c = irc.channels;
    while (c){
        irc_send_raw("JOIN %s", c->name);
        c = c->next;
    }
}

/* Parse a single IRC line into prefix, command, params (simple) */
static void parse_irc_line(const char *line, char *prefix, char *cmd, char *params){
    prefix[0]=cmd[0]=params[0]=0;
    const char *p = line;
    if (*p == ':'){
        p++;
        const char *sp = strchr(p, ' ');
        if (!sp) return;
        size_t l = sp - p;
        memcpy(prefix, p, l); prefix[l]=0;
        p = sp + 1;
    }
    const char *sp = strchr(p, ' ');
    if (!sp){
        strcpy(cmd, p);
        return;
    }
    size_t l = sp - p;
    memcpy(cmd, p, l); cmd[l]=0;
    p = sp + 1;
    while (*p == ' ') p++;
    strcpy(params, p);
}
static void extract_nick_from_prefix(const char *prefix, char *nick, size_t nlen){
    const char *p = strchr(prefix, '!');
    if (!p){
        strncpy(nick, prefix, nlen-1); nick[nlen-1]=0; return;
    }
    size_t l = p - prefix;
    if (l >= nlen) l = nlen-1;
    memcpy(nick, prefix, l); nick[l]=0;
}

/* ---------------- IRC reader thread ---------------- */
static void *irc_reader_thread(void *arg){
    (void)arg;
    char buf[RECV_BUF];
    char line[RECV_BUF];
    while (1){
        pthread_mutex_lock(&irc.sock_mtx);
        int s = irc.sock;
        pthread_mutex_unlock(&irc.sock_mtx);
        if (s <= 0){
            sleep(1);
            continue;
        }
        ssize_t r = recv(s, buf, sizeof(buf)-1, 0);
        if (r <= 0){
            log_info("IRC connection lost (recv <=0)");
            pthread_mutex_lock(&irc.sock_mtx);
            if (irc.sock > 0) close(irc.sock);
            irc.sock = -1;
            irc.connected = 0;
            pthread_mutex_unlock(&irc.sock_mtx);
            sleep(2);
            continue;
        }
        buf[r]=0;
        char *p = buf;
        while (*p){
            char *nl = strstr(p, "\r\n");
            if (!nl) break;
            size_t len = nl - p;
            if (len >= sizeof(line)) len = sizeof(line)-1;
            memcpy(line, p, len); line[len]=0;
            p = nl + 2;
            char prefix[256], cmd[64], params[1024];
            parse_irc_line(line, prefix, cmd, params);
            if (strcasecmp(cmd, "PING") == 0){
                irc_send_raw("PONG %s", params);
            } else if (strcasecmp(cmd, "PRIVMSG") == 0){
                char target[256], message[1024];
                const char *sp = strchr(params, ' ');
                if (!sp) continue;
                size_t lt = sp - params;
                if (lt >= sizeof(target)) lt = sizeof(target)-1;
                memcpy(target, params, lt); target[lt]=0;
                const char *msgp = sp + 1;
                if (*msgp == ':') msgp++;
                strncpy(message, msgp, sizeof(message)-1); message[sizeof(message)-1]=0;
                // forward only if target is one of our channels
                channel_node_t *c = irc.channels;
                while (c){
                    if (strcmp(target, c->name) == 0){
                        char nick[64];
                        extract_nick_from_prefix(prefix, nick, sizeof(nick));
                        char out[RECV_BUF];
                        snprintf(out, sizeof(out), "%s: %s", nick, message);
                        broadcast_message_from_irc(out);
                        break;
                    }
                    c = c->next;
                }
            } else if (strcasecmp(cmd, "001") == 0){
                // welcome -> join channels
                irc_join_channels();
            }
        }
    }
    return NULL;
}

/* ---------------- IRC manager thread: iterate servers and maintain connection ---------------- */
static void *irc_manager_thread(void *arg){
    (void)arg;
    int attempt = 0;
    while (1){
        if (!irc.connected){
            // iterate servers starting from current_server
            server_node_t *snode = irc.current_server;
            int tried = 0;
            while (snode && tried < 100){ // safety
                char host[128]; int port = snode->port;
                strncpy(host, snode->host, sizeof(host)-1); host[127]=0;
                log_info("Attempting IRC connect to %s:%d", host, port);
                int s = irc_connect_to_server(host, port);
                if (s < 0){
                    log_info("Connect to %s:%d failed", host, port);
                    snode = snode->next ? snode->next : irc.servers; // cycle
                    tried++;
                    continue;
                }
                pthread_mutex_lock(&irc.sock_mtx);
                irc.sock = s;
                irc.connected = 1;
                irc.current_server = snode;
                pthread_mutex_unlock(&irc.sock_mtx);
                log_info("Connected to IRC server %s:%d", host, port);
                // send NICK/USER
                irc_send_raw("NICK %s", irc.nick);
                irc_send_raw("USER %s 0 * :ws-irc-bridge", irc.nick);
                break;
            }
            if (!irc.connected){
                attempt++;
                int backoff = (attempt < 6) ? (2 << attempt) : 60;
                log_info("All servers tried, backing off %d seconds", backoff);
                sleep(backoff);
                continue;
            } else attempt = 0;
        }
        sleep(1);
    }
    return NULL;
}

/* Public function to send a message to all configured IRC channels (thread-safe) */
static int irc_send_channel_message(const char *msg){
    char tmp[512];
    size_t j=0;
    for (size_t i=0;i<strlen(msg) && j+1<sizeof(tmp);i++){
        if (msg[i] == '\r' || msg[i] == '\n') continue;
        tmp[j++] = msg[i];
    }
    tmp[j]=0;
    if (strlen(tmp) == 0) return -1;
    int rc = -1;
    channel_node_t *c = irc.channels;
    while (c){
        if (irc_send_raw("PRIVMSG %s :%s", c->name, tmp) == 0) rc = 0;
        c = c->next;
    }
    return rc;
}

/* ---------------- High-level server handling ---------------- */
static volatile int keep_running = 1;
static void sigint_handler(int sig){ (void)sig; keep_running = 0; }

static int create_and_bind_listen(int port){
    return create_listen_socket(port);
}

/* Accept new TCP connection and handle HTTP or WebSocket upgrade */
static void handle_new_connection(int listenfd, fd_set *master_set, int *fdmax){
    int c = accept(listenfd, NULL, NULL);
    if (c < 0) return;
    char buf[RECV_BUF];
    int r = recv(c, buf, sizeof(buf)-1, 0);
    if (r <= 0) { close(c); return; }
    buf[r]=0;
    if (is_ws_upgrade(buf)){
        if (perform_handshake(c, buf) == 0){
            int idx = add_client(c);
            if (idx < 0){ log_info("Max clients reached, rejecting"); close(c); return; }
            FD_SET(c, master_set);
            if (c > *fdmax) *fdmax = c;
            log_info("New WebSocket client %s (fd=%d)", clients[idx].nick, c);
            char welcome[128]; snprintf(welcome, sizeof(welcome), "Welcome %s! Use /nick to change name.", clients[idx].nick);
            send_ws_text(c, welcome);
        } else {
            log_info("WebSocket handshake failed");
            close(c);
        }
    } else {
        if (is_get_root(buf)) send(c, INDEX_HTML, strlen(INDEX_HTML), 0);
        else {
            const char *notf = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
            send(c, notf, strlen(notf), 0);
        }
        close(c);
    }
}

/* Handle a message from a WebSocket client */
static void handle_ws_client(int idx, fd_set *master_set){
    char msg[RECV_BUF];
    int r = recv_ws_frame(clients[idx].fd, msg, sizeof(msg));
    if (r == -1){
        int fd = clients[idx].fd;
        remove_client_by_index(idx);
        FD_CLR(fd, master_set);
        return;
    } else if (r == -2){
        char announce[128]; snprintf(announce, sizeof(announce), "%s left", clients[idx].nick);
        broadcast_message_from_irc(announce);
        int fd = clients[idx].fd;
        remove_client_by_index(idx);
        FD_CLR(fd, master_set);
        return;
    } else if (r == -3){
        send_ws_control(clients[idx].fd, 0xA, NULL, 0);
        return;
    } else if (r == -4){
        return;
    } else if (r > 0){
        if (msg[0] == '/'){
            if (strncmp(msg, "/nick ", 6) == 0){
                char newn[NICK_MAX];
                snprintf(newn, sizeof(newn), "%.*s", NICK_MAX-1, msg+6);
                char *p = newn + strlen(newn);
                while (p > newn && (*(p-1)=='\r' || *(p-1)=='\n' || *(p-1)==' ')) *(--p)=0;
                if (strlen(newn) == 0) { send_ws_text(clients[idx].fd, "Usage: /nick <name>"); return; }
                char old[NICK_MAX]; strcpy(old, clients[idx].nick);
                pthread_mutex_lock(&clients_mtx);
                strncpy(clients[idx].nick, newn, NICK_MAX-1); clients[idx].nick[NICK_MAX-1]=0;
                pthread_mutex_unlock(&clients_mtx);
                char announce[128]; snprintf(announce, sizeof(announce), "%s is now %s", old, clients[idx].nick);
                broadcast_message_from_irc(announce);
            } else if (strncmp(msg, "/who", 4) == 0){
                char list[RECV_BUF]; list[0]=0;
                strcat(list, "Connected: ");
                int first = 1;
                pthread_mutex_lock(&clients_mtx);
                for (int i=0;i<MAX_CLIENTS;i++){
                    if (clients[i].fd){
                        if (!first) strcat(list, ", ");
                        strcat(list, clients[i].nick);
                        first = 0;
                    }
                }
                pthread_mutex_unlock(&clients_mtx);
                send_ws_text(clients[idx].fd, list);
            } else if (strncmp(msg, "/ircstatus", 10) == 0){
                char st[256];
                pthread_mutex_lock(&irc.sock_mtx);
                int c = irc.connected;
                char cur[128] = "none";
                if (irc.current_server && irc.current_server->host) snprintf(cur, sizeof(cur), "%s:%d", irc.current_server->host, irc.current_server->port);
                pthread_mutex_unlock(&irc.sock_mtx);
                snprintf(st, sizeof(st), "IRC connected=%d current=%s channels=%s", c, cur, irc.channels ? irc.channels->name : "none");
                send_ws_text(clients[idx].fd, st);
            } else {
                send_ws_text(clients[idx].fd, "Unknown command");
            }
        } else {
            broadcast_message_from_ws(idx, msg);
            if (irc.connected) irc_send_channel_message(msg);
        }
    }
}

/* ---------------- Minimal main ---------------- */
int main(void){
    signal(SIGINT, sigint_handler);

    /* Build dynamic lists from hardcoded arrays */
    irc.servers = servers_from_hardcoded(HARDCODED_SERVERS, HARDCODED_SERVER_COUNT);
    irc.channels = channels_from_hardcoded(HARDCODED_CHANNELS, HARDCODED_CHANNEL_COUNT);
    strncpy(irc.nick, HARDCODED_NICK, NICK_MAX-1); irc.nick[NICK_MAX-1]=0;
    irc.sock = -1; irc.connected = 0; pthread_mutex_init(&irc.sock_mtx, NULL);
    irc.current_server = irc.servers; // start at head

    init_clients();

    /* Start IRC manager and reader threads */
    pthread_t mgr, rdr;
    if (pthread_create(&mgr, NULL, irc_manager_thread, NULL) != 0){
        perror("pthread_create mgr"); return 1;
    }
    if (pthread_create(&rdr, NULL, irc_reader_thread, NULL) != 0){
        perror("pthread_create rdr"); return 1;
    }

    int listenfd = create_and_bind_listen(HARDCODED_LISTEN_PORT);
    if (listenfd < 0) return 1;
    fd_set master_set, read_fds;
    FD_ZERO(&master_set); FD_SET(listenfd, &master_set);
    int fdmax = listenfd;
    log_info("HTTP+WebSocket server listening on %d", HARDCODED_LISTEN_PORT);

    while (keep_running){
        read_fds = master_set;
        struct timeval tv = {1,0};
        int sel = select(fdmax+1, &read_fds, NULL, NULL, &tv);
        if (sel < 0){
            if (errno == EINTR) continue;
            perror("select"); break;
        }
        if (FD_ISSET(listenfd, &read_fds)){
            handle_new_connection(listenfd, &master_set, &fdmax);
        }
        pthread_mutex_lock(&clients_mtx);
        for (int i=0;i<MAX_CLIENTS;i++){
            if (clients[i].fd && FD_ISSET(clients[i].fd, &read_fds)){
                int fd = clients[i].fd;
                pthread_mutex_unlock(&clients_mtx);
                handle_ws_client(i, &master_set);
                pthread_mutex_lock(&clients_mtx);
                if (clients[i].fd == 0) FD_CLR(fd, &master_set);
            }
        }
        pthread_mutex_unlock(&clients_mtx);
    }

    log_info("Shutting down");
    pthread_mutex_lock(&clients_mtx);
    for (int i=0;i<MAX_CLIENTS;i++) if (clients[i].fd) close(clients[i].fd);
    pthread_mutex_unlock(&clients_mtx);
    close(listenfd);
    pthread_mutex_lock(&irc.sock_mtx);
    if (irc.sock > 0) close(irc.sock);
    irc.sock = -1; irc.connected = 0;
    pthread_mutex_unlock(&irc.sock_mtx);

    /* Free dynamic lists */
    server_node_t *s = irc.servers;
    while (s){ server_node_t *nx = s->next; free(s->host); free(s); s = nx; }
    channel_node_t *c = irc.channels;
    while (c){ channel_node_t *nx = c->next; free(c->name); free(c); c = nx; }

    return 0;
}
