// Compile: gcc -O2 -o nettop nettop.c -lpcap
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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

typedef struct {
    uint32_t laddr, raddr;
    uint16_t lport, rport;
    uint8_t proto; // 6=tcp 17=udp
} conn_t;

typedef struct {
    unsigned long in_bytes, out_bytes;
    unsigned long last_in, last_out;
    char name[256];
} proc_stat_t;

#define HASH_SIZE 65536
typedef struct map_entry { conn_t c; unsigned long inode; struct map_entry *next; } map_entry;
map_entry *conn_map[HASH_SIZE];

unsigned long hash_conn(const conn_t *c){
    return (c->laddr ^ c->raddr ^ ((c->lport<<16)|c->rport) ^ c->proto) % HASH_SIZE;
}

int conn_eq(const conn_t *a,const conn_t *b){
    return a->laddr==b->laddr && a->raddr==b->raddr && a->lport==b->lport && a->rport==b->rport && a->proto==b->proto;
}

void clear_conn_map(){
    for(int i=0;i<HASH_SIZE;i++){
        map_entry *e=conn_map[i];
        while(e){ map_entry *n=e->next; free(e); e=n; }
        conn_map[i]=NULL;
    }
}

void add_conn_map(conn_t c, unsigned long inode){
    unsigned long h=hash_conn(&c);
    map_entry *e=malloc(sizeof(*e));
    e->c=c; e->inode=inode; e->next=conn_map[h]; conn_map[h]=e;
}

unsigned long lookup_inode(const conn_t *c){
    unsigned long h=hash_conn(c);
    for(map_entry *e=conn_map[h]; e; e=e->next) if(conn_eq(&e->c,c)) return e->inode;
    return 0;
}

// parse /proc/net/{tcp,udp} (IPv4) entries
void build_conn_map(){
    clear_conn_map();
    const char *files[] = {"/proc/net/tcp","/proc/net/udp",NULL};
    for(int f=0; files[f]; ++f){
        FILE *fp = fopen(files[f],"r");
        if(!fp) continue;
        char line[512];
        fgets(line,sizeof(line),fp); // header
        while(fgets(line,sizeof(line),fp)){
            unsigned int sl; char local[64], rem[64]; unsigned long inode; int state, d;
            if(sscanf(line,"%u: %63s %63s %x %*s %*s %*s %*s %*s %lu",&sl,local,rem,&state,&inode)!=5) continue;
            unsigned int la,ra; unsigned int lp,rp;
            sscanf(local,"%x:%x",&la,&lp); sscanf(rem,"%x:%x",&ra,&rp);
            conn_t c;
            c.laddr = ntohl(la); c.raddr = ntohl(ra);
            c.lport = ntohs((uint16_t)lp); c.rport = ntohs((uint16_t)rp);
            c.proto = (files[f][strlen(files[f])-3]=='t')?6:17;
            add_conn_map(c,inode);
        }
        fclose(fp);
    }
}

// map inode -> pid by scanning /proc/*/fd
unsigned long inode_to_pid(unsigned long inode, char *procname, size_t pnlen){
    DIR *d = opendir("/proc");
    if(!d) return 0;
    struct dirent *de;
    while((de=readdir(d))){
        if(de->d_type!=DT_DIR) continue;
        char *endptr; long pid = strtol(de->d_name,&endptr,10);
        if(*endptr) continue;
        char fdpath[512];
        snprintf(fdpath,sizeof(fdpath),"/proc/%ld/fd",pid);
        DIR *fd = opendir(fdpath);
        if(!fd) continue;
        struct dirent *fde;
        while((fde=readdir(fd))){
            if(fde->d_type==DT_LNK){
                char link[512], target[512];
                snprintf(link,sizeof(link),"%s/%s",fdpath,fde->d_name);
                ssize_t r = readlink(link,target,sizeof(target)-1);
                if(r>0){ target[r]=0;
                    char want[64];
                    snprintf(want,sizeof(want),"socket:[%lu]",inode);
                    if(strcmp(target,want)==0){
                        // get process name
                        char comm[256];
                        snprintf(link,sizeof(link),"/proc/%ld/comm",pid);
                        FILE *c = fopen(link,"r");
                        if(c){ if(fgets(comm,sizeof(comm),c)){ comm[strcspn(comm,"\n")]=0; strncpy(procname,comm,pnlen-1); procname[pnlen-1]=0; } fclose(c); }
                        closedir(fd); closedir(d);
                        return (unsigned long)pid;
                    }
                }
            }
        }
        closedir(fd);
    }
    closedir(d);
    return 0;
}

// global aggregation (simple)
#include <pthread.h>
pthread_mutex_t agg_lock = PTHREAD_MUTEX_INITIALIZER;
typedef struct pid_entry { unsigned long pid; unsigned long in, out; char name[256]; struct pid_entry *next; } pid_entry;
pid_entry *pid_map[4096];

pid_entry *get_pid_entry(unsigned long pid){
    int h = pid % 4096;
    for(pid_entry *e=pid_map[h]; e; e=e->next) if(e->pid==pid) return e;
    pid_entry *n = calloc(1,sizeof(*n)); n->pid=pid; n->next=pid_map[h]; pid_map[h]=n; return n;
}

void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes){
    if(h->caplen < sizeof(struct ip)) return;
    const struct ip *ip = (struct ip*)(bytes + 14); // assume Ethernet
    if(ip->ip_v != 4) return;
    conn_t c;
    c.laddr = ntohl(ip->ip_src.s_addr);
    c.raddr = ntohl(ip->ip_dst.s_addr);
    c.proto = ip->ip_p;
    uint16_t sport=0,dport=0;
    if(c.proto==6 && h->caplen >= 14 + ip->ip_hl*4 + sizeof(struct tcphdr)){
        const struct tcphdr *tcp = (struct tcphdr*)(bytes + 14 + ip->ip_hl*4);
        sport = ntohs(tcp->th_sport); dport = ntohs(tcp->th_dport);
    } else if(c.proto==17 && h->caplen >= 14 + ip->ip_hl*4 + sizeof(struct udphdr)){
        const struct udphdr *udp = (struct udphdr*)(bytes + 14 + ip->ip_hl*4);
        sport = ntohs(udp->uh_sport); dport = ntohs(udp->uh_dport);
    } else return;
    c.lport = sport; c.rport = dport;
    unsigned long inode = lookup_inode(&c);
    if(!inode) {
        // try reverse (incoming)
        conn_t r = c; r.laddr=c.raddr; r.raddr=c.laddr; r.lport=c.rport; r.rport=c.lport;
        inode = lookup_inode(&r);
        if(inode) { // swap to mark as incoming
            unsigned long pid; char name[256]="?";
            pid = inode_to_pid(inode,name,sizeof(name));
            if(pid){
                pthread_mutex_lock(&agg_lock);
                pid_entry *pe = get_pid_entry(pid);
                pe->in += h->len;
                strncpy(pe->name,name,sizeof(pe->name)-1);
                pthread_mutex_unlock(&agg_lock);
            }
            return;
        }
        return;
    }
    unsigned long pid; char name[256]="?";
    pid = inode_to_pid(inode,name,sizeof(name));
    if(pid){
        pthread_mutex_lock(&agg_lock);
        pid_entry *pe = get_pid_entry(pid);
        pe->out += h->len;
        strncpy(pe->name,name,sizeof(pe->name)-1);
        pthread_mutex_unlock(&agg_lock);
    }
}

int main(){
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    if(pcap_findalldevs(&alldevs,errbuf)==-1){ fprintf(stderr,"pcap_findalldevs: %s\n",errbuf); return 1; }
    pcap_if_t *d = alldevs;
    if(!d){ fprintf(stderr,"No devices\n"); return 1; }
    // pick first non-loopback
    pcap_t *handle = NULL;
    for(pcap_if_t *i=d;i;i=i->next){
        if(i->flags & PCAP_IF_LOOPBACK) continue;
        handle = pcap_open_live(i->name, 65536, 1, 1000, errbuf);
        if(handle) break;
    }
    if(!handle) handle = pcap_open_live(d->name,65536,1,1000,errbuf);
    if(!handle){ fprintf(stderr,"pcap_open_live failed: %s\n",errbuf); return 1; }
    build_conn_map();
    // refresh conn map periodically in a background thread would be better; here we rebuild every second in main loop
    time_t last = time(NULL);
    pcap_loop(handle, 0, packet_handler, NULL);
    pcap_freealldevs(alldevs);
    pcap_close(handle);
    return 0;
}
