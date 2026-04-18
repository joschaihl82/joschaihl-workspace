// hls_cmaf_proxy.c
// HTTP proxy für Linux, der HLS/CMAF Segmente mitschneidet und als zusammenhängende Datei abspeichert.
// Anpassung: Port als Makro, robustes Init‑Segment‑Buffering mit konfigurierbarem Limit und Warnung,
// threadsicheres Logging für jede Aktion.
// Compile: gcc -o hls_cmaf_proxy hls_cmaf_proxy.c -pthread
// Nur für rechtmäßige Zwecke verwenden.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>

//
// Konfiguration (Makros)
// - LISTEN_PORT: Port, auf dem der Proxy lauscht (keine CLI-Argumente)
// - INIT_BUF_MAX: Maximale Größe (Bytes) für Init‑Segment‑Pufferung
// - INIT_BUF_WARN_PERCENT: Prozentwert, bei dem eine Warnung geloggt wird (z. B. 80)
//
#define LISTEN_PORT 8080
#define BACKLOG 128
#define BUF_SIZE 8192
#define INIT_BUF_MAX (512*1024)         // Standard: 512 KB
#define INIT_BUF_WARN_PERCENT 80        // Warnung bei 80% des Limits
#define LOG_TS_BUF 64

// ---------------------- Logging (threadsicher) ----------------------
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static void log_msg(const char *fmt, ...) {
    char ts[LOG_TS_BUF];
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm);

    pthread_mutex_lock(&log_mutex);
    fprintf(stderr, "[%s] ", ts);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    fflush(stderr);
    pthread_mutex_unlock(&log_mutex);
}

// ---------------------- Utilities ----------------------
static void sanitize_filename(const char *in, char *out, size_t outlen) {
    size_t j = 0;
    for (size_t i = 0; in[i] && j + 1 < outlen; ++i) {
        char c = in[i];
        if (isalnum((unsigned char)c) || c == '.' || c == '-' || c == '_') out[j++] = c;
        else out[j++] = '_';
    }
    out[j] = '\0';
}

static uint32_t be32(const unsigned char *p){ return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|((uint32_t)p[3]); }
static uint64_t be64(const unsigned char *p){
    return ((uint64_t)p[0]<<56)|((uint64_t)p[1]<<48)|((uint64_t)p[2]<<40)|((uint64_t)p[3]<<32)
         | ((uint64_t)p[4]<<24)|((uint64_t)p[5]<<16)|((uint64_t)p[6]<<8)|((uint64_t)p[7]);
}

// ---------------------- ISOBMFF Atom Parsing ----------------------
static int parse_box_header(const unsigned char *buf, size_t buflen, size_t offset, uint64_t *out_size, char *typebuf) {
    if (offset + 8 > buflen) return -1;
    uint32_t size32 = be32(buf + offset);
    memcpy(typebuf, buf + offset + 4, 4); typebuf[4]=0;
    if (size32 == 1) {
        if (offset + 16 > buflen) return -1;
        uint64_t x = be64(buf + offset + 8);
        *out_size = x;
    } else {
        *out_size = size32;
    }
    if (*out_size < 8) return -1;
    return 0;
}

static const char* detect_codec_from_init_buffer(const unsigned char *buf, size_t len) {
    size_t off = 0;
    while (off + 8 <= len) {
        uint64_t box_size; char type[5];
        if (parse_box_header(buf, len, off, &box_size, type) < 0) break;
        if (off + box_size > len) break;
        if (strcmp(type,"ftyp")==0) {
            // ftyp gefunden; weiter scannen nach moov/stsd
        } else if (strcmp(type,"moov")==0 || strcmp(type,"moof")==0) {
            size_t inner = off + 8;
            size_t end = off + box_size;
            while (inner + 8 <= end) {
                uint64_t isz; char itype[5];
                if (parse_box_header(buf, end, inner, &isz, itype) < 0) break;
                if (inner + isz > end) break;
                if (strcmp(itype,"stsd")==0) {
                    size_t sstart = inner + 8;
                    if (sstart + 8 > inner + isz) { inner += isz; continue; }
                    size_t scan_start = sstart + 8;
                    size_t scan_end = inner + isz;
                    size_t max_scan = scan_start + 512;
                    if (max_scan > scan_end) max_scan = scan_end;
                    for (size_t k = scan_start; k + 4 <= max_scan; ++k) {
                        const unsigned char *c = buf + k;
                        if (isalpha(c[0]) && isalpha(c[1]) && isalpha(c[2]) && isalpha(c[3])) {
                            if (memcmp(c,"avc1",4)==0) return ".mp4";
                            if (memcmp(c,"hvc1",4)==0 || memcmp(c,"hev1",4)==0) return ".hevc.mp4";
                            if (memcmp(c,"av01",4)==0) return ".av1.mp4";
                            if (memcmp(c,"mp4a",4)==0) return ".m4a";
                            if (memcmp(c,"opus",4)==0) return ".opus";
                            if (memcmp(c,"ac-3",4)==0 || memcmp(c,"dac3",4)==0) return ".ac3";
                        }
                    }
                }
                inner += isz;
            }
        }
        off += box_size;
    }
    return NULL;
}

static const char* detect_extension_fallback(const char *url, const unsigned char *peek, size_t n) {
    const char *lower = url ? strrchr(url, '.') : NULL;
    if (lower) {
        if (strcasestr(lower, ".ts")) return ".ts";
        if (strcasestr(lower, ".m4s") || strcasestr(lower, ".mp4")) return ".mp4";
        if (strcasestr(lower, ".aac")) return ".aac";
        if (strcasestr(lower, ".m3u8")) return ".m3u8";
        if (strcasestr(lower, ".mp3")) return ".mp3";
    }
    if (n >= 3) {
        if (peek[0] == 0x47) return ".ts";
        if ((peek[0] == 0xFF) && ((peek[1] & 0xF0) == 0xF0)) return ".aac";
        if (n >= 8 && memcmp(peek + 4, "ftyp", 4) == 0) return ".mp4";
        if (peek[0] == 0xFF && (peek[1] & 0xE0) == 0xE0) return ".mp3";
    }
    return ".bin";
}

// Buffer from origin until moov/moof found or limit reached. Returns detected extension or NULL.
// On return out_buf contains buffered bytes (malloc'd) and out_len length. Caller must free out_buf.
static const char* buffer_init_and_detect(int origin_fd, unsigned char **out_buf, size_t *out_len, size_t max_buf) {
    unsigned char *buf = malloc(max_buf);
    if (!buf) {
        log_msg("Init-buffering: malloc(%zu) failed", max_buf);
        return NULL;
    }
    size_t filled = 0;
    size_t warn_threshold = (max_buf * INIT_BUF_WARN_PERCENT) / 100;
    log_msg("Init-buffering: start (max %zu bytes, warn at %zu bytes)", max_buf, warn_threshold);

    while (1) {
        // compute how much we can read without exceeding max_buf
        size_t can_read = (filled < max_buf) ? (max_buf - filled) : 0;
        if (can_read == 0) {
            log_msg("Init-buffering: reached configured limit (%zu bytes). Stopping buffering and falling back.", max_buf);
            break;
        }
        size_t to_read = can_read > 4096 ? 4096 : can_read;
        ssize_t r = recv(origin_fd, buf + filled, to_read, 0);
        if (r < 0) {
            log_msg("Init-buffering: recv error: %s", strerror(errno));
            break;
        }
        if (r == 0) {
            log_msg("Init-buffering: origin closed while buffering");
            break;
        }
        filled += (size_t)r;
        log_msg("Init-buffering: received %zu bytes (total %zu)", (size_t)r, filled);

        if (filled >= warn_threshold && warn_threshold > 0) {
            log_msg("Init-buffering: WARNING: buffered %zu bytes (>= %d%% of limit %zu)", filled, INIT_BUF_WARN_PERCENT, max_buf);
            // ensure we only log the warning once per threshold crossing
            warn_threshold = (size_t)-1;
        }

        const char *ext = detect_codec_from_init_buffer(buf, filled);
        if (ext) {
            log_msg("Init-buffering: codec detected via moov/moof -> %s", ext);
            *out_buf = buf; *out_len = filled; return ext;
        }

        // Heuristic fallback: if we've buffered a reasonable amount and found ftyp, assume mp4 family
        if (filled >= 8192) {
            for (size_t i = 0; i + 8 <= filled; ++i) {
                if (memcmp(buf + i + 4, "ftyp", 4) == 0) {
                    log_msg("Init-buffering: ftyp found but no moov yet; fallback to .mp4");
                    *out_buf = buf; *out_len = filled; return ".mp4";
                }
            }
        }
    }

    log_msg("Init-buffering: finished without precise detection (buffered %zu bytes)", filled);
    *out_buf = buf; *out_len = filled;
    return NULL;
}

// ---------------------- Networking helpers ----------------------
static int connect_host(const char *host, const char *port) {
    struct addrinfo hints, *res, *rp;
    int sfd = -1;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, port, &hints, &res) != 0) return -1;
    for (rp = res; rp; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) continue;
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(sfd);
        sfd = -1;
    }
    freeaddrinfo(res);
    return sfd;
}

static ssize_t read_line(int fd, char *buf, size_t maxlen) {
    size_t i = 0;
    char c;
    ssize_t n;
    while (i + 1 < maxlen) {
        n = recv(fd, &c, 1, 0);
        if (n <= 0) return n;
        buf[i++] = c;
        if (i >= 2 && buf[i-2] == '\r' && buf[i-1] == '\n') break;
    }
    buf[i] = '\0';
    return i;
}

struct client_arg { int client_fd; };

// ---------------------- Client handler ----------------------
static void* handle_client(void *arg) {
    struct client_arg ca = *(struct client_arg*)arg;
    free(arg);
    int client = ca.client_fd;
    char line[4096];
    char method[16], url[2048], httpver[32];

    log_msg("Connection: client fd=%d accepted", client);

    ssize_t r = read_line(client, line, sizeof(line));
    if (r <= 0) { log_msg("Connection: read request line failed or closed"); close(client); return NULL; }
    log_msg("Request line: %s", line);
    if (sscanf(line, "%15s %2047s %31s", method, url, httpver) != 3) {
        log_msg("Request: malformed request line");
        close(client); return NULL;
    }
    log_msg("Parsed request: method=%s url=%s httpver=%s", method, url, httpver);
    if (strcasecmp(method, "GET") != 0) {
        const char *resp = "HTTP/1.1 405 Method Not Allowed\r\nContent-Length:0\r\n\r\n";
        send(client, resp, strlen(resp), 0);
        log_msg("Request: method not allowed -> 405");
        close(client); return NULL;
    }

    char headers[16384] = "";
    char host_hdr[512] = "";
    while (1) {
        ssize_t n = read_line(client, line, sizeof(line));
        if (n <= 0) { log_msg("Request: header read failed"); close(client); return NULL; }
        if (strcmp(line, "\r\n") == 0) break;
        strncat(headers, line, sizeof(headers)-strlen(headers)-1);
        if (strncasecmp(line, "Host:", 5) == 0) {
            char *p = line + 5;
            while (*p && isspace((unsigned char)*p)) p++;
            strncpy(host_hdr, p, sizeof(host_hdr)-1);
            char *cr = strchr(host_hdr, '\r'); if (cr) *cr = '\0';
            char *lf = strchr(host_hdr, '\n'); if (lf) *lf = '\0';
        }
    }
    log_msg("Request headers collected (Host: %s)", host_hdr[0] ? host_hdr : "(none)");

    char proto[16], host[512], path[2048], port[8];
    memset(proto,0,sizeof(proto)); memset(host,0,sizeof(host)); memset(path,0,sizeof(path)); strcpy(port,"80");
    if (sscanf(url, "%15[^:]://%511[^/]%2047[^\n]", proto, host, path) >= 2) {
        char *pcol = strchr(host, ':');
        if (pcol) {
            *pcol = '\0';
            strncpy(port, pcol+1, sizeof(port)-1);
        }
        if (path[0] == '\0') strcpy(path, "/");
    } else {
        if (host_hdr[0] == '\0') { log_msg("Request: no Host header and no absolute URL"); close(client); return NULL; }
        char tmp[512]; strncpy(tmp, host_hdr, sizeof(tmp)-1);
        char *pcol = strchr(tmp, ':');
        if (pcol) { *pcol = '\0'; strncpy(port, pcol+1, sizeof(port)-1); }
        strncpy(host, tmp, sizeof(host)-1);
        strncpy(path, url, sizeof(path)-1);
    }
    log_msg("Target origin: host=%s port=%s path=%s", host, port, path);

    int origin = connect_host(host, port);
    if (origin < 0) {
        log_msg("Origin connect failed: %s:%s", host, port);
        const char *resp = "HTTP/1.1 502 Bad Gateway\r\nContent-Length:0\r\n\r\n";
        send(client, resp, strlen(resp), 0);
        close(client); return NULL;
    }
    log_msg("Connected to origin %s:%s (fd=%d)", host, port, origin);

    char req[8192];
    snprintf(req, sizeof(req), "%s %s %s\r\nHost: %s\r\nConnection: close\r\n%s\r\n",
             method, path, httpver, host, headers);
    send(origin, req, strlen(req), 0);
    log_msg("Forwarded request to origin (fd=%d): %zu bytes", origin, strlen(req));

    char resp_line[4096];
    ssize_t n = read_line(origin, resp_line, sizeof(resp_line));
    if (n <= 0) { log_msg("Origin: failed to read status line"); close(origin); close(client); return NULL; }
    send(client, resp_line, strlen(resp_line), 0);
    log_msg("Origin status line: %s", resp_line);

    char resp_headers[16384] = "";
    char content_type[256] = "";
    long content_length = -1;
    int chunked = 0;
    while (1) {
        ssize_t m = read_line(origin, line, sizeof(line));
        if (m <= 0) { log_msg("Origin: header read failed"); close(origin); close(client); return NULL; }
        send(client, line, strlen(line), 0);
        if (strcmp(line, "\r\n") == 0) break;
        strncat(resp_headers, line, sizeof(resp_headers)-strlen(resp_headers)-1);
        if (strncasecmp(line, "Content-Type:", 13) == 0) {
            char *p = line + 13; while (*p && isspace((unsigned char)*p)) p++;
            strncpy(content_type, p, sizeof(content_type)-1);
            char *cr = strchr(content_type, '\r'); if (cr) *cr = '\0';
            char *lf = strchr(content_type, '\n'); if (lf) *lf = '\0';
        } else if (strncasecmp(line, "Content-Length:", 15) == 0) {
            content_length = atol(line + 15);
        } else if (strncasecmp(line, "Transfer-Encoding:", 18) == 0) {
            if (strcasestr(line, "chunked")) chunked = 1;
        }
    }
    log_msg("Origin headers received. Content-Type='%s' Content-Length=%ld chunked=%d",
            content_type[0] ? content_type : "(none)", content_length, chunked);

    int should_save = 0;
    if (strcasestr(path, ".m4s") || strcasestr(path, ".mp4") || strcasestr(path, ".ts") ||
        strcasestr(path, ".aac") || strcasestr(path, ".m3u8") || strcasestr(path, ".mp3")) {
        should_save = 1;
    } else if (content_type[0]) {
        if (strcasestr(content_type, "video/MP2T") || strcasestr(content_type, "video/mp2t")) should_save = 1;
        if (strcasestr(content_type, "video/mp4") || strcasestr(content_type, "application/octet-stream")) should_save = 1;
        if (strcasestr(content_type, "audio/aac") || strcasestr(content_type, "audio/mpeg")) should_save = 1;
    }
    log_msg("Save decision: should_save=%d", should_save);

    FILE *save_fp = NULL;
    char filename[1024];
    const char *ext = ".bin";
    unsigned char *init_buf = NULL;
    size_t init_len = 0;

    if (should_save && !chunked) {
        unsigned char smallpeek[16];
        ssize_t sp = recv(origin, smallpeek, sizeof(smallpeek), MSG_PEEK);
        if (sp > 0 && sp >= 8 && memcmp(smallpeek + 4, "ftyp", 4) == 0) {
            log_msg("Detected 'ftyp' in peek; starting robust init buffering");
            const char *det = buffer_init_and_detect(origin, &init_buf, &init_len, INIT_BUF_MAX);
            if (det) {
                ext = det;
                log_msg("Codec extension detected from init buffer: %s", ext);
            } else {
                if (init_len > 8 && memcmp(init_buf + 4, "ftyp", 4) == 0) ext = ".mp4";
                else ext = detect_extension_fallback(path, init_buf, init_len);
                log_msg("Fallback extension after buffering: %s (buffered %zu bytes)", ext, init_len);
            }
        } else {
            unsigned char peek2[1024];
            ssize_t got = recv(origin, peek2, sizeof(peek2), MSG_PEEK);
            if (got > 0) ext = detect_extension_fallback(path, peek2, (size_t)got);
            else ext = detect_extension_fallback(path, NULL, 0);
            log_msg("Non-ftyp response: fallback extension=%s (peeked %zd bytes)", ext, got);
        }
    } else if (should_save) {
        unsigned char peek2[1024];
        ssize_t got = recv(origin, peek2, sizeof(peek2), MSG_PEEK);
        if (got > 0) ext = detect_extension_fallback(path, peek2, (size_t)got);
        else ext = detect_extension_fallback(path, NULL, 0);
        log_msg("Chunked or unknown-length response: fallback extension=%s (peeked %zd bytes)", ext, got);
    }

    if (should_save) {
        char base[1024];
        snprintf(base, sizeof(base), "%s%s", host, path);
        sanitize_filename(base, filename, sizeof(filename));
        char finalname[1200];
        time_t t = time(NULL);
        snprintf(finalname, sizeof(finalname), "%s_%ld%s", filename, (long)t, ext);
        save_fp = fopen(finalname, "ab");
        if (!save_fp) {
            log_msg("fopen failed for %s: %s", finalname, strerror(errno));
            save_fp = NULL;
        } else {
            log_msg("Saving stream %s -> %s", path, finalname);
        }
    }

    if (init_buf && init_len > 0) {
        log_msg("Forwarding buffered init (%zu bytes) to client and file", init_len);
        size_t sent = 0;
        while (sent < init_len) {
            ssize_t w = send(client, init_buf + sent, init_len - sent, 0);
            if (w <= 0) { log_msg("Error sending buffered init to client: %s", strerror(errno)); break; }
            sent += (size_t)w;
        }
        if (save_fp) {
            size_t wrote = fwrite(init_buf, 1, init_len, save_fp);
            fflush(save_fp);
            log_msg("Wrote %zu init bytes to file", wrote);
        }
        free(init_buf);
        init_buf = NULL;
        init_len = 0;
    }

    // Stream body
    if (chunked) {
        log_msg("Streaming chunked body");
        char chunk_line[128];
        while (1) {
            ssize_t ln = read_line(origin, chunk_line, sizeof(chunk_line));
            if (ln <= 0) { log_msg("Chunked: read_line failed"); break; }
            send(client, chunk_line, strlen(chunk_line), 0);
            unsigned int chunk_size = 0;
            sscanf(chunk_line, "%x", &chunk_size);
            if (chunk_size == 0) {
                log_msg("Chunked: final chunk");
                while (1) {
                    ssize_t tln = read_line(origin, chunk_line, sizeof(chunk_line));
                    if (tln <= 0) break;
                    send(client, chunk_line, strlen(chunk_line), 0);
                    if (strcmp(chunk_line, "\r\n") == 0) break;
                }
                break;
            }
            size_t remaining = chunk_size;
            unsigned char buf[BUF_SIZE];
            while (remaining > 0) {
                ssize_t rcv = recv(origin, buf, remaining > sizeof(buf) ? sizeof(buf) : remaining, 0);
                if (rcv <= 0) { remaining = 0; break; }
                send(client, buf, rcv, 0);
                if (save_fp) {
                    size_t wrote = fwrite(buf, 1, rcv, save_fp);
                    if (wrote != (size_t)rcv) log_msg("Warning: fwrite wrote %zu of %zd", wrote, rcv);
                }
                remaining -= rcv;
            }
            // read CRLF after chunk
            char crlf2[2];
            recv(origin, crlf2, 2, 0);
            send(client, crlf2, 2, 0);
        }
    } else if (content_length >= 0) {
        log_msg("Streaming body with Content-Length=%ld", content_length);
        long remaining = content_length;
        unsigned char buf[BUF_SIZE];
        while (remaining > 0) {
            ssize_t toread = remaining > BUF_SIZE ? BUF_SIZE : remaining;
            ssize_t rcv = recv(origin, buf, toread, 0);
            if (rcv <= 0) { log_msg("Content-Length: recv returned %zd", rcv); break; }
            send(client, buf, rcv, 0);
            if (save_fp) {
                size_t wrote = fwrite(buf, 1, rcv, save_fp);
                if (wrote != (size_t)rcv) log_msg("Warning: fwrite wrote %zu of %zd", wrote, rcv);
            }
            remaining -= rcv;
        }
    } else {
        log_msg("Streaming body until close");
        unsigned char buf[BUF_SIZE];
        ssize_t rcv;
        while ((rcv = recv(origin, buf, sizeof(buf), 0)) > 0) {
            send(client, buf, rcv, 0);
            if (save_fp) {
                size_t wrote = fwrite(buf, 1, rcv, save_fp);
                if (wrote != (size_t)rcv) log_msg("Warning: fwrite wrote %zu of %zd", wrote, rcv);
            }
        }
        log_msg("Stream ended (recv returned %zd)", rcv);
    }

    if (save_fp) {
        fclose(save_fp);
        log_msg("Closed saved file");
    }
    close(origin);
    close(client);
    log_msg("Closed connection: client fd=%d origin fd closed", client);
    return NULL;
}

// ---------------------- Main ----------------------
int main(void) {
    int port = LISTEN_PORT;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { log_msg("socket() failed: %s", strerror(errno)); return 1; }
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) { log_msg("bind() failed: %s", strerror(errno)); return 1; }
    if (listen(sock, BACKLOG) < 0) { log_msg("listen() failed: %s", strerror(errno)); return 1; }
    log_msg("Proxy listening on port %d (no CLI args, macro LISTEN_PORT). INIT_BUF_MAX=%d bytes, WARN=%d%%",
            port, INIT_BUF_MAX, INIT_BUF_WARN_PERCENT);
    while (1) {
        struct sockaddr_in cli;
        socklen_t clilen = sizeof(cli);
        int client = accept(sock, (struct sockaddr*)&cli, &clilen);
        if (client < 0) { log_msg("accept() failed: %s", strerror(errno)); continue; }
        log_msg("Accepted connection from %s:%d -> fd=%d", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port), client);
        pthread_t tid;
        struct client_arg *ca = malloc(sizeof(*ca));
        if (!ca) { log_msg("malloc failed for client_arg"); close(client); continue; }
        ca->client_fd = client;
        if (pthread_create(&tid, NULL, handle_client, ca) != 0) {
            log_msg("pthread_create failed: %s", strerror(errno));
            close(client); free(ca); continue;
        }
        pthread_detach(tid);
    }
    close(sock);
    return 0;
}
