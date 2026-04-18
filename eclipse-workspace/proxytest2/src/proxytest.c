// proxy_only_working.c
// Reads data.txt and prints only working proxies sorted by total latency (TCP + PROTO).
// Supported prefixes: socks4:// socks5:// http:// https:// or host:port (defaults to http).
// Compile: gcc -O2 -Wall -pthread -o proxy_only_working proxy_only_working.c

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>

#define LINE_MAX_LEN 2048
#define DEFAULT_TIMEOUT_MS 3000
#define WORKER_COUNT 50

typedef enum { MODE_HTTP, MODE_HTTPS, MODE_SOCKS4, MODE_SOCKS5 } proto_mode_t;

typedef struct {
    char *proxy_host;
    char *proxy_port;
    proto_mode_t mode;
} job_t;

typedef struct job_node_s {
    job_t job;
    struct job_node_s *next;
} job_node_t;

typedef struct {
    job_node_t *head, *tail;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    int closed;
} queue_t;

typedef struct {
    char *host;
    char *port;
    proto_mode_t mode;
    long tcp_ms;
    long proto_ms;
    long total_ms;
} result_t;

static queue_t q;
static pthread_mutex_t results_mtx = PTHREAD_MUTEX_INITIALIZER;
static result_t *results = NULL;
static size_t results_count = 0;
static size_t results_cap = 0;
static int timeout_ms = DEFAULT_TIMEOUT_MS;

static void queue_init(queue_t *q) {
    q->head = q->tail = NULL;
    pthread_mutex_init(&q->mtx, NULL);
    pthread_cond_init(&q->cond, NULL);
    q->closed = 0;
}

static void queue_push(queue_t *q, job_t job) {
    job_node_t *n = malloc(sizeof(*n));
    if (!n) { perror("malloc"); exit(1); }
    n->job = job;
    n->next = NULL;
    pthread_mutex_lock(&q->mtx);
    if (q->tail) q->tail->next = n; else q->head = n;
    q->tail = n;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mtx);
}

static int queue_pop(queue_t *q, job_t *out) {
    pthread_mutex_lock(&q->mtx);
    while (!q->head && !q->closed) pthread_cond_wait(&q->cond, &q->mtx);
    if (!q->head) { pthread_mutex_unlock(&q->mtx); return 0; }
    job_node_t *n = q->head;
    q->head = n->next;
    if (!q->head) q->tail = NULL;
    pthread_mutex_unlock(&q->mtx);
    *out = n->job;
    free(n);
    return 1;
}

static void queue_close(queue_t *q) {
    pthread_mutex_lock(&q->mtx);
    q->closed = 1;
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->mtx);
}

static long timespec_diff_ms(const struct timespec *a, const struct timespec *b) {
    long s = a->tv_sec - b->tv_sec;
    long ns = a->tv_nsec - b->tv_nsec;
    return s * 1000 + ns / 1000000;
}

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int wait_fd(int fd, int events, int timeout_ms) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = events;
    int r;
    do { r = poll(&pfd, 1, timeout_ms); } while (r < 0 && errno == EINTR);
    return r;
}

static int connect_nonblocking_addr(struct addrinfo *ai, int timeout_ms, struct timespec *t_start, struct timespec *t_end, int *out_sock) {
    int s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (s < 0) return -1;
    set_nonblocking(s);
    clock_gettime(CLOCK_MONOTONIC, t_start);
    int rc = connect(s, ai->ai_addr, ai->ai_addrlen);
    if (rc == 0) {
        clock_gettime(CLOCK_MONOTONIC, t_end);
        *out_sock = s;
        return 0;
    } else if (errno == EINPROGRESS) {
        int pol = wait_fd(s, POLLOUT, timeout_ms);
        if (pol > 0) {
            int err = 0;
            socklen_t len = sizeof(err);
            if (getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &len) < 0) err = errno;
            clock_gettime(CLOCK_MONOTONIC, t_end);
            if (err == 0) {
                *out_sock = s;
                return 0;
            } else {
                close(s);
                errno = err;
                return -1;
            }
        } else if (pol == 0) {
            close(s);
            errno = ETIMEDOUT;
            return -1;
        } else {
            close(s);
            return -1;
        }
    } else {
        close(s);
        return -1;
    }
}

static int tcp_connect_to(const char *host, const char *port, int timeout_ms, int *out_sock, struct timespec *t_start, struct timespec *t_end) {
    struct addrinfo hints, *res, *rp;
    int gai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    if ((gai = getaddrinfo(host, port, &hints, &res)) != 0) {
        errno = EINVAL;
        return -1;
    }
    int ok = -1;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        if (connect_nonblocking_addr(rp, timeout_ms, t_start, t_end, out_sock) == 0) {
            ok = 0;
            break;
        }
    }
    freeaddrinfo(res);
    return ok;
}

static ssize_t send_all_timeout(int fd, const void *buf, size_t len, int timeout_ms) {
    size_t sent = 0;
    const char *p = buf;
    while (sent < len) {
        int pol = wait_fd(fd, POLLOUT, timeout_ms);
        if (pol <= 0) return -1;
        ssize_t w = send(fd, p + sent, len - sent, 0);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        sent += w;
    }
    return (ssize_t)sent;
}

static ssize_t recv_some_timeout(int fd, void *buf, size_t len, int timeout_ms) {
    int pol = wait_fd(fd, POLLIN, timeout_ms);
    if (pol <= 0) return -1;
    ssize_t r = recv(fd, buf, len, 0);
    return r;
}

/* Protocol actions */

static int do_http_connect(int sock, const char *t_host, const char *t_port, int timeout_ms) {
    char req[512];
    snprintf(req, sizeof(req),
             "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
             t_host, t_port, t_host, t_port);
    if (send_all_timeout(sock, req, strlen(req), timeout_ms) < 0) return -1;
    char buf[1024];
    size_t got = 0;
    while (1) {
        ssize_t r = recv_some_timeout(sock, buf + got, sizeof(buf) - got - 1, timeout_ms);
        if (r <= 0) return -1;
        got += r;
        buf[got] = '\0';
        if (strstr(buf, "\r\n\r\n")) break;
        if (got >= sizeof(buf) - 1) break;
    }
    if (strstr(buf, "200") != NULL) return 0;
    return -1;
}

static int do_socks4(int sock, const char *t_host, const char *t_port, int timeout_ms) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(t_host, t_port, &hints, &res) != 0) return -1;
    struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
    uint16_t portn = sin->sin_port;
    uint32_t ipn = sin->sin_addr.s_addr;
    freeaddrinfo(res);

    unsigned char req[9];
    req[0] = 0x04;
    req[1] = 0x01;
    memcpy(&req[2], &portn, 2);
    memcpy(&req[4], &ipn, 4);
    if (send_all_timeout(sock, req, 8, timeout_ms) < 0) return -1;
    unsigned char zero = 0x00;
    if (send_all_timeout(sock, &zero, 1, timeout_ms) < 0) return -1;
    unsigned char resp[8];
    ssize_t r = recv_some_timeout(sock, resp, sizeof(resp), timeout_ms);
    if (r < 8) return -1;
    if (resp[1] == 0x5A) return 0;
    return -1;
}

static int do_socks5(int sock, const char *t_host, const char *t_port, int timeout_ms) {
    unsigned char greet[3] = {0x05, 0x01, 0x00};
    if (send_all_timeout(sock, greet, 3, timeout_ms) < 0) return -1;
    unsigned char sel[2];
    ssize_t r = recv_some_timeout(sock, sel, 2, timeout_ms);
    if (r != 2) return -1;
    if (sel[0] != 0x05 || sel[1] == 0xFF) return -1;

    unsigned char req[512];
    size_t reqlen = 0;
    req[0] = 0x05; req[1] = 0x01; req[2] = 0x00;
    struct in6_addr in6;
    struct in_addr in4;
    if (inet_pton(AF_INET, t_host, &in4) == 1) {
        req[3] = 0x01;
        memcpy(&req[4], &in4, 4);
        reqlen = 4 + 4;
    } else if (inet_pton(AF_INET6, t_host, &in6) == 1) {
        req[3] = 0x04;
        memcpy(&req[4], &in6, 16);
        reqlen = 4 + 16;
    } else {
        req[3] = 0x03;
        size_t hlen = strlen(t_host);
        if (hlen > 255) return -1;
        req[4] = (unsigned char)hlen;
        memcpy(&req[5], t_host, hlen);
        reqlen = 5 + hlen;
    }
    uint16_t portn = htons((uint16_t)atoi(t_port));
    memcpy(&req[reqlen], &portn, 2);
    reqlen += 2;
    if (send_all_timeout(sock, req, 3 + reqlen, timeout_ms) < 0) return -1;

    unsigned char header[4];
    r = recv_some_timeout(sock, header, 4, timeout_ms);
    if (r != 4) return -1;
    if (header[0] != 0x05 || header[1] != 0x00) return -1;
    unsigned char atyp = header[3];
    size_t toread = 0;
    if (atyp == 0x01) toread = 4 + 2;
    else if (atyp == 0x04) toread = 16 + 2;
    else if (atyp == 0x03) {
        unsigned char lenb;
        r = recv_some_timeout(sock, &lenb, 1, timeout_ms);
        if (r != 1) return -1;
        toread = (size_t)lenb + 2;
    } else return -1;
    unsigned char *rest = malloc(toread);
    if (!rest) return -1;
    r = recv_some_timeout(sock, rest, toread, timeout_ms);
    free(rest);
    if (r != (ssize_t)toread) return -1;
    return 0;
}

/* Add successful result to global array (thread-safe) */
static void add_result(const char *host, const char *port, proto_mode_t mode, long tcp_ms, long proto_ms) {
    pthread_mutex_lock(&results_mtx);
    if (results_count == results_cap) {
        size_t newcap = results_cap ? results_cap * 2 : 256;
        result_t *tmp = realloc(results, newcap * sizeof(result_t));
        if (!tmp) { pthread_mutex_unlock(&results_mtx); return; }
        results = tmp;
        results_cap = newcap;
    }
    result_t *r = &results[results_count++];
    r->host = strdup(host);
    r->port = strdup(port);
    r->mode = mode;
    r->tcp_ms = tcp_ms;
    r->proto_ms = proto_ms;
    r->total_ms = tcp_ms + proto_ms;
    pthread_mutex_unlock(&results_mtx);
}

static void test_proxy(job_t *j) {
    struct timespec t_tcp_start, t_tcp_end, t_proto_end;
    int sock = -1;
    if (tcp_connect_to(j->proxy_host, j->proxy_port, timeout_ms, &sock, &t_tcp_start, &t_tcp_end) != 0) {
        return; // skip non-working
    }
    long tcp_ms = timespec_diff_ms(&t_tcp_end, &t_tcp_start);

    const char *t_host = "example.com";
    const char *t_port = (j->mode == MODE_HTTPS) ? "443" : "80";

    int proto_ok = 0;
    if (j->mode == MODE_HTTP || j->mode == MODE_HTTPS) {
        if (do_http_connect(sock, t_host, t_port, timeout_ms) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &t_proto_end);
            proto_ok = 1;
        } else { close(sock); return; }
    } else if (j->mode == MODE_SOCKS4) {
        if (do_socks4(sock, t_host, t_port, timeout_ms) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &t_proto_end);
            proto_ok = 1;
        } else { close(sock); return; }
    } else if (j->mode == MODE_SOCKS5) {
        if (do_socks5(sock, t_host, t_port, timeout_ms) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &t_proto_end);
            proto_ok = 1;
        } else { close(sock); return; }
    }

    long proto_ms = timespec_diff_ms(&t_proto_end, &t_tcp_end);
    if (proto_ok) add_result(j->proxy_host, j->proxy_port, j->mode, tcp_ms, proto_ms);
    close(sock);
}

static void *worker(void *arg) {
    (void)arg;
    job_t job;
    while (queue_pop(&q, &job)) {
        test_proxy(&job);
        free(job.proxy_host);
        free(job.proxy_port);
    }
    return NULL;
}

static int parse_line_to_job(char *line, job_t *out) {
    char *s = line;
    while (*s && (*s == ' ' || *s == '\t')) s++;
    char *e = s + strlen(s) - 1;
    while (e >= s && (*e == '\n' || *e == '\r' || *e == ' ' || *e == '\t')) { *e = '\0'; e--; }
    if (*s == '\0') return 0;

    proto_mode_t mode = MODE_HTTP;
    const char *p = s;
    if (strncmp(p, "socks5://", 9) == 0) { mode = MODE_SOCKS5; p += 9; }
    else if (strncmp(p, "socks4://", 9) == 0) { mode = MODE_SOCKS4; p += 9; }
    else if (strncmp(p, "http://", 7) == 0) { mode = MODE_HTTP; p += 7; }
    else if (strncmp(p, "https://", 8) == 0) { mode = MODE_HTTPS; p += 8; }

    char *colon = strrchr((char*)p, ':');
    if (!colon) return -1;
    *colon = '\0';
    char *host = strdup(p);
    char *port = strdup(colon + 1);
    if (!host || !port) { free(host); free(port); return -1; }

    out->proxy_host = host;
    out->proxy_port = port;
    out->mode = mode;
    return 1;
}

/* Compare function for sorting by total_ms ascending */
static int cmp_results(const void *a, const void *b) {
    const result_t *ra = a;
    const result_t *rb = b;
    if (ra->total_ms < rb->total_ms) return -1;
    if (ra->total_ms > rb->total_ms) return 1;
    return 0;
}

int main(void) {
    FILE *f = fopen("data.txt", "r");
    if (!f) { perror("fopen data.txt"); return 1; }

    queue_init(&q);

    pthread_t threads[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i) {
        if (pthread_create(&threads[i], NULL, worker, NULL) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    char line[LINE_MAX_LEN];
    while (fgets(line, sizeof(line), f)) {
        job_t job;
        int r = parse_line_to_job(line, &job);
        if (r == 1) queue_push(&q, job);
    }
    fclose(f);

    queue_close(&q);
    for (int i = 0; i < WORKER_COUNT; ++i) pthread_join(threads[i], NULL);

    pthread_mutex_lock(&results_mtx);
    if (results_count > 0) qsort(results, results_count, sizeof(result_t), cmp_results);
    printf("# Working proxies sorted by total latency (TCP+PROTO) ascending\n");
    for (size_t i = 0; i < results_count; ++i) {
        result_t *r = &results[i];
        const char *mode_s = (r->mode == MODE_SOCKS5) ? "socks5" :
                             (r->mode == MODE_SOCKS4) ? "socks4" :
                             (r->mode == MODE_HTTPS) ? "https" : "http";
        printf("%s:%s\tmode=%s\tTCP=%ldms\tPROTO=%ldms\tTOTAL=%ldms\n",
               r->host, r->port, mode_s, r->tcp_ms, r->proto_ms, r->total_ms);
        free(r->host);
        free(r->port);
    }
    free(results);
    pthread_mutex_unlock(&results_mtx);

    return 0;
}
