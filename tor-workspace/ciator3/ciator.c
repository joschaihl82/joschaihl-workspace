// ciator_fixed.c - Hardcoded single-file proxy with mock directory, circuit handshake simulation,
// and SOCKS5 handling. This file fixes undefined ErrCode, missing E_* macros, and TTunnel type conflicts,
// and removes non-C tokens. No file I/O; only sockets and in-memory.
//
// Build: gcc -O2 -Wall -o ciator ciator_fixed.c
// Run:   ./ciator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// --- [ DEFINITIONS & CONSTANTS ] -------------------------------------------

#define PROXY_BUF_SIZE 512
#define MAX_HOST_LEN 256
#define MAX_RELAYS 32
#define MAX_RETRY_COUNT 5

// Simulated Directory Server Details (Used to fetch relay IPs)
#define DIR_HOST "dir.tor-mock-net.org"
#define DIR_PORT 80
#define DIR_PATH "/tor/consensus/latest"

// Tor Protocol Constants
#define CELL_LEN 512
#define CELL_HEADER_LEN 5
#define PAYLOAD_MAX_LEN 256

// SOCKS Protocol Constants
#define SOCKS_VER 0x05
#define SOCKS_REP_SUCCESS 0x00
#define SOCKS_ATYP_DOMAIN 0x03
#define SOCKS_ATYP_IPV4   0x01

// Error Codes
typedef int ErrCode;
#define E_SUCCESS      0
#define E_CONN_FAIL    1
#define E_HTTP_FAIL    2
#define E_DIR_PARSE    3
#define E_SOCKS_PROTO  4
#define E_TOR_PROTO    5
#define E_RETRY_LIMIT  6
#define E_NO_RELAYS    7

typedef struct sockaddr_in SockAddr;

// --- [ LOGGING ] ------------------------------------------------------------

static void log_ts(FILE* f) {
  struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts);
  struct tm tm; localtime_r(&ts.tv_sec, &tm);
  char buf[64];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
  fprintf(f, "%s.%03ld ", buf, ts.tv_nsec / 1000000);
}

static void log_msg(const char* level, const char* fmt, ...) {
  FILE* dest = (strcmp(level, "ERR")==0) ? stderr : stdout;
  log_ts(dest);
  fprintf(dest, "[%s] ", level);
  va_list ap; va_start(ap, fmt);
  vfprintf(dest, fmt, ap);
  va_end(ap);
}

#define LOGI(...) log_msg("INFO", __VA_ARGS__)
#define LOGE(...) log_msg("ERR",  __VA_ARGS__)

// --- [ DATA STRUCTURES ] ---------------------------------------------------

// 1. Relay Node Structure
typedef struct {
  char ip[16];
  uint16_t orPort; // Onion Router Port (usually 443)
  uint16_t dirPort; // Directory Port
} RelayNode;

// 2. Directory Listing
typedef struct {
  RelayNode nodes[MAX_RELAYS];
  int count;
} DirectoryList;

// 3. Connection Handler (Simplified, blocking connection)
typedef struct {
  int fd;
  SockAddr peer;
  unsigned char buffer[PROXY_BUF_SIZE];
  char host[MAX_HOST_LEN];
  uint16_t port;
} Conn;

// Forward Declaration for Tor Tunnel
typedef struct TTunnel_s TTunnel;
typedef struct SConn_s SConn;
typedef struct TStream_s TStream;

// 4. Tor Circuit (Simplified)
typedef struct {
  TTunnel *tunnel;
  Conn nodeConn;
  uint16_t circId;
  // STUB: Cryptographic state would go here (DH keys, digest, etc.)
} Circuit;

// 5. Tor Tunnel (Manages Circuit creation and retries)
struct TTunnel_s {
  DirectoryList *dir;
  Circuit *circ;
  int retryCount;
  RelayNode currentNode;
  uint16_t currentCircId;
};

// 6. Tor Stream (Simulates end-to-end data flow)
struct TStream_s {
    TTunnel *tunnel;
    uint16_t strmId;
    // STUB: Buffers, I/O handlers
};

// 7. SOCKS Connection (Client connection)
struct SConn_s {
    int fd;
    char destHost[MAX_HOST_LEN];
    uint16_t destPort;
    unsigned char readBuf[PROXY_BUF_SIZE];
};

// --- [ UTIL FUNCTIONS ] ----------------------------------------------------

void util_i16_to_be(unsigned char* buf, uint16_t val) {
  buf[0] = (val >> 8) & 0xFF;
  buf[1] = val & 0xFF;
}

uint16_t util_i16_from_be(const unsigned char* buf) {
  return (uint16_t)((buf[0] << 8) | buf[1]);
}

uint16_t util_rand_id() {
  return (uint16_t)(1 + (rand() % 65534));
}

void util_exponential_backoff_delay(int attempt) {
  if (attempt <= 1) return; // No delay on first retry (attempt 2)
  long delay_sec = 1 << (attempt - 2); // Delays of 1, 2, 4, 8 seconds
  LOGI("[BACKOFF] Retrying in %ld seconds (Attempt %d/%d)...\n",
       delay_sec, attempt, MAX_RETRY_COUNT);
  sleep((unsigned int)delay_sec);
}

// --- [ SOCKET HELPERS ] ----------------------------------------------------

int sock_connect(const char* ip, uint16_t port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror("[SOCKET] socket");
    return -1;
  }

  SockAddr addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
    struct hostent *he = gethostbyname(ip);
    if (he == NULL) {
        fprintf(stderr, "[SOCKET] DNS lookup failed for %s\n", ip);
        close(fd);
        return -1;
    }
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
  }

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    // Expected on failure; return -1 and let caller retry
    close(fd);
    return -1;
  }

  return fd;
}

ErrCode sock_read_exact(int fd, unsigned char* buf, size_t len) {
  size_t bytes_read = 0;
  while (bytes_read < len) {
    ssize_t n = recv(fd, buf + bytes_read, len - bytes_read, 0);
    if (n <= 0) {
      if (n == 0) LOGE("[SOCKET] Connection closed unexpectedly.\n");
      else perror("[SOCKET] Read error");
      return E_CONN_FAIL;
    }
    bytes_read += (size_t)n;
  }
  return E_SUCCESS;
}

// --- [ DIRECTORY FETCHING ] --------------------------------------------------

ErrCode directory_parse(const char* responseBody, DirectoryList* dir) {
    if (!responseBody || dir->count > 0) return E_DIR_PARSE;

    LOGI("[PARSER] Analyzing simulated directory consensus...\n");
    int added = 0;

    // Hardcoded mock relays (RFC 5737 TEST-NET-1)
    const char* mock_ips[] = {
        "192.0.2.101",
        "192.0.2.102",
        "192.0.2.103",
        "192.0.2.104",
        "192.0.2.105"
    };

    for (int i = 0; i < (int)(sizeof(mock_ips)/sizeof(mock_ips[0])) && added < MAX_RELAYS; i++) {
        RelayNode *node = &dir->nodes[added];
        strncpy(node->ip, mock_ips[i], sizeof(node->ip)-1);
        node->ip[sizeof(node->ip)-1] = '\0';
        node->orPort = 443;
        node->dirPort = 80;
        dir->count++;
        added++;
        LOGI("[PARSER] Found relay: %s:%u\n", node->ip, node->orPort);
    }

    if (dir->count == 0) return E_NO_RELAYS;
    LOGI("[PARSER] Successfully loaded %d mock relays.\n", dir->count);
    return E_SUCCESS;
}

ErrCode directory_fetch(DirectoryList* dir) {
  LOGI("[HTTP] Attempting to fetch directory listing from %s:%d%s\n",
       DIR_HOST, DIR_PORT, DIR_PATH);

  int fd = sock_connect(DIR_HOST, DIR_PORT);
  if (fd < 0) {
    LOGE("[HTTP] Failed to connect to directory server.\n");
    // Fallback: parse embedded mock body (hardcoded)
    static const char* EMBED_BODY =
      "r mockA id ts 192.0.2.101 443 80\n"
      "r mockB id ts 192.0.2.102 443 80\n"
      "r mockC id ts 192.0.2.103 443 80\n"
      "r mockD id ts 192.0.2.104 443 80\n"
      "r mockE id ts 192.0.2.105 443 80\n";
    return directory_parse(EMBED_BODY, dir);
  }

  char request[512];
  int req_len = snprintf(request, sizeof(request),
                         "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
                         DIR_PATH, DIR_HOST);
  if (send(fd, request, req_len, 0) < 0) {
    perror("[HTTP] Send failed");
    close(fd);
    return E_HTTP_FAIL;
  }

  char http_response[4096] = {0};
  ssize_t total_read = 0;
  ssize_t n;

  while ((n = recv(fd, http_response + total_read, sizeof(http_response) - 1 - total_read, 0)) > 0) {
      total_read += n;
  }
  close(fd);

  if (total_read == 0) {
    LOGE("[HTTP] Received empty response.\n");
    return E_HTTP_FAIL;
  }

  const char* body_start = strstr(http_response, "\r\n\r\n");
  if (!body_start) {
    LOGE("[HTTP] Invalid HTTP response format (no header/body separator).\n");
    return E_HTTP_FAIL;
  }
  body_start += 4;

  if (strncmp(http_response, "HTTP/1.1 200", 12) != 0 &&
      strncmp(http_response, "HTTP/1.0 200", 12) != 0) {
    LOGE("[HTTP] Server returned non-200 status.\n");
    return E_HTTP_FAIL;
  }

  return directory_parse(body_start, dir);
}

// --- [ TOR CIRCUIT AND TUNNEL LOGIC ] --------------------------------------

TTunnel* tunnel_create(DirectoryList *dir) {
  TTunnel *t = (TTunnel*)calloc(1, sizeof(TTunnel));
  if (!t) return NULL;
  t->dir = dir;
  return t;
}

ErrCode tunnel_select_next_node(TTunnel *t) {
  if (t->dir->count == 0) {
    LOGE("[TUNNEL] No relays available in directory list.\n");
    return E_NO_RELAYS;
  }
  int index = (t->retryCount - 1) % t->dir->count;
  t->currentNode = t->dir->nodes[index];
  return E_SUCCESS;
}

ErrCode tunnel_handle_connect_complete(TTunnel *t);
ErrCode tunnel_handle_circuit_created(TTunnel *t);

ErrCode tunnel_start_connect(TTunnel *t) {
  t->retryCount++;

  if (t->retryCount > MAX_RETRY_COUNT) {
    LOGE("[TUNNEL] Max retry limit reached (%d). Failed to build circuit.\n", MAX_RETRY_COUNT);
    return E_RETRY_LIMIT;
  }

  util_exponential_backoff_delay(t->retryCount);

  if (tunnel_select_next_node(t) != E_SUCCESS) {
      return E_NO_RELAYS;
  }

  LOGI("[TUNNEL] Attempting to connect to entry node %s:%u (Attempt %d/%d)...\n",
       t->currentNode.ip, t->currentNode.orPort, t->retryCount, MAX_RETRY_COUNT);

  int fd = sock_connect(t->currentNode.ip, t->currentNode.orPort);

  if (fd < 0) {
    LOGE("[TUNNEL] Connection failed (IP: %s). Retrying...\n", t->currentNode.ip);
    return E_CONN_FAIL;
  }

  if (t->circ) {
      close(t->circ->nodeConn.fd);
      free(t->circ);
      t->circ = NULL;
  }
  t->circ = (Circuit*)calloc(1, sizeof(Circuit));
  t->circ->tunnel = t;
  t->circ->nodeConn.fd = fd;
  t->circ->circId = util_rand_id();

  return tunnel_handle_connect_complete(t);
}

ErrCode tunnel_handle_connect_complete(TTunnel *t) {
  LOGI("[TUNNEL] TCP connection established. Sending CREATE cell (CircID: %u)...\n", t->circ->circId);

  unsigned char create_cell[CELL_LEN] = {0};
  util_i16_to_be(create_cell, t->circ->circId);
  create_cell[2] = 0x01; // CELL_CREATE

  if (send(t->circ->nodeConn.fd, create_cell, CELL_LEN, 0) != CELL_LEN) {
    perror("[TUNNEL] Failed to send CREATE cell");
    close(t->circ->nodeConn.fd);
    return E_TOR_PROTO;
  }

  unsigned char created_cell[CELL_LEN] = {0};
  if (sock_read_exact(t->circ->nodeConn.fd, created_cell, CELL_LEN) != E_SUCCESS) {
    LOGE("[TUNNEL] Failed to read CREATED cell.\n");
    close(t->circ->nodeConn.fd);
    return E_TOR_PROTO;
  }

  uint16_t received_circId = util_i16_from_be(created_cell);
  uint8_t command = created_cell[2];

  if (received_circId != t->circ->circId || command != 0x02) {
    LOGE("[TUNNEL] Invalid CREATED cell response (ID: %u, CMD: %u).\n", received_circId, command);
    close(t->circ->nodeConn.fd);
    return E_TOR_PROTO;
  }

  return tunnel_handle_circuit_created(t);
}

ErrCode tunnel_handle_circuit_created(TTunnel *t) {
  LOGI("[TUNNEL] Circuit successfully created and handshake complete (CircID: %u).\n",
       t->circ->circId);
  t->retryCount = 0;
  return E_SUCCESS;
}

ErrCode tunnel_connect_robust(TTunnel *t) {
    ErrCode result = E_CONN_FAIL;
    while (result != E_SUCCESS) {
        result = tunnel_start_connect(t);

        if (result == E_RETRY_LIMIT) {
            LOGE("[TUNNEL] Failed after all retries.\n");
            return result;
        }
        if (result == E_NO_RELAYS) {
            LOGE("[TUNNEL] No relays found to try.\n");
            return result;
        }
        // On E_CONN_FAIL/E_TOR_PROTO, loop continues and retries.
    }
    return E_SUCCESS;
}

// --- [ SOCKS CONNECTION HANDLER ] --------------------------------------------

void sconn_respond_ok(SConn *sconn, Circuit *circ) {
  unsigned char response[10] = {0};
  response[0] = SOCKS_VER;
  response[1] = SOCKS_REP_SUCCESS;
  response[2] = 0x00;
  response[3] = SOCKS_ATYP_IPV4;
  // bind addr and port left zero
  send(sconn->fd, response, 10, 0);
}

void sconn_respond_err(SConn *sconn) {
  unsigned char response[10] = {0};
  response[0] = SOCKS_VER;
  response[1] = 0x01; // general failure
  response[2] = 0x00;
  response[3] = SOCKS_ATYP_IPV4;
  send(sconn->fd, response, 10, 0);
}

ErrCode sconn_get_request(SConn *sconn) {
  unsigned char buf[PROXY_BUF_SIZE];

  // Method negotiation
  if (sock_read_exact(sconn->fd, buf, 3) != E_SUCCESS || buf[0] != SOCKS_VER) {
    return E_SOCKS_PROTO;
  }
  buf[0] = SOCKS_VER; buf[1] = 0x00; // NO AUTH
  send(sconn->fd, buf, 2, 0);

  // VER, CMD, RSV, ATYP
  if (sock_read_exact(sconn->fd, buf, 4) != E_SUCCESS || buf[0] != SOCKS_VER) {
    return E_SOCKS_PROTO;
  }
  if (buf[1] != 0x01) { // CONNECT
    LOGE("[SOCKS] Only CONNECT command is supported.\n");
    return E_SOCKS_PROTO;
  }
  if (buf[3] != SOCKS_ATYP_DOMAIN) {
    LOGE("[SOCKS] Only Domain Address type (0x03) is supported.\n");
    return E_SOCKS_PROTO;
  }

  // Domain length
  if (sock_read_exact(sconn->fd, buf, 1) != E_SUCCESS) return E_SOCKS_PROTO;
  uint8_t dlen = buf[0];
  if (dlen == 0 || dlen > MAX_HOST_LEN - 1) return E_SOCKS_PROTO;

  // Domain + port
  if (sock_read_exact(sconn->fd, buf, dlen + 2) != E_SUCCESS) return E_SOCKS_PROTO;

  memcpy(sconn->destHost, buf, dlen);
  sconn->destHost[dlen] = '\0';
  sconn->destPort = util_i16_from_be(buf + dlen);

  return E_SUCCESS;
}

ErrCode stream_open_and_tunnel_data(TTunnel *t, SConn *sconn) {
  LOGI("[STREAM] Opening stream for %s:%u...\n", sconn->destHost, sconn->destPort);

  if (!t->circ) {
    LOGE("[STREAM] Error: No active circuit.\n");
    return E_CONN_FAIL;
  }

  uint16_t strmId = util_rand_id();

  unsigned char begin_cell[CELL_LEN] = {0};
  util_i16_to_be(begin_cell, t->circ->circId);
  begin_cell[2] = 0x03; // CELL_RELAY

  unsigned char *relay_head = begin_cell + CELL_HEADER_LEN;
  util_i16_to_be(relay_head, strmId);
  relay_head[2] = 0x01; // RELAY_BEGIN

  unsigned char *payload = relay_head + 11;
  int payload_len = snprintf((char*)payload, PAYLOAD_MAX_LEN, "%s:%d", sconn->destHost, sconn->destPort);
  if (payload_len < 0) return E_TOR_PROTO;
  payload[payload_len + 1] = 0;
  util_i16_to_be(relay_head + 9, payload_len + 1);

  if (send(t->circ->nodeConn.fd, begin_cell, CELL_LEN, 0) != CELL_LEN) {
    perror("[STREAM] Failed to send RELAY_BEGIN");
    return E_TOR_PROTO;
  }

  LOGI("[STREAM] Stream opened (StreamID: %u). Beginning data transfer...\n", strmId);
  return E_SUCCESS;
}

// --- [ PROXY MAIN LOOP ] -----------------------------------------------------

void handle_client_connection(TTunnel *t, int clientFd) {
  SConn *sconn = (SConn*)calloc(1, sizeof(SConn));
  if (!sconn) { close(clientFd); return; }
  sconn->fd = clientFd;

  LOGI("\n[PROXY] Incoming SOCKS connection accepted (FD: %d).\n", clientFd);

  if (!t->circ) {
    LOGE("[PROXY] Circuit is not ready. Rejecting request.\n");
    sconn_respond_err(sconn);
    close(sconn->fd);
    free(sconn);
    return;
  }

  ErrCode socks_err = sconn_get_request(sconn);

  if (socks_err != E_SUCCESS) {
    LOGE("[PROXY] SOCKS protocol error: %d. Closing connection.\n", socks_err);
    sconn_respond_err(sconn);
    close(sconn->fd);
    free(sconn);
    return;
  }

  LOGI("[PROXY] Request: CONNECT %s:%u\n", sconn->destHost, sconn->destPort);

  ErrCode stream_err = stream_open_and_tunnel_data(t, sconn);

  if (stream_err != E_SUCCESS) {
    LOGE("[PROXY] Tor stream failed: %d. Rejecting.\n", stream_err);
    sconn_respond_err(sconn);
    close(sconn->fd);
    free(sconn);
    return;
  }

  sconn_respond_ok(sconn, t->circ);
  LOGI("[PROXY] SOCKS connection established. Data shuffling begins.\n");

  // Demo: close immediately to avoid blocking; replace with poll loop if needed
  close(sconn->fd);
  LOGI("[PROXY] Client connection closed (Simulated shuffle complete).\n");

  free(sconn);
}

int proxy_run_listener(TTunnel *t, int port) {
  int listenFd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenFd < 0) { perror("[LISTENER] socket"); return 1; }

  int opt = 1;
  setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  SockAddr addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (bind(listenFd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("[LISTENER] bind"); close(listenFd); return 1;
  }

  if (listen(listenFd, 10) < 0) {
    perror("[LISTENER] listen"); close(listenFd); return 1;
  }

  LOGI("\n[LISTENER] SOCKS proxy ready on 127.0.0.1:%d.\n", port);
  LOGI("[LISTENER] Awaiting client connections...\n");

  while (1) {
    SockAddr clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientLen);

    if (clientFd >= 0) {
      handle_client_connection(t, clientFd);
    } else {
      if (errno == EINTR) continue;
      perror("[LISTENER] accept error");
      break;
    }
  }

  close(listenFd);
  return 0;
}

// --- [ CHROMIUM STARTUP ] ----------------------------------------------------

int set_socks_env(const char* host, int port) {
  char val[64];
  int n = snprintf(val, sizeof(val), "%s:%d", host, port);
  if (n <= 0 || (size_t)n >= sizeof(val)) return -1;
  return setenv("SOCKS5_PROXY", val, 1);
}

int start_chromium_with_socks(const char* host, int port) {
  pid_t pid = fork();
  if (pid < 0) {
    perror("[CHROMIUM] fork failed");
    return -1;
  }
  if (pid == 0) {
    char proxy_arg[128];
    char resolver_arg[128];
    snprintf(proxy_arg, sizeof(proxy_arg), "socks5://%s:%d", host, port);
    snprintf(resolver_arg, sizeof(resolver_arg), "MAP * ~NOTFOUND , EXCLUDE localhost");

    execlp("chromium", "chromium",
           "--proxy-server", proxy_arg,
           "--host-resolver-rules", resolver_arg,
           (char*)NULL);
    perror("[CHROMIUM] exec failed");
    _exit(127);
  }
  return 0;
}

// --- [ MAIN ENTRY POINT ] ----------------------------------------------------

int main(void) {
  srand((unsigned int)time(NULL));
  const int local_socks_port = 5060;

  LOGI("Tor Proxy (Fixed Edition) starting...\n");
  LOGI("================================================\n");

  // 1. Fetch Directory Listing
  DirectoryList dir = {0};
  ErrCode fetch_result = directory_fetch(&dir);

  if (fetch_result != E_SUCCESS) {
    LOGE("\n[FATAL] Failed to fetch or parse directory. Error code: %d. Exiting.\n", fetch_result);
    return 1;
  }

  // 2. Setup Tor Tunnel
  TTunnel *tunnel = tunnel_create(&dir);
  if (!tunnel) {
    LOGE("\n[FATAL] Failed to allocate tunnel memory. Exiting.\n");
    return 1;
  }

  // 3. Robustly Connect (Circuit Setup with Retries)
  ErrCode connect_result = tunnel_connect_robust(tunnel);

  if (connect_result != E_SUCCESS) {
    LOGE("\n[FATAL] Final failure connecting to Tor entry guards. Exiting.\n");
    if (tunnel->circ) {
      close(tunnel->circ->nodeConn.fd);
      free(tunnel->circ);
    }
    free(tunnel);
    return 1;
  }

  LOGI("\n[SUCCESS] Tor Circuit is ACTIVE and ready to route traffic.\n");
  LOGI("========================================================\n");

  // Optional: set ENV and start Chromium bound to this proxy (hardcoded)
  if (set_socks_env("127.0.0.1", local_socks_port) != 0) {
    LOGE("[CHROMIUM] Failed to set SOCKS5_PROXY env.\n");
  }
  if (start_chromium_with_socks("127.0.0.1", local_socks_port) != 0) {
    LOGE("[CHROMIUM] Failed to start Chromium with SOCKS.\n");
  }

  // 4. Start Proxy Listener
  int proxy_status = proxy_run_listener(tunnel, local_socks_port);

  // Cleanup
  if (tunnel->circ) {
      close(tunnel->circ->nodeConn.fd);
      free(tunnel->circ);
  }
  free(tunnel);

  return proxy_status;
}

