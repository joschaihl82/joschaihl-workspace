#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>

/* -------------------------------------------------------------------------
 * Konfiguration und Konstanten
 * ------------------------------------------------------------------------- */

#define MAX_PROXY_ENTRIES       1024        /* Maximale Anzahl der Proxies */
#define MAX_CONNECTIONS         512         /* Maximale gleichzeitige Verbindungen */
#define MAX_POLL_FDS            (MAX_CONNECTIONS * 2 + 1)
#define BUF_SIZE                16384       /* Puffergröße für Datentransfer */
#define MAX_IP_LEN              INET_ADDRSTRLEN
#define PROBE_HISTORY_SIZE      16          /* Vergrößerte Historie für Median-Latenz */
#define TOP_PROXY_COUNT         32          /* Anzahl der besten Proxies für Failover-Queue */
#define PROXY_FAILOVER_LIMIT    5           /* Max. Versuche pro Client-Anfrage */
#define G_MAX_LOG_MESSAGE_SIZE  512

#define MAX_DESIRED_LATENCY_MS  20          /* Maximale Latenz für die höchste Priorisierung */

/* SOCKS5 Response Codes (unverändert) */
#define SOCKS_VER_5              0x05
#define SOCKS_REP_SUCCESS        0x00
#define SOCKS_REP_FAILURE        0x01
#define SOCKS_REP_NET_UNREACH    0x03
#define SOCKS_REP_CONN_REFUSED   0x05
#define SOCKS_REP_CMD_UNSUPP     0x07
#define SOCKS_REP_ATYP_UNSUPP    0x08
#define SOCKS_AUTH_NO_ACCEPTABLE 0xFF

/* Connection States (unverändert) */
#define CONN_STATE_CLOSED        0
#define CONN_STATE_AUTH          1
#define CONN_STATE_REQUEST       2
#define CONN_STATE_CONNECTING    3
#define CONN_STATE_TUNNEL_INIT   4
#define CONN_STATE_SHUTTLE       5

/* Service Configuration - HIER SIND DIE ANPASSUNGEN */
static const uint16_t G_GATEWAY_PORT = 8060;
static const int G_PROBE_INTERVAL_S = 1;    /* <<< NEU: Probing-Intervall auf 1 Sekunde reduziert >>> */
static const int G_PROBE_TIMEOUT_MS = 10;   /* <<< NEU: Proxy-Speedtest Timeout auf 10ms reduziert >>> */
static const char *const PROXY_FILE_NAME = "socks5.txt";

/* -------------------------------------------------------------------------
 * Globale Datenstrukturen
 * ------------------------------------------------------------------------- */

typedef struct {
    char ip[MAX_IP_LEN];
    uint16_t port;
    int latency_history[PROBE_HISTORY_SIZE];
    int history_index;
    int avg_latency_ms;           /* Median-Latenz (Ping) */
    uint64_t weighted_score;      /* Ranking-Score (kleiner ist besser) */
    time_t last_success_ts;
    int total_probes;
    int consecutive_failures;
    int is_valid;
    int is_ready;
    long traffic_in;
    long traffic_out;
} proxy_entry_t;

typedef struct {
    int client_fd;
    int proxy_fd;
    int state;
    int current_proxy_idx;
    int attempted_proxies[PROXY_FAILOVER_LIMIT];
    int attempted_count;
    uint8_t socks_request_buffer[512];
    ssize_t socks_request_len;
    char client_ip[MAX_IP_LEN];
    long long start_time_ms;
    long long last_activity_ms;
} connection_t;

typedef struct {
    long long total_bytes_shuttle;
    long long total_connections_handled;
    long long total_failovers_performed;
    time_t start_time;
    int current_active_connections;
} global_stats_t;

static proxy_entry_t G_PROXIES[MAX_PROXY_ENTRIES];
static int G_VALID_PROXY_COUNT = 0;
static connection_t G_CONNECTIONS[MAX_CONNECTIONS];
static struct pollfd G_POLL_FDS[MAX_POLL_FDS];
static int G_POLL_COUNT = 0;
static int G_LISTEN_FD = -1;
static volatile sig_atomic_t G_RUN = 1;
static pthread_t G_RANKING_THREAD;
static pthread_mutex_t G_PROXIES_MUTEX;
static int G_TOP_PROXIES_INDICES[TOP_PROXY_COUNT];
static int G_TOP_PROXIES_COUNT = 0;
static global_stats_t G_STATS = {0};

/* -------------------------------------------------------------------------
 * Hilfsfunktionen für Logging und Zeit
 * ------------------------------------------------------------------------- */

static void app_log_message(const char *type, const char *format, ...) {
    char time_buf[64];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));

    char msg_buf[G_MAX_LOG_MESSAGE_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msg_buf, sizeof(msg_buf), format, args);
    va_end(args);

    fprintf(stderr, "[%s.%03ld] [%s] %s\n", time_buf, tv.tv_usec / 1000, type, msg_buf);
}
static void log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char log_buffer[G_MAX_LOG_MESSAGE_SIZE];
    vsnprintf(log_buffer, sizeof(log_buffer), format, args);
    va_end(args);
    app_log_message("INFO", "%s", log_buffer);
}
static void log_warn(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char log_buffer[G_MAX_LOG_MESSAGE_SIZE];
    vsnprintf(log_buffer, sizeof(log_buffer), format, args);
    va_end(args);
    app_log_message("WARN", "%s", log_buffer);
}
static void log_err(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char log_buffer[G_MAX_LOG_MESSAGE_SIZE];
    vsnprintf(log_buffer, sizeof(log_buffer), format, args);
    va_end(args);
    app_log_message("ERROR", "%s", log_buffer);
}
static long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000;
}

/* -------------------------------------------------------------------------
 * Socket- und I/O-Hilfsfunktionen
 * ------------------------------------------------------------------------- */

static int set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        log_err("fcntl(F_GETFL) Fehler: %s", strerror(errno));
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        log_err("fcntl(F_SETFL) Fehler: %s", strerror(errno));
        return -1;
    }
    return 0;
}
static void close_socket_safely(int *fd_ptr) {
    if (*fd_ptr >= 0) {
        if (close(*fd_ptr) == -1) {
            if (errno != EINTR) {
                log_warn("Fehler beim Schließen von FD %d: %s", *fd_ptr, strerror(errno));
            }
        }
        *fd_ptr = -1;
    }
}
static int get_socket_error(int fd) {
    int err = 0;
    socklen_t len = sizeof(err);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
        return errno;
    }
    return err;
}
static void remove_fd_from_poll(int fd_to_remove) {
    for (int i = 1; i < G_POLL_COUNT; ) {
        if (G_POLL_FDS[i].fd == fd_to_remove) {
            G_POLL_FDS[i] = G_POLL_FDS[G_POLL_COUNT - 1];
            G_POLL_COUNT--;
            break;
        } else {
            i++;
        }
    }
}
static void send_socks_reply(int client_fd, uint8_t rep, uint8_t atyp) {
    uint8_t reply[10] = { SOCKS_VER_5, rep, 0x00, atyp, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    if (send(client_fd, reply, 10, 0) != 10) {
        log_warn("Fehler beim Senden der SOCKS-Antwort (REP %d)", rep);
    }
}

/* -------------------------------------------------------------------------
 * Proxy-Ranking-Logik
 * ------------------------------------------------------------------------- */

static int compare_ints(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}
static int calculate_median_latency(int *history) {
    int valid_latencies[PROBE_HISTORY_SIZE];
    int count = 0;
    for (int i = 0; i < PROBE_HISTORY_SIZE; ++i) {
        if (history[i] > 0) {
            valid_latencies[count++] = history[i];
        }
    }
    if (count == 0) return 0;
    qsort(valid_latencies, count, sizeof(int), compare_ints);
    return valid_latencies[count / 2];
}

/**
 * NEUE SCORING-FUNKTION: Priorisiert Proxies mit Latenz <= 20ms.
 * Niedriger Score ist besser.
 */
static uint64_t calculate_proxy_score(const proxy_entry_t *p) {
    const uint64_t NOT_READY_SCORE = 0xFFFFFFFFUL; 

    if (!p->is_ready) {
        /* Tote Proxies bekommen die höchste Strafe */
        return NOT_READY_SCORE + (uint64_t)p->consecutive_failures * 10000;
    }
    
    uint64_t score = p->avg_latency_ms;
    
    if (p->avg_latency_ms > MAX_DESIRED_LATENCY_MS) {
        /* Hohe Strafe für Proxies, die langsamer als 20ms sind, um sie hinter alle schnellen zu verschieben. */
        score += 1000000; 
    }
    
    /* Sekundäre Tie-Breaker */
    score += (uint64_t)p->consecutive_failures * 50; 
    uint64_t total_traffic = (uint64_t)p->traffic_in + (uint64_t)p->traffic_out;
    score += total_traffic / (1024 * 1024); 
    
    return score;
}

static void update_proxy_status(proxy_entry_t *p, int new_latency_ms) {
    p->total_probes++;
    p->latency_history[p->history_index] = (new_latency_ms > 0) ? new_latency_ms : 0;
    p->history_index = (p->history_index + 1) % PROBE_HISTORY_SIZE;

    if (new_latency_ms > 0) {
        p->last_success_ts = time(NULL);
        p->consecutive_failures = 0;
    } else {
        p->consecutive_failures++;
    }

    p->avg_latency_ms = calculate_median_latency(p->latency_history);

    int zero_count = 0;
    for (int i = 0; i < PROBE_HISTORY_SIZE; i++) {
        if (p->latency_history[i] == 0) zero_count++;
    }

    /* Ein Proxy gilt als "Ready", wenn mindestens die Hälfte der letzten Probes erfolgreich war
       ODER die letzte Probe erfolgreich war. */
    if (new_latency_ms > 0 || (zero_count < PROBE_HISTORY_SIZE / 2)) {
        p->is_ready = 1;
    } else {
        p->is_ready = 0;
    }

    p->weighted_score = calculate_proxy_score(p);
}
static int compare_proxy_indices(const void *a, const void *b) {
    int idx_a = *(const int *)a;
    int idx_b = *(const int *)b;
    uint64_t score_a = G_PROXIES[idx_a].weighted_score;
    uint64_t score_b = G_PROXIES[idx_b].weighted_score;
    if (score_a < score_b) return -1;
    if (score_a > score_b) return 1;
    return 0;
}
static void update_top_proxies_queue() {
    int current_proxies_found = 0;
    int temp_all_indices[MAX_PROXY_ENTRIES];

    for (int i = 0; i < G_VALID_PROXY_COUNT; ++i) {
        if (G_PROXIES[i].is_valid) {
            temp_all_indices[current_proxies_found++] = i;
        }
    }
    
    if (current_proxies_found == 0) {
        G_TOP_PROXIES_COUNT = 0;
        return;
    }

    qsort(temp_all_indices, current_proxies_found, sizeof(int), compare_proxy_indices);

    G_TOP_PROXIES_COUNT = (current_proxies_found < TOP_PROXY_COUNT) ? current_proxies_found : TOP_PROXY_COUNT;

    for (int i = 0; i < G_TOP_PROXIES_COUNT; ++i) {
        G_TOP_PROXIES_INDICES[i] = temp_all_indices[i];
    }
}
static int init_probe_socket(const char *ip, uint16_t port, struct sockaddr_in *addr_out) {
    int probe_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (probe_fd < 0) return -1;
    if (set_non_blocking(probe_fd) < 0) {
        close_socket_safely(&probe_fd);
        return -1;
    }
    memset(addr_out, 0, sizeof(struct sockaddr_in));
    addr_out->sin_family = AF_INET;
    addr_out->sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr_out->sin_addr) <= 0) {
        close_socket_safely(&probe_fd);
        return -1;
    }
    return probe_fd;
}

/**
 * Führt den Proxy-Ping mit dem NEUEN 10ms Timeout aus.
 */
static int probe_proxy_latency(const char *ip, uint16_t port) {
    struct sockaddr_in addr;
    int probe_fd = init_probe_socket(ip, port, &addr);
    if (probe_fd < 0) return 0;
    long long start_time = get_time_ms();

    if (connect(probe_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == 0) {
        close_socket_safely(&probe_fd);
        return (int)(get_time_ms() - start_time);
    }
    
    if (errno != EINPROGRESS) {
        close_socket_safely(&probe_fd);
        return 0;
    }

    struct pollfd pfd = { .fd = probe_fd, .events = POLLOUT };
    int ret = poll(&pfd, 1, G_PROBE_TIMEOUT_MS); /* <<< Verwendung des 10ms Timeout >>> */
    int latency = 0;

    if (ret > 0) {
        if (pfd.revents & POLLOUT) {
            if (get_socket_error(probe_fd) == 0) {
                latency = (int)(get_time_ms() - start_time);
                latency = (latency == 0) ? 1 : latency;
            }
        }
    }
    close_socket_safely(&probe_fd);
    return latency;
}
static void *ranking_thread_func(void *arg) {
    log_info("Ranking-Thread gestartet. Timeout: %dms. Intervall: %ds. Niedrig-Latenz-Grenze: %dms.", G_PROBE_TIMEOUT_MS, G_PROBE_INTERVAL_S, MAX_DESIRED_LATENCY_MS);
    while (G_RUN) {
        long long start_time = get_time_ms();
        pthread_mutex_lock(&G_PROXIES_MUTEX);
        
        for (int i = 0; i < G_VALID_PROXY_COUNT; ++i) {
            proxy_entry_t *p = &G_PROXIES[i];
            if (!p->is_valid) continue;
            int latency = probe_proxy_latency(p->ip, p->port);
            update_proxy_status(p, latency);
        }
        update_top_proxies_queue();
        
        pthread_mutex_unlock(&G_PROXIES_MUTEX);

        long long end_time = get_time_ms();
        long long elapsed_ms = end_time - start_time;
        long long sleep_ms = (long long)G_PROBE_INTERVAL_S * 1000 - elapsed_ms; /* <<< Verwendung des 1s Intervalls >>> */

        if (sleep_ms > 0) {
            struct timespec ts;
            ts.tv_sec = sleep_ms / 1000;
            ts.tv_nsec = (sleep_ms % 1000) * 1000000;
            nanosleep(&ts, NULL);
        }
    }
    log_info("Ranking-Thread beendet.");
    return NULL;
}

/* -------------------------------------------------------------------------
 * Verbindungs- und Failover-Management (unverändert)
 * ------------------------------------------------------------------------- */

static int find_conn_by_fd(int fd) {
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        if (G_CONNECTIONS[i].client_fd == fd || G_CONNECTIONS[i].proxy_fd == fd) return i;
    }
    return -1;
}
static void cleanup_connection_fds(connection_t *conn) {
    if (conn->client_fd >= 0) {
        remove_fd_from_poll(conn->client_fd);
        close_socket_safely(&conn->client_fd);
    }
    if (conn->proxy_fd >= 0) {
        remove_fd_from_poll(conn->proxy_fd);
        close_socket_safely(&conn->proxy_fd);
    }
}
static void close_connection(int conn_idx, const char *reason) {
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    cleanup_connection_fds(conn);
    long long duration = get_time_ms() - conn->start_time_ms;
    log_info("[%s] Verbindung geschlossen. Dauer: %lldms. Grund: %s", conn->client_ip, duration, reason);
    memset(conn, 0, sizeof(connection_t));
    conn->state = CONN_STATE_CLOSED;
    G_STATS.current_active_connections--;
}
static int select_next_best_proxy(connection_t *conn) {
    pthread_mutex_lock(&G_PROXIES_MUTEX);
    for (int i = 0; i < G_TOP_PROXIES_COUNT; ++i) {
        int proxy_idx = G_TOP_PROXIES_INDICES[i];
        
        if (!G_PROXIES[proxy_idx].is_ready && conn->attempted_count < PROXY_FAILOVER_LIMIT) {
             continue;
        }

        int already_tried = 0;
        for (int j = 0; j < conn->attempted_count; ++j) {
            if (conn->attempted_proxies[j] == proxy_idx) {
                already_tried = 1;
                break;
            }
        }
        
        if (!already_tried) {
            if (conn->attempted_count < PROXY_FAILOVER_LIMIT) {
                 conn->attempted_proxies[conn->attempted_count++] = proxy_idx;
            } else {
                 pthread_mutex_unlock(&G_PROXIES_MUTEX);
                 return -1;
            }
            pthread_mutex_unlock(&G_PROXIES_MUTEX);
            return proxy_idx;
        }
    }
    pthread_mutex_unlock(&G_PROXIES_MUTEX);
    return -1;
}
static int connect_to_upstream_proxy(int conn_idx) {
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    if (conn->current_proxy_idx < 0 || conn->current_proxy_idx >= G_VALID_PROXY_COUNT) return 0;
    proxy_entry_t *proxy = &G_PROXIES[conn->current_proxy_idx];

    conn->proxy_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->proxy_fd < 0) {
        log_err("[%s] Konnte Proxy-Socket nicht erstellen: %s", conn->client_ip, strerror(errno));
        return 0;
    }

    if (set_non_blocking(conn->proxy_fd) < 0) {
        close_socket_safely(&conn->proxy_fd);
        return 0;
    }

    struct sockaddr_in proxy_addr;
    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(proxy->port);
    if (inet_pton(AF_INET, proxy->ip, &proxy_addr.sin_addr) <= 0) {
        log_err("[%s] Ungültige Proxy-IP: %s", conn->client_ip, proxy->ip);
        close_socket_safely(&conn->proxy_fd);
        return 0;
    }

    log_info("[%s] Versuche Verbindung zu Proxy #%d: %s:%hu (Score %llu, Versuch %d/%d)",
             conn->client_ip, conn->current_proxy_idx, proxy->ip, proxy->port,
             (unsigned long long)proxy->weighted_score, conn->attempted_count, PROXY_FAILOVER_LIMIT);

    if (connect(conn->proxy_fd, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0) {
        if (errno != EINPROGRESS) {
            log_err("[%s] Connect zu Proxy %s:%hu fehlgeschlagen: %s", conn->client_ip, proxy->ip, proxy->port, strerror(errno));
            close_socket_safely(&conn->proxy_fd);
            return 0;
        }
    }
    
    G_POLL_FDS[G_POLL_COUNT].fd = conn->proxy_fd;
    G_POLL_FDS[G_POLL_COUNT].events = POLLOUT;
    G_POLL_COUNT++;

    conn->state = CONN_STATE_CONNECTING;
    return 1;
}
static int attempt_failover(int conn_idx) {
    connection_t *conn = &G_CONNECTIONS[conn_idx];

    if (conn->proxy_fd > 0) {
        remove_fd_from_poll(conn->proxy_fd);
        close_socket_safely(&conn->proxy_fd);
        conn->proxy_fd = -1;
    }
    
    G_STATS.total_failovers_performed++;

    int next_proxy_idx = select_next_best_proxy(conn);
    if (next_proxy_idx < 0) {
        log_err("[%s] Alle %d besten Proxies fehlgeschlagen/erschöpft. Endgültige Schließung.", conn->client_ip, PROXY_FAILOVER_LIMIT);
        send_socks_reply(conn->client_fd, SOCKS_REP_NET_UNREACH, 0x01);
        return 0;
    }
    conn->current_proxy_idx = next_proxy_idx;

    if (!connect_to_upstream_proxy(conn_idx)) {
        log_err("[%s] Fehler beim Aufbau der neuen Proxy-Verbindung. Erneutes Failover.", conn->client_ip);
        return attempt_failover(conn_idx);
    }
    
    conn->state = CONN_STATE_CONNECTING;
    return 1;
}

/* -------------------------------------------------------------------------
 * SOCKS5-Handshake und Tunnel-Logik (unverändert)
 * ------------------------------------------------------------------------- */

static int handle_socks5_auth(int conn_idx, const uint8_t *buffer, ssize_t len) {
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    if (len < 3 || buffer[0] != SOCKS_VER_5) {
        send_socks_reply(conn->client_fd, SOCKS_REP_FAILURE, 0x01);
        return 0;
    }
    uint8_t nmethods = buffer[1];
    if (len < (ssize_t)(2 + nmethods)) {
        send_socks_reply(conn->client_fd, SOCKS_REP_FAILURE, 0x01);
        return 0;
    }
    int auth_ok = 0;
    for (int i = 0; i < nmethods; i++) {
        if (buffer[2 + i] == 0x00) {
            auth_ok = 1;
            break;
        }
    }
    if (!auth_ok) {
        uint8_t reply[2] = { SOCKS_VER_5, SOCKS_AUTH_NO_ACCEPTABLE };
        send(conn->client_fd, reply, 2, 0);
        return 0;
    }
    uint8_t reply[2] = { SOCKS_VER_5, 0x00 };
    if (send(conn->client_fd, reply, 2, 0) != 2) {
        return 0;
    }
    conn->state = CONN_STATE_REQUEST;
    return 1;
}
static int parse_and_store_socks5_request(int conn_idx, const uint8_t *buffer, ssize_t len) {
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    if (len < 7 || buffer[0] != SOCKS_VER_5 || buffer[1] != 0x01 || buffer[2] != 0x00) {
        send_socks_reply(conn->client_fd, SOCKS_REP_CMD_UNSUPP, 0x01);
        return 0;
    }
    uint8_t atyp = buffer[3];
    size_t min_len = 0;
    if (atyp == 0x01) min_len = 4 + 4 + 2;
    else if (atyp == 0x03) min_len = 4 + 1 + buffer[4] + 2;
    else {
        send_socks_reply(conn->client_fd, SOCKS_REP_ATYP_UNSUPP, 0x01);
        return 0;
    }
    if (len < (ssize_t)min_len) {
        send_socks_reply(conn->client_fd, SOCKS_REP_FAILURE, atyp);
        return 0;
    }
    if (len > sizeof(conn->socks_request_buffer)) {
         send_socks_reply(conn->client_fd, SOCKS_REP_FAILURE, 0x01);
         return 0;
    }
    
    memcpy(conn->socks_request_buffer, buffer, len);
    conn->socks_request_len = len;
    return 1;
}
static int forward_socks5_request(int conn_idx) {
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    if (conn->proxy_fd < 0 || conn->socks_request_len <= 0) return 0;
    
    ssize_t sent_len = send(conn->proxy_fd, conn->socks_request_buffer, conn->socks_request_len, 0);
    
    if (sent_len != conn->socks_request_len) {
        log_err("[%s] Fehler beim Senden der SOCKS-Anfrage an Proxy.", conn->client_ip);
        return 0;
    }
    conn->state = CONN_STATE_TUNNEL_INIT;
    return 1;
}
static int handle_proxy_socks5_response(int conn_idx) {
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    uint8_t buffer[BUF_SIZE];
    ssize_t len = recv(conn->proxy_fd, buffer, sizeof(buffer), 0);
    
    if (len <= 0) {
        return attempt_failover(conn_idx);
    }
    if (len < 10 || buffer[0] != SOCKS_VER_5) {
        return attempt_failover(conn_idx);
    }
    if (buffer[1] != SOCKS_REP_SUCCESS) {
        log_err("[%s] Proxy meldet Fehler 0x%02x. Versuche Failover.", conn->client_ip, buffer[1]);
        return attempt_failover(conn_idx);
    }

    ssize_t sent_len = send(conn->client_fd, buffer, len, 0);
    if (sent_len != len) {
        return 0;
    }

    conn->state = CONN_STATE_SHUTTLE;
    conn->last_activity_ms = get_time_ms();

    for (int i = 1; i < G_POLL_COUNT; ++i) {
        if (G_POLL_FDS[i].fd == conn->client_fd || G_POLL_FDS[i].fd == conn->proxy_fd) {
            G_POLL_FDS[i].events = POLLIN;
        }
    }
    log_info("[%s] Tunnel erfolgreich über Proxy #%d aufgebaut. Starte Shuttle.",
             conn->client_ip, conn->current_proxy_idx);
    return 1;
}
static ssize_t shuttle_data(int conn_idx, int from_fd, int to_fd) {
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    char buffer[BUF_SIZE];
    ssize_t recv_len = recv(from_fd, buffer, BUF_SIZE, 0);

    if (recv_len <= 0) {
        if (recv_len == 0) return 0;
        if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) return 1;
        return -1;
    }

    ssize_t sent_len = send(to_fd, buffer, recv_len, 0);
    if (sent_len != recv_len) {
        return -1;
    }

    conn->last_activity_ms = get_time_ms();
    G_STATS.total_bytes_shuttle += sent_len;

    if (from_fd == conn->client_fd) {
        G_PROXIES[conn->current_proxy_idx].traffic_out += sent_len;
    } else {
        G_PROXIES[conn->current_proxy_idx].traffic_in += sent_len;
    }
    return sent_len;
}
static int handle_data_shuttle(int conn_idx, int fd) {
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    ssize_t transfer_result;

    if (fd == conn->client_fd) {
        transfer_result = shuttle_data(conn_idx, conn->client_fd, conn->proxy_fd);
        if (transfer_result <= 0) {
            return 0;
        }
    } else {
        transfer_result = shuttle_data(conn_idx, conn->proxy_fd, conn->client_fd);
        if (transfer_result <= 0) {
            log_warn("[%s] Proxy-Socket geschlossen/Fehler während Datentransfer. Führe Failover durch.", conn->client_ip);
            return attempt_failover(conn_idx);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------
 * Haupt-Event-Handler (unverändert)
 * ------------------------------------------------------------------------- */

static int handle_read_event(int fd_idx) {
    int fd = G_POLL_FDS[fd_idx].fd;
    int conn_idx = find_conn_by_fd(fd);
    if (conn_idx < 0) return 0;
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    int is_client = (fd == conn->client_fd);

    uint8_t buffer[BUF_SIZE];
    ssize_t len;

    switch (conn->state) {
        case CONN_STATE_CLOSED:
        case CONN_STATE_AUTH:
            len = recv(conn->client_fd, buffer, sizeof(buffer), 0);
            if (len <= 0) return 0;
            return handle_socks5_auth(conn_idx, buffer, len);

        case CONN_STATE_REQUEST:
            len = recv(conn->client_fd, buffer, sizeof(buffer), 0);
            if (len <= 0) return 0;
            if (!parse_and_store_socks5_request(conn_idx, buffer, len)) return 0;
            
            conn->attempted_count = 0;
            if (!attempt_failover(conn_idx)) return 0;
            
            G_POLL_FDS[fd_idx].events = 0;
            return 1;

        case CONN_STATE_TUNNEL_INIT:
            if (!is_client) {
                return handle_proxy_socks5_response(conn_idx);
            }
            log_warn("[%s] Unerwarteter Datenfluss vom Client während Tunnel-Init.", conn->client_ip);
            return 0;

        case CONN_STATE_SHUTTLE:
            return handle_data_shuttle(conn_idx, fd);

        default:
            return 0;
    }
}
static int handle_write_event(int fd_idx) {
    int fd = G_POLL_FDS[fd_idx].fd;
    int conn_idx = find_conn_by_fd(fd);
    if (conn_idx < 0) return 0;
    connection_t *conn = &G_CONNECTIONS[conn_idx];
    
    if (fd == conn->proxy_fd && conn->state == CONN_STATE_CONNECTING) {
        int err = get_socket_error(fd);
        if (err != 0) {
            pthread_mutex_lock(&G_PROXIES_MUTEX);
            update_proxy_status(&G_PROXIES[conn->current_proxy_idx], 0);
            update_top_proxies_queue();
            pthread_mutex_unlock(&G_PROXIES_MUTEX);

            return attempt_failover(conn_idx);
        }
        
        G_POLL_FDS[fd_idx].events = POLLIN;
        if (!forward_socks5_request(conn_idx)) {
            return attempt_failover(conn_idx);
        }
        return 1;
    }
    return 1;
}
static int accept_new_client() {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(G_LISTEN_FD, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) log_err("Fehler bei accept(): %s", strerror(errno));
        return 0;
    }

    int conn_idx = -1;
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        if (G_CONNECTIONS[i].state == CONN_STATE_CLOSED) {
            conn_idx = i;
            break;
        }
    }

    if (conn_idx < 0) {
        send_socks_reply(client_fd, SOCKS_REP_FAILURE, 0x01);
        close(client_fd);
        log_warn("Maximale Verbindungsanzahl (%d) erreicht.", MAX_CONNECTIONS);
        return 0;
    }

    connection_t *conn = &G_CONNECTIONS[conn_idx];
    memset(conn, 0, sizeof(connection_t));

    conn->client_fd = client_fd;
    conn->proxy_fd = -1;
    conn->state = CONN_STATE_AUTH;
    conn->attempted_count = 0;
    conn->start_time_ms = get_time_ms();
    conn->last_activity_ms = conn->start_time_ms;
    inet_ntop(AF_INET, &client_addr.sin_addr, conn->client_ip, MAX_IP_LEN);

    if (set_non_blocking(client_fd) < 0) {
        close(client_fd);
        conn->state = CONN_STATE_CLOSED;
        return 0;
    }
    
    if (G_POLL_COUNT >= MAX_POLL_FDS) {
        log_err("Poll-Array ist voll. Schließe neue Verbindung.");
        close(client_fd);
        return 0;
    }
    G_POLL_FDS[G_POLL_COUNT].fd = client_fd;
    G_POLL_FDS[G_POLL_COUNT].events = POLLIN;
    G_POLL_COUNT++;
    G_STATS.total_connections_handled++;
    G_STATS.current_active_connections++;
    
    return 1;
}
static void process_poll_results(int poll_ret) {
    if (poll_ret <= 0) return;

    for (int i = 1; i < G_POLL_COUNT && poll_ret > 0; ) {
        if (G_POLL_FDS[i].revents) {
            poll_ret--;
            int fd = G_POLL_FDS[i].fd;
            int conn_idx = find_conn_by_fd(fd);
            int connection_active = 1;
            int is_client_fd = (conn_idx >= 0 && fd == G_CONNECTIONS[conn_idx].client_fd);

            if (G_POLL_FDS[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                if (conn_idx >= 0) {
                    if (is_client_fd) {
                        connection_active = 0;
                    } else {
                        connection_active = attempt_failover(conn_idx);
                    }
                } else {
                    connection_active = 0;
                }
            }

            if (connection_active && (G_POLL_FDS[i].revents & POLLOUT)) {
                if (conn_idx >= 0) {
                    connection_active = handle_write_event(i);
                }
            }

            if (connection_active && (G_POLL_FDS[i].revents & POLLIN)) {
                if (conn_idx >= 0) {
                    connection_active = handle_read_event(i);
                }
            }

            if (!connection_active && conn_idx >= 0) {
                close_connection(conn_idx, "Verbindungsfehler/EOF/Failover-Limit erreicht");
            }
            
            if (connection_active || conn_idx < 0) {
                i++;
            }
            
        } else {
            i++;
        }
    }
}

/* -------------------------------------------------------------------------
 * Hauptfunktionen
 * ------------------------------------------------------------------------- */

static int init_listen_socket() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        log_err("Konnte Listen-Socket nicht erstellen: %s", strerror(errno));
        return -1;
    }
    int opt_val = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)) < 0) {
        log_err("setsockopt(SO_REUSEADDR) fehlgeschlagen: %s", strerror(errno));
        close(listen_fd);
        return -1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(G_GATEWAY_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        log_err("Bind fehlgeschlagen auf Port %d: %s", G_GATEWAY_PORT, strerror(errno));
        close(listen_fd);
        return -1;
    }
    if (listen(listen_fd, 512) < 0) {
        log_err("Listen fehlgeschlagen: %s", strerror(errno));
        close(listen_fd);
        return -1;
    }
    log_info("Gateway lauscht auf Port %d", G_GATEWAY_PORT);
    return listen_fd;
}
static int load_proxies_from_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        log_err("Konnte Datei %s nicht öffnen: %s", filename, strerror(errno));
        G_VALID_PROXY_COUNT = 0;
        return 0;
    }
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), fp) && count < MAX_PROXY_ENTRIES) {
        char *newline = strchr(line, '\n');
        if (newline) *newline = '\0';
        char *port_str = strchr(line, ':');
        if (port_str) {
            *port_str = '\0';
            port_str++;
            proxy_entry_t *p = &G_PROXIES[count];
            strncpy(p->ip, line, MAX_IP_LEN - 1);
            p->ip[MAX_IP_LEN - 1] = '\0';
            p->port = (uint16_t)atoi(port_str);
            if (p->port > 0 && inet_pton(AF_INET, p->ip, &((struct in_addr){0})) == 1) {
                memset(p->latency_history, 0, sizeof(p->latency_history));
                p->history_index = 0;
                p->avg_latency_ms = 9999;
                p->weighted_score = 0xFFFFFFFFUL;
                p->last_success_ts = 0;
                p->total_probes = 0;
                p->consecutive_failures = 0;
                p->is_ready = 0;
                p->is_valid = 1;
                p->traffic_in = 0;
                p->traffic_out = 0;
                count++;
            } else {
                log_warn("Ungültige Zeile übersprungen: %s", line);
            }
        }
    }
    fclose(fp);
    G_VALID_PROXY_COUNT = count;
    log_info("%d Proxies erfolgreich aus %s geladen.", count, filename);
    return count;
}
static void print_proxy_stats() {
    log_info("----------------------------------------------------------------------------------------------------------------------------------------------------------------");
    log_info("Proxy Load Balancer Status Report (Uptime: %lds | Connections: %d | Total Traffic: %.2f MB | Failovers: %lld | Low-Latency-Target: <%dms | Probe Timeout: %dms)",
             time(NULL) - G_STATS.start_time, 
             G_STATS.current_active_connections, 
             (double)G_STATS.total_bytes_shuttle / (1024.0 * 1024.0),
             G_STATS.total_failovers_performed,
             MAX_DESIRED_LATENCY_MS,
             G_PROBE_TIMEOUT_MS);
    log_info("Top %d Proxies (von %d Geladenen):", G_TOP_PROXIES_COUNT, G_VALID_PROXY_COUNT);
    log_info("----------------------------------------------------------------------------------------------------------------------------------------------------------------");
    log_info("%-5s | %-21s | %-12s | %-8s | %-12s | %-12s | %-12s | %-12s", 
             "Rank", "IP:Port", "Status", "Median(ms)", "Score", "Traffic IN(KB)", "Traffic OUT(KB)", "Failures");
    log_info("----------------------------------------------------------------------------------------------------------------------------------------------------------------");
    
    for (int i = 0; i < G_TOP_PROXIES_COUNT; ++i) {
        int idx = G_TOP_PROXIES_INDICES[i];
        proxy_entry_t *p = &G_PROXIES[idx];
        const char *status = p->is_ready ? (p->avg_latency_ms <= MAX_DESIRED_LATENCY_MS ? "FAST" : "READY") : "DOWN";
        log_info("%-5d | %-15s:%-5hu | %-12s | %-8d | %-12llu | %-12.2f | %-12.2f | %-8d", 
                 i + 1, 
                 p->ip, p->port, 
                 status, 
                 p->avg_latency_ms, 
                 (unsigned long long)p->weighted_score, 
                 (double)p->traffic_in / 1024.0, 
                 (double)p->traffic_out / 1024.0,
                 p->consecutive_failures);
    }
    log_info("----------------------------------------------------------------------------------------------------------------------------------------------------------------");
}
static void run_main_loop() {
    int poll_timeout_ms = 1000;
    static time_t last_report_time = 0;
    
    if (G_VALID_PROXY_COUNT == 0) poll_timeout_ms = 10000;

    int poll_ret = poll(G_POLL_FDS, G_POLL_COUNT, poll_timeout_ms);

    if (poll_ret < 0) {
        if (errno == EINTR) return;
        log_err("Fehler im poll(): %s", strerror(errno));
        G_RUN = 0;
        return;
    }

    if (poll_ret > 0) {
        if (G_POLL_FDS[0].revents & POLLIN) {
            accept_new_client();
            poll_ret--;
        }
        process_poll_results(poll_ret);
    }
    
    if (time(NULL) - last_report_time >= 30) {
        print_proxy_stats();
        last_report_time = time(NULL);
    }
}
static void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        G_RUN = 0;
        log_info("Shutdown-Signal empfangen. Beende Programm...");
    }
}
static void initialize_global_state() {
    for (int i = 0; i < MAX_PROXY_ENTRIES; ++i) {
        memset(&G_PROXIES[i], 0, sizeof(proxy_entry_t));
    }
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        memset(&G_CONNECTIONS[i], 0, sizeof(connection_t));
        G_CONNECTIONS[i].state = CONN_STATE_CLOSED;
        G_CONNECTIONS[i].client_fd = -1;
        G_CONNECTIONS[i].proxy_fd = -1;
    }
    memset(G_TOP_PROXIES_INDICES, 0, sizeof(G_TOP_PROXIES_INDICES));
    G_POLL_COUNT = 0;
    G_LISTEN_FD = -1;
    G_TOP_PROXIES_COUNT = 0;
    memset(&G_STATS, 0, sizeof(G_STATS));
    G_STATS.start_time = time(NULL);
    G_RUN = 1;
}
static void cleanup_and_exit() {
    log_info("Führe Aufräumarbeiten durch...");
    G_RUN = 0;

    if (G_RANKING_THREAD) {
        pthread_cancel(G_RANKING_THREAD);
        pthread_join(G_RANKING_THREAD, NULL);
    }
    
    for(int i = 0; i < MAX_CONNECTIONS; ++i) {
        if (G_CONNECTIONS[i].state != CONN_STATE_CLOSED) {
            close_connection(i, "Global Shutdown");
        }
    }
    
    close_socket_safely(&G_LISTEN_FD);
    pthread_mutex_destroy(&G_PROXIES_MUTEX);
    log_info("Proxy Load Balancer erfolgreich beendet. Gesamt-Traffic: %.2f MB.", (double)G_STATS.total_bytes_shuttle / (1024.0 * 1024.0));
}

/* -------------------------------------------------------------------------
 * Main-Funktion
 * ------------------------------------------------------------------------- */

int main() {
    
    initialize_global_state();
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    if (pthread_mutex_init(&G_PROXIES_MUTEX, NULL) != 0) {
        log_err("Mutex-Initialisierung fehlgeschlagen.");
        return 1;
    }
    
    if (load_proxies_from_file(PROXY_FILE_NAME) == 0) {
        log_err("Keine gültigen Proxies geladen. Überprüfen Sie die Datei %s.", PROXY_FILE_NAME);
    }
    
    G_LISTEN_FD = init_listen_socket();
    if (G_LISTEN_FD < 0) {
        pthread_mutex_destroy(&G_PROXIES_MUTEX);
        return 1;
    }
    if (set_non_blocking(G_LISTEN_FD) < 0) {
        close(G_LISTEN_FD);
        pthread_mutex_destroy(&G_PROXIES_MUTEX);
        return 1;
    }
    
    G_POLL_FDS[0].fd = G_LISTEN_FD;
    G_POLL_FDS[0].events = POLLIN;
    G_POLL_COUNT = 1;

    if (pthread_create(&G_RANKING_THREAD, NULL, ranking_thread_func, NULL) != 0) {
        log_err("Konnte Ranking-Thread nicht starten.");
        close(G_LISTEN_FD);
        pthread_mutex_destroy(&G_PROXIES_MUTEX);
        return 1;
    }

    log_info("Starte Haupt-Event-Loop. Warte auf Client-Verbindungen...");
    while (G_RUN) {
        run_main_loop();
    }

    cleanup_and_exit();
    return 0;
}
