/*
 * tinydump_appinsight.c
 *
 * Tiny tcpdump-like sniffer with application-layer interpretation.
 * - Default device: wlan0
 * - Default filter: "" (capture everything)
 * - Interprets DNS, HTTP, HTTPS (SNI), DHCP, TCP/UDP basic info
 * - Best-effort process name lookup via /proc scanning (may require privileges)
 *
 * Compile:
 *   gcc -O2 -Wall -o tinydump_appinsight tinydump_appinsight.c -lpcap
 *
 * Run:
 *   sudo ./tinydump_appinsight
 *   sudo ./tinydump_appinsight eth0 "port 80"
 *
 * Notes:
 * - IPv4 only for application parsing in this example.
 * - For high packet rates, DNS resolution and /proc scanning should be cached/refreshed asynchronously.
 */

#define _GNU_SOURCE
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>

#define SNAPLEN 65535
#define PROMISC 1
#define TIMEOUT_MS 1000
#define CACHE_SIZE 1024
#define CACHE_TTL_SEC 60

static volatile sig_atomic_t stop_requested = 0;
static pcap_t *global_handle = NULL;

static void handle_sigint(int sig) {
    (void)sig;
    stop_requested = 1;
    if (global_handle) pcap_breakloop(global_handle);
}

/* ---------- utilities ---------- */

static void print_time(const struct pcap_pkthdr *h) {
    char buf[64];
    time_t t = h->ts.tv_sec;
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    printf("%s.%06ld ", buf, (long)h->ts.tv_usec);
}

static int ensure_bounds(const struct pcap_pkthdr *h, size_t offset, size_t need) {
    if (h->caplen < offset + need) return 0;
    return 1;
}

/* ---------- reverse DNS cache ---------- */

struct dns_cache_entry {
    uint32_t ip;               /* network byte order IPv4 address */
    char name[NI_MAXHOST];
    time_t ts;
    int used;
};

static struct dns_cache_entry dns_cache[CACHE_SIZE];

static inline unsigned int ip_hash(uint32_t ip) {
    return (unsigned int)((ip * 2654435761u) & (CACHE_SIZE - 1));
}

static void ip_to_str(uint32_t ip_net, char *buf, size_t bufsz) {
    struct in_addr a;
    a.s_addr = ip_net;
    inet_ntop(AF_INET, &a, buf, (socklen_t)bufsz);
}

static const char *resolve_hostname_ipv4(uint32_t ip_net) {
    if (ip_net == 0) return "-";
    unsigned int idx = ip_hash(ip_net);
    time_t now = time(NULL);

    for (unsigned int i = 0; i < CACHE_SIZE; ++i) {
        unsigned int j = (idx + i) & (CACHE_SIZE - 1);
        if (dns_cache[j].used) {
            if (dns_cache[j].ip == ip_net) {
                if (now - dns_cache[j].ts <= CACHE_TTL_SEC) {
                    return dns_cache[j].name;
                } else {
                    break;
                }
            }
        } else {
            idx = j;
            break;
        }
    }

    struct sockaddr_in sa;
    char hostbuf[NI_MAXHOST];
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = ip_net;

    int rc = getnameinfo((struct sockaddr *)&sa, sizeof(sa), hostbuf, sizeof(hostbuf), NULL, 0, NI_NAMEREQD);
    if (rc != 0) {
        ip_to_str(ip_net, hostbuf, sizeof(hostbuf));
    }

    for (unsigned int i = 0; i < CACHE_SIZE; ++i) {
        unsigned int j = (idx + i) & (CACHE_SIZE - 1);
        if (!dns_cache[j].used || dns_cache[j].ip == ip_net || (now - dns_cache[j].ts) > CACHE_TTL_SEC) {
            dns_cache[j].used = 1;
            dns_cache[j].ip = ip_net;
            dns_cache[j].ts = now;
            strncpy(dns_cache[j].name, hostbuf, sizeof(dns_cache[j].name) - 1);
            dns_cache[j].name[sizeof(dns_cache[j].name) - 1] = '\0';
            return dns_cache[j].name;
        }
    }

    dns_cache[idx].used = 1;
    dns_cache[idx].ip = ip_net;
    dns_cache[idx].ts = now;
    strncpy(dns_cache[idx].name, hostbuf, sizeof(dns_cache[idx].name) - 1);
    dns_cache[idx].name[sizeof(dns_cache[idx].name) - 1] = '\0';
    return dns_cache[idx].name;
}

/* ---------- process name lookup via /proc (best-effort) ---------- */

static int read_file_to_buf(const char *path, char *buf, size_t bufsz) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    if (!fgets(buf, (int)bufsz, f)) { fclose(f); return -1; }
    size_t len = strlen(buf);
    if (len && buf[len-1] == '\n') buf[len-1] = '\0';
    fclose(f);
    return 0;
}

static int find_inode_for_port(const char *proto, int port, char *inode_buf, size_t inode_bufsz) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/net/%s", proto);
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    char line[512];
    if (!fgets(line, sizeof(line), f)) { fclose(f); return -1; }

    while (fgets(line, sizeof(line), f)) {
        unsigned long inode = 0;
        char local_addr[64];
        int items = sscanf(line, "%*d: %63s %*s %*s %*s %*s %*s %*s %*s %lu", local_addr, &inode);
        if (items >= 2) {
            char *colon = strchr(local_addr, ':');
            if (!colon) continue;
            unsigned int p;
            if (sscanf(colon + 1, "%x", &p) != 1) continue;
            if ((int)p == port) {
                snprintf(inode_buf, inode_bufsz, "%lu", inode);
                fclose(f);
                return 0;
            }
        }
    }
    fclose(f);
    return -1;
}

static int find_pid_by_inode(const char *inode, char *procname_buf, size_t bufsz) {
    DIR *pdir = opendir("/proc");
    if (!pdir) return -1;
    struct dirent *dent;
    while ((dent = readdir(pdir)) != NULL) {
        char *endptr;
        long pid = strtol(dent->d_name, &endptr, 10);
        if (*endptr != '\0') continue;

        char fdpath[PATH_MAX];
        snprintf(fdpath, sizeof(fdpath), "/proc/%ld/fd", pid);
        DIR *fddir = opendir(fdpath);
        if (!fddir) continue;

        struct dirent *fdent;
        while ((fdent = readdir(fddir)) != NULL) {
            if (strcmp(fdent->d_name, ".") == 0 || strcmp(fdent->d_name, "..") == 0) continue;
            char linkpath[PATH_MAX];
            char target[PATH_MAX];
            snprintf(linkpath, sizeof(linkpath), "%s/%s", fdpath, fdent->d_name);
            ssize_t r = readlink(linkpath, target, sizeof(target)-1);
            if (r <= 0) continue;
            target[r] = '\0';
            char want[64];
            snprintf(want, sizeof(want), "socket:[%s]", inode);
            if (strcmp(target, want) == 0) {
                char commpath[PATH_MAX];
                snprintf(commpath, sizeof(commpath), "/proc/%ld/comm", pid);
                if (read_file_to_buf(commpath, procname_buf, bufsz) == 0) {
                    closedir(fddir);
                    closedir(pdir);
                    return 0;
                } else {
                    char cmdpath[PATH_MAX];
                    snprintf(cmdpath, sizeof(cmdpath), "/proc/%ld/cmdline", pid);
                    FILE *cf = fopen(cmdpath, "r");
                    if (cf) {
                        if (fgets(procname_buf, (int)bufsz, cf)) {
                            for (size_t i = 0; i < bufsz && procname_buf[i]; ++i) {
                                if (procname_buf[i] == '\0') procname_buf[i] = ' ';
                            }
                            fclose(cf);
                            closedir(fddir);
                            closedir(pdir);
                            return 0;
                        }
                        fclose(cf);
                    }
                    closedir(fddir);
                    closedir(pdir);
                    return -1;
                }
            }
        }
        closedir(fddir);
    }
    closedir(pdir);
    return -1;
}

static void get_process_name_for_local_port(const char *proto, int port, char *outbuf, size_t outbufsz) {
    char inode[64];
    if (find_inode_for_port(proto, port, inode, sizeof(inode)) == 0) {
        char pname[256];
        if (find_pid_by_inode(inode, pname, sizeof(pname)) == 0) {
            size_t len = strlen(pname);
            while (len && (pname[len-1] == '\n' || pname[len-1] == '\r')) { pname[len-1] = '\0'; len--; }
            strncpy(outbuf, pname, outbufsz-1);
            outbuf[outbufsz-1] = '\0';
            return;
        }
    }
    strncpy(outbuf, "-", outbufsz-1);
    outbuf[outbufsz-1] = '\0';
}

/* ---------- DNS parsing (very small decoder) ---------- */

static void parse_dns(const u_char *payload, size_t plen) {
    if (plen < 12) { printf("DNS truncated\n"); return; }
    uint16_t qdcount = ntohs(*(uint16_t *)(payload + 4));
    uint16_t ancount = ntohs(*(uint16_t *)(payload + 6));
    uint16_t flags = ntohs(*(uint16_t *)(payload + 2));
    int is_response = (flags & 0x8000) != 0;
    printf("DNS %s q=%u a=%u ", is_response ? "resp" : "qry", qdcount, ancount);

    /* parse first question name if present */
    size_t off = 12;
    if (qdcount > 0) {
        /* decode QNAME */
        char qname[256] = {0};
        size_t qpos = 0;
        while (off < plen) {
            uint8_t len = payload[off++];
            if (len == 0) break;
            if (len & 0xC0) { /* compression pointer - stop */
                if (off >= plen) break;
                uint8_t ptr = payload[off++];
                (void)ptr;
                break;
            }
            if (off + len > plen) break;
            if (qpos + len + 1 < sizeof(qname)) {
                memcpy(qname + qpos, payload + off, len);
                qpos += len;
                qname[qpos++] = '.';
            }
            off += len;
        }
        if (qpos && qname[qpos-1] == '.') qname[qpos-1] = '\0';
        /* skip qtype/qclass */
        if (off + 4 <= plen) off += 4;
        printf("Q=%s ", qname[0] ? qname : "-");
    }
    printf("\n");
}

/* ---------- HTTP parsing (very small) ---------- */

static int is_http_method(const char *s, size_t len) {
    const char *methods[] = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH", "CONNECT", "TRACE"};
    for (size_t i = 0; i < sizeof(methods)/sizeof(methods[0]); ++i) {
        size_t mlen = strlen(methods[i]);
        if (len >= mlen && strncmp(s, methods[i], mlen) == 0 && s[mlen] == ' ') return 1;
    }
    return 0;
}

static void parse_http_request(const u_char *payload, size_t plen) {
    /* look for request line and Host header */
    const char *p = (const char *)payload;
    const char *end = p + plen;
    /* find end of headers */
    const char *hdr_end = strstr(p, "\r\n\r\n");
    if (!hdr_end) return;
    size_t hdr_len = hdr_end - p;
    /* first line */
    const char *line_end = memchr(p, '\n', hdr_len);
    if (!line_end) return;
    size_t first_len = line_end - p;
    if (first_len > 0 && p[first_len-1] == '\r') first_len--;
    if (!is_http_method(p, first_len)) return;
    char reqline[512] = {0};
    strncpy(reqline, p, first_len < sizeof(reqline)-1 ? first_len : sizeof(reqline)-1);
    /* find Host header */
    const char *host = NULL;
    const char *cur = p + first_len + 1;
    while (cur < p + hdr_len) {
        const char *next = memchr(cur, '\n', p + hdr_len - cur);
        if (!next) break;
        size_t l = next - cur;
        if (l > 0 && cur[l-1] == '\r') l--;
        if (l >= 5 && strncasecmp(cur, "Host:", 5) == 0) {
            const char *val = cur + 5;
            while (val < next && (*val == ' ' || *val == '\t')) val++;
            static char hostbuf[256];
            size_t copylen = next - val;
            if (copylen > 0 && val[copylen-1] == '\r') copylen--;
            if (copylen >= sizeof(hostbuf)) copylen = sizeof(hostbuf)-1;
            memcpy(hostbuf, val, copylen);
            hostbuf[copylen] = '\0';
            host = hostbuf;
            break;
        }
        cur = next + 1;
    }
    printf("HTTP Request: %s", reqline);
    if (host) printf(" Host: %s", host);
    printf("\n");
}

/* ---------- TLS ClientHello SNI extraction (very small) ---------- */

static const char *parse_tls_sni(const u_char *payload, size_t plen, char *outbuf, size_t outbufsz) {
    /* Minimal parsing:
     * TLS record header: 5 bytes: type(1)=22 handshake, version(2), length(2)
     * Handshake: type(1)=1 ClientHello, length(3)
     * Skip to extensions and find extension type 0x0000 (server_name)
     */
    if (plen < 5) return NULL;
    const u_char *p = payload;
    uint8_t rec_type = p[0];
    if (rec_type != 22) return NULL; /* not handshake */
    uint16_t rec_len = (p[3] << 8) | p[4];
    if (5 + rec_len > plen) return NULL;
    p += 5;
    size_t remain = rec_len;
    if (remain < 4) return NULL;
    uint8_t hs_type = p[0];
    if (hs_type != 1) return NULL; /* not ClientHello */
    uint32_t hs_len = ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
    if (hs_len + 4 > remain) return NULL;
    p += 4;
    size_t hs_rem = hs_len;
    /* skip: client_version(2) + random(32) + session_id_len(1) + session_id + cipher_suites_len(2) + cipher_suites + comp_methods_len(1) + comp_methods */
    if (hs_rem < 2 + 32 + 1) return NULL;
    p += 2 + 32;
    hs_rem -= 34;
    if (hs_rem < 1) return NULL;
    uint8_t sid_len = p[0];
    p += 1;
    hs_rem -= 1;
    if (hs_rem < sid_len) return NULL;
    p += sid_len;
    hs_rem -= sid_len;
    if (hs_rem < 2) return NULL;
    uint16_t cs_len = (p[0] << 8) | p[1];
    p += 2;
    hs_rem -= 2;
    if (hs_rem < cs_len) return NULL;
    p += cs_len;
    hs_rem -= cs_len;
    if (hs_rem < 1) return NULL;
    uint8_t comp_len = p[0];
    p += 1;
    hs_rem -= 1;
    if (hs_rem < comp_len) return NULL;
    p += comp_len;
    hs_rem -= comp_len;
    /* now extensions: 2 bytes length */
    if (hs_rem < 2) return NULL;
    uint16_t ext_total = (p[0] << 8) | p[1];
    p += 2;
    hs_rem -= 2;
    if (hs_rem < ext_total) return NULL;
    const u_char *ext_end = p + ext_total;
    while (p + 4 <= ext_end) {
        uint16_t ext_type = (p[0] << 8) | p[1];
        uint16_t ext_len = (p[2] << 8) | p[3];
        p += 4;
        if (p + ext_len > ext_end) break;
        if (ext_type == 0x0000) { /* server_name */
            /* structure: list length(2) then entries: name_type(1) name_len(2) name */
            if (ext_len < 2) break;
            uint16_t list_len = (p[0] << 8) | p[1];
            const u_char *q = p + 2;
            const u_char *qend = p + ext_len;
            while (q + 3 <= qend) {
                uint8_t name_type = q[0];
                uint16_t name_len = (q[1] << 8) | q[2];
                q += 3;
                if (q + name_len > qend) break;
                if (name_type == 0) {
                    size_t copy = name_len < outbufsz-1 ? name_len : outbufsz-1;
                    memcpy(outbuf, q, copy);
                    outbuf[copy] = '\0';
                    return outbuf;
                }
                q += name_len;
            }
            break;
        }
        p += ext_len;
    }
    return NULL;
}

/* ---------- DHCP parsing (BOOTP + DHCP options) ---------- */

static void parse_dhcp(const u_char *payload, size_t plen) {
    /* BOOTP fixed header is 236 bytes, DHCP options start at offset 240 with magic cookie */
    if (plen < 240) { printf("DHCP truncated\n"); return; }
    const u_char *p = payload;
    uint8_t op = p[0];
    uint8_t htype = p[1];
    uint8_t hlen = p[2];
    uint32_t xid = ntohl(*(uint32_t *)(p + 4));
    uint32_t yiaddr = *(uint32_t *)(p + 16);
    const u_char *chaddr = p + 28;
    const u_char *options = payload + 240;
    size_t optlen = plen - 240;
    /* check magic cookie */
    if (payload[236] != 0x63 || payload[237] != 0x82 || payload[238] != 0x53 || payload[239] != 0x63) {
        printf("DHCP no cookie\n");
        return;
    }
    int msg_type = -1;
    char hostname[128] = {0};
    uint32_t req_ip = 0;
    size_t off = 0;
    while (off < optlen) {
        uint8_t code = options[off++];
        if (code == 0xff) break;
        if (code == 0x00) continue;
        if (off >= optlen) break;
        uint8_t len = options[off++];
        if (off + len > optlen) break;
        if (code == 53 && len == 1) { msg_type = options[off]; }
        else if (code == 12) { /* host name */
            size_t copy = len < sizeof(hostname)-1 ? len : sizeof(hostname)-1;
            memcpy(hostname, options + off, copy);
            hostname[copy] = '\0';
        } else if (code == 50 && len == 4) { /* requested IP */
            memcpy(&req_ip, options + off, 4);
        }
        off += len;
    }
    const char *mt = "-";
    if (msg_type == 1) mt = "DISCOVER";
    else if (msg_type == 2) mt = "OFFER";
    else if (msg_type == 3) mt = "REQUEST";
    else if (msg_type == 4) mt = "DECLINE";
    else if (msg_type == 5) mt = "ACK";
    else if (msg_type == 6) mt = "NAK";
    else if (msg_type == 7) mt = "RELEASE";
    else if (msg_type == 8) mt = "INFORM";
    char yi[INET_ADDRSTRLEN] = "-";
    char rip[INET_ADDRSTRLEN] = "-";
    if (yiaddr) { ip_to_str(yiaddr, yi, sizeof(yi)); }
    if (req_ip) { ip_to_str(req_ip, rip, sizeof(rip)); }
    printf("DHCP %s xid=0x%08x yi=%s req=%s host=%s\n", mt, xid, yi, rip, hostname[0] ? hostname : "-");
}

/* ---------- main packet parser ---------- */

static void parse_packet(const struct pcap_pkthdr *h, const u_char *bytes) {
    char procname[256] = "-";
    print_time(h);

    if (!ensure_bounds(h, 0, sizeof(struct ether_header))) {
        printf("%s truncated-eth\n", procname);
        return;
    }
    const struct ether_header *eth = (const struct ether_header *)bytes;
    uint16_t eth_type = ntohs(eth->ether_type);

    if (eth_type == ETHERTYPE_IP) {
        size_t ip_off = sizeof(struct ether_header);
        if (!ensure_bounds(h, ip_off, sizeof(struct ip))) {
            printf("%s truncated-ip\n", procname);
            return;
        }
        const struct ip *ip = (const struct ip *)(bytes + ip_off);
        int ihl = ip->ip_hl * 4;
        if (ihl < 20) { printf("%s bad-ip-hdrlen\n", procname); return; }
        if (!ensure_bounds(h, ip_off, ihl)) { printf("%s truncated-ip-hdr\n", procname); return; }

        uint32_t src_ip = ip->ip_src.s_addr;
        uint32_t dst_ip = ip->ip_dst.s_addr;
        const char *src_name = resolve_hostname_ipv4(src_ip);
        const char *dst_name = resolve_hostname_ipv4(dst_ip);

        if (ip->ip_p == IPPROTO_TCP) {
            size_t tcp_off = ip_off + ihl;
            if (!ensure_bounds(h, tcp_off, sizeof(struct tcphdr))) { printf("%s trunc-tcp\n", procname); return; }
            const struct tcphdr *tcp = (const struct tcphdr *)(bytes + tcp_off);
            unsigned short sport = ntohs(tcp->th_sport);
            unsigned short dport = ntohs(tcp->th_dport);
            int tcp_hdr_len = tcp->th_off * 4;
            size_t payload_off = tcp_off + tcp_hdr_len;
            size_t payload_len = 0;
            if (h->caplen > payload_off) payload_len = h->caplen - payload_off;
            const u_char *payload = bytes + payload_off;

            /* try to identify HTTP (port 80) */
            if (dport == 80 || sport == 80) {
                /* attempt HTTP request parse if payload starts with method */
                if (payload_len > 0 && payload_len < 65536) {
                    if (is_http_method((const char *)payload, payload_len)) {
                        get_process_name_for_local_port("tcp", dport, procname, sizeof(procname));
                        printf("%s HTTP %s -> %s %u -> %u ", procname, src_name, dst_name, sport, dport);
                        parse_http_request(payload, payload_len);
                        return;
                    }
                }
            }

            /* try TLS ClientHello SNI (port 443) */
            if (dport == 443 || sport == 443) {
                char sni[256];
                const char *s = parse_tls_sni(payload, payload_len, sni, sizeof(sni));
                if (s) {
                    get_process_name_for_local_port("tcp", dport, procname, sizeof(procname));
                    printf("%s TLS SNI %s -> %s %u -> %u SNI=%s len=%u\n", procname, src_name, dst_name, sport, dport, s, h->len);
                    return;
                }
            }

            /* default TCP summary */
            get_process_name_for_local_port("tcp", dport, procname, sizeof(procname));
            printf("%s IP %s -> %s TCP %u -> %u len %u\n", procname, src_name, dst_name, sport, dport, h->len);
        } else if (ip->ip_p == IPPROTO_UDP) {
            size_t udp_off = ip_off + ihl;
            if (!ensure_bounds(h, udp_off, sizeof(struct udphdr))) { printf("%s trunc-udp\n", procname); return; }
            const struct udphdr *udp = (const struct udphdr *)(bytes + udp_off);
            unsigned short sport = ntohs(udp->uh_sport);
            unsigned short dport = ntohs(udp->uh_dport);
            size_t payload_off = udp_off + sizeof(struct udphdr);
            size_t payload_len = 0;
            if (h->caplen > payload_off) payload_len = h->caplen - payload_off;
            const u_char *payload = bytes + payload_off;

            /* DNS (port 53) */
            if (dport == 53 || sport == 53) {
                get_process_name_for_local_port("udp", dport, procname, sizeof(procname));
                printf("%s %s -> %s UDP %u -> %u ", procname, src_name, dst_name, sport, dport);
                parse_dns(payload, payload_len);
                return;
            }

            /* DHCP (ports 67/68) */
            if ((dport == 67 || dport == 68 || sport == 67 || sport == 68)) {
                get_process_name_for_local_port("udp", dport, procname, sizeof(procname));
                printf("%s %s -> %s UDP %u -> %u ", procname, src_name, dst_name, sport, dport);
                parse_dhcp(payload, payload_len);
                return;
            }

            get_process_name_for_local_port("udp", dport, procname, sizeof(procname));
            printf("%s IP %s -> %s UDP %u -> %u len %u\n", procname, src_name, dst_name, sport, dport, h->len);
        } else {
            printf("%s %s -> %s PROTO %u len %u\n", procname, resolve_hostname_ipv4(src_ip), resolve_hostname_ipv4(dst_ip), ip->ip_p, h->len);
        }
    } else {
        printf("- ETH 0x%04x len %u\n", eth_type, h->len);
    }
}

/* pcap callback */
static void pcap_cb(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
    (void)user;
    parse_packet(h, bytes);
}

/* ---------- open device ---------- */

static pcap_t *open_device(const char *dev, const char *filter_expr) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(dev, SNAPLEN, PROMISC, TIMEOUT_MS, errbuf);
    if (!handle) {
        fprintf(stderr, "pcap_open_live failed: %s\n", errbuf);
        return NULL;
    }
    if (filter_expr && filter_expr[0] != '\0') {
        struct bpf_program fp;
        if (pcap_compile(handle, &fp, filter_expr, 1, PCAP_NETMASK_UNKNOWN) == -1 ||
            pcap_setfilter(handle, &fp) == -1) {
            fprintf(stderr, "filter error: %s\n", pcap_geterr(handle));
            pcap_freecode(&fp);
            pcap_close(handle);
            return NULL;
        }
        pcap_freecode(&fp);
    }
    return handle;
}

/* ---------- main ---------- */

int main(int argc, char **argv) {
    const char *default_dev = "wlan0";
    const char *default_filter = ""; /* user requested empty default filter */

    const char *dev = NULL;
    const char *filter = NULL;

    if ((CACHE_SIZE & (CACHE_SIZE - 1)) != 0) {
        fprintf(stderr, "CACHE_SIZE must be a power of two\n");
        return 1;
    }

    if (argc == 1) {
        dev = default_dev;
        filter = default_filter;
    } else if (argc == 2) {
        dev = argv[1];
        filter = default_filter;
    } else {
        dev = argv[1];
        filter = argv[2];
    }

    signal(SIGINT, handle_sigint);
    pcap_t *handle = open_device(dev, filter);
    if (!handle) return 2;
    global_handle = handle;

    printf("Listening on %s filter=%s\n", dev, (filter && filter[0]) ? filter : "none");
    if (pcap_loop(handle, -1, pcap_cb, NULL) == -1) {
        fprintf(stderr, "pcap_loop error: %s\n", pcap_geterr(handle));
    }

    pcap_close(handle);
    printf("Exiting\n");
    return 0;
}
