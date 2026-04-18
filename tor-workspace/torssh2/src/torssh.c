/*
 * tor_ssh_shell.c
 *
 * Minimal interactive SSH shell over Tor (SOCKS5) using libssh2.
 * - Connects to Tor SOCKS5 at 127.0.0.1:9050
 * - Issues SOCKS5 CONNECT to the .onion host on port 22
 * - Performs password auth (or can be adapted to publickey)
 * - Requests a PTY and starts an interactive shell, forwarding local terminal I/O
 *
 * Build:
 *   cc -std=c11 -Wall -O2 tor_ssh_shell.c -o tor_ssh_shell -lssh2
 *
 * Usage:
 *   Ensure Tor is running locally (SOCKS5 on 127.0.0.1:9050).
 *   Edit SSH_USERNAME / SSH_PASSWORD or replace with publickey auth.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <libssh2.h>

#define TOR_SOCKS_HOST "127.0.0.1"
#define TOR_SOCKS_PORT "9050"

#define ONION_HOST "rwbboh4lkjq232bguoj76oawi3xbhado6a7quzxgzhx5tuhcewxso4yd.onion"
#define ONION_PORT 22

/* Set your SSH credentials here */
#define SSH_USERNAME "josh"
#define SSH_PASSWORD "Bl3fkj42"   /* or set to NULL to use key auth */

static struct termios orig_term;

/* restore terminal on exit */
static void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
}

/* set terminal to raw mode (portable replacement for cfmakeraw) */
static int set_terminal_raw(void) {
    struct termios t;
    if (tcgetattr(STDIN_FILENO, &t) != 0) return -1;

    /* save original */
    orig_term = t;
    if (atexit(restore_terminal) != 0) {
        /* non-fatal */
    }

    /* Input modes: no break, no CR to NL, no parity check, no strip char,
       no start/stop output control. */
    t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    /* Output modes: disable post processing */
    t.c_oflag &= ~(OPOST);

    /* Control modes: set 8 bit chars */
    t.c_cflag &= ~(CSIZE | PARENB);
    t.c_cflag |= CS8;

    /* Local modes: echoing off, canonical off, no extended functions,
       no signal chars */
    t.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    /* Control chars: set read to return as soon as any data is available */
    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &t) != 0) return -1;
    return 0;
}

static int connect_tcp(const char *host, const char *port) {
    struct addrinfo hints, *res, *rp;
    int sock = -1;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((ret = getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo(%s:%s): %s\n", host, port, gai_strerror(ret));
        return -1;
    }

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1) continue;
        if (connect(sock, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(sock);
        sock = -1;
    }

    freeaddrinfo(res);

    if (sock == -1) {
        fprintf(stderr, "Failed to connect to %s:%s\n", host, port);
        return -1;
    }

    return sock;
}

/* Minimal SOCKS5 CONNECT to hostname:port via already-connected TCP socket */
static int socks5_connect(int sock, const char *host, int port) {
    unsigned char buf[512];
    ssize_t n;
    size_t hostlen = strlen(host);

    if (hostlen > 255) {
        fprintf(stderr, "Hostname too long for SOCKS5\n");
        return -1;
    }

    /* Greeting: VER=5, NMETHODS=1, METHOD=0 (no auth) */
    unsigned char greeting[3] = { 0x05, 0x01, 0x00 };
    n = send(sock, greeting, sizeof(greeting), 0);
    if (n != (ssize_t)sizeof(greeting)) { perror("send greeting"); return -1; }

    n = recv(sock, buf, 2, 0);
    if (n != 2 || buf[0] != 0x05 || buf[1] != 0x00) {
        fprintf(stderr, "SOCKS5: unexpected method response\n");
        return -1;
    }

    /* CONNECT request: VER=5, CMD=1, RSV=0, ATYP=3 (DOMAINNAME), LEN, HOST, PORT */
    size_t p = 0;
    buf[p++] = 0x05;
    buf[p++] = 0x01;
    buf[p++] = 0x00;
    buf[p++] = 0x03;
    buf[p++] = (unsigned char)hostlen;
    memcpy(&buf[p], host, hostlen);
    p += hostlen;
    buf[p++] = (unsigned char)((port >> 8) & 0xFF);
    buf[p++] = (unsigned char)(port & 0xFF);

    n = send(sock, buf, p, 0);
    if (n != (ssize_t)p) { perror("send connect"); return -1; }

    /* Response: VER, REP, RSV, ATYP, BND.ADDR..., BND.PORT */
    n = recv(sock, buf, sizeof(buf), 0);
    if (n < 4) { fprintf(stderr, "SOCKS5: short response\n"); return -1; }
    if (buf[0] != 0x05) { fprintf(stderr, "SOCKS5: bad version in response\n"); return -1; }
    if (buf[1] != 0x00) { fprintf(stderr, "SOCKS5: connect failed, REP=0x%02x\n", buf[1]); return -1; }

    return 0;
}

int main(void) {
    int sock = -1;
    int rc;
    LIBSSH2_SESSION *session = NULL;
    LIBSSH2_CHANNEL *channel = NULL;

    /* 1) Connect to Tor SOCKS5 */
    sock = connect_tcp(TOR_SOCKS_HOST, TOR_SOCKS_PORT);
    if (sock < 0) return 1;

    /* 2) Issue SOCKS5 CONNECT to the .onion host:22 */
    if (socks5_connect(sock, ONION_HOST, ONION_PORT) != 0) {
        close(sock);
        return 1;
    }

    /* 3) Initialize libssh2 */
    rc = libssh2_init(0);
    if (rc != 0) {
        fprintf(stderr, "libssh2_init failed (%d)\n", rc);
        close(sock);
        return 1;
    }

    session = libssh2_session_init();
    if (!session) {
        fprintf(stderr, "Could not allocate SSH session\n");
        libssh2_exit();
        close(sock);
        return 1;
    }

    /* Use blocking mode for simplicity */
    libssh2_session_set_blocking(session, 1);

    if (libssh2_session_handshake(session, sock)) {
        fprintf(stderr, "Failure establishing SSH session\n");
        libssh2_session_free(session);
        libssh2_exit();
        close(sock);
        return 1;
    }

    /* 4) Authenticate (password example; replace with key auth if needed) */
    if (SSH_PASSWORD && SSH_PASSWORD[0]) {
        rc = libssh2_userauth_password(session, SSH_USERNAME, SSH_PASSWORD);
        if (rc) {
            fprintf(stderr, "Authentication by password failed\n");
            libssh2_session_disconnect(session, "Bye");
            libssh2_session_free(session);
            libssh2_exit();
            close(sock);
            return 1;
        }
    } else {
        fprintf(stderr, "No password set; implement key auth here.\n");
        libssh2_session_disconnect(session, "No auth method implemented");
        libssh2_session_free(session);
        libssh2_exit();
        close(sock);
        return 1;
    }

    /* 5) Open a session channel and request a PTY + shell */
    channel = libssh2_channel_open_session(session);
    if (!channel) {
        fprintf(stderr, "Unable to open a session channel\n");
        libssh2_session_disconnect(session, "Channel open failed");
        libssh2_session_free(session);
        libssh2_exit();
        close(sock);
        return 1;
    }

    /* Determine local terminal size */
    struct winsize ws;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0) {
        ws.ws_col = 80; ws.ws_row = 24;
    }

    /* Request pty */
    if (libssh2_channel_request_pty_ex(channel, "xterm", (unsigned int)strlen("xterm"),
                                       NULL, 0,
                                       ws.ws_col, ws.ws_row, 0, 0) != 0) {
        /* If pty request fails, continue without pty */
        fprintf(stderr, "Warning: PTY request failed, continuing without PTY\n");
    }

    if (libssh2_channel_shell(channel) != 0) {
        fprintf(stderr, "Unable to request shell on channel\n");
        libssh2_channel_free(channel);
        libssh2_session_disconnect(session, "Shell request failed");
        libssh2_session_free(session);
        libssh2_exit();
        close(sock);
        return 1;
    }

    /* Put local terminal into raw mode */
    if (set_terminal_raw() != 0) {
        fprintf(stderr, "Warning: failed to set terminal raw mode\n");
    }

    /* 6) Interactive loop: forward stdin <-> channel */
    fd_set readfds;
    int maxfd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;
    char buf[4096];
    ssize_t nread;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        rc = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (rc < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        /* Data from remote -> read channel and write to stdout */
        if (FD_ISSET(sock, &readfds)) {
            nread = libssh2_channel_read(channel, buf, sizeof(buf));
            if (nread > 0) {
                ssize_t w = 0;
                while (w < nread) {
                    ssize_t wn = write(STDOUT_FILENO, buf + w, (size_t)(nread - w));
                    if (wn <= 0) break;
                    w += wn;
                }
            } else if (nread == 0) {
                /* remote closed */
                break;
            } else {
                if (nread == LIBSSH2_ERROR_EAGAIN) continue;
                break;
            }
        }

        /* Data from local stdin -> write to channel */
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            ssize_t r = read(STDIN_FILENO, buf, sizeof(buf));
            if (r > 0) {
                ssize_t written = 0;
                while (written < r) {
                    ssize_t wn = libssh2_channel_write(channel, buf + written, (size_t)(r - written));
                    if (wn < 0) {
                        if (wn == LIBSSH2_ERROR_EAGAIN) continue;
                        goto cleanup;
                    }
                    written += wn;
                }
            } else if (r == 0) {
                /* EOF on stdin: send EOF to remote */
                libssh2_channel_send_eof(channel);
            } else {
                if (errno == EINTR) continue;
                perror("read stdin");
                break;
            }
        }

        /* Check if channel is closed by remote */
        if (libssh2_channel_eof(channel)) {
            break;
        }
    }

cleanup:
    /* Close channel gracefully */
    if (channel) {
        libssh2_channel_close(channel);
        libssh2_channel_wait_closed(channel);
        libssh2_channel_free(channel);
        channel = NULL;
    }

    libssh2_session_disconnect(session, "Normal Shutdown");
    libssh2_session_free(session);
    libssh2_exit();

    close(sock);
    /* terminal restored by atexit handler */
    return 0;
}
