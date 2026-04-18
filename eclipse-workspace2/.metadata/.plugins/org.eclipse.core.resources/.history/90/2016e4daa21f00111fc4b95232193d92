// ws_http_chat.c - minimal HTTP + WebSocket chat server without libcrypto
// Compile: gcc -O2 -Wall ws_http_chat.c -o ws_http_chat
// Run: ./ws_http_chat 8080

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
#define BUF 8192
const char *WS_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

/* --- SHA1 implementation --- */
typedef struct { uint32_t h[5]; uint8_t buf[64]; uint64_t len; } SHA1_CTX;
static void sha1_init(SHA1_CTX *c){ c->h[0]=0x67452301;c->h[1]=0xEFCDAB89;c->h[2]=0x98BADCFE;c->h[3]=0x10325476;c->h[4]=0xC3D2E1F0; c->len=0; }
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
    if(off){ size_t need=64-off; if(len<need){ memcpy(c->buf+off,data,len); return; } memcpy(c->buf+off,data,need); sha1_transform(c,c->buf); i+=need; }
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
    for(int i=0;i<5;i++){ out[4*i]=(c->h[i]>>24)&0xFF; out[4*i+1]=(c->h[i]>>16)&0xFF; out[4*i+2]=(c->h[i]>>8)&0xFF; out[4*i+3]=c->h[i]&0xFF; }
}

/* --- Base64 encode --- */
static char *b64enc(const unsigned char *in,size_t len){
    static const char T[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t outlen = 4*((len+2)/3);
    char *out = malloc(outlen+1); if(!out) return NULL;
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

/* --- Serve a simple HTML chat page (embedded) --- */
static const char *INDEX_HTML =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=utf-8\r\n"
"Connection: close\r\n\r\n"
"<!doctype html>\n"
"<html>\n<head>\n<meta charset='utf-8'>\n<title>WebSocket Chat</title>\n<style>body{font-family:Arial;margin:20px}#log{height:60vh;border:1px solid #ccc;padding:8px;overflow:auto}#msg{width:80%}</style>\n</head>\n<body>\n<h2>WebSocket Chat</h2>\n<div id='log'></div>\n<input id='msg' placeholder='Type message...' autofocus>\n<button id='send'>Send</button>\n<script>\n(function(){\n  var log=document.getElementById('log');\n  var ws=new WebSocket((location.protocol==='https:'?'wss://':'ws://')+location.host+location.pathname);\n  ws.onopen=function(){ log.innerHTML+='<div><em>Connected</em></div>'; };\n  ws.onmessage=function(e){ var d=document.createElement('div'); d.textContent=e.data; log.appendChild(d); log.scrollTop=log.scrollHeight; };\n  ws.onclose=function(){ log.innerHTML+='<div><em>Disconnected</em></div>'; };\n  document.getElementById('send').onclick=function(){ var m=document.getElementById('msg').value; if(!m) return; ws.send(m); document.getElementById('msg').value=''; };\n  document.getElementById('msg').addEventListener('keydown',function(e){ if(e.key==='Enter'){ document.getElementById('send').click(); } });\n})();\n</script>\n</body>\n</html>\r\n";

/* --- Handshake --- */
static int do_handshake(int client,const char *req){
    const char *k_hdr="Sec-WebSocket-Key:";
    char *k = strstr(req,k_hdr); if(!k) return -1;
    k += strlen(k_hdr); while(*k==' ') k++;
    char key[256]={0}; sscanf(k,"%255s",key);
    char src[512]; snprintf(src,sizeof(src),"%s%s",key,WS_GUID);
    SHA1_CTX ctx; sha1_init(&ctx); sha1_update(&ctx,(uint8_t*)src,strlen(src));
    uint8_t sha[20]; sha1_final(&ctx,sha);
    char *acc = b64enc(sha,20); if(!acc) return -1;
    char resp[512]; int n = snprintf(resp,sizeof(resp),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n\r\n", acc);
    free(acc);
    return send(client,resp,n,0)==n?0:-1;
}

/* --- WebSocket frame receive/send --- */
static int recv_frame(int fd,char *out,size_t outsz){
    unsigned char hdr[2];
    if(recv(fd,hdr,2,MSG_WAITALL)!=2) return -1;
    int opcode = hdr[0]&0x0F; int masked = hdr[1]&0x80;
    uint64_t len = hdr[1]&0x7F;
    if(len==126){ uint16_t v; if(recv(fd,&v,2,MSG_WAITALL)!=2) return -1; len=ntohs(v); }
    else if(len==127){ uint64_t v; if(recv(fd,&v,8,MSG_WAITALL)!=8) return -1; len=be64toh(v); }
    unsigned char mask[4]; if(masked){ if(recv(fd,mask,4,MSG_WAITALL)!=4) return -1; }
    if(len>outsz-1) return -1;
    if(recv(fd,out,len,MSG_WAITALL)!=(ssize_t)len) return -1;
    if(masked) for(uint64_t i=0;i<len;i++) out[i]^=mask[i%4];
    out[len]=0;
    if(opcode==0x8) return -2; // close
    return (int)len;
}
static int send_text(int fd,const char *msg){
    size_t len=strlen(msg); unsigned char hdr[10]; int h=0;
    hdr[0]=0x81;
    if(len<126){ hdr[1]=len; h=2; }
    else if(len<65536){ hdr[1]=126; uint16_t v=htons((uint16_t)len); memcpy(hdr+2,&v,2); h=4; }
    else{ hdr[1]=127; uint64_t be=htobe64(len); memcpy(hdr+2,&be,8); h=10; }
    if(send(fd,hdr,h,0)!=h) return -1; if(send(fd,msg,len,0)!=(ssize_t)len) return -1; return 0;
}

int main(int argc,char **argv){
    if(argc<2){ fprintf(stderr,"Usage: %s <port>\n",argv[0]); return 1; }
    int port=atoi(argv[1]);
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd<0){ perror("socket"); return 1; }
    int opt=1; setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    struct sockaddr_in addr = { .sin_family=AF_INET, .sin_port=htons(port), .sin_addr.s_addr=INADDR_ANY };
    if(bind(listenfd,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("bind"); return 1; }
    if(listen(listenfd, 32)<0){ perror("listen"); return 1; }
    int clients[MAX_CLIENTS] = {0};
    fd_set rfds;
    printf("HTTP+WebSocket server listening on %d\n", port);
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(listenfd,&rfds);
        int maxfd = listenfd;
        for (int i=0;i<MAX_CLIENTS;i++) if (clients[i]) { FD_SET(clients[i],&rfds); if (clients[i]>maxfd) maxfd=clients[i]; }
        if (select(maxfd+1,&rfds,NULL,NULL,NULL) < 0) break;
        if (FD_ISSET(listenfd,&rfds)) {
            int c = accept(listenfd,NULL,NULL);
            if(c<0) continue;
            // read initial request (HTTP GET or WebSocket upgrade)
            char buf[BUF];
            int r = recv(c, buf, sizeof(buf)-1, 0);
            if (r<=0) { close(c); continue; }
            buf[r]=0;
            if (strstr(buf,"Upgrade: websocket")) {
                if (do_handshake(c, buf)==0) {
                    int i; for (i=0;i<MAX_CLIENTS && clients[i];i++);
                    if (i<MAX_CLIENTS) clients[i]=c; else { close(c); }
                } else close(c);
            } else {
                // serve index for GET /
                if (strncmp(buf,"GET /",5)==0) {
                    send(c, INDEX_HTML, strlen(INDEX_HTML), 0);
                } else {
                    // simple 404
                    const char *notf = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
                    send(c, notf, strlen(notf), 0);
                }
                close(c);
            }
        }
        for (int i=0;i<MAX_CLIENTS;i++) if (clients[i] && FD_ISSET(clients[i],&rfds)) {
            char msg[BUF];
            int r = recv_frame(clients[i], msg, sizeof(msg));
            if (r <= 0) { close(clients[i]); clients[i]=0; continue; }
            // broadcast to others
            for (int j=0;j<MAX_CLIENTS;j++) if (clients[j] && j!=i) send_text(clients[j], msg);
        }
    }
    close(listenfd);
    return 0;
}
