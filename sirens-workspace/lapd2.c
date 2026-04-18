/*
 v2ka_no_libasound.c
 Single-file program that:
  - auto-detects /dev/snd nodes (tries hw:0,0 mapping first)
  - probes ioctl-discovered pcm nodes and picks a readable capture + writable playback pair
  - uses driver defaults only (no libasound, no hwparam ioctls)
  - implements an 8-band logarithmic filterbank (HP + LP biquads per band)
  - stereo processing, per-band dB gains, live stdin control (set/show/quit)
  - no -lasound required; compile with: cc -O2 -std=c11 -Wall -o v2ka_no_libasound v2ka_no_libasound.c
 Notes:
  - This is a pragmatic POC. Some ALSA devices require libasound to configure hw params;
    this program tries multiple /dev/snd/pcm* nodes and uses driver defaults where possible.
  - Run as a user in the audio group or as root.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <sys/select.h>

#include <sound/asound.h> /* for SNDRV_* constants (kernel headers) */

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
#ifndef M_PI_2
#define M_PI_2 (M_PI * 0.5f)
#endif

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

/* ---------------- small math replacements (standard names) ---------------- */
static inline float clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
static inline float db_to_lin(float db) { return powf(10.0f, db * 0.05f); }

/* ---------------- tiny "shim" for simple PCM open/read/write (no libasound) ---------------- */
typedef struct {
    int fd;
    int format;         /* SNDRV_PCM_FORMAT_* */
    unsigned int channels;
    unsigned int rate;
} pcm_shim_t;

static int pcm_shim_open(pcm_shim_t **out, const char *path, int for_capture) {
    if (!out || !path) return -1;
    int flags = for_capture ? O_RDONLY : (O_WRONLY | O_NONBLOCK);
    int fd = open(path, flags);
    if (fd < 0) return -errno;
    pcm_shim_t *p = calloc(1, sizeof(*p));
    if (!p) { close(fd); return -ENOMEM; }
    p->fd = fd;
    p->format = SNDRV_PCM_FORMAT_FLOAT_LE; /* default guess */
    p->channels = 2;
    p->rate = 44100;
    *out = p;
    return 0;
}
static void pcm_shim_close(pcm_shim_t *p) { if (!p) return; if (p->fd >= 0) close(p->fd); free(p); }

/* ---------------- DSP: 8-band log filterbank (HP+LP biquads) ---------------- */
#define NBANDS 8
#define DEFAULT_Q 1.6f
#define BLOCK 256

typedef struct { float b0,b1,b2,a1,a2,x1,x2,y1,y2; } biquad_t;
static void biquad_init(biquad_t *b) { memset(b,0,sizeof(*b)); b->b0 = 1.0f; }
static inline float biquad_process(biquad_t *b, float x) {
    float y = b->b0*x + b->b1*b->x1 + b->b2*b->x2 - b->a1*b->y1 - b->a2*b->y2;
    b->x2 = b->x1; b->x1 = x;
    b->y2 = b->y1; b->y1 = y;
    return y;
}
static void biquad_calc_lowpass(biquad_t *b, float fs, float f0, float Q) {
    if (f0 <= 0.0f) { biquad_init(b); return; }
    float w0 = 2.0f * M_PI * f0 / fs; if (w0 > M_PI) w0 = M_PI;
    float cosw0 = cosf(w0), sinw0 = sinf(w0);
    float alpha = sinw0 / (2.0f * Q);
    float a0 = 1.0f + alpha;
    b->b0 = ((1.0f - cosw0) * 0.5f) / a0;
    b->b1 = (1.0f - cosw0) / a0;
    b->b2 = ((1.0f - cosw0) * 0.5f) / a0;
    b->a1 = -2.0f * cosw0 / a0;
    b->a2 = (1.0f - alpha) / a0;
    b->x1=b->x2=b->y1=b->y2=0.0f;
}
static void biquad_calc_highpass(biquad_t *b, float fs, float f0, float Q) {
    if (f0 <= 0.0f) { biquad_init(b); return; }
    float w0 = 2.0f * M_PI * f0 / fs; if (w0 > M_PI) w0 = M_PI;
    float cosw0 = cosf(w0), sinw0 = sinf(w0);
    float alpha = sinw0 / (2.0f * Q);
    float a0 = 1.0f + alpha;
    b->b0 = ((1.0f + cosw0) * 0.5f) / a0;
    b->b1 = (-(1.0f + cosw0)) / a0;
    b->b2 = ((1.0f + cosw0) * 0.5f) / a0;
    b->a1 = -2.0f * cosw0 / a0;
    b->a2 = (1.0f - alpha) / a0;
    b->x1=b->x2=b->y1=b->y2=0.0f;
}

typedef struct {
    biquad_t hp;
    biquad_t lp;
    float f_low, f_high, Q;
    float gain_db, gain_lin;
} band_t;

typedef struct {
    band_t bands[NBANDS];
    float fs;
} filterbank_t;

static void filterbank_init(filterbank_t *fb, float fs) {
    fb->fs = fs;
    for (int i=0;i<NBANDS;i++) {
        biquad_init(&fb->bands[i].hp);
        biquad_init(&fb->bands[i].lp);
        fb->bands[i].f_low = fb->bands[i].f_high = 0.0f;
        fb->bands[i].Q = DEFAULT_Q;
        fb->bands[i].gain_db = 0.0f;
        fb->bands[i].gain_lin = 1.0f;
    }
}

/* design NBANDS logarithmic from f_start to f_end (clamped to Nyquist*0.999) */
static void filterbank_design_log(filterbank_t *fb, float f_start, float f_end, float Q) {
    float nyq = fb->fs * 0.5f;
    if (f_end > nyq*0.999f) f_end = nyq*0.999f;
    if (f_start <= 0.0f) f_start = 1.0f;
    float r = powf(f_end / f_start, 1.0f / (NBANDS - 1));
    for (int i=0;i<NBANDS;i++) {
        float fc = f_start * powf(r, (float)i);
        float bwf = sqrtf(r);
        float f_low = fc / bwf;
        float f_high = fc * bwf;
        if (f_low >= nyq) f_low = nyq*0.999f;
        if (f_high >= nyq) f_high = nyq*0.999f;
        fb->bands[i].f_low = f_low;
        fb->bands[i].f_high = f_high;
        fb->bands[i].Q = Q;
        biquad_calc_highpass(&fb->bands[i].hp, fb->fs, f_low, Q);
        biquad_calc_lowpass(&fb->bands[i].lp, fb->fs, f_high, Q);
        fb->bands[i].gain_db = 0.0f;
        fb->bands[i].gain_lin = 1.0f;
    }
}

/* per-channel states */
typedef struct {
    biquad_t hp[NBANDS];
    biquad_t lp[NBANDS];
    float gain_lin[NBANDS];
} channel_state_t;

static void prepare_channel_state(filterbank_t *fb, channel_state_t *ch) {
    for (int i=0;i<NBANDS;i++) {
        ch->hp[i] = fb->bands[i].hp;
        ch->lp[i] = fb->bands[i].lp;
        ch->hp[i].x1 = ch->hp[i].x2 = ch->hp[i].y1 = ch->hp[i].y2 = 0.0f;
        ch->lp[i].x1 = ch->lp[i].x2 = ch->lp[i].y1 = ch->lp[i].y2 = 0.0f;
        ch->gain_lin[i] = fb->bands[i].gain_lin;
    }
}
static void update_channel_gains(filterbank_t *fb, channel_state_t *ch) {
    for (int i=0;i<NBANDS;i++) ch->gain_lin[i] = fb->bands[i].gain_lin;
}
static inline void process_frame(filterbank_t *fb, channel_state_t *chL, channel_state_t *chR,
                                 float inL, float inR, float *outL, float *outR) {
    float sumL = 0.0f, sumR = 0.0f;
    for (int b=0;b<NBANDS;b++) {
        float tL = biquad_process(&chL->hp[b], inL);
        tL = biquad_process(&chL->lp[b], tL);
        tL *= chL->gain_lin[b];
        sumL += tL;
        float tR = biquad_process(&chR->hp[b], inR);
        tR = biquad_process(&chR->lp[b], tR);
        tR *= chR->gain_lin[b];
        sumR += tR;
    }
    *outL = sumL; *outR = sumR;
}

/* ---------------- device probing and selection (ioctl + scan) ---------------- */
typedef struct { char path[MAX_PATH]; } node_t;
typedef struct {
    int card;
    int dev;
    node_t nodes[16];
    int node_count;
} pcm_device_entry_t;

static int collect_pcm_nodes_from_card(int card, pcm_device_entry_t **out_entries, int *out_count) {
    if (!out_entries || !out_count) return -1;
    *out_entries = NULL; *out_count = 0;
    char control_path[MAX_PATH];
    snprintf(control_path, sizeof(control_path), "/dev/snd/controlC%d", card);
    int ctl = open(control_path, O_RDONLY | O_NONBLOCK);
    if (ctl < 0) return -2;
    int dev = -1;
    pcm_device_entry_t *entries = NULL; int capacity = 0;
    while (1) {
        int rc = ioctl(ctl, SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE, &dev);
        if (rc < 0) {
            if (errno == ENOTTY || errno == ENOSYS) { close(ctl); free(entries); return -3; }
            break;
        }
        if (dev < 0) break;
        char prefix[64]; snprintf(prefix, sizeof(prefix), "pcmC%uD%u", (unsigned)card, (unsigned)dev);
        DIR *d = opendir("/dev/snd");
        if (!d) continue;
        struct dirent *e;
        pcm_device_entry_t entry; entry.card = card; entry.dev = dev; entry.node_count = 0;
        while ((e = readdir(d)) != NULL) {
            if (strncmp(e->d_name, prefix, strlen(prefix)) != 0) continue;
            size_t L = strlen(e->d_name);
            if (L <= strlen(prefix)) continue;
            char path[MAX_PATH]; snprintf(path, sizeof(path), "/dev/snd/%s", e->d_name);
            if (entry.node_count < (int)(sizeof(entry.nodes)/sizeof(entry.nodes[0]))) {
                strncpy(entry.nodes[entry.node_count].path, path, MAX_PATH-1);
                entry.nodes[entry.node_count].path[MAX_PATH-1] = '\0';
                entry.node_count++;
            }
        }
        closedir(d);
        if (entry.node_count > 0) {
            if (*out_count >= capacity) {
                int newcap = capacity ? capacity * 2 : 8;
                pcm_device_entry_t *tmp = realloc(entries, newcap * sizeof(pcm_device_entry_t));
                if (!tmp) { free(entries); close(ctl); return -4; }
                entries = tmp; capacity = newcap;
            }
            entries[*out_count] = entry; (*out_count)++;
        }
    }
    close(ctl);
    *out_entries = entries;
    return 0;
}

static int collect_all_pcm_nodes_generic(pcm_device_entry_t **out_entries, int *out_count) {
    *out_entries = NULL; *out_count = 0;
    pcm_device_entry_t *entries = malloc(sizeof(pcm_device_entry_t));
    if (!entries) return -1;
    entries[0].card = -1; entries[0].dev = -1; entries[0].node_count = 0;
    DIR *d = opendir("/dev/snd");
    if (!d) { free(entries); return -2; }
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (strncmp(e->d_name, "pcm", 3) != 0) continue;
        char path[MAX_PATH]; snprintf(path, sizeof(path), "/dev/snd/%s", e->d_name);
        if (entries[0].node_count < (int)(sizeof(entries[0].nodes)/sizeof(entries[0].nodes[0]))) {
            strncpy(entries[0].nodes[entries[0].node_count].path, path, MAX_PATH-1);
            entries[0].nodes[entries[0].node_count].path[MAX_PATH-1] = '\0';
            entries[0].node_count++;
        }
    }
    closedir(d);
    if (entries[0].node_count == 0) { free(entries); return -3; }
    *out_entries = entries; *out_count = 1;
    return 0;
}

static int test_readable_capture(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    unsigned char tmp[4];
    ssize_t r = read(fd, tmp, sizeof(tmp));
    if (r >= 0) { lseek(fd, 0, SEEK_SET); close(fd); return 0; }
    int err = errno; close(fd);
    if (err == EBADFD) return -2;
    return -1;
}
static int test_writable_playback(const char *path) {
    int fd = open(path, O_WRONLY | O_NONBLOCK);
    if (fd < 0) { fd = open(path, O_RDWR | O_NONBLOCK); if (fd < 0) return -1; }
    ssize_t w = write(fd, NULL, 0); (void)w; close(fd); return 0;
}

static int score_pair(const char *cap, const char *play, const pcm_device_entry_t *ecap, const pcm_device_entry_t *eplay) {
    int score = 0;
    if (ecap && eplay && ecap->card == eplay->card && ecap->dev == eplay->dev) score += 10;
    size_t Lp = strlen(play); if (Lp>0 && (play[Lp-1]=='p' || play[Lp-1]=='P')) score += 5;
    size_t Lc = strlen(cap); if (Lc>0 && (cap[Lc-1]=='c' || cap[Lc-1]=='C')) score += 3;
    int devcap=-1, devplay=-1;
    sscanf(cap, "/dev/snd/pcmC%*dD%d", &devcap);
    sscanf(play, "/dev/snd/pcmC%*dD%d", &devplay);
    if (devcap>=0 && devcap<4) score += 2;
    if (devplay>=0 && devplay<4) score += 2;
    return score;
}

static int find_best_pair(pcm_device_entry_t *entries, int nentries, char *out_cap, size_t cap_len, char *out_play, size_t play_len) {
    int best_score = -100000; char best_cap[MAX_PATH]={0}, best_play[MAX_PATH]={0};
    for (int i=0;i<nentries;i++) {
        for (int ci=0; ci<entries[i].node_count; ci++) {
            const char *cpath = entries[i].nodes[ci].path;
            int rc = test_readable_capture(cpath);
            if (rc == 0) {
                for (int j=0;j<nentries;j++) {
                    for (int pj=0; pj<entries[j].node_count; pj++) {
                        const char *ppath = entries[j].nodes[pj].path;
                        if (strcmp(ppath, cpath)==0) continue;
                        int wp = test_writable_playback(ppath);
                        if (wp == 0) {
                            int sc = score_pair(cpath, ppath, &entries[i], &entries[j]);
                            if (sc > best_score) { best_score = sc; strncpy(best_cap, cpath, sizeof(best_cap)-1); strncpy(best_play, ppath, sizeof(best_play)-1); }
                        }
                    }
                }
            }
        }
    }
    if (best_score < 0) return -1;
    strncpy(out_cap, best_cap, cap_len-1); out_cap[cap_len-1]=0;
    strncpy(out_play, best_play, play_len-1); out_play[play_len-1]=0;
    return 0;
}

/* ---------------- CLI helpers ---------------- */
static void print_help(void) {
    fprintf(stderr,
        "Commands (stdin):\n"
        "  set L|R <band 1..8> <gain dB>   : set band gain (e.g. set L 3 -6.0)\n"
        "  show                            : show current gains\n"
        "  quit | q                        : exit\n");
}

/* ---------------- main: probe, select, open, process ---------------- */
int main(int argc, char **argv) {
    char *hw_arg = NULL; int VERBOSE = 0;
    for (int i=1;i<argc;i++) {
        if (strcmp(argv[i], "-v")==0 || strcmp(argv[i],"--verbose")==0) { VERBOSE = 1; continue; }
        if (!hw_arg) hw_arg = argv[i];
    }

    fprintf(stderr, "v2ka_no_libasound: probe /dev/snd nodes (driver defaults only)\n");
    if (VERBOSE) fprintf(stderr, "verbose enabled\n");

    /* collect control cards */
    DIR *d = opendir("/dev/snd");
    if (!d) { perror("opendir /dev/snd"); return 1; }
    struct dirent *ent;
    int cards[32]; int ncard = 0;
    while ((ent = readdir(d)) != NULL) {
        if (strncmp(ent->d_name, "controlC", 8) == 0) {
            int card = -1;
            if (sscanf(ent->d_name + 8, "%d", &card) == 1) {
                if (ncard < (int)(sizeof(cards)/sizeof(cards[0]))) cards[ncard++] = card;
            }
        }
    }
    closedir(d);

    pcm_device_entry_t *all_entries = NULL; int total_entries = 0;

    /* if user asked hw:0,0 prefer that mapping */
    if (hw_arg) {
        if (strncmp(hw_arg, "hw", 2)==0 || isdigit((unsigned char)hw_arg[0])) {
            int card=-1, dev=-1; const char *s = hw_arg;
            if (strncmp(s,"hw:",3)==0) s+=3;
            if (sscanf(s,"%d,%d",&card,&dev)>=1 && card>=0) {
                pcm_device_entry_t *entries = NULL; int nentries=0;
                if (collect_pcm_nodes_from_card(card, &entries, &nentries)==0 && nentries>0) {
                    all_entries = entries; total_entries = nentries;
                    if (VERBOSE) fprintf(stderr,"hw mapping: collected %d entries for card %d\n", nentries, card);
                } else if (entries) free(entries);
            }
        }
    }

    /* otherwise collect for all cards */
    for (int i=0;i<ncard && total_entries==0;i++) {
        pcm_device_entry_t *entries = NULL; int nentries=0;
        int rc = collect_pcm_nodes_from_card(cards[i], &entries, &nentries);
        if (rc==0 && nentries>0) {
            pcm_device_entry_t *tmp = realloc(all_entries, (total_entries + nentries) * sizeof(pcm_device_entry_t));
            if (!tmp) { free(all_entries); free(entries); fprintf(stderr,"alloc fail\n"); return 1; }
            all_entries = tmp;
            for (int k=0;k<nentries;k++) all_entries[total_entries + k] = entries[k];
            total_entries += nentries;
            free(entries);
        } else { if (entries) free(entries); }
    }

    /* fallback generic scan */
    if (total_entries == 0) {
        if (collect_all_pcm_nodes_generic(&all_entries, &total_entries) != 0) {
            fprintf(stderr, "No pcm nodes discovered; aborting\n"); return 1;
        }
    }

    fprintf(stderr, "Discovered %d pcm device entries\n", total_entries);
    for (int i=0;i<total_entries;i++) {
        fprintf(stderr, "entry %d: card=%d dev=%d nodes=%d\n", i, all_entries[i].card, all_entries[i].dev, all_entries[i].node_count);
        for (int j=0;j<all_entries[i].node_count;j++) fprintf(stderr, "  - %s\n", all_entries[i].nodes[j].path);
    }

    char cap_path[MAX_PATH] = {0}, play_path[MAX_PATH] = {0};
    if (find_best_pair(all_entries, total_entries, cap_path, sizeof(cap_path), play_path, sizeof(play_path)) != 0) {
        fprintf(stderr, "No working capture->playback pair found using driver defaults.\n");
        fprintf(stderr, "Run 'arecord -l', 'aplay -l' and 'fuser -v /dev/snd/*' for diagnostics.\n");
        free(all_entries); return 1;
    }

    fprintf(stderr, "Selected pair: capture=%s  playback=%s\n", cap_path, play_path);

    /* open final shims */
    pcm_shim_t *cap = NULL, *play = NULL;
    if (pcm_shim_open(&cap, cap_path, 1) < 0) { perror("open capture"); free(all_entries); return 1; }
    if (strcmp(cap_path, play_path) == 0) play = cap;
    else {
        if (pcm_shim_open(&play, play_path, 0) < 0) { perror("open playback"); pcm_shim_close(cap); free(all_entries); return 1; }
    }

    /* assume sample rate (driver defaults) */
    float fs = 44100.0f;
    fprintf(stderr, "Assuming sample rate: %.1f Hz (driver defaults)\n", fs);

    /* build filterbank and channel states */
    filterbank_t fb; filterbank_init(&fb, fs);
    float nyq = fs * 0.5f;
    filterbank_design_log(&fb, 1000.0f, nyq*0.999f, DEFAULT_Q);
    channel_state_t chL, chR; prepare_channel_state(&fb, &chL); prepare_channel_state(&fb, &chR);

    /* non-blocking stdin */
    int stdin_fd = fileno(stdin);
    int fl = fcntl(stdin_fd, F_GETFL, 0); fcntl(stdin_fd, F_SETFL, fl | O_NONBLOCK);
    print_help();

    /* detect format by peeking */
    int detected_format = SNDRV_PCM_FORMAT_FLOAT_LE;
    {
        unsigned char tmp[4096];
        ssize_t r = read(cap->fd, tmp, sizeof(tmp));
        if (r <= 0) { fprintf(stderr, "Format detection peek failed (r=%zd); assuming FLOAT_LE\n", r); detected_format = SNDRV_PCM_FORMAT_FLOAT_LE; }
        else {
            size_t samples_f = r / 4; size_t okf = 0;
            for (size_t i=0;i+3<(size_t)r;i+=4) { float v; memcpy(&v,tmp+i,4); if (v==v && fabsf(v) < 10.0f) okf++; }
            if (samples_f>0 && okf > samples_f/2) detected_format = SNDRV_PCM_FORMAT_FLOAT_LE;
            else {
                size_t samples_s16 = r/2, oks16=0;
                for (size_t i=0;i+1<(size_t)r;i+=2) { int16_t v; memcpy(&v,tmp+i,2); if (v!=INT16_MIN) oks16++; }
                if (samples_s16>0 && oks16 > samples_s16/2) detected_format = SNDRV_PCM_FORMAT_S16_LE;
                else detected_format = SNDRV_PCM_FORMAT_S32_LE;
            }
            lseek(cap->fd, 0, SEEK_SET);
        }
    }
    fprintf(stderr, "Detected format code: %d\n", detected_format);
    size_t bytes_per_sample = (detected_format==SNDRV_PCM_FORMAT_FLOAT_LE)?4:
                              (detected_format==SNDRV_PCM_FORMAT_S32_LE)?4:2;
    size_t frame_bytes = bytes_per_sample * cap->channels;
    fprintf(stderr, "Assuming stereo interleaved frames, frame_bytes=%zu\n", frame_bytes);

    unsigned char rawbuf[BLOCK * 4 * 2 + 128];
    float inL[BLOCK], inR[BLOCK], outL[BLOCK], outR[BLOCK];

    int running = 1;
    while (running) {
        /* stdin commands */
        fd_set rfds; struct timeval tv; FD_ZERO(&rfds); FD_SET(stdin_fd, &rfds); tv.tv_sec = 0; tv.tv_usec = 0;
        if (select(stdin_fd+1, &rfds, NULL, NULL, &tv) > 0 && FD_ISSET(stdin_fd, &rfds)) {
            char line[256]; ssize_t n = read(stdin_fd, line, sizeof(line)-1);
            if (n > 0) {
                line[n] = '\0';
                char *p = line;
                while (p && *p) {
                    char *nl = strchr(p, '\n'); if (nl) *nl = '\0';
                    while (*p && isspace((unsigned char)*p)) p++;
                    if (*p) {
                        if (strncmp(p,"quit",4)==0 || strncmp(p,"q",1)==0) { running = 0; break; }
                        else if (strncmp(p,"show",4)==0) {
                            fprintf(stderr,"Band gains (dB):\n");
                            for (int b=0;b<NBANDS;b++) fprintf(stderr,"  %d: L=%.2f dB  R=%.2f dB\n", b+1, fb.bands[b].gain_db, fb.bands[b].gain_db);
                        } else if (strncmp(p,"set",3)==0) {
                            char ch; int band; float db;
                            if (sscanf(p+3, " %c %d %f", &ch, &band, &db) == 3) {
                                if (band>=1 && band<=NBANDS) {
                                    int idx = band-1;
                                    fb.bands[idx].gain_db = db;
                                    fb.bands[idx].gain_lin = db_to_lin(db);
                                    update_channel_gains(&fb, &chL); update_channel_gains(&fb, &chR);
                                    fprintf(stderr,"Set %c band %d -> %.2f dB\n", ch, band, db);
                                } else fprintf(stderr,"band out of range\n");
                            } else fprintf(stderr,"invalid set syntax\n");
                        } else if (strncmp(p,"help",4)==0) print_help();
                        else fprintf(stderr,"unknown command: %s\n", p);
                    }
                    if (!nl) break;
                    p = nl + 1;
                }
            }
        }

        /* read block */
        ssize_t want = (ssize_t)(BLOCK * frame_bytes);
        ssize_t got = 0;
        while (got < want) {
            ssize_t r = read(cap->fd, rawbuf + got, want - got);
            if (r > 0) { got += r; continue; }
            if (r == 0) { fprintf(stderr,"capture read returned 0\n"); running = 0; break; }
            if (r < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) { struct timespec ts = {0,2000000}; nanosleep(&ts,NULL); continue; }
                if (errno == EBADFD) { fprintf(stderr,"read EBADFD — device does not support read() in this mode\n"); running = 0; break; }
                perror("read capture"); running = 0; break;
            }
        }
        if (!running) break;
        size_t frames = got / frame_bytes;

        /* deinterleave & convert */
        if (detected_format == SNDRV_PCM_FORMAT_FLOAT_LE) {
            for (size_t i=0;i<frames;i++) {
                float l,r; memcpy(&l, rawbuf + i*frame_bytes + 0*bytes_per_sample, 4);
                memcpy(&r, rawbuf + i*frame_bytes + 1*bytes_per_sample, 4);
                inL[i] = l; inR[i] = r;
            }
        } else if (detected_format == SNDRV_PCM_FORMAT_S32_LE) {
            for (size_t i=0;i<frames;i++) {
                int32_t l32,r32; memcpy(&l32, rawbuf + i*frame_bytes + 0*bytes_per_sample, 4);
                memcpy(&r32, rawbuf + i*frame_bytes + 1*bytes_per_sample, 4);
                inL[i] = (float)l32 / 2147483648.0f; inR[i] = (float)r32 / 2147483648.0f;
            }
        } else {
            for (size_t i=0;i<frames;i++) {
                int16_t l16,r16; memcpy(&l16, rawbuf + i*frame_bytes + 0*bytes_per_sample, 2);
                memcpy(&r16, rawbuf + i*frame_bytes + 1*bytes_per_sample, 2);
                inL[i] = (float)l16 / 32768.0f; inR[i] = (float)r16 / 32768.0f;
            }
        }

        /* process */
        for (size_t i=0;i<frames;i++) process_frame(&fb, &chL, &chR, inL[i], inR[i], &outL[i], &outR[i]);

        /* convert back & interleave */
        unsigned char outbuf[BLOCK * 4 * 2 + 128];
        if (detected_format == SNDRV_PCM_FORMAT_FLOAT_LE) {
            for (size_t i=0;i<frames;i++) {
                memcpy(outbuf + i*frame_bytes + 0*bytes_per_sample, &outL[i], 4);
                memcpy(outbuf + i*frame_bytes + 1*bytes_per_sample, &outR[i], 4);
            }
        } else if (detected_format == SNDRV_PCM_FORMAT_S32_LE) {
            for (size_t i=0;i<frames;i++) {
                int32_t l32 = (int32_t)(clampf(outL[i], -1.0f, 0.999999f) * 2147483647.0f);
                int32_t r32 = (int32_t)(clampf(outR[i], -1.0f, 0.999999f) * 2147483647.0f);
                memcpy(outbuf + i*frame_bytes + 0*bytes_per_sample, &l32, 4);
                memcpy(outbuf + i*frame_bytes + 1*bytes_per_sample, &r32, 4);
            }
        } else {
            for (size_t i=0;i<frames;i++) {
                int16_t l16 = (int16_t)(clampf(outL[i], -1.0f, 0.999999f) * 32767.0f);
                int16_t r16 = (int16_t)(clampf(outR[i], -1.0f, 0.999999f) * 32767.0f);
                memcpy(outbuf + i*frame_bytes + 0*bytes_per_sample, &l16, 2);
                memcpy(outbuf + i*frame_bytes + 1*bytes_per_sample, &r16, 2);
            }
        }

        /* write */
        ssize_t wrote = 0; ssize_t wantw = (ssize_t)(frames * frame_bytes);
        while (wrote < wantw) {
            ssize_t w = write(play->fd, outbuf + wrote, wantw - wrote);
            if (w > 0) { wrote += w; continue; }
            if (w < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) { struct timespec ts = {0,2000000}; nanosleep(&ts,NULL); continue; }
                perror("write playback"); running = 0; break;
            }
        }
    }

    pcm_shim_close(cap);
    if (play && play != cap) pcm_shim_close(play);
    if (all_entries) free(all_entries);
    fprintf(stderr, "v2ka_no_libasound: exit\n");
    return 0;
}

