/* proxy_verbose_macro.c
   Kompiliere mit: gcc -O2 -Wall -Wextra -o proxy_verbose_macro proxy_verbose_macro.c
   Starten: sudo ./proxy_verbose_macro
   Hinweis: Einfaches, single-threaded Beispiel. Nur für Lernzwecke.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PROXY_PORT 8888
#define LISTEN_BACKLOG 16
#define BUF_SIZE 8192
#define MAX_SAVE_SIZE (1024*1024) /* 1 MiB */
#define HEX_PREVIEW 64

static FILE *logfile = NULL;

/* Zeitstempel für Logs */
static void log_time(char *buf, size_t len) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);
    snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
             tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec/1000000);
}

/* Zentrale Logfunktion mit va_list und va_copy */
static void logv(const char *fmt, ...) {
    char tbuf[64];
    log_time(tbuf, sizeof(tbuf));
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[%s] ", tbuf);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");

    if (logfile) {
        va_list ap2;
        va_copy(ap2, ap);
        fprintf(logfile, "[%s] ", tbuf);
        vfprintf(logfile, fmt, ap2);
        fprintf(logfile, "\n");
        fflush(logfile);
        va_end(ap2);
    }

    va_end(ap);
}

/* Hex preview der ersten N Bytes eines Buffers */
static void hex_preview(const unsigned char *buf, size_t n) {
    size_t i, lim = n < HEX_PREVIEW ? n : HEX_PREVIEW;
    fprintf(stderr, "  HEX:");
    if (logfile) fprintf(logfile, "  HEX:");
    for (i = 0; i < lim; ++i) {
        fprintf(stderr, " %02x", buf[i]);
        if (logfile) fprintf(logfile, " %02x", buf[i]);
    }
    if (n > lim) {
        fprintf(stderr, " ... (%zu bytes total)", n);
        if (logfile) fprintf(logfile, " ... (%zu bytes total)", n);
    }
    fprintf(stderr, "\n");
    if (logfile) fprintf(logfile, "\n");
}

/* Listener erstellen */
static int create_listener(int port) {
    int s, opt = 1;
    struct sockaddr_in addr;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return -1; }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(s); return -1; }
    if (listen(s, LISTEN_BACKLOG) < 0) { perror("listen"); close(s); return -1; }
    return s;
}

/* Host:Port parsen */
static int parse_host_port(const char *hosthdr, char *host, size_t hlen, int *port) {
    const char *p = strchr(hosthdr, ':');
    if (!p) {
        strncpy(host, hosthdr, hlen-1);
        host[hlen-1] = '\0';
        *port = 80;
    } else {
        size_t n = p - hosthdr;
        if (n >= hlen) n = hlen-1;
        strncpy(host, hosthdr, n);
        host[n] = '\0';
        *port = atoi(p+1);
        if (*port == 0) *port = 80;
    }
    return 0;
}

/* Verbindung zum Zielhost */
static int connect_to_host(const char *host, int port) {
    struct addrinfo hints, *res, *rp;
    char portstr[16];
    int s = -1;
    snprintf(portstr, sizeof(portstr), "%d", port);
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, portstr, &hints, &res) != 0) return -1;
    for (rp = res; rp; rp = rp->ai_next) {
        s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (s < 0) continue;
        if (connect(s, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(s);
        s = -1;
    }
    freeaddrinfo(res);
    return s;
}

/* Prüft, ob Content-Type audio ist */
static int is_audio_content(const char *headers) {
    if (!headers) return 0;
    const char *p = strcasestr(headers, "content-type:");
    if (!p) return 0;
    p = strchr(p, ':');
    if (!p) return 0;
    p++;
    while (*p == ' ') p++;
    if (strncasecmp(p, "audio/", 6) == 0) return 1;
    return 0;
}

/* Entfernt führende und nachfolgende Leerzeichen */
static void trim(char *s) {
    char *p = s;
    while (*p == ' ' || *p == '\t') p++;
    if (p != s) memmove(s, p, strlen(p)+1);
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == ' ' || s[len-1] == '\t')) s[--len] = '\0';
}

int main(void) {
    logfile = fopen("proxy_verbose.log", "a");
    if (!logfile) {
        fprintf(stderr, "Warnung: konnte proxy_verbose.log nicht öffnen, schreibe nur stderr\n");
    }

    int port = PROXY_PORT;
    int listen_fd = create_listener(port);
    if (listen_fd < 0) {
        logv("Failed to create listener on port %d", port);
        return 1;
    }
    logv("Proxy listening on port %d", port);

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);
        int client = accept(listen_fd, (struct sockaddr*)&cli_addr, &cli_len);
        if (client < 0) { logv("accept failed: %s", strerror(errno)); continue; }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli_addr.sin_addr, client_ip, sizeof(client_ip));
        logv("Accepted connection from %s:%d (fd=%d)", client_ip, ntohs(cli_addr.sin_port), client);

        /* Lese erste Anfrage (vereinfachte Annahme: Header passen in Buffer) */
        char buf[BUF_SIZE];
        ssize_t n = recv(client, buf, sizeof(buf)-1, 0);
        if (n <= 0) { logv("recv from client failed or closed: %s", strerror(errno)); close(client); continue; }
        buf[n] = '\0';
        logv("Read %zd bytes from client %s", n, client_ip);
        hex_preview((unsigned char*)buf, n);

        /* Extrahiere Host-Header */
        char *hostline = NULL;
        char *saveptr = NULL;
        char *line = strtok_r(buf, "\r\n", &saveptr);
        char request_line[512] = {0};
        if (line) strncpy(request_line, line, sizeof(request_line)-1);
        logv("Request-Line: %s", request_line);
        while (line) {
            if (strncasecmp(line, "Host:", 5) == 0) { hostline = line + 5; trim(hostline); break; }
            line = strtok_r(NULL, "\r\n", &saveptr);
        }
        if (!hostline) {
            logv("No Host header found, closing connection");
            close(client);
            continue;
        }
        logv("Host header: %s", hostline);

        char host[256]; int target_port;
        parse_host_port(hostline, host, sizeof(host), &target_port);
        logv("Parsed host: %s port: %d", host, target_port);

        int server = connect_to_host(host, target_port);
        if (server < 0) { logv("Failed to connect to target %s:%d", host, target_port); close(client); continue; }
        logv("Connected to target %s:%d (fd=%d)", host, target_port, server);

        /* Sende die ursprüngliche Anfrage weiter.
           Hinweis: In diesem einfachen Beispiel wird nur der bereits gelesene Buffer weitergeleitet. */
        ssize_t sent = send(server, buf, n, 0);
        logv("Forwarded %zd bytes to server %s:%d", sent, target_port, target_port);

        /* Weiterleiten der Antwort und Header‑Analyse */
        char headers[65536] = {0};
        ssize_t total_saved = 0;
        int header_collected = 0;
        int save_allowed = 1;
        FILE *outf = NULL;
        while ((n = recv(server, buf, sizeof(buf), 0)) > 0) {
            logv("Received %zd bytes from server %s", n, host);
            hex_preview((unsigned char*)buf, n);

            /* Header sammeln */
            if (!header_collected) {
                size_t cur = strlen(headers);
                size_t tocopy = (size_t)n;
                if (cur + tocopy < sizeof(headers)-1) {
                    memcpy(headers+cur, buf, tocopy);
                    headers[cur+tocopy] = '\0';
                } else {
                    /* Header zu groß */
                    headers[sizeof(headers)-1] = '\0';
                }
                char *sep = strstr(headers, "\r\n\r\n");
                if (sep) {
                    header_collected = 1;
                    logv("Response headers collected (truncated to %zu bytes)", strlen(headers));
                    /* Logge Header-Auszug */
                    size_t hloglen = strlen(headers) < 1024 ? strlen(headers) : 1024;
                    char htmp[1025]; memcpy(htmp, headers, hloglen); htmp[hloglen] = '\0';
                    logv("Response header preview:\n%s", htmp);

                    if (is_audio_content(headers)) {
                        save_allowed = 0;
                        logv("Detected audio Content-Type in response headers. Saving disabled for this response.");
                    } else {
                        logv("No audio Content-Type detected. Saving allowed for this response.");
                    }

                    if (save_allowed) {
                        char fname[256];
                        snprintf(fname, sizeof(fname), "proxy_saved_verbose_%ld.bin", time(NULL));
                        outf = fopen(fname, "wb");
                        if (outf) logv("Opened file %s for saving response body", fname);
                        else { logv("Failed to open file %s for writing: %s", fname, strerror(errno)); save_allowed = 0; }
                    }
                }
            }

            /* Speichern wenn erlaubt und Limit nicht überschritten */
            if (save_allowed && outf) {
                if (total_saved + n > MAX_SAVE_SIZE) {
                    fclose(outf);
                    outf = NULL;
                    save_allowed = 0;
                    logv("Save limit exceeded (%zd bytes). Stopped saving this response.", total_saved + n);
                } else {
                    size_t w = fwrite(buf, 1, n, outf);
                    if (w != (size_t)n) {
                        logv("Write error while saving response: %s", strerror(errno));
                        fclose(outf);
                        outf = NULL;
                        save_allowed = 0;
                    } else {
                        total_saved += w;
                        logv("Saved %zu bytes to file (total saved %zd bytes)", w, total_saved);
                    }
                }
            }

            /* Leite Daten an Client weiter */
            ssize_t tosend = n;
            ssize_t off = 0;
            while (tosend > 0) {
                ssize_t s = send(client, buf + off, tosend, 0);
                if (s <= 0) {
                    logv("send to client failed: %s", strerror(errno));
                    break;
                }
                off += s;
                tosend -= s;
            }
            logv("Forwarded %zd bytes to client %s", off, client_ip);
            if (off < n) break;
        }

        if (n == 0) logv("Server closed connection normally");
        else if (n < 0) logv("recv from server failed: %s", strerror(errno));

        if (outf) { fclose(outf); logv("Closed saved file after finishing response"); }
        close(server);
        close(client);
        logv("Closed connection for client %s", client_ip);
    }

    if (logfile) fclose(logfile);
    close(listen_fd);
    return 0;
}
