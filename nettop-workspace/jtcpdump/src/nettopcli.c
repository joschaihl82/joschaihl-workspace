/* tinydump_expanded.c
 * Combined single-file chunk: CLI, capture, basic parser, and graceful shutdown.
 * This is the first part of a larger ~1000 line project.
 */

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

/* Packet parser: Ethernet + IPv4 + TCP/UDP minimal */
static void parse_packet(const struct pcap_pkthdr *h, const u_char *bytes) {
    print_time(h);

    if (!ensure_bounds(h, 0, sizeof(struct ether_header))) {
        printf("truncated-eth\n");
        return;
    }
    const struct ether_header *eth = (const struct ether_header *)bytes;
    uint16_t eth_type = ntohs(eth->ether_type);

    if (eth_type == ETHERTYPE_IP) {
        size_t ip_off = sizeof(struct ether_header);
        if (!ensure_bounds(h, ip_off, sizeof(struct ip))) {
            printf("truncated-ip\n");
            return;
        }
        const struct ip *ip = (const struct ip *)(bytes + ip_off);
        int ihl = ip->ip_hl * 4;
        if (ihl < 20) { printf("bad-ip-hdrlen\n"); return; }
        if (!ensure_bounds(h, ip_off, ihl)) { printf("truncated-ip-hdr\n"); return; }

        char src[INET_ADDRSTRLEN], dst[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip->ip_src, src, sizeof(src));
        inet_ntop(AF_INET, &ip->ip_dst, dst, sizeof(dst));
        printf("IP %s -> %s ", src, dst);

        if (ip->ip_p == IPPROTO_TCP) {
            size_t tcp_off = ip_off + ihl;
            if (!ensure_bounds(h, tcp_off, sizeof(struct tcphdr))) { printf("trunc-tcp\n"); return; }
            const struct tcphdr *tcp = (const struct tcphdr *)(bytes + tcp_off);
            printf("TCP %u -> %u", ntohs(tcp->th_sport), ntohs(tcp->th_dport));
        } else if (ip->ip_p == IPPROTO_UDP) {
            size_t udp_off = ip_off + ihl;
            if (!ensure_bounds(h, udp_off, sizeof(struct udphdr))) { printf("trunc-udp\n"); return; }
            const struct udphdr *udp = (const struct udphdr *)(bytes + udp_off);
            printf("UDP %u -> %u", ntohs(udp->uh_sport), ntohs(udp->uh_dport));
        } else {
            printf("PROTO %u", ip->ip_p);
        }
    } else {
        printf("ETH 0x%04x", eth_type);
    }
    printf(" len %u\n", h->len);
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
