/* btrfs_undelete_safe_fixed.c
 *
 * Read-only, safer, more thorough Btrfs scanner for deleted-file candidates.
 * - Default: verbose, single-threaded, thorough scan.
 * - --slow: sequential, large-window scan (slower but more likely to find metadata).
 * - --find-magic: only report offsets where "BTRFS" appears.
 * - Listing remains read-only. Restore requires --confirm-restore and --outdir.
 *
 * Build:
 *   gcc -O2 -pthread -o btrfs_undelete_safe_fixed btrfs_undelete_safe_fixed.c
 *
 * Note: This is still heuristic and not a full Btrfs implementation.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <inttypes.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>

#define MAGIC "BTRFS"
#define MAGIC_LEN 5
/* Increased window and overlap for thorough scanning */
#define LEAF_WINDOW (256 * 1024)        /* 256 KiB parse window */
#define DEFAULT_THREADS 1               /* safer default */
#define DEFAULT_SCRATCH (4 * 1024 * 1024) /* 4 MiB per-thread buffer */
#define PROGRESS_SAMPLE_MS 500
#define ETA_SMOOTH_ALPHA 0.12
#define INODE_BUCKETS 16384
#define MAX_NAME_LEN 2048

/* simplified key types */
#define BTRFS_INODE_ITEM_KEY 1
#define BTRFS_EXTENT_DATA_KEY 5
#define BTRFS_DIR_ITEM_KEY   6

/* in-memory maps */
typedef struct inode_info { uint64_t ino,size,nlink; struct inode_info *next; } inode_info_t;
typedef struct dir_entry { uint64_t parent_ino, ino; char *name; struct dir_entry *next; } dir_entry_t;

static inode_info_t *inode_buckets[INODE_BUCKETS];
static pthread_mutex_t inode_lock = PTHREAD_MUTEX_INITIALIZER;
static dir_entry_t *dir_head = NULL;
static pthread_mutex_t dir_lock = PTHREAD_MUTEX_INITIALIZER;

/* globals/options */
static const char *g_device = NULL;
static size_t g_threads = DEFAULT_THREADS;
static int g_do_list = 0;
static int g_do_restore = 0;
static int g_find_magic = 0;
static int g_slow = 0;
static int g_quiet = 0;
static const char *g_list_prefix = NULL;
static const char *g_restore_path = NULL;
static const char *g_outdir = NULL;
static int g_confirm_restore = 0;

/* progress */
static volatile unsigned long long g_scanned = 0ULL;
static unsigned long long g_total = 0ULL;
static double g_speed_ema = 0.0;
static volatile sig_atomic_t g_terminate = 0;

/* helpers */
static void sigint_handler(int s) { (void)s; g_terminate = 1; }
static char *xstrdup(const char *s){ if(!s) return NULL; size_t n=strlen(s)+1; char *p=malloc(n); if(p) memcpy(p,s,n); return p; }
static void vdbg(const char *fmt, ...){ if(g_quiet) return; va_list ap; va_start(ap,fmt); vfprintf(stderr,fmt,ap); va_end(ap); }
static inline uint64_t bucket_idx(uint64_t ino){ return ino % INODE_BUCKETS; }
static inline uint32_t le32(const void *p){ const uint8_t*b=p; return (uint32_t)b[0]|((uint32_t)b[1]<<8)|((uint32_t)b[2]<<16)|((uint32_t)b[3]<<24); }
static inline uint64_t le64(const void *p){ const uint8_t*b=p; return (uint64_t)b[0]|((uint64_t)b[1]<<8)|((uint64_t)b[2]<<16)|((uint64_t)b[3]<<24)|((uint64_t)b[4]<<32)|((uint64_t)b[5]<<40)|((uint64_t)b[6]<<48)|((uint64_t)b[7]<<56); }

/* map management */
static void store_inode(uint64_t ino,uint64_t size,uint64_t nlink){
    uint64_t idx=bucket_idx(ino);
    pthread_mutex_lock(&inode_lock);
    inode_info_t *p=inode_buckets[idx];
    while(p){ if(p->ino==ino){ p->size=size; p->nlink=nlink; pthread_mutex_unlock(&inode_lock); return; } p=p->next; }
    inode_info_t *n=malloc(sizeof(*n)); if(!n){ pthread_mutex_unlock(&inode_lock); return; }
    n->ino=ino; n->size=size; n->nlink=nlink; n->next=inode_buckets[idx]; inode_buckets[idx]=n;
    pthread_mutex_unlock(&inode_lock);
}
static inode_info_t *find_inode(uint64_t ino){ inode_info_t *p=inode_buckets[bucket_idx(ino)]; while(p){ if(p->ino==ino) return p; p=p->next; } return NULL; }
static void add_dir(uint64_t parent,uint64_t ino,const char *name){ if(!name) return; dir_entry_t *e=malloc(sizeof(*e)); if(!e) return; e->parent_ino=parent; e->ino=ino; e->name=xstrdup(name); pthread_mutex_lock(&dir_lock); e->next=dir_head; dir_head=e; pthread_mutex_unlock(&dir_lock); }

/* free maps */
static void free_maps(void){
    for(size_t i=0;i<INODE_BUCKETS;++i){ inode_info_t *p=inode_buckets[i]; while(p){ inode_info_t *n=p->next; free(p); p=n; } inode_buckets[i]=NULL; }
    dir_entry_t *d=dir_head; while(d){ dir_entry_t *n=d->next; free(d->name); free(d); d=n; } dir_head=NULL;
}

/* heuristic parser: more permissive scanning of a larger window */
static void parse_window(const uint8_t *buf,size_t len){
    if(!buf || len < 64) return;
    /* header check: ensure we have a plausible node header */
    if(len >= 0x20){
        uint32_t level = le32(buf + 0x18);
        if(level != 0) {
            /* still continue: some metadata may be inline elsewhere */
        }
    }
    const uint8_t *p = buf;
    const uint8_t *end = buf + len;
    /* scan for key-like sequences more liberally */
    while(p + 24 < end){
        uint64_t objid = le64(p);
        uint32_t type = le32(p + 8);
        if(objid == 0 || objid > (1ULL<<52)){ p += 8; continue; }
        if(type == BTRFS_INODE_ITEM_KEY){
            if(p + 40 < end){
                uint64_t nlink = le64(p + 12);
                uint64_t size = le64(p + 20);
                store_inode(objid,size,nlink);
            }
            p += 24;
        } else if(type == BTRFS_DIR_ITEM_KEY){
            if(p + 24 < end){
                uint64_t inode_ref = le64(p + 12);
                size_t maxn = (size_t)(end - (p + 20));
                if(maxn > MAX_NAME_LEN-1) maxn = MAX_NAME_LEN-1;
                char name[MAX_NAME_LEN]; memset(name,0,sizeof(name));
                memcpy(name,p+20,maxn);
                /* sanitize */
                for(size_t i=0;i<maxn;i++){ unsigned char c=(unsigned char)name[i]; if(c==0) break; if(c<0x20 || c=='\n' || c=='\r'){ name[i]=0; break; } }
                if(name[0]) add_dir(objid,inode_ref,name);
            }
            p += 24;
        } else {
            p += 8;
        }
    }
}

/* find-magic mode: scan sequentially and print offsets */
static int find_magic_mode(const char *device){
    int fd=open(device,O_RDONLY|O_CLOEXEC);
    if(fd<0){ fprintf(stderr,"open(%s): %s\n",device,strerror(errno)); return -1; }
    unsigned long long size64=0;
    struct stat st;
    if(fstat(fd,&st)==0 && S_ISREG(st.st_mode)) size64=(unsigned long long)st.st_size;
    else {
        if(ioctl(fd,BLKGETSIZE64,&size64)<0){ /* fallback: try reading some bytes */ size64=0; }
    }
    vdbg("find-magic: device size=%llu\n",size64);
    const size_t BUF=4*1024*1024;
    uint8_t *buf=malloc(BUF);
    if(!buf){ close(fd); return -1; }
    off_t pos=0;
    while(!g_terminate){
        ssize_t r = pread(fd,buf,BUF,pos);
        if(r<=0) break;
        for(ssize_t i=0;i<r-MAGIC_LEN;i++){
            if(memcmp(buf+i,MAGIC,MAGIC_LEN)==0){
                printf("MAGIC at offset %" PRIu64 "\n",(uint64_t)(pos + i));
            }
        }
        pos += r;
        if(size64 && (unsigned long long)pos >= size64) break;
    }
    free(buf);
    close(fd);
    return 0;
}

static inline off_t min(off_t a, off_t b) { return a < b ? a : b; }

/* threaded reader for thorough sequential scanning (slower but robust) */
typedef struct { size_t id; int fd; off_t start; off_t end; size_t scratch; } reader_ctx_t;
static void *reader_thread(void *arg){
    reader_ctx_t *ctx = (reader_ctx_t*)arg;
    if(!ctx) return NULL;
    uint8_t *buf = malloc(ctx->scratch);
    if(!buf) return NULL;
    off_t pos = ctx->start;
    while(!g_terminate && pos < ctx->end){
        size_t toread = (size_t)min((off_t)ctx->scratch, ctx->end - pos);
        ssize_t r = pread(ctx->fd, buf, toread, pos);
        if(r<=0){ if(r<0 && errno==EINTR) continue; break; }
        __sync_fetch_and_add(&g_scanned, (unsigned long long)r);
        /* scan for magic inside buffer and parse larger windows around hits */
        uint8_t *p = buf;
        size_t bufsz = (size_t)r;
        size_t i = 0;
        while(i + MAGIC_LEN <= bufsz){
            if(memcmp(buf + i, MAGIC, MAGIC_LEN) == 0){
                off_t node_off = pos + i;
                /* read a larger window around node_off */
                size_t avail = LEAF_WINDOW;
                uint8_t *win = malloc(avail);
                if(win){
                    ssize_t got = pread(ctx->fd, win, avail, node_off);
                    if(got > 0) parse_window(win, (size_t)got);
                    free(win);
                }
                i += MAGIC_LEN;
            } else i++;
        }
        pos += r;
    }
    free(buf);
    return NULL;
}

/* orchestration: choose slow sequential scan or faster mmap/read-chunk scan */
static int scan_and_populate(const char *device, size_t threads, int slow){
    int fd = open(device, O_RDONLY | O_CLOEXEC);
    if(fd<0){ fprintf(stderr,"open(%s): %s\n",device,strerror(errno)); return -1; }
    unsigned long long size64 = 0;
    struct stat st;
    if(fstat(fd,&st)==0 && S_ISREG(st.st_mode)) size64 = (unsigned long long)st.st_size;
    else {
        if(ioctl(fd,BLKGETSIZE64,&size64) < 0) size64 = 0;
    }
    if(size64 == 0) vdbg("Warning: device size reported as 0 (stream/block device or ioctl failed)\n");
    g_total = size64;
    /* if user requested find-magic only */
    if(g_find_magic){
        close(fd);
        return find_magic_mode(device);
    }
    /* slow mode: sequential thorough scan (single-threaded or multi-threaded with large windows) */
    if(slow || size64 == 0){
        vdbg("Using slow sequential scan (thorough, may be slow)\n");
        /* spawn threads reading disjoint ranges */
        if(threads < 1) threads = 1;
        pthread_t *tids = calloc(threads, sizeof(pthread_t));
        reader_ctx_t *ctxs = calloc(threads, sizeof(reader_ctx_t));
        off_t chunk = (size64>0) ? (off_t)(size64 / threads) : (off_t)(64*1024*1024); /* if unknown size, give each thread 64MiB windows sequentially */
        for(size_t i=0;i<threads;i++){
            off_t start = (size64>0) ? i*chunk : (off_t)(i*chunk);
            off_t end = (size64>0) ? ((i==threads-1)? (off_t)size64 : start + chunk) : start + chunk;
            ctxs[i].id = i; ctxs[i].fd = fd; ctxs[i].start = start; ctxs[i].end = end; ctxs[i].scratch = DEFAULT_SCRATCH;
            pthread_create(&tids[i], NULL, reader_thread, &ctxs[i]);
        }
        /* progress monitor */
        struct timespec t0,tn; clock_gettime(CLOCK_MONOTONIC,&t0);
        while(!g_terminate){
            struct timespec ts; ts.tv_sec = PROGRESS_SAMPLE_MS/1000; ts.tv_nsec = (PROGRESS_SAMPLE_MS%1000)*1000000; nanosleep(&ts,NULL);
            unsigned long long cur = __sync_fetch_and_add(&g_scanned,0ULL);
            clock_gettime(CLOCK_MONOTONIC,&tn);
            double elapsed = (tn.tv_sec - t0.tv_sec) + (tn.tv_nsec - t0.tv_nsec)/1e9;
            double speed = (elapsed>0.0)? (cur/(1024.0*1024.0))/elapsed : 0.0;
            if(g_speed_ema==0.0) g_speed_ema = speed; else g_speed_ema = ETA_SMOOTH_ALPHA*speed + (1.0-ETA_SMOOTH_ALPHA)*g_speed_ema;
            if(g_total) {
                double pct = (double)cur*100.0/(double)g_total;
                unsigned long long rem = (cur>=g_total)?0ULL:(g_total-cur);
                double eta = (g_speed_ema>0.0)? (rem/(1024.0*1024.0))/g_speed_ema : -1.0;
                fprintf(stderr,"\rScanned: %6.2f%%  %6.2fMB/%6.2fMB  %5.2fMB/s  ETA: ",
                        pct, cur/(1024.0*1024.0), g_total/(1024.0*1024.0), g_speed_ema);
                if(eta>=0){ int hh=(int)(eta/3600), mm=(int)((eta-hh*3600)/60), ss=(int)eta%60; if(hh) fprintf(stderr,"%02dh%02dm%02ds",hh,mm,ss); else fprintf(stderr,"%02dm%02ds",mm,ss); }
                else fprintf(stderr,"--:--:--");
                fflush(stderr);
                if(cur>=g_total) break;
            } else {
                fprintf(stderr,"\rScanned: %llu bytes  %5.2fMB/s", (unsigned long long)cur, g_speed_ema); fflush(stderr);
            }
        }
        for(size_t i=0;i<threads;i++) pthread_join(tids[i],NULL);
        fprintf(stderr,"\n");
        free(tids); free(ctxs);
        close(fd);
        return 0;
    }

    /* faster mode: chunked read with memmem and parse windows (still more permissive than before) */
    vdbg("Using chunked scan mode (balanced speed/coverage)\n");
    const size_t CHUNK = DEFAULT_SCRATCH;
    uint8_t *buf = malloc(CHUNK);
    if(!buf){ close(fd); return -1; }
    off_t pos = 0;
    while(!g_terminate){
        ssize_t r = pread(fd, buf, CHUNK, pos);
        if(r<=0) break;
        __sync_fetch_and_add(&g_scanned, (unsigned long long)r);
        /* scan for magic and parse around hits */
        size_t bufsz = (size_t)r;
        for(size_t i=0;i+MAGIC_LEN<=bufsz;i++){
            if(memcmp(buf+i,MAGIC,MAGIC_LEN)==0){
                off_t node_off = pos + i;
                size_t avail = LEAF_WINDOW;
                uint8_t *win = malloc(avail);
                if(win){
                    ssize_t got = pread(fd, win, avail, node_off);
                    if(got>0) parse_window(win,(size_t)got);
                    free(win);
                }
            }
        }
        pos += r;
        if(g_total && (unsigned long long)pos >= g_total) break;
    }
    free(buf);
    close(fd);
    return 0;
}

/* list candidates */
static int list_candidates(const char *prefix){
    if(!prefix) prefix="/";
    printf("Deleted inode candidates under '%s':\n", prefix);
    int found=0;
    for(size_t b=0;b<INODE_BUCKETS;b++){
        inode_info_t *p = inode_buckets[b];
        while(p){
            if(p->nlink==0){
                int printed=0;
                for(dir_entry_t *d=dir_head; d; d=d->next){
                    if(d->ino==p->ino){
                        char path[4096];
                        snprintf(path,sizeof(path),"(parent_ino=%" PRIu64 ")/%s", d->parent_ino, d->name);
                        if(strstr(path,prefix)){
                            printf("inode %" PRIu64 " size=%" PRIu64 " bytes: %s\n", p->ino, p->size, path);
                            printed=1; found=1;
                        }
                    }
                }
                if(!printed){
                    if(strcmp(prefix,"/")==0 || prefix[0]=='\0'){
                        printf("inode %" PRIu64 " size=%" PRIu64 " bytes: (no dir_item found)\n", p->ino, p->size);
                        found=1;
                    }
                }
            }
            p=p->next;
        }
    }
    if(!found) printf("(no candidates found)\n");
    return 0;
}

/* very limited restore (unchanged semantics): requires --confirm-restore */
static int restore_by_path(const char *device,const char *path,const char *outdir){
    if(!g_confirm_restore){ fprintf(stderr,"Restore requires --confirm-restore\n"); return 1; }
    if(!path || !outdir) return 1;
    const char *basename = strrchr(path,'/'); basename = basename?basename+1:path;
    uint64_t target=0;
    for(dir_entry_t *d=dir_head; d; d=d->next) if(strcmp(d->name,basename)==0){ target=d->ino; break; }
    if(!target){ fprintf(stderr,"No dir_item matching '%s'\n",basename); return 1; }
    inode_info_t *ii = find_inode(target);
    if(!ii){ fprintf(stderr,"No inode_item for inode %" PRIu64 "\n", target); return 1; }
    if(access(outdir,F_OK)!=0){ if(mkdir(outdir,0700)!=0){ fprintf(stderr,"mkdir failed: %s\n",strerror(errno)); return 1; } }
    /* heuristic extent scan: read device and look for extent_data keys referencing target */
    int fd=open(device,O_RDONLY|O_CLOEXEC); if(fd<0){ fprintf(stderr,"open failed: %s\n",strerror(errno)); return 1; }
    unsigned long long size64=0; struct stat st; if(fstat(fd,&st)==0 && S_ISREG(st.st_mode)) size64=(unsigned long long)st.st_size; else if(ioctl(fd,BLKGETSIZE64,&size64)<0) size64=0;
    char outpath[PATH_MAX]; snprintf(outpath,sizeof(outpath),"%s/recovered_ino_%" PRIu64 ".bin", outdir, target);
    int outfd=open(outpath,O_CREAT|O_WRONLY|O_TRUNC,0600); if(outfd<0){ close(fd); return 1; }
    const size_t SCAN=4*1024*1024; uint8_t *buf=malloc(SCAN); if(!buf){ close(fd); close(outfd); return 1; }
    off_t pos=0; size_t total_written=0;
    while(!g_terminate && (size64==0 || (unsigned long long)pos < size64)){
        size_t toread = (size_t)min((off_t)SCAN, (off_t)(size64? (off_t)(size64 - (unsigned long long)pos) : (off_t)SCAN));
        ssize_t r = pread(fd, buf, toread, pos);
        if(r<=0){ if(r<0 && errno==EINTR) continue; break; }
        uint8_t *p = buf; size_t bufsz = (size_t)r;
        for(size_t i=0;i+MAGIC_LEN<=bufsz;i++){
            if(memcmp(buf+i,MAGIC,MAGIC_LEN)==0){
                off_t node_off = pos + i;
                size_t avail = LEAF_WINDOW;
                uint8_t *win = malloc(avail);
                if(win){
                    ssize_t got = pread(fd, win, avail, node_off);
                    if(got>0){
                        const uint8_t *q = win; const uint8_t *end = win + (size_t)got;
                        while(q + 24 < end){
                            uint64_t objid = le64(q);
                            uint32_t type = le32(q+8);
                            q += 24;
                            if(type == BTRFS_EXTENT_DATA_KEY && objid == target){
                                if(q + 16 <= end){
                                    uint64_t phys = le64(q);
                                    uint64_t len = le64(q+8);
                                    if(phys && len && (size64==0 || phys + len <= size64)){
                                        uint8_t *dat = malloc((size_t)len);
                                        if(dat){
                                            ssize_t got2 = pread(fd, dat, (size_t)len, (off_t)phys);
                                            if(got2 == (ssize_t)len){
                                                if(write(outfd, dat, (size_t)len) == (ssize_t)len) total_written += (size_t)len;
                                            }
                                            free(dat);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    free(win);
                }
            }
        }
        pos += r;
        if(size64 && (unsigned long long)pos >= size64) break;
    }
    free(buf); close(fd); close(outfd);
    if(total_written==0){ unlink(outpath); fprintf(stderr,"No extents found or unsupported format\n"); return 1; }
    printf("Wrote %zu bytes to %s\n", total_written, outpath);
    return 0;
}

/* CLI and main */
static void usage(const char *p){
    fprintf(stderr,"Usage:\n  %s --device DEVICE --list PATH_PREFIX [--threads N] [--slow] [--find-magic] [--quiet]\n  %s --device DEVICE --restore PATH --outdir OUTDIR --confirm-restore\n", p, p);
}
int main(int argc,char **argv){
    if(argc<2){ usage(argv[0]); return 1; }
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"--device")==0 && i+1<argc) g_device=argv[++i];
        else if(strcmp(argv[i],"--threads")==0 && i+1<argc) g_threads = (size_t)atoi(argv[++i]);
        else if(strcmp(argv[i],"--list")==0 && i+1<argc){ g_do_list=1; g_list_prefix=argv[++i]; }
        else if(strcmp(argv[i],"--restore")==0 && i+1<argc){ g_do_restore=1; g_restore_path=argv[++i]; }
        else if(strcmp(argv[i],"--outdir")==0 && i+1<argc) g_outdir=argv[++i];
        else if(strcmp(argv[i],"--confirm-restore")==0) g_confirm_restore=1;
        else if(strcmp(argv[i],"--find-magic")==0) g_find_magic=1;
        else if(strcmp(argv[i],"--slow")==0) g_slow=1;
        else if(strcmp(argv[i],"--quiet")==0) g_quiet=1;
        else { usage(argv[0]); return 1; }
    }
    if(!g_device){ fprintf(stderr,"--device required\n"); return 1; }
    if(!g_do_list && !g_do_restore && !g_find_magic){ fprintf(stderr,"Choose --list, --restore or --find-magic\n"); return 1; }
    signal(SIGINT, sigint_handler); signal(SIGTERM, sigint_handler);
    /* populate maps */
    if(scan_and_populate(g_device, g_threads, g_slow) != 0){ fprintf(stderr,"scan failed\n"); free_maps(); return 1; }
    if(g_do_list) list_candidates(g_list_prefix?g_list_prefix:"/");
    if(g_do_restore) restore_by_path(g_device, g_restore_path, g_outdir);
    free_maps();
    return 0;
}

