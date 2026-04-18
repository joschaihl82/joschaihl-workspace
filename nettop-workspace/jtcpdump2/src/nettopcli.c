/*
 * tinydump_procname.c
 * Tiny tcpdump-like sniffer that prepends the process name owning the local port.
 *
 * Build:
 *   gcc -O2 -Wall -o tinydump_procname tinydump_procname.c -lpcap
 *
 * Usage:
 *   sudo ./tinydump_procname <interface> [filter]
 *
 * Notes:
 * - This looks up process ownership by scanning /proc/net/{tcp,udp} and /proc/<pid>/fd.
 * - It's a best-effort approach and may be slow if called per-packet; suitable for demo.
 */

#define _GNU_SOURCE
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
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

static volatile sig_atomic_t stop_requested = 0;
static pcap_t *global_handle = NULL;

static void handle_sigint(int sig) {
    (void)sig;
    stop_requested = 1;
    if (global_handle) pcap_breakloop(global_handle);
}

/* Utility: print timestamp */
static void print_time(const struct pcap_pkthdr *h) {
    char buf[64];
    time_t t = h->ts.tv_sec;
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    printf("%s.%06ld ", buf, (long)h->ts.tv_usec);
}

/* Defensive bounds check helper */
static int ensure_bounds(const struct pcap_pkthdr *h, size_t offset, size_t need) {
    if (h->caplen < offset + need) return 0;
    return 1;
}

/* Read a whole file into buffer (small files only) */
static int read_file_to_buf(const char *path, char *buf, size_t bufsz) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    if (!fgets(buf, (int)bufsz, f)) { fclose(f); return -1; }
    /* strip newline */
    size_t len = strlen(buf);
    if (len && buf[len-1] == '\n') buf[len-1] = '\0';
    fclose(f);
    return 0;
}

/* Parse hex port from /proc/net/* lines: "0100007F:1F90" -> port 8080 */
static int hexstr_to_port(const char *hexstr) {
    unsigned int port;
    if (sscanf(hexstr, "%*[^:]:%x", &port) == 1) return (int)port;
    return -1;
}

/* Find inode(s) for a given local port in /proc/net/{tcp,udp}
 * Returns the first matching inode as string in inode_buf (caller-supplied).
 * inode_buf must be at least 32 bytes.
 * proto: "tcp" or "udp"
 */
static int find_inode_for_port(const char *proto, int port, char *inode_buf, size_t inode_bufsz) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/net/%s", proto);
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    char line[512];
    /* skip header */
    if (!fgets(line, sizeof(line), f)) { fclose(f); return -1; }

    while (fgets(line, sizeof(line), f)) {
        /* fields: sl local_address rem_address st tx_queue rx_queue tr tm->when retrnsmt uid timeout inode ... */
        /* We extract local_address (field 2) and inode (field 10 or later) */
        char local_addr[64], rem_addr[64], rest[256];
        unsigned long inode = 0;
        int items = sscanf(line, "%*d: %63s %63s %*s %*s %*s %*s %*s %*s %lu", local_addr, rem_addr, &inode);
        if (items >= 3) {
            /* local_addr is like "0100007F:1F90" */
            /* extract port */
            char *colon = strchr(local_addr, ':');
            if (!colon) continue;
            unsigned int p;
            if (sscanf(colon + 1, "%x", &p) != 1) continue;
            if ((int)p == port) {
                snprintf(inode_buf, inode_bufsz, "%lu", inode);
                fclose(f);
                return 0;
            }
        } else {
            /* fallback parse: tokenise */
            char *tok = strtok(line, " ");
            int field = 0;
            char local[64] = {0};
            char inode_s[32] = {0};
            while (tok) {
                field++;
                if (field == 2) strncpy(local, tok, sizeof(local)-1);
                if (field == 10) strncpy(inode_s, tok, sizeof(inode_s)-1);
                tok = strtok(NULL, " ");
            }
            if (local[0] && inode_s[0]) {
                char *c = strchr(local, ':');
                if (c) {
                    unsigned int p;
                    if (sscanf(c+1, "%x", &p) == 1 && (int)p == port) {
                        strncpy(inode_buf, inode_s, inode_bufsz-1);
                        fclose(f);
                        return 0;
                    }
                }
            }
        }
    }
    fclose(f);
    return -1;
}

/* Given an inode string like "12345", scan /proc/<pid>/fd/* symlinks to find a symlink "socket:[12345]".
 * If found, read /proc/<pid>/comm into procname_buf.
 * procname_buf must be at least bufsz bytes.
 * Returns 0 on success, -1 on not found.
 */
static int find_pid_by_inode(const char *inode, char *procname_buf, size_t bufsz) {
    DIR *pdir = opendir("/proc");
    if (!pdir) return -1;
    struct dirent *dent;
    while ((dent = readdir(pdir)) != NULL) {
        /* skip non-numeric entries */
        char *endptr;
        long pid = strtol(dent->d_name, &endptr, 10);
        if (*endptr != '\0') continue;

        /* build fd dir path */
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
            /* target may be like "socket:[12345]" */
            char want[64];
            snprintf(want, sizeof(want), "socket:[%s]", inode);
            if (strcmp(target, want) == 0) {
                /* read /proc/<pid>/comm */
                char commpath[PATH_MAX];
                snprintf(commpath, sizeof(commpath), "/proc/%ld/comm", pid);
                if (read_file_to_buf(commpath, procname_buf, bufsz) == 0) {
                    closedir(fddir);
                    closedir(pdir);
                    return 0;
                } else {
                    /* try /proc/<pid>/cmdline as fallback */
                    char cmdpath[PATH_MAX];
                    snprintf(cmdpath, sizeof(cmdpath), "/proc/%ld/cmdline", pid);
                    FILE *cf = fopen(cmdpath, "r");
                    if (cf) {
                        if (fgets(procname_buf, (int)bufsz, cf)) {
                            /* cmdline is NUL-separated; replace NUL with space */
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

/* High-level: given protocol and port, return process name (caller buffer) or "-" */
static void get_process_name_for_local_port(const char *proto, int port, char *outbuf, size_t outbufsz) {
    char inode[64];
    if (find_inode_for_port(proto, port, inode, sizeof(inode)) == 0) {
        char pname[256];
        if (find_pid_by_inode(inode, pname, sizeof(pname)) == 0) {
            /* trim whitespace */
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

/* Packet parser: Ethernet + IPv4 + TCP/UDP minimal, prints process name first */
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

        char src[INET_ADDRSTRLEN], dst[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip->ip_src, src, sizeof(src));
        inet_ntop(AF_INET, &ip->ip_dst, dst, sizeof(dst));

        /* Determine local port: if packet is from remote->local or local->remote?
         * For simplicity we try to find owner of destination port first (server), then source port.
         */
        if (ip->ip_p == IPPROTO_TCP) {
            size_t tcp_off = ip_off + ihl;
            if (!ensure_bounds(h, tcp_off, sizeof(struct tcphdr))) { printf("%s trunc-tcp\n", procname); return; }
            const struct tcphdr *tcp = (const struct tcphdr *)(bytes + tcp_off);
            unsigned short sport = ntohs(tcp->th_sport);
            unsigned short dport = ntohs(tcp->th_dport);

            /* try destination port first */
            get_process_name_for_local_port("tcp", dport, procname, sizeof(procname));
            printf("%s IP %s -> %s TCP %u -> %u len %u\n", procname, src, dst, sport, dport, h->len);
        } else if (ip->ip_p == IPPROTO_UDP) {
            size_t udp_off = ip_off + ihl;
            if (!ensure_bounds(h, udp_off, sizeof(struct udphdr))) { printf("%s trunc-udp\n", procname); return; }
            const struct udphdr *udp = (const struct udphdr *)(bytes + udp_off);
            unsigned short sport = ntohs(udp->uh_sport);
            unsigned short dport = ntohs(udp->uh_dport);

            get_process_name_for_local_port("udp", dport, procname, sizeof(procname));
            printf("%s IP %s -> %s UDP %u -> %u len %u\n", procname, src, dst, sport, dport, h->len);
        } else {
            printf("%s IP %s -> %s PROTO %u len %u\n", procname, src, dst, ip->ip_p, h->len);
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

/* Main: simple CLI */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <interface> [filter]\n", argv[0]);
        return 1;
    }
    const char *dev = argv[1];
    const char *filter = (argc >= 3) ? argv[2] : NULL;

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
