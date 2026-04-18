#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h> // For srand(time(NULL))

// --- [ DEFINITIONS & TYPES ] -------------------------------------------------

#define PROXY_BUF_SIZE 512
#define MAX_HOST_LEN 256
#define SOCKS_VER 0x05
#define SOCKS_CMD_CONNECT 0x01
#define SOCKS_AUTH_NONE 0x00
#define SOCKS_ATYP_DOMAIN 0x03
#define SOCKS_REP_SUCCESS 0x00
#define PROG_VER "0.3 (ANSI-C)"

// Simple error codes
typedef int ErrCode;
#define E_SUCCESS 0
#define E_CONN_FAIL 1
#define E_SOCKS_PROTO 2
#define E_TOR_PROTO 3
#define E_DESTROYED 4

typedef struct sockaddr_in SockAddr;

// Forward declarations for structs
typedef struct Conn_s Conn;
typedef struct Circ_s Circ;
typedef struct SRVList_s SRVList;
typedef struct TTunnel_s TTunnel;
typedef struct TStream_s TStream;
typedef struct SStream_s SStream;
typedef struct SConn_s SConn;
typedef struct PShuf_s PShuf;
typedef struct TProxy_s TProxy;

// getopt declarations (required by original main)
extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;
extern int getopt(int argc, char * const argv[], const char *optstring);

// --- [ CALLBACK HANDLERS ] ---------------------------------------------------

typedef void (*WriteHdlr)(ErrCode err);
typedef void (*ReadHdlr)(unsigned char* buf, int read);
typedef void (*ConnHdlr)(ErrCode err);
typedef void (*StreamHdlr)(SConn* userContext, TStream* stream, ErrCode err);
typedef void (*SocksReqHdlr)(SConn* conn, char* host, uint16_t port, ErrCode err);
typedef void (*TTunnelErrHdlr)(ErrCode err);

// Global context pointer to simulate C++ 'this' pointer capture in callbacks
PShuf *G_SHUFFER_CONTEXT = NULL;
SConn *G_SOCKS_CONTEXT = NULL;

// --- [ UTILS ] ---------------------------------------------------------------

void util_i16_to_be(unsigned char* buf, uint16_t val) {
  buf[0] = (val >> 8) & 0xFF;
  buf[1] = val & 0xFF;
}

uint16_t util_i16_from_be(unsigned char* buf) {
  return (uint16_t)((buf[0] << 8) | buf[1]);
}

uint16_t util_rand_id() {
  return (uint16_t)(rand() % 65535);
}

// --- [ SHUFFLE STREAM (SS) INTERFACE ] ---------------------------------------

// VTable for SStream
struct SStream_vtable {
  void (*read)(SStream* self, ReadHdlr hdlr);
  void (*write)(SStream* self, unsigned char* buf, int len, WriteHdlr hdlr);
  void (*close)(SStream* self);
};

struct SStream_s {
  struct SStream_vtable *vt;
  void *data;
};

// --- [ TOR PROTOCOL STUBS ] --------------------------------------------------

// STUB: RSA public key structure
typedef struct { int dummy; } RSA_Key;

// STUB: Connection (simplified socket)
struct Conn_s {
  int fd;
  SockAddr peer;
};

// STUB: Server Listing (Directory info)
struct SRVList_s {
  char ident[21];
  char ip[16];
  uint16_t port;
  RSA_Key *key;
};

// STUB: Circuit (Tor protocol core)
struct Circ_s {
  Conn *conn;
  uint16_t circId;
};

// STUB: RSA Key functions
RSA_Key* srvlist_get_key(SRVList* s) {
  return s->key;
}

// STUB: Server Listing Group functions (Directory lookup)
SRVList* srvlist_get_random() {
  SRVList *s = (SRVList*)calloc(1, sizeof(SRVList));
  if (!s) return NULL;
  strcpy(s->ip, "192.0.2.1");
  s->port = 443;
  s->key = (RSA_Key*)calloc(1, sizeof(RSA_Key));
  return s;
}

// STUB: Circuit functions
Circ* circ_new(Conn *c, RSA_Key *k, uint16_t id, TTunnel *t) {
  Circ *circ = (Circ*)calloc(1, sizeof(Circ));
  if (circ) {
    circ->conn = c;
    circ->circId = id;
  }
  return circ;
}
void circ_destroy(Circ *c) { free(c->conn); free(c); }
void circ_create(Circ *c, ConnHdlr hdlr) { hdlr(E_SUCCESS); }
void circ_connect(Circ *c, uint16_t strmId, char *dest, StreamHdlr hdlr, SConn* userContext) {
  TStream *ts = (TStream*)calloc(1, sizeof(TStream));
  ts->strmId = strmId;
  ts->t = (TTunnel*)c; // Abusing pointer for TStream's tunnel field in stub
  hdlr(userContext, ts, E_SUCCESS); // STUB: immediately succeed and return a TStream
}
void circ_read(Circ *c, uint16_t strmId, ReadHdlr hdlr) { hdlr(NULL, 0); }
void circ_write(Circ *c, uint16_t strmId, unsigned char* buf, int len, WriteHdlr hdlr) { hdlr(E_SUCCESS); }
void circ_strm_close(Circ *c, uint16_t strmId) { }
SockAddr circ_get_laddr(Circ *c) { SockAddr sa = {0}; sa.sin_port = htons(9050); return sa; }

// --- [ TOR TUNNEL STREAM (TStream) ] -----------------------------------------

struct TStream_s {
  SStream ss;
  TTunnel *t;
  uint16_t strmId;
};

// TStream VTable implementations
void tstrm_read(SStream *self, ReadHdlr hdlr) {
  TStream *t = (TStream*)self->data;
  // circ_read(t->t->circ, t->strmId, hdlr); // STUB: Disabled to prevent infinite loop in minimal stub
  hdlr(NULL, 0);
}

void tstrm_write(SStream *self, unsigned char* buf, int len, WriteHdlr hdlr) {
  TStream *t = (TStream*)self->data;
  circ_write((Circ*)t->t, t->strmId, buf, len, hdlr);
}

void tstrm_close(SStream *self) {
  TStream *t = (TStream*)self->data;
  circ_strm_close((Circ*)t->t, t->strmId);
  free(t);
}

struct SStream_vtable tstrm_vt = {
  &tstrm_read, &tstrm_write, &tstrm_close,
};

TStream* tstrm_new(uint16_t strmId, TTunnel *t) {
  TStream *ts = (TStream*)calloc(1, sizeof(TStream));
  if (ts) {
    ts->ss.vt = &tstrm_vt;
    ts->ss.data = ts;
    ts->strmId = strmId;
    ts->t = t;
  }
  return ts;
}

// --- [ TOR TUNNEL (TTunnel) ] ------------------------------------------------

struct TTunnel_s {
  void *iosvc;
  SRVList *srvList;
  Circ *circ;
  TTunnelErrHdlr errHdlr;
  Conn nodeConn;

  // Stored callbacks
  ConnHdlr connHdlr;
};

// Forward Declarations for state machine
void ttunnel_on_conn_done(TTunnel *t, ErrCode err);
void ttunnel_on_circ_done(TTunnel *t, ConnHdlr hdlr, ErrCode err);

TTunnel* ttunnel_new(void *iosvc, SRVList *srvList, TTunnelErrHdlr errHdlr) {
  TTunnel *t = (TTunnel*)calloc(1, sizeof(TTunnel));
  if (t) {
    t->iosvc = iosvc;
    t->srvList = srvList;
    t->errHdlr = errHdlr;
    t->nodeConn.fd = -1;
  }
  return t;
}

void ttunnel_connect(TTunnel *t, ConnHdlr hdlr) {
  t->connHdlr = hdlr;
  t->nodeConn.fd = 10;
  fprintf(stderr, "TTunnel: Connecting to node %s...\n", t->srvList->ip);
  ttunnel_on_conn_done(t, E_SUCCESS);
}

void ttunnel_open_stream(TTunnel *t, char *host, uint16_t port, SConn* userContext, StreamHdlr hdlr) {
  uint16_t strmId = util_rand_id();
  char dest[MAX_HOST_LEN + 8];
  sprintf(dest, "%s:%d", host, port);

  // STUB: circ->connect(streamId, destination, SConn* userContext, ...)
  circ_connect(t->circ, strmId, dest, hdlr, userContext);
}

void ttunnel_on_conn_done(TTunnel *t, ErrCode err) {
  if (err != E_SUCCESS) {
    t->errHdlr(err);
    return;
  }
  RSA_Key *key = srvlist_get_key(t->srvList);
  uint16_t circId = util_rand_id();
  t->circ = circ_new(&t->nodeConn, key, circId, t);
  circ_create(t->circ, (ConnHdlr)ttunnel_on_circ_done);
}

void ttunnel_on_circ_done(TTunnel *t, ConnHdlr hdlr, ErrCode err) {
  if (err != E_SUCCESS) {
    t->connHdlr(err);
    return;
  }
  t->connHdlr(E_SUCCESS);
}

// --- [ SOCKS CONNECTION (SConn) ] --------------------------------------------

struct SConn_s {
  SStream ss;
  int fd;
  unsigned char buf[PROXY_BUF_SIZE];
  char destHost[MAX_HOST_LEN];
  uint16_t destPort;

  // Stored callbacks
  SocksReqHdlr reqHdlr;
  ReadHdlr readHdlr;
  WriteHdlr writeHdlr;
};

// SConn VTable implementations
void sconn_read(SStream *self, ReadHdlr hdlr) {
  SConn *s = (SConn*)self->data;
  s->readHdlr = hdlr;
  int n = recv(s->fd, s->buf, PROXY_BUF_SIZE, 0);
  if (n > 0) hdlr(s->buf, n);
  else hdlr(NULL, n);
}

void sconn_write(SStream *self, unsigned char* buf, int len, WriteHdlr hdlr) {
  SConn *s = (SConn*)self->data;
  s->writeHdlr = hdlr;
  if (send(s->fd, buf, len, 0) == len) {
    hdlr(E_SUCCESS);
  } else {
    hdlr(E_CONN_FAIL);
  }
}

void sconn_close(SStream *self) {
  SConn *s = (SConn*)self->data;
  if (s->fd >= 0) close(s->fd);
  free(s);
}

struct SStream_vtable sconn_vt = {
  &sconn_read, &sconn_write, &sconn_close,
};

SConn* sconn_new(int fd) {
  SConn *s = (SConn*)calloc(1, sizeof(SConn));
  if (s) {
    s->ss.vt = &sconn_vt;
    s->ss.data = s;
    s->fd = fd;
  }
  return s;
}

void sconn_respond_err(SConn *s) {
  s->buf[0] = SOCKS_VER;
  s->buf[1] = 0x01;
  s->buf[2] = 0x00;
  s->buf[3] = 0x01;
  send(s->fd, s->buf, 10, 0);
}

void sconn_respond_ok(SConn *s, SockAddr local) {
  s->buf[0] = SOCKS_VER;
  s->buf[1] = SOCKS_REP_SUCCESS;
  s->buf[2] = 0x00;
  s->buf[3] = 0x01;
  memcpy(s->buf + 4, &local.sin_addr.s_addr, 4);
  util_i16_to_be(s->buf + 8, ntohs(local.sin_port));
  send(s->fd, s->buf, 10, 0);
}

void sconn_get_req(SConn *s, SocksReqHdlr hdlr) {
  s->reqHdlr = hdlr;
  // STUB: SOCKS5 negotiation
  if (recv(s->fd, s->buf, 3, 0) != 3 || s->buf[0] != SOCKS_VER) {
    hdlr(s, NULL, 0, E_SOCKS_PROTO);
    return;
  }
  s->buf[0] = SOCKS_VER; s->buf[1] = SOCKS_AUTH_NONE;
  send(s->fd, s->buf, 2, 0);

  unsigned char header[5];
  if (recv(s->fd, header, 5, 0) != 5) {
    hdlr(s, NULL, 0, E_SOCKS_PROTO);
    return;
  }
  if (header[1] != SOCKS_CMD_CONNECT || header[3] != SOCKS_ATYP_DOMAIN) {
    hdlr(s, NULL, 0, E_SOCKS_PROTO);
    return;
  }

  uint8_t dlen = header[4];
  if (dlen == 0 || dlen > MAX_HOST_LEN - 1) {
    hdlr(s, NULL, 0, E_SOCKS_PROTO);
    return;
  }

  if (recv(s->fd, s->buf, dlen + 2, 0) != dlen + 2) {
    hdlr(s, NULL, 0, E_SOCKS_PROTO);
    return;
  }

  memcpy(s->destHost, s->buf, dlen);
  s->destHost[dlen] = '\0';
  s->destPort = util_i16_from_be(s->buf + dlen);

  hdlr(s, s->destHost, s->destPort, E_SUCCESS);
}

// --- [ PROXY SHUFFLER (PShuf) ] ----------------------------------------------

struct PShuf_s {
  SStream *s;
  SStream *n;
  unsigned char sBuf[PROXY_BUF_SIZE];
  unsigned char nBuf[PROXY_BUF_SIZE];
};

// Forward Declarations for callbacks
void pshuf_on_s_read(unsigned char *buf, int read);
void pshuf_on_n_read(unsigned char *buf, int read);
void pshuf_on_s_write(ErrCode err);
void pshuf_on_n_write(ErrCode err);

PShuf* pshuf_new(SStream *s, SStream *n) {
  PShuf *p = (PShuf*)calloc(1, sizeof(PShuf));
  if (p) {
    p->s = s;
    p->n = n;
  }
  return p;
}

void pshuf_shuffle(PShuf *p) {
  G_SHUFFER_CONTEXT = p;
  p->s->vt->read(p->s, pshuf_on_s_read);
  p->n->vt->read(p->n, pshuf_on_n_read);
}

void pshuf_close(PShuf *p) {
  p->s->vt->close(p->s);
  p->n->vt->close(p->n);
  free(p);
}

// Data from SOCKS -> NODE
void pshuf_on_s_read(unsigned char *buf, int read) {
  if (read > 0) {
    G_SHUFFER_CONTEXT->n->vt->write(G_SHUFFER_CONTEXT->n, buf, read, pshuf_on_n_write);
  } else {
    pshuf_close(G_SHUFFER_CONTEXT);
  }
}

void pshuf_on_n_write(ErrCode err) {
  if (err == E_SUCCESS) {
    G_SHUFFER_CONTEXT->s->vt->read(G_SHUFFER_CONTEXT->s, pshuf_on_s_read);
  } else {
    pshuf_close(G_SHUFFER_CONTEXT);
  }
}

// Data from NODE -> SOCKS
void pshuf_on_n_read(unsigned char *buf, int read) {
  if (read > 0) {
    G_SHUFFER_CONTEXT->s->vt->write(G_SHUFFER_CONTEXT->s, buf, read, pshuf_on_s_write);
  } else {
    pshuf_close(G_SHUFFER_CONTEXT);
  }
}

void pshuf_on_s_write(ErrCode err) {
  if (err == E_SUCCESS) {
    G_SHUFFER_CONTEXT->n->vt->read(G_SHUFFER_CONTEXT->n, pshuf_on_n_read);
  } else {
    pshuf_close(G_SHUFFER_CONTEXT);
  }
}

// --- [ TOR PROXY (TProxy) ] --------------------------------------------------

struct Opts_s {
  char *host;
  int port;
  int random;
};

struct TProxy_s {
  int listenFd;
  TTunnel *t;
};

// Forward Declarations for callbacks
void tproxy_on_socks_req(SConn *conn, char *host, uint16_t port, ErrCode err);
void tproxy_on_strm_open(SConn* userContext, TStream *strm, ErrCode err);

TProxy* tproxy_new(TTunnel *t, int listenPort) {
  TProxy *p = (TProxy*)calloc(1, sizeof(TProxy));
  if (!p) return NULL;
  p->t = t;

  p->listenFd = socket(AF_INET, SOCK_STREAM, 0);
  if (p->listenFd < 0) { perror("socket"); free(p); return NULL; }

  int opt = 1;
  setsockopt(p->listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  SockAddr addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(listenPort);

  if (bind(p->listenFd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind"); close(p->listenFd); free(p); return NULL;
  }

  if (listen(p->listenFd, 5) < 0) {
    perror("listen"); close(p->listenFd); free(p); return NULL;
  }

  return p;
}

void tproxy_accept(TProxy *p) {
  SockAddr clientAddr;
  socklen_t clientLen = sizeof(clientAddr);
  int clientFd = accept(p->listenFd, (struct sockaddr *)&clientAddr, &clientLen);

  if (clientFd >= 0) {
    fprintf(stderr, "TProxy: Connection accepted. FD: %d\n", clientFd);
    SConn *conn = sconn_new(clientFd);
    if (!conn) { close(clientFd); return; }

    sconn_get_req(conn, tproxy_on_socks_req);
  } else {
    perror("accept");
  }
}

void tproxy_on_socks_req(SConn *conn, char *host, uint16_t port, ErrCode err) {
  if (err != E_SUCCESS) {
    sconn_respond_err(conn);
    sconn_close(&conn->ss);
    return;
  }

  fprintf(stderr, "TProxy: SOCKS req for %s:%d\n", host, port);

  // Set global context for next callback to access TProxy context
  TProxy *p = (TProxy*)conn->ss.data; // STUB: Assumes conn->ss.data contains a pointer to TProxy (needs correction for real logic)
  TTunnel *t = G_SOCKS_CONTEXT; // Use G_SOCKS_CONTEXT as a STUB for the global TTunnel

  // Open Stream on Tor Tunnel, passing SConn* as user context
  ttunnel_open_stream(t, host, port, conn, tproxy_on_strm_open);
}

void tproxy_on_strm_open(SConn* socksConn, TStream *strm, ErrCode err) {
  if (err != E_SUCCESS || strm == NULL) {
    sconn_respond_err(socksConn);
    sconn_close(&socksConn->ss);
    return;
  }

  fprintf(stderr, "TProxy: Stream open. Starting shuffle.\n");

  SockAddr local = circ_get_laddr((Circ*)strm->t); // STUB: Use t field for circ address lookup
  sconn_respond_ok(socksConn, local);

  PShuf *shuf = pshuf_new(&socksConn->ss, &strm->ss);
  pshuf_shuffle(shuf);
}

// --- [ MAIN ] ----------------------------------------------------------------

void print_usage(const char* name) {
  fprintf(stderr, "\nUsage: %s -n <ip> | -r -p <local port>\n", name);
  fprintf(stderr, "  -n <ip>           -- Single exit node IP to use.\n");
  fprintf(stderr, "  -r                -- Randomly selected exit node.\n");
  fprintf(stderr, "  -p <local port>   -- Local port for SOCKS proxy (default: 5060).\n");
  fprintf(stderr, "  -h                -- Print this help message.\n");
  exit(0);
}

int parse_opts(int argc, char **argv, struct Opts_s *opts) {
  int c;
  opts->port = 5060;
  opts->random = 0;
  opts->host = NULL;

  opterr = 0;

  while ((c = getopt(argc, argv, "n:p:rh")) != -1) {
    switch (c) {
    case 'n':
      opts->host = optarg;
      break;
    case 'p':
      opts->port = atoi(optarg);
      break;
    case 'r':
      opts->random = 1;
      break;
    case 'h':
      print_usage(argv[0]);
    case '?':
      fprintf(stderr, "Unknown option: %c\n", optopt);
      print_usage(argv[0]);
    default:
      return 0;
    }
  }

  if (opts->host == NULL && opts->random == 0) return 0;

  return 1;
}

void on_tt_error(ErrCode err) {
  fprintf(stderr, "FATAL: TTunnel error %d. Exiting.\n", err);
  exit(1);
}

void on_tt_ready(ErrCode err) {
  if (err != E_SUCCESS) {
    on_tt_error(err);
    return;
  }
  fprintf(stderr, "TTunnel: Circuit built. Proxy operational.\n");
}

int main(int argc, char** argv) {
  struct Opts_s opts;

  if (!parse_opts(argc, argv, &opts)) {
    print_usage(argv[0]);
    return 2;
  }

  fprintf(stderr, "torproxy %s by Moxie Marlinspike (C Port).\n", PROG_VER);
  fprintf(stderr, "Retrieving directory info...\n");

  srand(time(NULL));

  // Get Server Listing
  SRVList *srvList = srvlist_get_random();
  if (srvList == NULL) {
    fprintf(stderr, "Error: No Server Listing found.\n");
    return 1;
  }

  // Create TorTunnel
  TTunnel *t = ttunnel_new(NULL, srvList, on_tt_error);
  if (!t) {
    fprintf(stderr, "Error: Failed to create TTunnel.\n");
    return 1;
  }
  G_SOCKS_CONTEXT = t; // STUB: Set global TTunnel context for use in tproxy_on_socks_req

  // Connect the Tunnel (establishes the single-hop circuit)
  ttunnel_connect(t, on_tt_ready);

  // Create TorProxy
  TProxy *p = tproxy_new(t, opts.port);
  if (!p) return 1;

  fprintf(stderr, "SOCKS proxy ready on port %d.\n", opts.port);
  fprintf(stderr, "Awaiting connection... (STUB: Blocking I/O simulation)\n");

  while (1) {
    tproxy_accept(p);
  }

  return 0;
}
