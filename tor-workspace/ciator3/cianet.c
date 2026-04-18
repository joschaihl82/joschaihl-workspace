// cia_netranges.c
// Build: gcc -O2 -Wall -o cia_netranges cia_netranges.c
// Run:   ./cia_netranges
// Optional: ARIN WHOIS host override via ARIN_HOST env (default whois.arin.net)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define WHOIS_PORT 43
#define WHOIS_BUF 8192
#define MAX_RESULTS 256
#define MAX_LINE 1024

typedef struct {
    char netrange[128];   // e.g., "198.35.26.0 - 198.35.26.255"
    char cidr[64];        // e.g., "198.35.26.0/24"
    char orgname[128];    // e.g., "Amazon.com, Inc." or "Central Intelligence Agency"
    char origin_as[64];   // e.g., "AS16509"
    char source[64];      // "IP whois" or "Org whois"
} NetRec;

typedef struct {
    NetRec items[MAX_RESULTS];
    int count;
} NetSet;

static int str_starts(const char* s, const char* prefix) {
    size_t lp = strlen(prefix);
    return strncasecmp(s, prefix, lp) == 0;
}

static void trim_end(char* s) {
    size_t n = strlen(s);
    while (n && (s[n-1] == '\r' || s[n-1] == '\n' || isspace((unsigned char)s[n-1]))) {
        s[--n] = '\0';
    }
}

static int tcp_connect(const char* host, int port) {
    char portstr[16];
    snprintf(portstr, sizeof(portstr), "%d", port);
    struct addrinfo hints = {0}, *res = NULL, *it = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    int err = getaddrinfo(host, portstr, &hints, &res);
    if (err) {
        fprintf(stderr, "[WHOIS] getaddrinfo(%s): %s\n", host, gai_strerror(err));
        return -1;
    }
    int fd = -1;
    for (it = res; it != NULL; it = it->ai_next) {
        fd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (fd < 0) continue;
        if (connect(fd, it->ai_addr, it->ai_addrlen) == 0) break;
        close(fd);
        fd = -1;
    }
    freeaddrinfo(res);
    if (fd < 0) fprintf(stderr, "[WHOIS] connect(%s:%d) failed\n", host, port);
    return fd;
}

static int whois_query(const char* server, const char* query, char* out, size_t outsz) {
    int fd = tcp_connect(server, WHOIS_PORT);
    if (fd < 0) return -1;
    char qline[512];
    snprintf(qline, sizeof(qline), "%s\r\n", query);
    ssize_t w = send(fd, qline, strlen(qline), 0);
    if (w < 0) {
        perror("[WHOIS] send");
        close(fd);
        return -1;
    }
    size_t off = 0;
    for (;;) {
        ssize_t n = recv(fd, out + off, outsz - 1 - off, 0);
        if (n <= 0) break;
        off += (size_t)n;
        if (off >= outsz - 1) break;
    }
    out[off] = '\0';
    close(fd);
    return 0;
}

static void netset_add(NetSet* set, const char* netrange, const char* cidr,
                       const char* orgname, const char* origin_as, const char* source)
{
    if (set->count >= MAX_RESULTS) return;
    NetRec* r = &set->items[set->count++];
    snprintf(r->netrange, sizeof(r->netrange), "%s", netrange ? netrange : "");
    snprintf(r->cidr, sizeof(r->cidr), "%s", cidr ? cidr : "");
    snprintf(r->orgname, sizeof(r->orgname), "%s", orgname ? orgname : "");
    snprintf(r->origin_as, sizeof(r->origin_as), "%s", origin_as ? origin_as : "");
    snprintf(r->source, sizeof(r->source), "%s", source ? source : "");
}

static void parse_arin_ip_whois(const char* text, NetSet* out) {
    char line[MAX_LINE];
    const char* p = text;
    char netrange[128] = "";
    char cidr[64] = "";
    char orgname[128] = "";
    char origin_as[64] = "";

    while (*p) {
        size_t i = 0;
        while (*p && *p != '\n' && i < sizeof(line)-1) line[i++] = *p++;
        if (*p == '\n') p++;
        line[i] = '\0';
        trim_end(line);
        if (!*line) continue;

        // Reset when a new NetHandle section starts (heuristic)
        if (str_starts(line, "NetHandle:")) {
            // flush previous (if any)
            if (netrange[0] || cidr[0] || orgname[0] || origin_as[0]) {
                netset_add(out, netrange, cidr, orgname, origin_as, "IP whois");
                netrange[0] = cidr[0] = orgname[0] = origin_as[0] = '\0';
            }
        } else if (str_starts(line, "NetRange:")) {
            snprintf(netrange, sizeof(netrange), "%s", line + strlen("NetRange:"));
            while (*netrange && isspace((unsigned char)*netrange)) memmove(netrange, netrange+1, strlen(netrange));
        } else if (str_starts(line, "CIDR:")) {
            snprintf(cidr, sizeof(cidr), "%s", line + strlen("CIDR:"));
            while (*cidr && isspace((unsigned char)*cidr)) memmove(cidr, cidr+1, strlen(cidr));
        } else if (str_starts(line, "OrgName:")) {
            snprintf(orgname, sizeof(orgname), "%s", line + strlen("OrgName:"));
            while (*orgname && isspace((unsigned char)*orgname)) memmove(orgname, orgname+1, strlen(orgname));
        } else if (str_starts(line, "OriginAS:")) {
            snprintf(origin_as, sizeof(origin_as), "%s", line + strlen("OriginAS:"));
            while (*origin_as && isspace((unsigned char)*origin_as)) memmove(origin_as, origin_as+1, strlen(origin_as));
        }
    }
    // flush last
    if (netrange[0] || cidr[0] || orgname[0] || origin_as[0]) {
        netset_add(out, netrange, cidr, orgname, origin_as, "IP whois");
    }
}

static void parse_arin_org_whois(const char* text, NetSet* out) {
    // We look for NetRange and CIDR blocks in org-based listings
    char line[MAX_LINE];
    const char* p = text;
    char netrange[128] = "";
    char cidr[64] = "";
    char orgname[128] = "";
    char origin_as[64] = "";

    // Try to capture OrgName early
    {
        const char* org = strcasestr(text, "OrgName:");
        if (org) {
            org += 8;
            while (*org == ' ' || *org == '\t') org++;
            size_t k = 0;
            while (org[k] && org[k] != '\n' && k < sizeof(orgname)-1) { orgname[k] = org[k]; k++; }
            orgname[k] = '\0';
            trim_end(orgname);
        } else {
            snprintf(orgname, sizeof(orgname), "%s", "Central Intelligence Agency");
        }
    }

    while (*p) {
        size_t i = 0;
        while (*p && *p != '\n' && i < sizeof(line)-1) line[i++] = *p++;
        if (*p == '\n') p++;
        line[i] = '\0';
        trim_end(line);
        if (!*line) continue;

        if (str_starts(line, "NetRange:")) {
            snprintf(netrange, sizeof(netrange), "%s", line + strlen("NetRange:"));
            while (*netrange && isspace((unsigned char)*netrange)) memmove(netrange, netrange+1, strlen(netrange));
        } else if (str_starts(line, "CIDR:")) {
            snprintf(cidr, sizeof(cidr), "%s", line + strlen("CIDR:"));
            while (*cidr && isspace((unsigned char)*cidr)) memmove(cidr, cidr+1, strlen(cidr));
        } else if (str_starts(line, "OriginAS:")) {
            snprintf(origin_as, sizeof(origin_as), "%s", line + strlen("OriginAS:"));
            while (*origin_as && isspace((unsigned char)*origin_as)) memmove(origin_as, origin_as+1, strlen(origin_as));
        }

        // When we have a netrange, flush an item
        if (netrange[0]) {
            netset_add(out, netrange, cidr, orgname, origin_as, "Org whois");
            netrange[0] = cidr[0] = origin_as[0] = '\0';
        }
    }
}

static void resolve_domain_ips(const char* host, char*** out_ips, int* out_count) {
    *out_ips = NULL;
    *out_count = 0;
    struct addrinfo hints = {0}, *res = NULL, *it = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    int err = getaddrinfo(host, "80", &hints, &res);
    if (err) {
        fprintf(stderr, "[DNS] %s: %s\n", host, gai_strerror(err));
        return;
    }
    // Collect unique IPv4/IPv6 literals (WHOIS ARIN is IPv4-centric; we'll include IPv4 only for now)
    char** ips = malloc(sizeof(char*) * 64);
    int cap = 64, cnt = 0;
    for (it = res; it != NULL; it = it->ai_next) {
        if (it->ai_family == AF_INET) {
            char buf[INET_ADDRSTRLEN];
            struct sockaddr_in* sin = (struct sockaddr_in*)it->ai_addr;
            if (!inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf))) continue;
            // Deduplicate
            int dup = 0;
            for (int i = 0; i < cnt; i++) if (strcmp(ips[i], buf) == 0) { dup = 1; break; }
            if (dup) continue;
            if (cnt >= cap) { cap *= 2; ips = realloc(ips, sizeof(char*) * cap); }
            ips[cnt] = strdup(buf);
            cnt++;
        }
    }
    freeaddrinfo(res);
    *out_ips = ips;
    *out_count = cnt;
}

static void free_ips(char** ips, int count) {
    if (!ips) return;
    for (int i = 0; i < count; i++) free(ips[i]);
    free(ips);
}

static void print_netset(const NetSet* set) {
    // Consolidated output (unique by NetRange+CIDR)
    printf("\n=== CIA-related net ranges (candidate) ===\n");
    for (int i = 0; i < set->count; i++) {
        const NetRec* r = &set->items[i];
        if (!r->netrange[0] && !r->cidr[0]) continue;
        printf("- Source: %s\n", r->source);
        if (r->orgname[0])   printf("  OrgName: %s\n", r->orgname);
        if (r->origin_as[0]) printf("  OriginAS: %s\n", r->origin_as);
        if (r->netrange[0])  printf("  NetRange: %s\n", r->netrange);
        if (r->cidr[0])      printf("  CIDR: %s\n", r->cidr);
        printf("\n");
    }
}

int main(void) {
    const char* arin_host = getenv("ARIN_HOST");
    if (!arin_host || !*arin_host) arin_host = "whois.arin.net";

    NetSet results = {0};

    // Step 1: Resolve CIA domains
    char** ips_cia = NULL; int n_cia = 0;
    char** ips_www = NULL; int n_www = 0;
    resolve_domain_ips("cia.gov", &ips_cia, &n_cia);
    resolve_domain_ips("www.cia.gov", &ips_www, &n_www);

    printf("Resolved cia.gov IPv4 count: %d\n", n_cia);
    for (int i = 0; i < n_cia; i++) printf("  %s\n", ips_cia[i]);
    printf("Resolved www.cia.gov IPv4 count: %d\n", n_www);
    for (int i = 0; i < n_www; i++) printf("  %s\n", ips_www[i]);

    // Step 2: WHOIS for each IP
    for (int i = 0; i < n_cia; i++) {
        char buf[WHOIS_BUF] = {0};
        if (whois_query(arin_host, ips_cia[i], buf, sizeof(buf)) == 0) {
            parse_arin_ip_whois(buf, &results);
        } else {
            fprintf(stderr, "[WHOIS] Failed for %s\n", ips_cia[i]);
        }
    }
    for (int i = 0; i < n_www; i++) {
        char buf[WHOIS_BUF] = {0};
        if (whois_query(arin_host, ips_www[i], buf, sizeof(buf)) == 0) {
            parse_arin_ip_whois(buf, &results);
        } else {
            fprintf(stderr, "[WHOIS] Failed for %s\n", ips_www[i]);
        }
    }

    // Step 3: Org WHOIS attempt (heuristic)
    {
        char buf[WHOIS_BUF] = {0};
        // ARIN supports keyword queries; trying org name directly
        if (whois_query(arin_host, "Central Intelligence Agency", buf, sizeof(buf)) == 0) {
            parse_arin_org_whois(buf, &results);
        } else {
            fprintf(stderr, "[WHOIS] Org search failed\n");
        }
    }

    // Output
    print_netset(&results);

    free_ips(ips_cia, n_cia);
    free_ips(ips_www, n_www);
    return 0;
}

