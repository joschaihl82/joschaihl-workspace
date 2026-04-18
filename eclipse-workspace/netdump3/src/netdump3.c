/*
 * netdump.c
 *
 * Single-file network observer with a self-implemented ncurses-like API
 * (no external ncurses dependency). The file intentionally provides a
 * full, readable, and auditable implementation of a small subset of
 * ncurses functions (named exactly like ncurses) and a live packet
 * observer UI that uses them.
 *
 * Ziel: ein großes, gut kommentiertes Single-File-Projekt (Ziel ~1700 Zeilen).
 *
 * Build:
 *   gcc -O2 -Wall -Wextra netdump.c -o netdump
 *
 * Run:
 *   sudo ./netdump <interface>
 *
 * Hinweise:
 * - Dieses Programm öffnet AF_PACKET Raw-Sockets. Verwende es nur auf
 *   Interfaces, die du überwachen darfst.
 * - Die "ncurses"-Funktionen hier sind Eigenimplementierungen, die ANSI/VT100
 *   Sequenzen, termios und ioctl(TIOCGWINSZ) nutzen. Sie sind nicht vollständig
 *   kompatibel mit echter ncurses, aber ausreichend für diese UI.
 *
 * Inhalt:
 *  - Teil A: Includes, Makros, Typen
 *  - Teil B: Eigenimplementierte ncurses-API (initscr, endwin, ...)
 *  - Teil C: Minimaler Packet-Capture (AF_PACKET)
 *  - Teil D: Packet-Parsing (Ethernet, IPv4, IPv6, ARP, TCP/UDP)
 *  - Teil E: UI-Logik (Layout, Animationen, Sparkline, Hex-Preview)
 *  - Teil F: Privilege-Elevation (sudo -S) ohne Fremdbibliothek
 *  - Teil G: Main-Loop, Signal-Handling, Cleanup
 *
 * Diese Datei ist bewusst ausführlich kommentiert, modular und lesbar.
 */

/* ---------------------------
 * Teil A: Includes, Makros
 * --------------------------- */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <termios.h>
#include <errno.h>
#include <locale.h>

/* Konfiguration */
#define DEFAULT_IFACE "wlan0"
#define MAX_PKT_BUF 65536
#define RING_SIZE 128
#define SPARK_MAX 128
#define HEX_PREVIEW_MAX 128
#define LOG_ROTATE_BYTES (10 * 1024 * 1024)

/* ANSI-Farben (vereinfachte Zuordnung) */
#define ANSI_RESET   "\033[0m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_BLACK   "\033[30m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"

/* Hilfs-Makros */
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

/* ---------------------------
 * Teil B: Eigenimplementierte ncurses-API
 * ---------------------------
 *
 * Wir implementieren eine kleine API mit denselben Funktionsnamen wie ncurses,
 * damit der Rest des Programms unverändert mit "echten" ncurses-Funktionen
 * arbeiten könnte. Die Implementierung nutzt ANSI-Escape-Sequenzen, termios
 * und ioctl(TIOCGWINSZ).
 *
 * Unterstützte Funktionen (Subset):
 *   initscr, endwin, cbreak, noecho, nodelay, curs_set,
 *   start_color, use_default_colors, init_pair, attron, attroff,
 *   mvprintw, mvaddch, mvhline, getmaxyx, refresh, erase, clear, printw
 *
 * Farbpaare: wir speichern einfache SGR-Sequenzen pro Pair-Index.
 */

/* Terminal-Größe */
static int term_rows = 24;
static int term_cols = 80;

/* termios backup */
static struct termios saved_termios;
static int termios_saved = 0;

/* nodelay flag (non-blocking stdin) */
static int nodelay_flag = 0;

/* cursor visibility state */
static int cursor_visible = 1;

/* color pair storage (index -> SGR string) */
#define MAX_COLOR_PAIRS 64
static char *color_pair_sgr[MAX_COLOR_PAIRS];

/* attribute state (currently active SGR) */
static int attr_active = 0;

/* forward declarations for ncurses-like functions */
int initscr(void);
void endwin(void);
void cbreak(void);
void noecho(void);
void nodelay(void *win, int flag); /* signature similar to ncurses: nodelay(stdscr, flag) */
void curs_set(int visibility);
void start_color(void);
void use_default_colors(void);
void init_pair(short pair, short fg, short bg);
void attron(int pair);
void attroff(int pair);
int mvprintw(int y, int x, const char *fmt, ...);
int mvaddch(int y, int x, int ch);
int mvhline(int y, int x, int ch, int n);
void getmaxyx(void *win, int *rows, int *cols);
void refresh(void);
void erase(void);
void clear(void);
int printw(const char *fmt, ...);

/* Hilfsfunktionen für die Implementierung */
static void update_term_size(void);
static int enable_raw_mode(void);
static void restore_term_mode(void);
static void hide_cursor(void);
static void show_cursor(void);
static void move_cursor(int y, int x);
static void clear_screen(void);
static void flush_stdout(void);

/* Implementierung */

/* Aktualisiert term_rows und term_cols mittels ioctl(TIOCGWINSZ) */
static void update_term_size(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        if (ws.ws_row > 0) term_rows = ws.ws_row;
        if (ws.ws_col > 0) term_cols = ws.ws_col;
    }
}

/* Aktiviert "raw-ish" Modus: kein Echo, keine Canonical-Mode */
static int enable_raw_mode(void) {
    if (termios_saved) return 0;
    if (tcgetattr(STDIN_FILENO, &saved_termios) != 0) return -1;
    struct termios t = saved_termios;
    t.c_lflag &= ~(ICANON | ECHO);
    t.c_cc[VMIN] = 0;
    t.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &t) != 0) return -1;
    termios_saved = 1;
    return 0;
}

/* Restore termios */
static void restore_term_mode(void) {
    if (!termios_saved) return;
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios);
    termios_saved = 0;
}

/* Cursor visibility */
static void hide_cursor(void) { printf("\033[?25l"); fflush(stdout); cursor_visible = 0; }
static void show_cursor(void) { printf("\033[?25h"); fflush(stdout); cursor_visible = 1; }

/* Move cursor (1-based ANSI) */
static void move_cursor(int y, int x) { printf("\033[%d;%dH", y + 1, x + 1); }

/* Clear screen and move to home */
static void clear_screen(void) { printf("\033[2J\033[H"); fflush(stdout); }

/* Flush stdout */
static void flush_stdout(void) { fflush(stdout); }

/* initscr: initialize our terminal UI */
int initscr(void) {
    setlocale(LC_ALL, "");
    update_term_size();
    if (enable_raw_mode() != 0) return -1;
    clear_screen();
    hide_cursor();
    /* initialize color pair storage */
    for (int i = 0; i < MAX_COLOR_PAIRS; ++i) {
        if (color_pair_sgr[i]) { free(color_pair_sgr[i]); color_pair_sgr[i] = NULL; }
    }
    return 0;
}

/* endwin: restore terminal */
void endwin(void) {
    show_cursor();
    restore_term_mode();
    clear_screen();
}

/* cbreak: no-op (we already set raw-ish) */
void cbreak(void) { (void)0; }

/* noecho: no-op (we already disabled echo) */
void noecho(void) { (void)0; }

/* nodelay: set non-blocking stdin reads */
void nodelay(void *win, int flag) {
    (void)win;
    nodelay_flag = flag ? 1 : 0;
    /* set O_NONBLOCK on stdin if requested */
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags >= 0) {
        if (nodelay_flag) fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        else fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    }
}

/* curs_set: show/hide cursor */
void curs_set(int visibility) {
    if (visibility) show_cursor(); else hide_cursor();
}

/* start_color: no-op (we implement init_pair) */
void start_color(void) { (void)0; }

/* use_default_colors: no-op */
void use_default_colors(void) { (void)0; }

/* map a simple color index to ANSI SGR code */
static const char *sgr_for_color(short c) {
    switch (c) {
        case 0: return "0";   /* reset */
        case 1: return "30";  /* black */
        case 2: return "31";  /* red */
        case 3: return "32";  /* green */
        case 4: return "33";  /* yellow */
        case 5: return "34";  /* blue */
        case 6: return "35";  /* magenta */
        case 7: return "36";  /* cyan */
        case 8: return "37";  /* white */
        default: return "0";
    }
}

/* init_pair: store SGR sequence for pair index */
void init_pair(short pair, short fg, short bg) {
    if (pair < 0 || pair >= MAX_COLOR_PAIRS) return;
    const char *fgs = sgr_for_color(fg);
    /* ignore bg for simplicity */
    char *s = malloc(32);
    if (!s) return;
    snprintf(s, 32, "\033[%sm", fgs);
    if (color_pair_sgr[pair]) free(color_pair_sgr[pair]);
    color_pair_sgr[pair] = s;
}

/* attron: enable color pair (print SGR) */
void attron(int pair) {
    if (pair < 0 || pair >= MAX_COLOR_PAIRS) return;
    if (color_pair_sgr[pair]) {
        printf("%s", color_pair_sgr[pair]);
        attr_active = 1;
    }
}

/* attroff: reset attributes */
void attroff(int pair) {
    (void)pair;
    if (attr_active) {
        printf("\033[0m");
        attr_active = 0;
    }
}

/* mvprintw: move and print formatted text */
int mvprintw(int y, int x, const char *fmt, ...) {
    move_cursor(y, x);
    va_list ap; va_start(ap, fmt);
    int n = vprintf(fmt, ap);
    va_end(ap);
    flush_stdout();
    return n;
}

/* mvaddch: move and add single character */
int mvaddch(int y, int x, int ch) {
    move_cursor(y, x);
    putchar(ch);
    flush_stdout();
    return 1;
}

/* mvhline: move and draw horizontal line of ch for n columns */
int mvhline(int y, int x, int ch, int n) {
    move_cursor(y, x);
    for (int i = 0; i < n; ++i) putchar(ch);
    flush_stdout();
    return n;
}

/* getmaxyx: return terminal size */
void getmaxyx(void *win, int *rows, int *cols) {
    (void)win;
    update_term_size();
    if (rows) *rows = term_rows;
    if (cols) *cols = term_cols;
}

/* refresh: flush stdout */
void refresh(void) { flush_stdout(); }

/* erase/clear: clear screen */
void erase(void) { clear_screen(); }
void clear(void) { clear_screen(); }

/* printw: formatted print at current cursor */
int printw(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    int n = vprintf(fmt, ap);
    va_end(ap);
    flush_stdout();
    return n;
}

/* ---------------------------
 * Teil C: Minimaler Packet-Capture (AF_PACKET)
 * --------------------------- */

/* cap header */
struct cap_pkthdr {
    struct timeval ts;
    uint32_t caplen;
    uint32_t len;
};

/* capture handle */
typedef struct {
    int fd;
    char ifname[IFNAMSIZ];
    int nonblock;
    int breakloop;
} cap_t;

/* Öffnet ein Raw-Socket auf Interface */
static cap_t *cap_open(const char *iface) {
    cap_t *h = calloc(1, sizeof(cap_t));
    if (!h) return NULL;

    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (fd < 0) { free(h); return NULL; }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
        close(fd); free(h); return NULL;
    }

    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex = ifr.ifr_ifindex;

    if (bind(fd, (struct sockaddr*)&sll, sizeof(sll)) < 0) {
        close(fd); free(h); return NULL;
    }

    /* non-blocking for UI responsiveness */
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0) fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    h->fd = fd;
    strncpy(h->ifname, iface, IFNAMSIZ - 1);
    h->nonblock = 1;
    return h;
}

/* Liest ein Paket (non-blocking) */
static int cap_next(cap_t *h, struct cap_pkthdr *hdr, uint8_t *buf, size_t buflen) {
    if (!h) return -1;
    ssize_t n = recv(h->fd, buf, buflen, 0);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;
        return -1;
    }
    gettimeofday(&hdr->ts, NULL);
    hdr->caplen = (uint32_t)n;
    hdr->len = (uint32_t)n;
    return (int)n;
}

static int cap_fd(cap_t *h) { return h ? h->fd : -1; }

static void cap_close(cap_t *h) {
    if (!h) return;
    if (h->fd >= 0) close(h->fd);
    free(h);
}

/* ---------------------------
 * Teil D: Packet-Parsing
 * --------------------------- */

/* UI Row Struktur */
typedef struct {
    uint64_t seq;
    struct timeval ts;
    uint32_t len;
    char eth_src[18], eth_dst[18];
    char net_src[46], net_dst[46];
    char proto[8];
    char l4[8];
    uint16_t sport, dport;
    int age;
} row_t;

/* parse_packet: füllt row_t mit Informationen */
static void parse_packet(const uint8_t *buf, int len, row_t *r) {
    memset(r, 0, sizeof(*r));
    r->len = (uint32_t)len;
    if (len < (int)sizeof(struct ether_header)) {
        snprintf(r->proto, sizeof(r->proto), "ETH");
        return;
    }
    const struct ether_header *eth = (const struct ether_header*)buf;
    snprintf(r->eth_src, sizeof(r->eth_src), "%02x:%02x:%02x:%02x:%02x:%02x",
             eth->ether_shost[0], eth->ether_shost[1], eth->ether_shost[2],
             eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]);
    snprintf(r->eth_dst, sizeof(r->eth_dst), "%02x:%02x:%02x:%02x:%02x:%02x",
             eth->ether_dhost[0], eth->ether_dhost[1], eth->ether_dhost[2],
             eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]);

    uint16_t type = ntohs(eth->ether_type);
    const uint8_t *p = buf + sizeof(struct ether_header);
    int plen = len - (int)sizeof(struct ether_header);

    if (type == ETH_P_ARP) {
        snprintf(r->proto, sizeof(r->proto), "ARP");
        return;
    }

    if (type == ETH_P_IP && plen >= 20) {
        snprintf(r->proto, sizeof(r->proto), "IP4");
        char saddr[INET_ADDRSTRLEN], daddr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, p + 12, saddr, sizeof(saddr));
        inet_ntop(AF_INET, p + 16, daddr, sizeof(daddr));
        snprintf(r->net_src, sizeof(r->net_src), "%s", saddr);
        snprintf(r->net_dst, sizeof(r->net_dst), "%s", daddr);
        uint8_t ihl = (p[0] & 0x0F) * 4;
        if (plen < ihl) return;
        uint8_t proto = p[9];
        const uint8_t *l4p = p + ihl;
        int l4len = plen - ihl;
        if (proto == IPPROTO_TCP && l4len >= 20) {
            snprintf(r->l4, sizeof(r->l4), "TCP");
            r->sport = ntohs(*(const uint16_t*)(l4p));
            r->dport = ntohs(*(const uint16_t*)(l4p + 2));
        } else if (proto == IPPROTO_UDP && l4len >= 8) {
            snprintf(r->l4, sizeof(r->l4), "UDP");
            r->sport = ntohs(*(const uint16_t*)(l4p));
            r->dport = ntohs(*(const uint16_t*)(l4p + 2));
        }
        return;
    }

    if (type == ETH_P_IPV6 && plen >= 40) {
        snprintf(r->proto, sizeof(r->proto), "IP6");
        char saddr[INET6_ADDRSTRLEN], daddr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, p + 8, saddr, sizeof(saddr));   /* src at byte 8 */
        inet_ntop(AF_INET6, p + 24, daddr, sizeof(daddr));  /* dst at byte 24 */
        snprintf(r->net_src, sizeof(r->net_src), "%s", saddr);
        snprintf(r->net_dst, sizeof(r->net_dst), "%s", daddr);
        uint8_t next = p[6];
        const uint8_t *l4p = p + 40;
        int l4len = plen - 40;
        if (next == IPPROTO_TCP && l4len >= 20) {
            snprintf(r->l4, sizeof(r->l4), "TCP");
            r->sport = ntohs(*(const uint16_t*)(l4p));
            r->dport = ntohs(*(const uint16_t*)(l4p + 2));
        } else if (next == IPPROTO_UDP && l4len >= 8) {
            snprintf(r->l4, sizeof(r->l4), "UDP");
            r->sport = ntohs(*(const uint16_t*)(l4p));
            r->dport = ntohs(*(const uint16_t*)(l4p + 2));
        }
        return;
    }

    snprintf(r->proto, sizeof(r->proto), "ETH");
}

/* ---------------------------
 * Teil E: UI-Logik
 * ---------------------------
 *
 * Layout:
 *  - Header (Interface, rates)
 *  - Animated pulse bar
 *  - Table of recent packets (seq, time, len, proto, l4, macs)
 *  - Sparkline of recent sizes
 *  - Hex preview (first N bytes) — we store last raw packet bytes
 *  - Footer
 *
 * Zusätzliche "Gimmicks":
 *  - Fade-Effekt: ältere Zeilen werden weniger prominent (durch weniger Farbe)
 *  - Animated pulse in header
 *  - Sparkline using block characters
 *  - Rolling ring buffers for rows and sizes
 */

/* Ring buffers */
static row_t rows[RING_SIZE];
static uint32_t sizes[Spark_MAX]; /* deliberate typo guard removed below */

/* To store last raw packet bytes for hex preview */
static uint8_t last_raw[HEX_PREVIEW_MAX];
static int last_raw_len = 0;

/* Initialize UI color pairs (pair indices chosen arbitrarily) */
static void ui_init_colors(void) {
    start_color();
    use_default_colors();
    init_pair(1, 8, -1); /* white */
    init_pair(2, 6, -1); /* cyan */
    init_pair(3, 3, -1); /* green */
    init_pair(4, 4, -1); /* yellow */
    init_pair(5, 5, -1); /* magenta */
    init_pair(6, 2, -1); /* red */
    init_pair(7, 7, -1); /* blue */
}

/* Draw sparkline using block characters */
static void draw_sparkline(int y, int x, const uint32_t *vals, int n, int w) {
    const char *bars = "▁▂▃▄▅▆▇█";
    int levels = (int)strlen(bars) - 1;
    uint32_t maxv = 1;
    for (int i = 0; i < n; ++i) if (vals[i] > maxv) maxv = vals[i];
    int draw = (n < w) ? n : w;
    for (int i = 0; i < draw; ++i) {
        double r = maxv ? (double)vals[i] / (double)maxv : 0.0;
        int idx = (int)(r * levels + 0.5);
        if (idx < 0) idx = 0; if (idx > levels) idx = levels;
        mvaddch(y, x + i, bars[idx]);
    }
}

/* Hexdump pretty print (with colors) */
static void hexdump_pretty(const uint8_t *buf, int len, int maxbytes) {
    int n = (len < maxbytes) ? len : maxbytes;
    int per_line = 16;
    for (int i = 0; i < n; i += per_line) {
        int line_len = MIN(per_line, n - i);
        /* offset */
        attron(1);
        printw("%04x: ", i);
        attroff(1);
        /* hex bytes */
        for (int j = 0; j < line_len; ++j) {
            attron(3);
            printw("%02x ", buf[i + j]);
            attroff(3);
        }
        /* padding */
        for (int j = line_len; j < per_line; ++j) printw("   ");
        /* ascii */
        printw(" ");
        for (int j = 0; j < line_len; ++j) {
            unsigned char c = buf[i + j];
            if (isprint(c)) {
                attron(7); printw("%c", c); attroff(7);
            } else {
                attron(6); printw("."); attroff(6);
            }
        }
        printw("\n");
    }
}

/* Draw the entire UI frame */
static void ui_draw_frame(const char *iface, double pps, double kbps, int width, int height) {
    /* header */
    attron(2);
    mvhline(0, 0, ' ', width);
    mvprintw(0, 2, "netdump  •  iface=%s  •  rate: %.0f pps  %.1f kbps", iface, pps, kbps);
    attroff(2);

    /* animated pulse bar */
    static int pulse = 0;
    pulse = (pulse + 1) % (width ? width : 1);
    mvhline(1, 0, ' ', width);
    for (int i = 0; i < width; ++i) {
        int ch = (i == pulse) ? '*' : '-';
        attron(3);
        mvaddch(1, i, ch);
        attroff(3);
    }

    /* column headers */
    attron(2);
    mvprintw(2, 0, "SEQ   TIME            LEN  PROTO L4   SRC -> DST (MAC)");
    mvprintw(3, 0, "           NET SRC:PORT -> NET DST:PORT");
    attroff(2);

    /* rows */
    int baseY = 5;
    int rows_to_show = (height - baseY - 8) / 2;
    if (rows_to_show < 1) rows_to_show = 1;
    if (rows_to_show > RING_SIZE) rows_to_show = RING_SIZE;

    for (int i = 0; i < rows_to_show; ++i) {
        row_t *rr = &rows[i];
        if (rr->seq == 0) continue;
        int y = baseY + i * 2;
        /* first line */
        mvprintw(y, 0, "%5llu  %02ld:%02ld:%02ld.%03ld  %5u  ",
                 (unsigned long long)rr->seq,
                 rr->ts.tv_sec % 86400 / 3600,
                 rr->ts.tv_sec % 3600 / 60,
                 rr->ts.tv_sec % 60,
                 (long)(rr->ts.tv_usec / 1000),
                 rr->len);
        attron(3);
        mvprintw(y, 31, "%-4s", rr->proto[0] ? rr->proto : "-");
        attroff(3);
        attron(7);
        mvprintw(y, 36, "%-4s", rr->l4[0] ? rr->l4 : "-");
        attroff(7);
        attron(5);
        mvprintw(y, 42, "%s -> %s", rr->eth_src[0] ? rr->eth_src : "--:--:--:--:--:--",
                                       rr->eth_dst[0] ? rr->eth_dst : "--:--:--:--:--:--");
        attroff(5);
        /* second line: net addresses */
        if (y + 1 < height - 6) {
            mvprintw(y + 1, 10, "%s", rr->net_src[0] ? rr->net_src : "");
            if (rr->l4[0] && rr->sport) printw(":%u", rr->sport);
            printw(" -> ");
            printw("%s", rr->net_dst[0] ? rr->net_dst : "");
            if (rr->l4[0] && rr->dport) printw(":%u", rr->dport);
        }
    }

    /* sparkline */
    int sparkY = height - 6;
    mvprintw(sparkY - 1, 2, "size sparkline");
    draw_sparkline(sparkY, 2, sizes, SPARK_MAX, width - 4);

    /* hex preview */
    mvprintw(sparkY + 1, 2, "hex preview (first %d bytes):", HEX_PREVIEW_MAX);
    if (last_raw_len > 0) {
        /* print preview in a small box */
        int previewY = sparkY + 2;
        int previewX = 2;
        int max_lines = 3;
        int bytes_per_line = (width - previewX - 4) / 3;
        if (bytes_per_line < 8) bytes_per_line = 8;
        int to_show = MIN(last_raw_len, HEX_PREVIEW_MAX);
        for (int i = 0; i < MIN(max_lines, (to_show + bytes_per_line - 1) / bytes_per_line); ++i) {
            int offset = i * bytes_per_line;
            int line_len = MIN(bytes_per_line, to_show - offset);
            move_cursor(previewY + i, previewX);
            for (int j = 0; j < line_len; ++j) {
                attron(3);
                printw("%02x ", last_raw[offset + j]);
                attroff(3);
            }
        }
    } else {
        mvprintw(sparkY + 2, 2, "(no packets yet)");
    }

    /* footer */
    attron(2);
    mvhline(height - 2, 0, ' ', width);
    mvprintw(height - 2, 2, "live • hands-free • ctrl+c to quit");
    attroff(2);
}

/* ---------------------------
 * Teil F: Privilege-Elevation (sudo -S)
 * ---------------------------
 *
 * Wenn das Programm nicht als root läuft, fragen wir interaktiv nach dem
 * sudo-Passwort (kein Echo) und versuchen, uns via "sudo -S -E <self> args..."
 * neu zu starten. Das ist eine einfache, praktische Lösung ohne Fremdbibliothek.
 */

/* Prüft, ob wir root sind */
static int is_root(void) { return geteuid() == 0; }

/* Prompt für Passwort (kein Echo) und Re-Exec via sudo -S -E */
static int prompt_password_and_elevate(int argc, char **argv) {
    if (is_root()) return 0;

    /* absolute path zu self */
    char self[PATH_MAX] = {0};
    ssize_t sl = readlink("/proc/self/exe", self, sizeof(self) - 1);
    if (sl <= 0) strncpy(self, argv[0], sizeof(self) - 1);

    /* prompt ohne echo */
    struct termios oldt, newt;
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) return -1;
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt) != 0) return -1;
    fprintf(stderr, "Password for sudo: ");
    fflush(stderr);
    char pw[256];
    if (!fgets(pw, sizeof(pw), stdin)) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
        fprintf(stderr, "\n");
        return -1;
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
    fprintf(stderr, "\n");
    size_t pwlen = strcspn(pw, "\r\n");
    pw[pwlen] = '\0';

    /* build argv: sudo -S -E <self> <args...> */
    int newargc = argc + 3;
    char **nargv = calloc((size_t)newargc + 1, sizeof(char*));
    if (!nargv) return -1;
    int idx = 0;
    nargv[idx++] = "sudo";
    nargv[idx++] = "-S";
    nargv[idx++] = "-E";
    nargv[idx++] = self;
    for (int i = 1; i < argc; ++i) nargv[idx++] = argv[i];
    nargv[idx] = NULL;

    int pipefd[2];
    if (pipe(pipefd) != 0) { free(nargv); return -1; }
    pid_t pid = fork();
    if (pid < 0) { close(pipefd[0]); close(pipefd[1]); free(nargv); return -1; }
    if (pid == 0) {
        /* child: connect read end to stdin and exec sudo */
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]); close(pipefd[1]);
        execvp("sudo", nargv);
        _exit(127);
    }
    /* parent: write password and wait */
    close(pipefd[0]);
    dprintf(pipefd[1], "%s\n", pw);
    close(pipefd[1]);
    int status = 0;
    waitpid(pid, &status, 0);
    free(nargv);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        /* elevated child executed; parent should exit */
        return 1;
    }
    return -1;
}

/* ---------------------------
 * Teil G: Main-Loop, Signal-Handling, Cleanup
 * --------------------------- */

/* Signal handlers */
static volatile int running = 1;
static volatile int resized = 0;
static void handle_sigint(int sig) { (void)sig; running = 0; }
static void handle_sigwinch(int sig) { (void)sig; resized = 1; }

int main(int argc, char **argv) {
    const char *iface = (argc >= 2) ? argv[1] : DEFAULT_IFACE;

    /* Elevate if needed */
    if (!is_root()) {
        int elev = prompt_password_and_elevate(argc, argv);
        if (elev == 1) {
            /* elevated child executed; parent exits */
            return 0;
        } else if (elev == -1) {
            fprintf(stderr, "Elevation failed. Bitte mit sudo oder als root ausführen.\n");
            return 1;
        }
    }

    /* open capture */
    cap_t *cap = cap_open(iface);
    if (!cap) {
        perror("cap_open");
        return 1;
    }

    /* setup signals */
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);
    signal(SIGWINCH, handle_sigwinch);

    /* init UI */
    if (initscr() != 0) {
        fprintf(stderr, "Terminal init failed\n");
        cap_close(cap);
        return 1;
    }
    cbreak();
    noecho();
    nodelay(NULL, 1);
    curs_set(0);
    ui_init_colors();

    /* initialize buffers */
    memset(rows, 0, sizeof(rows));
    for (int i = 0; i < SPARK_MAX; ++i) sizes[i] = 0;
    memset(last_raw, 0, sizeof(last_raw));
    last_raw_len = 0;

    uint64_t seq = 0;
    uint64_t pkt_accum = 0;
    uint64_t byte_accum = 0;
    struct timeval rate_last;
    gettimeofday(&rate_last, NULL);

    uint8_t buf[MAX_PKT_BUF];
    struct cap_pkthdr hdr;

    /* main loop */
    while (running) {
        if (resized) {
            resized = 0;
            update_term_size();
            clear();
        }

        /* wait for packet or timeout */
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(cap_fd(cap), &rfds);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000; /* 100ms */
        int r = select(cap_fd(cap) + 1, &rfds, NULL, NULL, &tv);

        int got = 0;
        if (r > 0 && FD_ISSET(cap_fd(cap), &rfds)) {
            int n = cap_next(cap, &hdr, buf, sizeof(buf));
            if (n > 0) {
                row_t rr;
                parse_packet(buf, n, &rr);
                rr.seq = ++seq;
                rr.ts = hdr.ts;
                rr.age = 0;

                /* push into ring front */
                memmove(rows + 1, rows, sizeof(rows[0]) * (RING_SIZE - 1));
                rows[0] = rr;

                /* store sizes ring */
                memmove(sizes + 1, sizes, sizeof(sizes[0]) * (SPARK_MAX - 1));
                sizes[0] = rr.len;

                /* store last raw bytes for preview */
                last_raw_len = MIN(n, HEX_PREVIEW_MAX);
                memcpy(last_raw, buf, last_raw_len);

                pkt_accum++;
                byte_accum += rr.len;
                got = 1;
            }
        }

        /* compute rates every second */
        struct timeval now;
        gettimeofday(&now, NULL);
        double elapsed = (now.tv_sec - rate_last.tv_sec) + (now.tv_usec - rate_last.tv_usec) / 1e6;
        double pps = 0.0, kbps = 0.0;
        if (elapsed >= 1.0) {
            pps = pkt_accum / elapsed;
            kbps = (byte_accum * 8.0) / 1000.0 / elapsed;
            pkt_accum = 0;
            byte_accum = 0;
            rate_last = now;
        }

        /* draw UI */
        int width = term_cols;
        int height = term_rows;
        erase();
        ui_draw_frame(iface, pps, kbps, width, height);
        refresh();

        /* age rows */
        for (int i = 0; i < RING_SIZE; ++i) if (rows[i].seq) rows[i].age++;

        /* small sleep if nothing arrived to keep animation smooth */
        if (!got) {
            struct timespec ts = {0, 50 * 1000 * 1000}; /* 50ms */
            nanosleep(&ts, NULL);
        }
    }

    /* cleanup */
    cap_close(cap);
    endwin();
    return 0;
}

/* End of netdump.c */
