#define _POSIX_C_SOURCE 200112L
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

static int connect_xsock(const char *path) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    struct sockaddr_un sa;
    memset(&sa,0,sizeof(sa));
    sa.sun_family = AF_UNIX;
    strncpy(sa.sun_path, path, sizeof(sa.sun_path)-1);
    if (connect(fd, (struct sockaddr*)&sa, sizeof(sa)) < 0) { close(fd); return -1; }
    return fd;
}

/* helpers to read/write exactly n bytes */
static ssize_t readn(int fd, void *buf, size_t n) {
    size_t off=0; while (off<n) {
        ssize_t r = read(fd, (char*)buf+off, n-off);
        if (r<=0) return r;
        off += r;
    }
    return off;
}
static ssize_t writen(int fd, const void *buf, size_t n) {
    size_t off=0; while (off<n) {
        ssize_t w = write(fd, (const char*)buf+off, n-off);
        if (w<=0) return w;
        off += w;
    }
    return off;
}

/* pack 16/32 big-endian */
static void put16(uint8_t *b, uint16_t v){ b[0]=v>>8; b[1]=v&0xFF; }
static void put32(uint8_t *b, uint32_t v){ b[0]=v>>24; b[1]=(v>>16)&0xFF; b[2]=(v>>8)&0xFF; b[3]=v&0xFF; }

int main(void) {
    const char *sock = "/tmp/.X11-unix/X0";
    int fd = connect_xsock(sock);
    if (fd < 0) { perror("connect"); return 1; }

    /* Send setup request: byte order 'l' (little), protocol 11.0, no auth */
    uint8_t setup[12] = { 'l', 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0 };
    // auth name length and data are zero; pad to 4 bytes already satisfied
    if (writen(fd, setup, sizeof(setup)) != sizeof(setup)) { perror("write setup"); close(fd); return 1; }

    /* Read setup reply header (first 8 bytes) */
    uint8_t rep8[8];
    if (readn(fd, rep8, 8) != 8) { perror("read reply"); close(fd); return 1; }
    if (rep8[0] != 1) { fprintf(stderr,"setup failed, status=%u\n", rep8[0]); close(fd); return 1; }
    uint16_t major = (rep8[1]<<8)|rep8[2];
    uint16_t minor = (rep8[3]<<8)|rep8[4];
    uint16_t auth_len = (rep8[6]<<8)|rep8[7];

    /* read remaining setup bytes: length in 4-byte units follows in bytes 6-7? simpler: read full setup per length field */
    // The setup reply includes a 2-byte length at offset 6 in 4-byte units; we already read 8 bytes, need to read (length*4 - 8)
    uint32_t rest_units = (rep8[6]<<8)|rep8[7];
    size_t rest_bytes = rest_units*4;
    uint8_t *rest = malloc(rest_bytes ? rest_bytes : 1);
    if (rest_bytes && readn(fd, rest, rest_bytes) != (ssize_t)rest_bytes) { perror("read rest"); free(rest); close(fd); return 1; }

    /* parse minimal info: vendor length at offset 0.. but to keep short, find first 4-byte aligned value: skip parsing and assume default visual TrueColor */
    (void)major; (void)minor;

    /* Build a CreateWindow request (opcode 1) and MapWindow (opcode 8) and CreateGC (opcode 55) and PutImage (opcode 72) */
    // For simplicity, we will use resource IDs starting at 0x01000000
    uint32_t xid_base = 0x01000000;
    uint32_t win = xid_base + 1;
    uint32_t gc  = xid_base + 2;
    uint32_t drawable = win;
    uint16_t W = 800, H = 600;

    // CreateWindow: request length in 4-byte units
    uint8_t cw[32];
    memset(cw,0,sizeof(cw));
    cw[0] = 1; // opcode
    // depth = 0 (copy from parent), pad
    put16(cw+2, 8); // length (will set later)
    put32(cw+4, win);
    put32(cw+8, 0); // parent: use root 0 (server will reject if wrong, but many servers accept 0? risky)
    put16(cw+12, 0); put16(cw+14, 0); // x,y
    put16(cw+16, W); put16(cw+18, H);
    put16(cw+20, 0); // border width
    put32(cw+24, 0x00000000); // class & visual & value mask omitted (not strictly correct)
    // set length to 8 (32 bytes)
    put16(cw+2, 8);
    if (writen(fd, cw, 32) != 32) { perror("write CreateWindow"); free(rest); close(fd); return 1; }

    // MapWindow
    uint8_t mw[8] = {8,0,0,2,0,0,0,0};
    put32(mw+4, win);
    if (writen(fd, mw, 8) != 8) { perror("write MapWindow"); free(rest); close(fd); return 1; }

    // CreateGC
    uint8_t cg[12];
    memset(cg,0,sizeof(cg));
    cg[0]=55; // CreateGC
    put16(cg+2, 3);
    put32(cg+4, gc);
    put32(cg+8, drawable);
    if (writen(fd, cg, 12) != 12) { perror("write CreateGC"); free(rest); close(fd); return 1; }

    srand((unsigned)time(NULL));
    // Prepare PutImage header template (format ZPixmap, depth 24)
    while (1) {
        // build a small image buffer (e.g., 200x150) and send PutImage repeatedly
        int iw = W, ih = H;
        size_t pixels = iw * ih;
        size_t imgbytes = pixels * 4; // 32-bit per pixel
        // header 16 bytes + data padded to 4
        uint32_t units = (16 + imgbytes + 3) / 4;
        uint8_t hdr[16];
        memset(hdr,0,16);
        hdr[0] = 72; // PutImage
        hdr[1] = 2;  // format ZPixmap
        put16(hdr+2, units);
        put32(hdr+4, drawable);
        put32(hdr+8, gc);
        put16(hdr+12, iw); put16(hdr+14, ih);
        // write header
        if (writen(fd, hdr, 16) != 16) break;
        // write pixel data (random)
        for (size_t i=0;i<pixels;i++) {
            uint8_t px[4];
            px[0] = rand() & 0xFF; // B
            px[1] = rand() & 0xFF; // G
            px[2] = rand() & 0xFF; // R
            px[3] = 0; // pad
            if (writen(fd, px, 4) != 4) goto done;
        }
        // flush by sleeping a bit

    }
done:
    free(rest);
    close(fd);
    return 0;
}
