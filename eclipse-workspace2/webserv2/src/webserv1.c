// ws_http_chat_manyfuncs2.c - many small functions, minimal main()
// Compile: gcc -O2 -Wall ws_http_chat_manyfuncs2.c -o ws_http_chat
// Run: ./ws_http_chat <port>

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <endian.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 64
#define RECV_BUF 8192
const char *WS_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

/* ---------------- Embedded HTML ---------------- */
static const char *INDEX_HTML =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=utf-8\r\n"
"Connection: close\r\n\r\n"
"<!doctype html>\n"
"<html>\n<head>\n<meta charset='utf-8'>\n<title>WebSocket Chat</title>\n<style>body{font-family:Arial;margin:20px}#log{height:60vh;border:1px solid #ccc;padding:8px;overflow:auto}#msg{width:80%}</style>\n</head>\n<body>\n<h2>WebSocket Chat</h2>\n<div id='log'></div>\n<input id='msg' placeholder='Type message...' autofocus>\n<button id='send'>Send</button>\n<script>\n(function(){\n  var log=document.getElementById('log');\n  var ws=new WebSocket((location.protocol==='https:'?'wss://':'ws://')+location.host+location.pathname);\n  ws.onopen=function(){ log.innerHTML+='<div><em>Connected</em></div>'; };\n  ws.onmessage=function(e){ var d=document.createElement('div'); d.textContent=e.data; log.appendChild(d); log.scrollTop=log.scrollHeight; };\n  ws.onclose=function(){ log.innerHTML+='<div><em>Disconnected</em></div>'; };\n  document.getElementById('send').onclick=function(){ var m=document.getElementById('msg').value; if(!m) return; ws.send(m); document.getElementById('msg').value=''; };\n  document.getElementById('msg').addEventListener('keydown',function(e){ if(e.key==='Enter'){ document.getElementById('send').click(); } });\n})();\n</script>\n</body>\n</html>\r\n";

/* ---------------- SHA1 (small) ---------------- */
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

/* ---------------- Base64 ---------------- */
static char *base64_encode(const unsigned char *in,size_t len){
    static const char T[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t outlen = 4*((len+2)/3);
    char *out = malloc(outlen+1);
    if(!out) return NULL;
    char *p = out;
    for(size_t i=0;i<len;i+=3){
        uint32_t v = in[i]<<16;
        if(i+1<len) v |= in[i+1]<<8;
        if(i+2<len) v |= in[i+2];
        *p++ = T[(v>>18)&0x3F];
        *p++ = T[(v>>12)&0x3F];
        *p++ = (i+1<len)?T[(v>>6)&0x3F]:'=';
        *p++ = (i+2<len)?T[v&0x3F]:'=';
    }
    *p = 0;
    return out;
}

/* ---------------- Networking helpers ---------------- */
static int create_listen_socket(int port){
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return -1; }
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(port), .sin_addr.s_addr = INADDR_ANY };
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(s); return -1; }
    if (listen(s, 32) < 0) { perror("listen"); close(s); return -1; }
    return s;
}

/* ---------------- HTTP helpers ---------------- */
static int is_get_root(const char *req){ return (strncmp(req, "GET /", 5) == 0); }
static int is_ws_upgrade(const char *req){
    return (strstr(req, "Upgrade: websocket") != NULL || strstr(req, "Upgrade: WebSocket") != NULL);
}
static char *get_header_value(const char *req, const char *header){
    char *p = strstr(req, header);
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

/* ---------------- Handshake helpers ---------------- */
static int compute_accept(const char *client_key, char **out_accept){
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
static int send_handshake(int fd, const char *accept_key){
    char resp[512];
    int n = snprintf(resp, sizeof(resp),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n\r\n", accept_key);
    if (n <= 0) return -1;
    return send(fd, resp, n, 0) == n ? 0 : -1;
}
static int do_handshake(int fd, const char *req){
    char *key = get_header_value(req, "Sec-WebSocket-Key:");
    if (!key) return -1;
    char *accept = NULL;
    int rc = compute_accept(key, &accept);
    free(key);
    if (rc != 0) return -1;
    rc = send_handshake(fd, accept);
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

static int read_frame(int fd, char *out, size_t outsz){
    unsigned char hdr[2];
    if (recv_all(fd, hdr, 2) != 2) return -1;
    int opcode = hdr[0] & 0x0F;
    int masked = hdr[1] & 0x80;
    uint64_t len = hdr[1] & 0x7F;
    if (len == 126) {
        uint16_t v;
        if (recv_all(fd, &v, 2) != 2) return -1;
        len = ntohs(v);
    } else if (len == 127) {
        uint64_t v;
        if (recv_all(fd, &v, 8) != 8) return -1;
        len = be64toh(v);
    }
    unsigned char mask[4] = {0,0,0,0};
    if (masked) {
        if (recv_all(fd, mask, 4) != 4) return -1;
    }
    if (len > outsz - 1) return -1;
    if (recv_all(fd, out, len) != (ssize_t)len) return -1;
    if (masked) for (uint64_t i=0;i<len;i++) out[i] ^= mask[i%4];
    out[len]=0;
    if (opcode == 0x8) return -2; // close
    if (opcode == 0x9) return -3; // ping
    return (int)len;
}

static int send_text_frame(int fd, const char *msg){
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
static int add_client(int clients[], int fd){
    for (int i=0;i<MAX_CLIENTS;i++) if (!clients[i]) { clients[i]=fd; return i; }
    return -1;
}
static void close_client(int clients[], int idx){
    if (clients[idx]) { close(clients[idx]); clients[idx]=0; }
}
static void broadcast(int clients[], int sender_idx, const char *msg){
    for (int i=0;i<MAX_CLIENTS;i++) if (clients[i] && i!=sender_idx) send_text_frame(clients[i], msg);
}

/* ---------------- High-level connection handling ---------------- */
static void handle_http(int client_fd, const char *req){
    if (is_get_root(req)) send(client_fd, INDEX_HTML, strlen(INDEX_HTML), 0);
    else {
        const char *notf = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        send(client_fd, notf, strlen(notf), 0);
    }
}

static void accept_and_handle(int listen_fd, int clients[]){
    int c = accept(listen_fd, NULL, NULL);
    if (c < 0) return;
    char buf[RECV_BUF];
    ssize_t r = recv(c, buf, sizeof(buf)-1, 0);
    if (r <= 0) { close(c); return; }
    buf[r]=0;
    if (is_ws_upgrade(buf)) {
        if (do_handshake(c, buf) == 0) {
            if (add_client(clients, c) < 0) close(c);
        } else close(c);
    } else {
        handle_http(c, buf);
        close(c);
    }
}

static void handle_client(int clients[], int idx){
    char msg[RECV_BUF];
    int r = read_frame(clients[idx], msg, sizeof(msg));
    if (r == -1 || r == -2) { close_client(clients, idx); return; }
    if (r == -3) return; // ping ignored
    broadcast(clients, idx, msg);
}

/* ---------------- Server run loop (keeps main tiny) ---------------- */
static int run_server(int port){
    int listen_fd = create_listen_socket(port);
    if (listen_fd < 0) return -1;
    int clients[MAX_CLIENTS] = {0};
    fd_set rfds;
    printf("Server listening on %d\n", port);
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(listen_fd, &rfds);
        int maxfd = listen_fd;
        for (int i=0;i<MAX_CLIENTS;i++) if (clients[i]) { FD_SET(clients[i], &rfds); if (clients[i] > maxfd) maxfd = clients[i]; }
        if (select(maxfd+1, &rfds, NULL, NULL, NULL) < 0) break;
        if (FD_ISSET(listen_fd, &rfds)) accept_and_handle(listen_fd, clients);
        for (int i=0;i<MAX_CLIENTS;i++) if (clients[i] && FD_ISSET(clients[i], &rfds)) handle_client(clients, i);
    }
    close(listen_fd);
    for (int i=0;i<MAX_CLIENTS;i++) if (clients[i]) close(clients[i]);
    return 0;
}

/* ---------------- Minimal main ---------------- */
int main(int argc, char **argv){
    if (argc < 2) { fprintf(stderr, "Usage: %s <port>\n", argv[0]); return 1; }
    int port = atoi(argv[1]);
    return run_server(port) == 0 ? 0 : 1;
}
