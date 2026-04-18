/*
 * tinydump_hostnames.c
 * Tiny tcpdump-like sniffer that prepends the process name owning the local port
 * and prints hostnames instead of IP addresses with a small reverse-DNS cache.
 *
 * Build:
 *   gcc -O2 -Wall -o tinydump_hostnames tinydump_hostnames.c -lpcap
 *
 * Usage:
 *   sudo ./tinydump_hostnames <interface> [filter]
 *
 * Notes:
 * - Focuses on IPv4 packets.
 * - Uses /proc scanning to map local ports to process names (best-effort).
 * - Uses a simple fixed-size cache for reverse DNS lookups.
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

/* Timestamp printing */
static void print_time(const struct pcap_pkthdr *h) {
    char buf[64];
    time_t t = h->ts.tv_sec;
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    printf("%s.%06ld ", buf, (long)h->ts.tv_usec);
}

/* Bounds check helper */
static int ensure_bounds(const struct pcap_pkthdr *h, size_t offset, size_t need) {
    if (h->caplen < offset + need) return 0;
    return 1;
}

/* Simple reverse DNS cache entry */
struct dns_cache_entry {
    uint32_t ip;               /* network byte order IPv4 address */
    char name[NI_MAXHOST];     /* resolved hostname or numeric string */
    time_t ts;                 /* last update time */
    int used;
};

/* Fixed-size hash table with linear probing */
static struct dns_cache_entry dns_cache[CACHE_SIZE];

/* Hash IPv4 address to index */
static inline unsigned int ip_hash(uint32_t ip) {
    /* simple multiplicative hash */
    return (unsigned int)((ip * 2654435761u) & (CACHE_SIZE - 1));
}

/* Convert IPv4 address to numeric string */
static void ip_to_str(uint32_t ip_net, char *buf, size_t bufsz) {
    struct in_addr a;
    a.s_addr = ip_net;
    inet_ntop(AF_INET, &a, buf, (socklen_t)bufsz);
}

/* Lookup hostname in cache or perform reverse lookup */
static const char *resolve_hostname_ipv4(uint32_t ip_net) {
    if (ip_net == 0) return "-";
    unsigned int idx = ip_hash(ip_net);
    time_t now = time(NULL);

    /* search with linear probing */
    for (unsigned int i = 0; i < CACHE_SIZE; ++i) {
        unsigned int j = (idx + i) & (CACHE_SIZE - 1);
        if (dns_cache[j].used) {
            if (dns_cache[j].ip == ip_net) {
                /* check TTL */
                if (now - dns_cache[j].ts <= CACHE_TTL_SEC) {
                    return dns_cache[j].name;
                } else {
                    /* expired: fall through to refresh */
                    break;
                }
            }
        } else {
            /* empty slot: will use it */
            idx = j;
            break;
        }
    }

    /* perform reverse lookup */
    struct sockaddr_in sa;
    char hostbuf[NI_MAXHOST];
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = ip_net;

    int rc = getnameinfo((struct sockaddr *)&sa, sizeof(sa), hostbuf, sizeof(hostbuf), NULL, 0, NI_NAMEREQD);
    if (rc != 0) {
        /* fallback to numeric */
        ip_to_str(ip_net, hostbuf, sizeof(hostbuf));
    }

    /* store in cache at idx (linear probe to find a slot) */
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

    /* if cache full and no slot found, overwrite initial idx */
    dns_cache[idx].used = 1;
    dns_cache[idx].ip = ip_net;
    dns_cache[idx].ts = now;
    strncpy(dns_cache[idx].name, hostbuf, sizeof(dns_cache[idx].name) - 1);
    dns_cache[idx].name[sizeof(dns_cache[idx].name) - 1] = '\0';
    return dns_cache[idx].name;
}

/* Read a whole file into buffer (small files only) */
static int read_file_to_buf(const char *path, char *buf, size_t bufsz) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    if (!fgets(buf, (int)bufsz, f)) { fclose(f); return -1; }
    size_t len = strlen(buf);
    if (len && buf[len-1] == '\n') buf[len-1] = '\0';
    fclose(f);
    return 0;
}

/* Find inode for local port in /proc/net/{tcp,udp} */
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

/* Find process name by scanning /proc/<pid>/fd for socket:[inode] */
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

/* High-level: map proto+port to process name or "-" */
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

/* Packet parser prints process name then hostnames for src and dst */
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

            get_process_name_for_local_port("tcp", dport, procname, sizeof(procname));
            printf("%s %s -> %s TCP %u -> %u len %u\n", procname, src_name, dst_name, sport, dport, h->len);
        } else if (ip->ip_p == IPPROTO_UDP) {
            size_t udp_off = ip_off + ihl;
            if (!ensure_bounds(h, udp_off, sizeof(struct udphdr))) { printf("%s trunc-udp\n", procname); return; }
            const struct udphdr *udp = (const struct udphdr *)(bytes + udp_off);
            unsigned short sport = ntohs(udp->uh_sport);
            unsigned short dport = ntohs(udp->uh_dport);

            get_process_name_for_local_port("udp", dport, procname, sizeof(procname));
            printf("%s %s -> %s UDP %u -> %u len %u\n", procname, src_name, dst_name, sport, dport, h->len);
        } else {
            printf("%s %s -> %s PROTO %u len %u\n", procname, src_name, dst_name, ip->ip_p, h->len);
        }
    } else {
        printf("%s ETH 0x%04x len %u\n", procname, eth_type, h->len);
    }
}

/* pcap callback */
static void pcap_cb(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
    (void)user;
    parse_packet(h, bytes);
}

/* Open device and apply filter if provided */
static pcap_t *open_device(const char *dev, const char *filter_expr) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(dev, SNAPLEN, PROMISC, TIMEOUT_MS, errbuf);
    if (!handle) {
        fprintf(stderr, "pcap_open_live failed: %s\n", errbuf);
        return NULL;
    }
    if (filter_expr) {
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

/* Main */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <interface> [filter]\n", argv[0]);
        return 1;
    }
    const char *dev = argv[1];
    const char *filter = (argc >= 3) ? argv[2] : NULL;

    /* ensure CACHE_SIZE is power of two */
    if ((CACHE_SIZE & (CACHE_SIZE - 1)) != 0) {
        fprintf(stderr, "CACHE_SIZE must be a power of two\n");
        return 1;
    }

    signal(SIGINT, handle_sigint);
    pcap_t *handle = open_device(dev, filter);
    if (!handle) return 2;
    global_handle = handle;

    printf("Listening on %s filter=%s\n", dev, filter ? filter : "none");
    if (pcap_loop(handle, -1, pcap_cb, NULL) == -1) {
        fprintf(stderr, "pcap_loop error: %s\n", pcap_geterr(handle));
    }

    pcap_close(handle);
    printf("Exiting\n");
    return 0;
}
