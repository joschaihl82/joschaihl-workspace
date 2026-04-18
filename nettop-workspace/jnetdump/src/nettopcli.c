/*
 * netdump.c
 * Compile: gcc -O2 -o netdump netdump.c -lpcap -pthread
 * Run: sudo ./netdump
 *
 * Captures on wlan0, aggregates per-PID network usage (TCP/UDP IPv4).
 * Uses /proc/net/{tcp,udp} to map 4-tuples -> socket inode, and scans
 * /proc/fd to map inode -> pid + process name. Prints a table every second.
 *
 * This is a best-effort tool and not as complete as nethogs, but it's
 * functional and reasonably efficient for small systems.
 **/

#define _GNU_SOURCE
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#define IFACE "wlan0"
#define SNAPLEN 65536
#define PROMISC 1
#define TIMEOUT_MS 1000
#define CONN_HASH_SIZE 65536
#define INODE_HASH_SIZE 65536
#define PID_HASH_SIZE 4096
#define MAX_DISPLAY 40

/* connection key (IPv4) */
typedef struct {
    uint32_t saddr;
    uint32_t daddr;
    uint16_t sport;
    uint16_t dport;
    uint8_t proto; /* 6=tcp,17=udp */
} conn_key_t;

/* connection -> inode mapping entry */
typedef struct conn_entry {
    conn_key_t key;
    unsigned long inode;
    struct conn_entry *next;
} conn_entry_t;

/* inode -> pid mapping entry */
typedef struct inode_entry {
    unsigned long inode;
    unsigned long pid;
    char name[128];
    struct inode_entry *next;
} inode_entry_t;

/* per-pid stats */
typedef struct pid_stat {
    unsigned long pid;
    char name[128];
    unsigned long long in_bytes;
    unsigned long long out_bytes;
    unsigned long long last_in;
    unsigned long long last_out;
    struct pid_stat *next;
} pid_stat_t;

/* global maps */
static conn_entry_t *conn_table[CONN_HASH_SIZE];
static inode_entry_t *inode_table[INODE_HASH_SIZE];
static pid_stat_t *pid_table[PID_HASH_SIZE];

/* locks */
static pthread_mutex_t maps_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t stats_lock = PTHREAD_MUTEX_INITIALIZER;

/* pcap handle */
static pcap_t *pcap_handle = NULL;

/* helpers: hashing */
static inline unsigned int conn_hash(const conn_key_t *k) {
    /* simple mixing */
    unsigned int h = k->saddr ^ (k->daddr << 1) ^ ((k->sport << 16) | k->dport) ^ (k->proto * 0x9e3779b1);
    return h % CONN_HASH_SIZE;
}
static inline unsigned int inode_hash(unsigned long inode) {
    return (unsigned int)((inode ^ (inode >> 16)) % INODE_HASH_SIZE);
}
static inline unsigned int pid_hash(unsigned long pid) {
    return (unsigned int)(pid % PID_HASH_SIZE);
}

/* clear conn table */
static void clear_conn_table() {
    for (int i = 0; i < CONN_HASH_SIZE; ++i) {
        conn_entry_t *e = conn_table[i];
        while (e) {
            conn_entry_t *n = e->next;
            free(e);
            e = n;
        }
        conn_table[i] = NULL;
    }
}

/* add conn -> inode */
static void add_conn_map(const conn_key_t *k, unsigned long inode) {
    unsigned int h = conn_hash(k);
    conn_entry_t *e = malloc(sizeof(conn_entry_t));
    if (!e) return;
    e->key = *k;
    e->inode = inode;
    e->next = conn_table[h];
    conn_table[h] = e;
}

/* lookup inode by conn key */
static unsigned long lookup_conn_inode(const conn_key_t *k) {
    unsigned int h = conn_hash(k);
    for (conn_entry_t *e = conn_table[h]; e; e = e->next) {
        if (e->key.saddr == k->saddr && e->key.daddr == k->daddr &&
            e->key.sport == k->sport && e->key.dport == k->dport &&
            e->key.proto == k->proto) {
            return e->inode;
        }
    }
    return 0;
}

/* clear inode table */
static void clear_inode_table() {
    for (int i = 0; i < INODE_HASH_SIZE; ++i) {
        inode_entry_t *e = inode_table[i];
        while (e) {
            inode_entry_t *n = e->next;
            free(e);
            e = n;
        }
        inode_table[i] = NULL;
    }
}

/* add inode -> pid,name */
static void add_inode_map(unsigned long inode, unsigned long pid, const char *name) {
    unsigned int h = inode_hash(inode);
    inode_entry_t *e = malloc(sizeof(inode_entry_t));
    if (!e) return;
    e->inode = inode;
    e->pid = pid;
    strncpy(e->name, name ? name : "?", sizeof(e->name)-1);
    e->name[sizeof(e->name)-1] = '\0';
    e->next = inode_table[h];
    inode_table[h] = e;
}

/* lookup pid,name by inode */
static unsigned long lookup_inode_pid(unsigned long inode, char *name_out, size_t nlen) {
    unsigned int h = inode_hash(inode);
    for (inode_entry_t *e = inode_table[h]; e; e = e->next) {
        if (e->inode == inode) {
            if (name_out && nlen) strncpy(name_out, e->name, nlen-1);
            if (name_out && nlen) name_out[nlen-1] = '\0';
            return e->pid;
        }
    }
    return 0;
}

/* pid stats helpers */
static pid_stat_t *get_pid_stat(unsigned long pid) {
    unsigned int h = pid_hash(pid);
    for (pid_stat_t *p = pid_table[h]; p; p = p->next) {
        if (p->pid == pid) return p;
    }
    pid_stat_t *n = calloc(1, sizeof(pid_stat_t));
    if (!n) return NULL;
    n->pid = pid;
    n->in_bytes = n->out_bytes = n->last_in = n->last_out = 0;
    n->name[0] = '\0';
    n->next = pid_table[h];
    pid_table[h] = n;
    return n;
}

/* parse /proc/net/{tcp,udp} and build conn_table */
static void build_conn_map_from_proc() {
    /* Rebuild conn_table from /proc/net/tcp and /proc/net/udp (IPv4) */
    clear_conn_table();

    const char *files[] = { "/proc/net/tcp", "/proc/net/udp", NULL };
    for (int fi = 0; files[fi]; ++fi) {
        FILE *f = fopen(files[fi], "r");
        if (!f) continue;
        char line[512];
        /* skip header */
        if (!fgets(line, sizeof(line), f)) { fclose(f); continue; }
        while (fgets(line, sizeof(line), f)) {
            /* Format: sl: local_address rem_address st tx_queue rx_queue tr tm->when retrnsmt uid timeout inode ... */
            char local[64], rem[64];
            unsigned int state;
            unsigned long inode;
            if (sscanf(line, "%*d: %63s %63s %x %*s %*s %*s %*s %*s %lu", local, rem, &state, &inode) < 4) continue;
            unsigned int la, lp, ra, rp;
            if (sscanf(local, "%x:%x", &la, &lp) != 2) continue;
            if (sscanf(rem, "%x:%x", &ra, &rp) != 2) continue;
            conn_key_t k;
            k.saddr = ntohl(la);
            k.daddr = ntohl(ra);
            k.sport = ntohs((uint16_t)lp);
            k.dport = ntohs((uint16_t)rp);
            k.proto = (files[fi][strlen(files[fi]) - 3] == 't') ? 6 : 17;
            add_conn_map(&k, inode);
        }
        fclose(f);
    }
}

/* build inode -> pid map by scanning /proc/<pid>/fd once */
static void build_inode_pid_map() {
    clear_inode_table();

    DIR *proc = opendir("/proc");
    if (!proc) return;
    struct dirent *de;
    while ((de = readdir(proc)) != NULL) {
        /* only numeric dirs */
        char *endptr;
        long pid = strtol(de->d_name, &endptr, 10);
        if (*endptr != '\0') continue;
        /* open /proc/<pid>/fd */
        char fdpath[256];
        snprintf(fdpath, sizeof(fdpath), "/proc/%ld/fd", pid);
        DIR *fdd = opendir(fdpath);
        if (!fdd) continue;
        struct dirent *fde;
        while ((fde = readdir(fdd)) != NULL) {
            if (fde->d_name[0] == '.') continue;
            char linkpath[512];
            char target[512];
            snprintf(linkpath, sizeof(linkpath), "%s/%s", fdpath, fde->d_name);
            ssize_t r = readlink(linkpath, target, sizeof(target)-1);
            if (r <= 0) continue;
            target[r] = '\0';
            /* socket:[12345] */
            unsigned long inode = 0;
            if (sscanf(target, "socket:[%lu]", &inode) == 1) {
                /* get process name from /proc/<pid>/comm */
                char commpath[256];
                char pname[128] = "?";
                snprintf(commpath, sizeof(commpath), "/proc/%ld/comm", pid);
                FILE *cf = fopen(commpath, "r");
                if (cf) {
                    if (fgets(pname, sizeof(pname), cf)) {
                        pname[strcspn(pname, "\n")] = '\0';
                    }
                    fclose(cf);
                }
                add_inode_map(inode, (unsigned long)pid, pname);
            }
        }
        closedir(fdd);
    }
    closedir(proc);
}

/* refresh maps (conn and inode->pid) under maps_lock */
static void refresh_maps() {
    pthread_mutex_lock(&maps_lock);
    build_conn_map_from_proc();
    build_inode_pid_map();
    pthread_mutex_unlock(&maps_lock);
}

/* packet handler */
static void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
    (void)user;
    if (h->caplen < 14) return; /* need ethernet header */
    const u_char *eth = bytes;
    uint16_t ethertype = (eth[12] << 8) | eth[13];
    if (ethertype != 0x0800) return; /* only IPv4 here */

    const struct ip *ip = (const struct ip *)(bytes + 14);
    if (ip->ip_v != 4) return;
    int iphdr = ip->ip_hl * 4;
    if (h->caplen < 14 + iphdr) return;

    conn_key_t k;
    k.saddr = ntohl(ip->ip_src.s_addr);
    k.daddr = ntohl(ip->ip_dst.s_addr);
    k.proto = ip->ip_p;
    uint16_t sport = 0, dport = 0;
    if (k.proto == IPPROTO_TCP) {
        if (h->caplen < 14 + iphdr + sizeof(struct tcphdr)) return;
        const struct tcphdr *tcp = (const struct tcphdr *)(bytes + 14 + iphdr);
        sport = ntohs(tcp->th_sport);
        dport = ntohs(tcp->th_dport);
    } else if (k.proto == IPPROTO_UDP) {
        if (h->caplen < 14 + iphdr + sizeof(struct udphdr)) return;
        const struct udphdr *udp = (const struct udphdr *)(bytes + 14 + iphdr);
        sport = ntohs(udp->uh_sport);
        dport = ntohs(udp->uh_dport);
    } else {
        return;
    }
    k.sport = sport;
    k.dport = dport;

    unsigned long inode = 0;
    unsigned long pid = 0;
    char pname[128] = "?";
    int incoming = 0;

    pthread_mutex_lock(&maps_lock);
    inode = lookup_conn_inode(&k);
    if (!inode) {
        /* try reverse direction (packet might be incoming) */
        conn_key_t r = k;
        r.saddr = k.daddr; r.daddr = k.saddr; r.sport = k.dport; r.dport = k.sport;
        inode = lookup_conn_inode(&r);
        if (inode) incoming = 1;
    }
    if (inode) {
        pid = lookup_inode_pid(inode, pname, sizeof(pname));
    }
    pthread_mutex_unlock(&maps_lock);

    if (!pid) return;

    /* update stats */
    pthread_mutex_lock(&stats_lock);
    pid_stat_t *ps = get_pid_stat(pid);
    if (!ps) { pthread_mutex_unlock(&stats_lock); return; }
    if (ps->name[0] == '\0') strncpy(ps->name, pname, sizeof(ps->name)-1);
    if (incoming) ps->in_bytes += h->len;
    else ps->out_bytes += h->len;
    pthread_mutex_unlock(&stats_lock);
}

/* pcap thread */
static void *pcap_thread_fn(void *arg) {
    (void)arg;
    if (!pcap_handle) return NULL;
    /* run forever; pcap_loop will call packet_handler */
    pcap_loop(pcap_handle, -1, packet_handler, NULL);
    return NULL;
}

/* comparator for sorting rows by total bytes desc */
typedef struct {
    unsigned long pid;
    char name[128];
    unsigned long long in;
    unsigned long long out;
    unsigned long long in_s;
    unsigned long long out_s;
} row_t;

static int row_cmp(const void *a, const void *b) {
    const row_t *x = a;
    const row_t *y = b;
    unsigned long long xa = x->in_s + x->out_s;
    unsigned long long ya = y->in_s + y->out_s;
    if (xa < ya) return 1;
    if (xa > ya) return -1;
    return 0;
}

/* print table once per second */
static void print_loop() {
    while (1) {
        sleep(1);
        /* refresh maps first */
        refresh_maps();

        /* collect stats snapshot */
        row_t rows[PID_HASH_SIZE * 2];
        int rc = 0;

        pthread_mutex_lock(&stats_lock);
        for (int i = 0; i < PID_HASH_SIZE; ++i) {
            for (pid_stat_t *p = pid_table[i]; p; p = p->next) {
                if (p->in_bytes == 0 && p->out_bytes == 0) continue;
                row_t r;
                r.pid = p->pid;
                strncpy(r.name, p->name[0] ? p->name : "?", sizeof(r.name)-1);
                r.name[sizeof(r.name)-1] = '\0';
                r.in = p->in_bytes;
                r.out = p->out_bytes;
                r.in_s = p->in_bytes - p->last_in;
                r.out_s = p->out_bytes - p->last_out;
                p->last_in = p->in_bytes;
                p->last_out = p->out_bytes;
                rows[rc++] = r;
                if (rc >= (int)(sizeof(rows)/sizeof(rows[0]))) break;
            }
            if (rc >= (int)(sizeof(rows)/sizeof(rows[0]))) break;
        }
        pthread_mutex_unlock(&stats_lock);

        if (rc == 0) {
            printf("\nNo traffic observed yet. Waiting...\n");
            continue;
        }

        qsort(rows, rc, sizeof(rows[0]), row_cmp);

        printf("\n%-6s %-22s %-12s %-12s %-8s %-8s\n", "PID", "Process", "In", "Out", "In/s", "Out/s");
        int show = rc < MAX_DISPLAY ? rc : MAX_DISPLAY;
        for (int i = 0; i < show; ++i) {
            printf("%-6lu %-22s %-12llu %-12llu %-8llu %-8llu\n",
                   rows[i].pid, rows[i].name,
                   rows[i].in, rows[i].out,
                   rows[i].in_s, rows[i].out_s);
        }
        fflush(stdout);
    }
}

/* initialize pcap on IFACE */
static int init_pcap(const char *iface) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_handle = pcap_open_live(iface, SNAPLEN, PROMISC, TIMEOUT_MS, errbuf);
    if (!pcap_handle) {
        fprintf(stderr, "pcap_open_live(%s) failed: %s\n", iface, errbuf);
        return -1;
    }
    /* set immediate mode if available (optional) */
#ifdef PCAP_SET_IMMEDIATE_MODE
    pcap_set_immediate_mode(pcap_handle, 1);
#endif
    /* no filter: listen to all ports and protocols (we filter in code to IPv4 TCP/UDP) */
    return 0;
}

int main(int argc, char **argv) {
    const char *iface = IFACE;
    if (argc >= 2) iface = argv[1];

    if (geteuid() != 0) {
        fprintf(stderr, "This program requires root privileges. Run with sudo.\n");
        return 1;
    }

    if (init_pcap(iface) != 0) return 1;

    /* initial map build */
    refresh_maps();

    /* start pcap thread */
    pthread_t pcap_thread;
    if (pthread_create(&pcap_thread, NULL, pcap_thread_fn, NULL) != 0) {
        fprintf(stderr, "Failed to create pcap thread: %s\n", strerror(errno));
        pcap_close(pcap_handle);
        return 1;
    }

    /* start print loop in main thread */
    print_loop();

    /* cleanup (never reached) */
    pcap_breakloop(pcap_handle);
    pthread_join(pcap_thread, NULL);
    pcap_close(pcap_handle);
    return 0;
}
