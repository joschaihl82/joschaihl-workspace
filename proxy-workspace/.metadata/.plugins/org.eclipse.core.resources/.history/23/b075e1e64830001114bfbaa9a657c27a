/* simple_proxy.c
   Kompiliere mit: gcc -O2 simple_proxy.c -o simple_proxy
   Starten: sudo ./simple_proxy 8888
   Hinweis: Einfaches, single-threaded Beispiel. Nur für Lernzwecke.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define LISTEN_BACKLOG 16
#define BUF_SIZE 8192
#define MAX_SAVE_SIZE (1024*1024) /* 1 MiB */

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

/* Hilfsfunktion: Host und Port aus Hostheader extrahieren */
static int parse_host_port(const char *hosthdr, char *host, size_t hlen, int *port) {
    const char *p = strchr(hosthdr, ':');
    if (!p) {
        strncpy(host, hosthdr, hlen-1);
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

/* Verbindet zum Zielserver */
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

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <listen_port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    int listen_fd = create_listener(port);
    if (listen_fd < 0) return 1;
    printf("Proxy listening on port %d\n", port);

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);
        int client = accept(listen_fd, (struct sockaddr*)&cli_addr, &cli_len);
        if (client < 0) { perror("accept"); continue; }

        /* Lese erste Anfrage (vereinfachte Annahme: Header passen in Buffer) */
        char buf[BUF_SIZE];
        ssize_t n = recv(client, buf, sizeof(buf)-1, 0);
        if (n <= 0) { close(client); continue; }
        buf[n] = '\0';

        /* Extrahiere Host-Header */
        char *hostline = NULL;
        char *line = strtok(buf, "\r\n");
        char request_line[512] = {0};
        if (line) strncpy(request_line, line, sizeof(request_line)-1);
        while (line) {
            if (strncasecmp(line, "Host:", 5) == 0) { hostline = line + 5; while (*hostline==' ') hostline++; break; }
            line = strtok(NULL, "\r\n");
        }
        if (!hostline) {
            close(client);
            continue;
        }

        char host[256]; int target_port;
        parse_host_port(hostline, host, sizeof(host), &target_port);
        int server = connect_to_host(host, target_port);
        if (server < 0) { close(client); continue; }

        /* Logge Anfragezeile und Host */
        printf("Request: %s | Host: %s:%d\n", request_line, host, target_port);

        /* Sende die ursprüngliche Anfrage weiter.
           Da wir strtok auf dem Buffer verwendet haben, bauen wir die Anfrage neu:
           Für Robustheit: wir re-recv die restlichen Daten nicht; dieses Beispiel ist minimal. */
        /* In realer Nutzung: man müsste die komplette Anfrage (inkl. Body) weiterleiten. */
        send(server, buf, n, 0);

        /* Weiterleiten der Antwort und Header‑Analyse */
        char headers[65536] = {0};
        ssize_t total_saved = 0;
        int header_collected = 0;
        int save_allowed = 1;
        FILE *outf = NULL;
        while ((n = recv(server, buf, sizeof(buf), 0)) > 0) {
            /* Wenn Header noch nicht vollständig gesammelt, suche nach "\r\n\r\n" */
            if (!header_collected) {
                char *sep = NULL;
                /* temporär in headers anhängen */
                size_t cur = strlen(headers);
                size_t tocopy = (size_t)n;
                if (cur + tocopy < sizeof(headers)-1) {
                    memcpy(headers+cur, buf, tocopy);
                    headers[cur+tocopy] = '\0';
                } else {
                    headers[sizeof(headers)-1] = '\0';
                }
                sep = strstr(headers, "\r\n\r\n");
                if (sep) {
                    header_collected = 1;
                    /* Prüfe Content-Type */
                    if (is_audio_content(headers)) {
                        save_allowed = 0; /* Speichern von Audio verhindern */
                    }
                    /* Falls speichern erlaubt und Content-Length klein genug, öffne Datei */
                    if (save_allowed) {
                        char fname[256];
                        snprintf(fname, sizeof(fname), "proxy_saved_%ld.bin", time(NULL));
                        outf = fopen(fname, "wb");
                        if (outf) printf("Saving non-audio response to %s\n", fname);
                    }
                }
            }

            /* Wenn speichern erlaubt, aber Gesamtgröße überschreitet, schließe Datei und setze save_allowed=0 */
            if (save_allowed && outf) {
                if (total_saved + n > MAX_SAVE_SIZE) {
                    fclose(outf);
                    outf = NULL;
                    save_allowed = 0;
                    printf("Save limit exceeded, stopping save for this response\n");
                } else {
                    fwrite(buf, 1, n, outf);
                    total_saved += n;
                }
            }

            /* Leite Daten an Client weiter */
            ssize_t sent = 0;
            while (sent < n) {
                ssize_t s = send(client, buf + sent, n - sent, 0);
                if (s <= 0) break;
                sent += s;
            }
            if (sent < n) break;
        }

        if (outf) fclose(outf);
        close(server);
        close(client);
    }

    close(listen_fd);
    return 0;
}
