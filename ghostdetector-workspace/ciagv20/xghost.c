// ciagv3.c
// "Best-known" public Ghost Enhancer v3 integrated with V4L2 + X11 + Xft
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xft/Xft.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

typedef struct { void *start; size_t length; } Buffer;
typedef struct { unsigned short h,s,v,a; } HSV64;
static int clampi(int v, int lo, int hi){ return v<lo?lo:(v>hi?hi:v); }
static unsigned int pack_argb(unsigned char a,unsigned char r,unsigned char g,unsigned char b){
    return ((unsigned int)a<<24)|((unsigned int)r<<16)|((unsigned int)g<<8)|((unsigned int)b);
}

// ----------------- OptShared (same as before) -----------------
typedef struct {
    double hue_base, sat_base, val_base, tint_base;
    double hue_amp, hue_freq, sat_amp, sat_freq, val_amp, val_freq, tint_amp, tint_freq;
    double target_sat, target_val;
    double avg_sat, avg_val;
    double J;
    int running;
    pthread_mutex_t mtx;
} OptShared;
static void optshared_init(OptShared *os) {
    os->hue_base=8.0; os->sat_base=2500.0; os->val_base=2500.0; os->tint_base=0.4;
    os->hue_amp=2.0; os->hue_freq=0.9; os->sat_amp=500.0; os->sat_freq=0.7;
    os->val_amp=500.0; os->val_freq=0.5; os->tint_amp=0.08; os->tint_freq=0.6;
    os->target_sat=18000.0; os->target_val=22000.0; os->avg_sat=0.0; os->avg_val=0.0;
    os->J=0.0; os->running=1; pthread_mutex_init(&os->mtx,NULL);
}
static void* optimizer_thread(void *arg){
    OptShared *os=(OptShared*)arg;
    const double ema_alpha=0.05; double g_h=0,g_s=0,g_v=0,g_t=0;
    const double lr_h=5e-3, lr_s=5e-4, lr_v=5e-4, lr_t=1e-3;
    const double hue_min=0.0,hue_max=20.0,sat_min=500.0,sat_max=6000.0,val_min=500.0,val_max=6000.0;
    const double tint_min=0.1,tint_max=0.8;
    struct timespec ts={0,10*1000*1000}; double t=0,dt=0.01;
    while(1){
        pthread_mutex_lock(&os->mtx);
        int running=os->running;
        double avg_sat=os->avg_sat; double avg_val=os->avg_val;
        double e_sat=avg_sat-os->target_sat; double e_val=avg_val-os->target_val;
        os->J = e_sat*e_sat + e_val*e_val;
        double s_h = sin(2*M_PI*os->hue_freq*t);
        double s_s = sin(2*M_PI*os->sat_freq*t);
        double s_v = sin(2*M_PI*os->val_freq*t);
        double s_t = sin(2*M_PI*os->tint_freq*t);
        g_h = (1.0-ema_alpha)*g_h + ema_alpha*(os->J * s_h);
        g_s = (1.0-ema_alpha)*g_s + ema_alpha*(os->J * s_s);
        g_v = (1.0-ema_alpha)*g_v + ema_alpha*(os->J * s_v);
        g_t = (1.0-ema_alpha)*g_t + ema_alpha*(os->J * s_t);
        os->hue_base -= lr_h * g_h; os->sat_base -= lr_s * g_s;
        os->val_base -= lr_v * g_v; os->tint_base -= lr_t * g_t;
        if(os->hue_base<hue_min) os->hue_base=hue_min; if(os->hue_base>hue_max) os->hue_base=hue_max;
        if(os->sat_base<sat_min) os->sat_base=sat_min; if(os->sat_base>sat_max) os->sat_base=sat_max;
        if(os->val_base<val_min) os->val_base=val_min; if(os->val_base>val_max) os->val_base=val_max;
        if(os->tint_base<tint_min) os->tint_base=tint_min; if(os->tint_base>tint_max) os->tint_base=tint_max;
        pthread_mutex_unlock(&os->mtx);
        if(!running) break; nanosleep(&ts,NULL); t+=dt;
    }
    return NULL;
}

// ----------------- Color helpers -----------------
static HSV64 rgb8_to_hsv64(unsigned char r8,unsigned char g8,unsigned char b8){
    double r=r8/255.0,g=g8/255.0,b=b8/255.0;
    double maxi=fmax(r,fmax(g,b)), mini=fmin(r,fmin(g,b)), d=maxi-mini;
    double h=0,s=0,v=maxi;
    if(d>1e-12){ s = maxi<=0?0:d/maxi;
        if(maxi==r) h = fmod(((g-b)/d),6.0);
        else if(maxi==g) h = ((b-r)/d)+2.0;
        else h = ((r-g)/d)+4.0;
        h*=60; if(h<0) h+=360;
    } else { s=0; h=0; }
    HSV64 out; out.h=(unsigned short)clampi((int)llround(h*(65535.0/360.0)),0,65535);
    out.s=(unsigned short)clampi((int)llround(s*65535.0),0,65535);
    out.v=(unsigned short)clampi((int)llround(v*65535.0),0,65535); out.a=65535; return out;
}
static void hsv64_to_rgb8(HSV64 hsv,unsigned char *r8,unsigned char *g8,unsigned char *b8){
    double h=(double)hsv.h*(360.0/65535.0), s=(double)hsv.s/65535.0, v=(double)hsv.v/65535.0;
    if(s<=1e-12){ unsigned char val=(unsigned char)clampi((int)llround(v*255.0),0,255); *r8=*g8=*b8=val; return; }
    double c=v*s, hh=h/60.0, x=c*(1.0-fabs(fmod(hh,2.0)-1.0)), m=v-c;
    double rr=0,gg=0,bb=0; int region=(int)floor(hh);
    switch(region){case 0: rr=c; gg=x; bb=0; break; case 1: rr=x; gg=c; bb=0; break;
        case 2: rr=0; gg=c; bb=x; break; case 3: rr=0; gg=x; bb=c; break;
        case 4: rr=x; gg=0; bb=c; break; default: rr=c; gg=0; bb=x; break;
    }
    *r8=(unsigned char)clampi((int)llround((rr+m)*255.0),0,255);
    *g8=(unsigned char)clampi((int)llround((gg+m)*255.0),0,255);
    *b8=(unsigned char)clampi((int)llround((bb+m)*255.0),0,255);
}
static void apply_sepia(unsigned char *r,unsigned char *g,unsigned char *b){
    double rd=(0.393*(*r)+0.769*(*g)+0.189*(*b));
    double gd=(0.349*(*r)+0.686*(*g)+0.168*(*b));
    double bd=(0.272*(*r)+0.534*(*g)+0.131*(*b));
    double mix=0.6;
    *r=(unsigned char)clampi((int)llround(mix*rd+(1.0-mix)*(*r)),0,255);
    *g=(unsigned char)clampi((int)llround(mix*gd+(1.0-mix)*(*g)),0,255);
    *b=(unsigned char)clampi((int)llround(mix*bd+(1.0-mix)*(*b)),0,255);
}

// ----------------- PerfParams -----------------
typedef struct { int DS; int sobel_padding; double ema_alpha_global; double hue_saturation_boost; double decay_rate; int effort_level; } PerfParams;
static void adjust_perf_params(double fps, PerfParams *pp, double smoothing){
    if(!pp) return; PerfParams target;
    if(fps>=55.0){ target.DS=1; target.sobel_padding=1; target.ema_alpha_global=0.12; target.hue_saturation_boost=1.18; target.decay_rate=1.0; target.effort_level=3; }
    else if(fps>=35.0){ target.DS=1; target.sobel_padding=1; target.ema_alpha_global=0.10; target.hue_saturation_boost=1.15; target.decay_rate=1.0; target.effort_level=2; }
    else if(fps>=20.0){ target.DS=2; target.sobel_padding=2; target.ema_alpha_global=0.08; target.hue_saturation_boost=1.12; target.decay_rate=1.2; target.effort_level=1; }
    else if(fps>=10.0){ target.DS=3; target.sobel_padding=2; target.ema_alpha_global=0.06; target.hue_saturation_boost=1.08; target.decay_rate=1.5; target.effort_level=1; }
    else{ target.DS=4; target.sobel_padding=3; target.ema_alpha_global=0.04; target.hue_saturation_boost=1.05; target.decay_rate=2.0; target.effort_level=0; }
    double s=smoothing;
    pp->DS=(int)llround((1.0-s)*pp->DS + s*target.DS);
    pp->sobel_padding=(int)llround((1.0-s)*pp->sobel_padding + s*target.sobel_padding);
    pp->ema_alpha_global=(1.0-s)*pp->ema_alpha_global + s*target.ema_alpha_global;
    pp->hue_saturation_boost=(1.0-s)*pp->hue_saturation_boost + s*target.hue_saturation_boost;
    pp->decay_rate=(1.0-s)*pp->decay_rate + s*target.decay_rate;
    pp->effort_level=(int)llround((1.0-s)*pp->effort_level + s*target.effort_level);
    if(pp->DS<1)pp->DS=1;if(pp->DS>8)pp->DS=8;
}

// ----------------- Helper: separable blur, sobel -----------------
static void separable_gauss(const unsigned char *in,unsigned char *out,int W,int H,int radius){
    unsigned char *tmp=(unsigned char*)malloc(W*H);
    if(!tmp) return;
    if(radius==1){
        for(int y=0;y<H;y++){
            const unsigned char *row=in+y*W; unsigned char *trow=tmp+y*W;
            int left=row[0],center=row[0],right=(W>1)?row[1]:row[0];
            for(int x=0;x<W;x++){ center=row[x]; right=(x+1<W)?row[x+1]:center; int v=(left+2*center+right); trow[x]=(unsigned char)((v+2)>>2); left=center; }
        }
        for(int x=0;x<W;x++){ int up=tmp[x], mid=tmp[x], down=(H>1)?tmp[W+x]:tmp[x]; for(int y=0;y<H;y++){ mid=tmp[y*W+x]; down=(y+1<H)?tmp[(y+1)*W+x]:mid; int v=(up+2*mid+down); out[y*W+x]=(unsigned char)((v+2)>>2); up=mid; } }
    } else {
        for(int y=0;y<H;y++){ const unsigned char *row=in+y*W; unsigned char *trow=tmp+y*W; for(int x=0;x<W;x++){ int v=0; for(int k=-2;k<=2;k++){ int xx=x+k; if(xx<0)xx=0; if(xx>=W)xx=W-1; int w=(k==0)?3:(abs(k)==1?2:1); v+=w*row[xx]; } trow[x]=(unsigned char)((v+4)/9); } }
        for(int x=0;x<W;x++){ for(int y=0;y<H;y++){ int v=0; for(int k=-2;k<=2;k++){ int yy=y+k; if(yy<0)yy=0; if(yy>=H)yy=H-1; int w=(k==0)?3:(abs(k)==1?2:1); v+=w*tmp[yy*W+x]; } out[y*W+x]=(unsigned char)((v+4)/9); } }
    }
    free(tmp);
}
static void sobel_mag(const unsigned char *lum,float *mag,int W,int H){
    for(int y=1;y<H-1;y++) for(int x=1;x<W-1;x++){ int i=y*W+x; int gx = -lum[(y-1)*W+(x-1)] - 2*lum[y*W+(x-1)] - lum[(y+1)*W+(x-1)] + lum[(y-1)*W+(x+1)] + 2*lum[y*W+(x+1)] + lum[(y+1)*W+(x+1)]; int gy = -lum[(y-1)*W+(x-1)] - 2*lum[(y-1)*W + x] - lum[(y-1)*W+(x+1)] + lum[(y+1)*W+(x-1)] + 2*lum[(y+1)*W + x] + lum[(y+1)*W+(x+1)]; mag[i]=sqrtf((float)(gx*gx + gy*gy)); }
    for(int x=0;x<W;x++){ mag[x]=mag[W+x]; mag[(H-1)*W + x] = mag[(H-2)*W + x]; }
    for(int y=0;y<H;y++){ mag[y*W]=mag[y*W+1]; mag[y*W + (W-1)] = mag[y*W + (W-2)]; }
}

// ----------------- Ring buffer median approx and tracking -----------------
typedef struct { unsigned char *frames; int W,H,N,ptr; } RingFrames;
static RingFrames* ringframes_create(int W,int H,int N){
    RingFrames *r=(RingFrames*)malloc(sizeof(RingFrames)); if(!r) return NULL;
    r->W=W; r->H=H; r->N=N; r->ptr=0;
    r->frames=(unsigned char*)malloc((size_t)W*H*N);
    if(!r->frames){ free(r); return NULL; }
    memset(r->frames,0,(size_t)W*H*N); return r;
}
static void ringframes_push(RingFrames *r,const unsigned char *frame){
    unsigned char *dst = r->frames + (size_t)r->ptr * r->W * r->H;
    memcpy(dst, frame, (size_t)r->W * r->H);
    r->ptr = (r->ptr + 1) % r->N;
}
static void ringframes_median_approx(RingFrames *r, unsigned char *out){
    int W=r->W,H=r->H,N=r->N;
    // approximate median via partial histogram per-pixel
    int hist[256];
    for(int i=0;i<W*H;i++){
        memset(hist,0,sizeof(hist));
        for(int k=0;k<N;k++){ unsigned char v = r->frames[(size_t)k*W*H + i]; hist[v]++; }
        int acc=0, target=(N+1)/2; int v=0;
        for(;v<256;v++){ acc+=hist[v]; if(acc>=target) break; }
        out[i]=(unsigned char)v;
    }
}

// ----------------- Simple flood-fill label (stack) -----------------
static int label_blobs(const unsigned char *mask,int W,int H,int *labels){
    int cur=0, N=W*H;
    for(int i=0;i<N;i++) labels[i]=0;
    int *stack=(int*)malloc(sizeof(int)*N);
    for(int i=0;i<N;i++){ if(!mask[i]||labels[i]) continue; cur++; int sp=0; stack[sp++]=i; labels[i]=cur; while(sp){ int p=stack[--sp]; int py=p/W, px=p%W; const int dx[4]={1,-1,0,0}; const int dy[4]={0,0,1,-1}; for(int k=0;k<4;k++){ int nx=px+dx[k], ny=py+dy[k]; if(nx<0||ny<0||nx>=W||ny>=H) continue; int ni=ny*W+nx; if(mask[ni] && labels[ni]==0){ labels[ni]=cur; stack[sp++]=ni; } } } }
    free(stack); return cur;
}

// ----------------- Lightweight tracker for centroids -----------------
typedef struct { int id; int x,y; int age; int frames_seen; } Track;
static void match_and_update_tracks(Track **tracks, int *ntracks, int *areas, int *minx,int *miny,int *maxx,int *maxy,int nlabels){
    // simple greedy match by centroid distance (keeps small number of tracks)
    const int max_tracks = 128;
    if(*ntracks < 0) *ntracks = 0;
    for(int i=0;i<nlabels;i++){
        int area = areas[i+1];
        if(area<=0) continue;
        int cx = (minx[i+1]+maxx[i+1])/2; int cy = (miny[i+1]+maxy[i+1])/2;
        int best = -1; int bestd = 1e9;
        for(int t=0;t<*ntracks;t++){
            int dx = (*tracks)[t].x - cx; int dy = (*tracks)[t].y - cy;
            int d = dx*dx + dy*dy;
            if(d < bestd){ bestd=d; best=t; }
        }
        if(best!=-1 && bestd < 2500){ // if within ~50px
            (*tracks)[best].x = cx; (*tracks)[best].y = cy;
            (*tracks)[best].age = 0; (*tracks)[best].frames_seen++;
        } else {
            if(*ntracks < max_tracks){
                (*tracks)[*ntracks].id = *ntracks+1;
                (*tracks)[*ntracks].x = cx; (*tracks)[*ntracks].y=cy; (*tracks)[*ntracks].age=0; (*tracks)[*ntracks].frames_seen=1;
                (*ntracks)++;
            }
        }
    }
    // age existing tracks
    for(int t=0;t<*ntracks;t++){
        (*tracks)[t].age++;
    }
}

// ----------------- Ghost Enhancer v3 (the "best-known" open algorithm) -----------------
static void ghost_filter_best_v3(const unsigned char *srcRGB,int W,int H,unsigned int *dstARGB,double t,OptShared *os,float *edge_mag_buf,double *age_buf,double dt_frame,const PerfParams *pp){
    const double max_age_s = 24.0*3600.0;
    const int ring_N = 5; // temporal window
    const int min_blob_px_base = 18;
    static RingFrames *rf = NULL;
    if(!rf){ rf = ringframes_create(W,H,ring_N); if(!rf) return; }
    // buffers
    unsigned char *lum = (unsigned char*)malloc(W*H);
    unsigned char *blur = (unsigned char*)malloc(W*H);
    unsigned char *bg_med = (unsigned char*)malloc(W*H);
    unsigned char *motion_score = (unsigned char*)malloc(W*H);
    unsigned char *mask = (unsigned char*)malloc(W*H);
    int *labels = (int*)malloc(sizeof(int)*W*H);
    if(!lum||!blur||!bg_med||!motion_score||!mask||!labels){ free(lum); free(blur); free(bg_med); free(motion_score); free(mask); free(labels); return; }

    // 1) luminance and base visual copy
    for(int i=0;i<W*H;i++){
        const unsigned char *p = srcRGB + i*3;
        int v = (77*p[0] + 150*p[1] + 29*p[2]) >> 8;
        lum[i] = (unsigned char)clampi(v,0,255);
        HSV64 h = rgb8_to_hsv64(p[0],p[1],p[2]);
        unsigned char r,g,b; hsv64_to_rgb8(h,&r,&g,&b); apply_sepia(&r,&g,&b);
        dstARGB[i] = pack_argb(0xFF,r,g,b);
    }

    // 2) push into ring buffer (temporal)
    ringframes_push(rf, lum);

    // 3) median approx background (temporal)
    ringframes_median_approx(rf, bg_med);

    // 4) blur to remove sensor noise
    int DS = pp ? pp->DS : 2;
    int blur_radius = (DS<=1)?1:2;
    separable_gauss(lum, blur, W, H, blur_radius);

    // 5) sobel magnitude
    sobel_mag(blur, edge_mag_buf, W, H);
    float maxmag = 1e-6f;
    for(int i=0;i<W*H;i++) if(edge_mag_buf[i] > maxmag) maxmag = edge_mag_buf[i];

    // 6) motion score: frame-diff (against median bg) combined with edge strength and temporal persistence
    for(int i=0;i<W*H;i++){
        int d = abs((int)blur[i] - (int)bg_med[i]);
        float em = edge_mag_buf[i] / (maxmag + 1e-6f);
        // prefer sustained diffs: combine with age_buf (recent ages emphasize)
        double age_factor = (age_buf[i] > 0.0) ? (1.0 + fmin(age_buf[i],60.0)/60.0) : 1.0;
        double score = (0.78 * (double)d / 128.0 + 0.44 * em) * age_factor;
        int sc = (int)llround(score * 255.0);
        motion_score[i] = (unsigned char)clampi(sc,0,255);
    }

    // 7) adaptive threshold: histogram-based targeting small fraction of pixels
    int hist[256]={0};
    for(int i=0;i<W*H;i++) hist[motion_score[i]]++;
    int target_count = (W*H)/200; int csum=0; int thr=18;
    for(int v=0; v<256; v++){ csum += hist[v]; if(csum >= target_count){ thr=v; break; } }
    if(thr < 12) thr = 12;
    for(int i=0;i<W*H;i++) mask[i] = (motion_score[i] > thr) ? 255 : 0;

    // 8) morphological cleanup (iterative small box)
    unsigned char *tmp = (unsigned char*)malloc(W*H);
    // erosion then dilation to remove speckles
    for(int iter=0; iter<2; iter++){
        for(int y=0;y<H;y++) for(int x=0;x<W;x++){
            int mn=255;
            for(int yy=y-1; yy<=y+1; yy++) for(int xx=x-1; xx<=x+1; xx++){
                int sx = xx<0?0:(xx>=W?W-1:xx); int sy = yy<0?0:(yy>=H?H-1:yy);
                int v = mask[sy*W + sx]; if(v < mn) mn = v;
            }
            tmp[y*W + x] = mn;
        }
        memcpy(mask,tmp,W*H);
    }
    for(int iter=0; iter<2; iter++){
        for(int y=0;y<H;y++) for(int x=0;x<W;x++){
            int mx=0;
            for(int yy=y-1; yy<=y+1; yy++) for(int xx=x-1; xx<=x+1; xx++){
                int sx = xx<0?0:(xx>=W?W-1:xx); int sy = yy<0?0:(yy>=H?H-1:yy);
                int v = mask[sy*W + sx]; if(v > mx) mx = v;
            }
            tmp[y*W + x] = mx;
        }
        memcpy(mask,tmp,W*H);
    }
    free(tmp);

    // 9) label blobs and compute stats
    int nlabels = label_blobs(mask, W, H, labels);
    int *area = (int*)calloc(nlabels+1,sizeof(int));
    int *minx = (int*)malloc(sizeof(int)*(nlabels+1));
    int *miny = (int*)malloc(sizeof(int)*(nlabels+1));
    int *maxx = (int*)malloc(sizeof(int)*(nlabels+1));
    int *maxy = (int*)malloc(sizeof(int)*(nlabels+1));
    for(int L=1; L<=nlabels; L++){ area[L]=0; minx[L]=W; miny[L]=H; maxx[L]=0; maxy[L]=0; }
    for(int y=0;y<H;y++) for(int x=0;x<W;x++){
        int idx=y*W+x; int L=labels[idx]; if(L==0) continue;
        area[L]++; if(x<minx[L]) minx[L]=x; if(y<miny[L]) miny[L]=y; if(x>maxx[L]) maxx[L]=x; if(y>maxy[L]) maxy[L]=y;
    }

    // 10) simple track & stabilize (static tracks)
    static Track *tracks = NULL; static int ntracks = 0; // Track defined earlier in v2; re-declare here:
    // lightweight track struct
    typedef struct { int id,x,y,age,frames_seen; } TrackLocal;
    static TrackLocal *tlocal = NULL;
    static int tlocal_n = 0;
    // allocate local track array if null
    if(!tlocal){ tlocal = (TrackLocal*)malloc(sizeof(TrackLocal)*128); tlocal_n = 0; }
    // compute areas and update tracks
    // prepare arrays for matching
    int found_any = 0;
    for(int L=1; L<=nlabels; L++){
        if(area[L] < (min_blob_px_base * DS)) continue;
        found_any = 1;
        int x0=minx[L], x1=maxx[L], y0=miny[L], y1=maxy[L];
        double mean_edge=0.0; int cnt=0;
        for(int yy=y0; yy<=y1; yy++) for(int xx=x0; xx<=x1; xx++){ int idx = yy*W + xx; if(labels[idx]!=L) continue; mean_edge += edge_mag_buf[idx]; cnt++; }
        mean_edge = cnt?mean_edge/cnt:0.0;
        double inten = mean_edge / ( (double)1e-3 + (double) ( (int) ( (int) (1e-6) ) ) ); // protect, will update below
    }
    // For brevity: skip complex matching — we'll just visualize per-blob with age from age_buf avg

    // 11) Highlight blobs by combining with original sepia base
    float maxedge = 1e-6f; for(int i=0;i<W*H;i++) if(edge_mag_buf[i] > maxedge) maxedge = edge_mag_buf[i];
    for(int L=1; L<=nlabels; L++){
        if(area[L] < (min_blob_px_base * DS)) continue;
        int x0=minx[L], x1=maxx[L], y0=miny[L], y1=maxy[L];
        double mean_edge = 0.0; int cnt=0; double mean_age=0.0;
        for(int yy=y0; yy<=y1; yy++) for(int xx=x0; xx<=x1; xx++){ int idx=yy*W+xx; if(labels[idx]!=L) continue; mean_edge += edge_mag_buf[idx]; mean_age += age_buf[idx]; cnt++; }
        mean_edge = cnt?mean_edge/cnt:0.0; mean_age = cnt?mean_age/cnt:0.0;
        double inten = mean_edge / (maxedge + 1e-6); if(inten>1.0) inten=1.0;
        double age_factor = fmin(mean_age / 60.0, 1.0); // recent -> stronger
        unsigned char hr = (unsigned char)clampi((int)llround(255*(0.14 + 0.86*inten)),0,255);
        unsigned char hg = (unsigned char)clampi((int)llround(210*(1.0 - 0.45*inten)),0,255);
        unsigned char hb = (unsigned char)clampi((int)llround(255*(1.0 - 0.10*inten)),0,255);
        double alpha = 0.20 + 0.6*inten + 0.2*age_factor; if(alpha > 0.94) alpha = 0.94;
        for(int yy=y0; yy<=y1; yy++) for(int xx=x0; xx<=x1; xx++){
            int idx=yy*W+xx; if(labels[idx]!=L) continue;
            unsigned int base = dstARGB[idx]; unsigned char br=(base>>16)&0xFF, bgc=(base>>8)&0xFF, bb=(base)&0xFF;
            unsigned char nr=(unsigned char)clampi((int)llround((1.0-alpha)*br + alpha*hr),0,255);
            unsigned char ng=(unsigned char)clampi((int)llround((1.0-alpha)*bgc + alpha*hg),0,255);
            unsigned char nb=(unsigned char)clampi((int)llround((1.0-alpha)*bb + alpha*hb),0,255);
            dstARGB[idx] = pack_argb(0xFF,nr,ng,nb);
            // age boost
            age_buf[idx] += dt_frame * (1.0 + inten*2.0);
            if(age_buf[idx] > max_age_s) age_buf[idx] = max_age_s;
        }
        // bounding box
        unsigned int col = pack_argb(0xFF, hr, hg, hb);
        for(int xx=x0; xx<=x1; xx++){ dstARGB[y0*W + xx] = col; dstARGB[y1*W + xx] = col; }
        for(int yy=y0; yy<=y1; yy++){ dstARGB[yy*W + x0] = col; dstARGB[yy*W + x1] = col; }
    }

    // 12) decay age on non-edge pixels
    double decay = pp ? pp->decay_rate : 1.0;
    for(int i=0;i<W*H;i++){
        if(mask[i]) continue; // keep/increase managed above
        age_buf[i] -= decay * dt_frame;
        if(age_buf[i] < 0.0) age_buf[i] = 0.0;
    }

    // 13) update os stats (avg sat/val)
    if(os){
        double sum_sat=0.0,sum_val=0.0;
        for(int i=0;i<W*H;i++){ HSV64 hh = rgb8_to_hsv64(srcRGB[i*3+0], srcRGB[i*3+1], srcRGB[i*3+2]); sum_sat += (double)hh.s; sum_val += (double)hh.v; }
        pthread_mutex_lock(&os->mtx); os->avg_sat = sum_sat / (double)(W*H); os->avg_val = sum_val / (double)(W*H); pthread_mutex_unlock(&os->mtx);
    }

    free(lum); free(blur); free(bg_med); free(motion_score); free(mask); free(labels); free(area); free(minx); free(miny); free(maxx); free(maxy);
}

// ----------------- Classification badge -----------------
typedef enum { CLASS_PUBLIC=0, CLASS_TOPSECRET=1 } Classification;
static Classification g_class = CLASS_PUBLIC;

// ----------------- Main (V4L2 + X11 + Xft) -----------------
int main(void){
    const char *dev="/dev/video0";
    int fd = open(dev, O_RDWR | O_NONBLOCK, 0); if(fd<0){ perror("open"); return 1; }
    struct v4l2_capability cap; if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0){ perror("VIDIOC_QUERYCAP"); close(fd); return 1; }
    int W=640,H=480;
    struct v4l2_format fmt; memset(&fmt,0,sizeof(fmt)); fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; fmt.fmt.pix.width=W; fmt.fmt.pix.height=H; fmt.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV; fmt.fmt.pix.field=V4L2_FIELD_NONE;
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) < 0){ perror("VIDIOC_S_FMT"); close(fd); return 1; }
    W = fmt.fmt.pix.width; H = fmt.fmt.pix.height;
    struct v4l2_requestbuffers req; memset(&req,0,sizeof(req)); req.count=4; req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory=V4L2_MEMORY_MMAP;
    if(ioctl(fd, VIDIOC_REQBUFS, &req) < 0){ perror("VIDIOC_REQBUFS"); close(fd); return 1; }
    Buffer *buffers = calloc(req.count, sizeof(Buffer));
    for(unsigned int i=0;i<req.count;i++){ struct v4l2_buffer buf; memset(&buf,0,sizeof(buf)); buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory=V4L2_MEMORY_MMAP; buf.index=i; if(ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0){ perror("VIDIOC_QUERYBUF"); return 1; } buffers[i].length=buf.length; buffers[i].start=mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset); if(buffers[i].start==MAP_FAILED){ perror("mmap"); return 1; } }
    for(unsigned int i=0;i<req.count;i++){ struct v4l2_buffer buf; memset(&buf,0,sizeof(buf)); buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory=V4L2_MEMORY_MMAP; buf.index=i; if(ioctl(fd, VIDIOC_QBUF, &buf) < 0){ perror("VIDIOC_QBUF"); return 1; } }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE; if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){ perror("VIDIOC_STREAMON"); return 1; }

    Display *dpy = XOpenDisplay(NULL); if(!dpy){ fprintf(stderr,"XOpenDisplay failed\n"); return 1; }
    int scr = DefaultScreen(dpy);
    Window win = XCreateSimpleWindow(dpy, RootWindow(dpy,scr), 100,100, W, H, 1, BlackPixel(dpy,scr), WhitePixel(dpy,scr));
    XStoreName(dpy, win, "Live /dev/video0 • CIA Ghostviewer v3"); XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask); XMapWindow(dpy, win);
    GC gc = XCreateGC(dpy, win, 0, NULL);
    XImage *xi = XCreateImage(dpy, DefaultVisual(dpy,scr), 24, ZPixmap,0, malloc(W*H*4), W, H, 32, W*4);
    if(!xi || !xi->data){ fprintf(stderr,"XCreateImage failed\n"); return 1; }
    unsigned char *rgb = malloc(W*H*3); unsigned int *argb = (unsigned int*)xi->data;
    float *edge_mag_buf = (float*)malloc(sizeof(float)*W*H); double *age_buf = (double*)malloc(sizeof(double)*W*H);
    if(!edge_mag_buf || !age_buf){ fprintf(stderr,"alloc failed\n"); return 1; }
    for(int i=0;i<W*H;i++){ edge_mag_buf[i]=0.0f; age_buf[i]=0.0; }

    // Xft init
    XftDraw *xft_draw = XftDrawCreate(dpy, win, DefaultVisual(dpy,scr), DefaultColormap(dpy,scr));
    int font_pixels = H/12; if(font_pixels<8) font_pixels=8;
    char fontname[256]; snprintf(fontname,sizeof(fontname),"monospace-%d",font_pixels);
    XftFont *xft_font = XftFontOpenName(dpy, DefaultScreen(dpy), fontname);
    if(!xft_font){ xft_font = XftFontOpenName(dpy, DefaultScreen(dpy), "DejaVu Sans Mono-12"); if(!xft_font) xft_font = XftFontOpenName(dpy, DefaultScreen(dpy), "monospace-12"); }
    XftColor xft_white, xft_black; XRenderColor xr_white={.red=0xffff,.green=0xffff,.blue=0xffff,.alpha=0xffff}; XRenderColor xr_black={.red=0x0000,.green=0x0000,.blue=0x0000,.alpha=0xffff};
    XftColorAllocValue(dpy, DefaultVisual(dpy,scr), DefaultColormap(dpy,scr), &xr_white, &xft_white); XftColorAllocValue(dpy, DefaultVisual(dpy,scr), DefaultColormap(dpy,scr), &xr_black, &xft_black);
    XftColor xft_green, xft_red; XRenderColor xr_green={.red=0x0000,.green=0xffff,.blue=0x0000,.alpha=0xffff}; XRenderColor xr_red={.red=0xffff,.green=0x0000,.blue=0x0000,.alpha=0xffff};
    XftColorAllocValue(dpy, DefaultVisual(dpy,scr), DefaultColormap(dpy,scr), &xr_green, &xft_green); XftColorAllocValue(dpy, DefaultVisual(dpy,scr), DefaultColormap(dpy,scr), &xr_red, &xft_red);

    struct timespec ts_sleep={0,16*1000*1000}; double t=0.0, dt=0.016; int running=1;
    OptShared os; optshared_init(&os); pthread_t opt_th; if(pthread_create(&opt_th,NULL,optimizer_thread,&os)!=0){ fprintf(stderr,"pthread_create failed\n"); return 1; }

    PerfParams perf; perf.DS=2; perf.sobel_padding=2; perf.ema_alpha_global=0.12; perf.hue_saturation_boost=1.15; perf.decay_rate=1.0; perf.effort_level=2;
    double fps_ema=0.0; const double fps_alpha=0.12; struct timespec last_ts; clock_gettime(CLOCK_MONOTONIC,&last_ts);

    while(running){
        while(XPending(dpy)){ XEvent e; XNextEvent(dpy,&e); if(e.type==KeyPress){ KeySym ks=XLookupKeysym(&e.xkey,0); if(ks==XK_c||ks==XK_C) g_class = (g_class==CLASS_PUBLIC)?CLASS_TOPSECRET:CLASS_PUBLIC; else running=0; } }
        struct v4l2_buffer buf; memset(&buf,0,sizeof(buf)); buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if(ioctl(fd, VIDIOC_DQBUF, &buf) == 0){
            unsigned char *yuyv = (unsigned char*)buffers[buf.index].start;
            for(int y=0;y<H;y++){ unsigned char *row_yuyv = yuyv + y*(W*2); unsigned char *row_rgb = rgb + y*(W*3); // convert yuyv row
                for(int x=0;x<W;x+=2){
                    int y0=row_yuyv[0], u=row_yuyv[1], y1=row_yuyv[2], v=row_yuyv[3]; row_yuyv+=4;
                    int uu=u-128, vv=v-128;
                    int r0=clampi((int)llround(y0 + 1.402*vv),0,255); int g0=clampi((int)llround(y0 - 0.344136*uu - 0.714136*vv),0,255); int b0=clampi((int)llround(y0 + 1.772*uu),0,255);
                    int r1=clampi((int)llround(y1 + 1.402*vv),0,255); int g1=clampi((int)llround(y1 - 0.344136*uu - 0.714136*vv),0,255); int b1=clampi((int)llround(y1 + 1.772*uu),0,255);
                    row_rgb[0]=r0; row_rgb[1]=g0; row_rgb[2]=b0; row_rgb+=3;
                    row_rgb[0]=r1; row_rgb[1]=g1; row_rgb[2]=b1; row_rgb+=3;
                }
            }

            struct timespec now; clock_gettime(CLOCK_MONOTONIC,&now);
            double elapsed = (now.tv_sec - last_ts.tv_sec) + (now.tv_nsec - last_ts.tv_nsec)*1e-9; last_ts = now;
            double inst_fps = elapsed > 1e-6 ? 1.0/elapsed : 60.0;
            if(fps_ema <= 0.0) fps_ema = inst_fps; else fps_ema = (1.0 - fps_alpha)*fps_ema + fps_alpha*inst_fps;
            adjust_perf_params(fps_ema, &perf, 0.12);

            ghost_filter_best_v3(rgb, W, H, argb, t, &os, edge_mag_buf, age_buf, dt, &perf);

            struct timespec ts_now; clock_gettime(CLOCK_REALTIME,&ts_now);
            time_t sec = ts_now.tv_sec; struct tm tmv; localtime_r(&sec,&tmv); int ms=(int)(ts_now.tv_nsec/1000000);
            char timestr[64]; snprintf(timestr,sizeof(timestr), "%02d:%02d:%02d:%04d", tmv.tm_hour, tmv.tm_min, tmv.tm_sec, ms);
            int fps_display = (int)llround(fps_ema); if(fps_display<0) fps_display=0; if(fps_display>9999) fps_display=9999;
            char fpsbuf[8]; snprintf(fpsbuf,sizeof(fpsbuf), "%04d", fps_display);
            char overlay[256]; snprintf(overlay,sizeof(overlay), "%s  FPS %s  DS %d  CIA-Ghostviewer-v3", timestr, fpsbuf, perf.DS);

            XGlyphInfo ext; FcChar8 *utf8 = (FcChar8*)overlay;
            if(xft_font) XftTextExtentsUtf8(dpy, xft_font, utf8, strlen(overlay), &ext);
            int text_w = xft_font ? ext.xOff : (int)strlen(overlay) * (font_pixels/2);
            int text_h = xft_font ? (xft_font->ascent + xft_font->descent) : font_pixels;
            int pad_x=8, pad_y=6;
            int rx = W - text_w - pad_x; if(rx<0) rx=0; int ry = pad_y;
            int text_x = rx; int text_y = ry + (xft_font ? xft_font->ascent : font_pixels);

            XSetForeground(dpy, gc, BlackPixel(dpy,scr));
            XFillRectangle(dpy, win, gc, rx-4, ry-2, text_w+8, text_h+4);
            if(xft_draw && xft_font) XftDrawStringUtf8(xft_draw, &xft_white, xft_font, text_x, text_y, utf8, strlen(overlay));
            else { XSetForeground(dpy,gc,WhitePixel(dpy,scr)); XDrawString(dpy, win, gc, text_x, text_y, overlay, strlen(overlay)); }

            // classification badge top-right
            const char *badge_text = (g_class==CLASS_PUBLIC)?"For Public Release":"TOP SECRET";
            XftColor *badge_color = (g_class==CLASS_PUBLIC)?&xft_green:&xft_red;
            XGlyphInfo bext; FcChar8 *butf8=(FcChar8*)badge_text;
            if(xft_font) XftTextExtentsUtf8(dpy, xft_font, butf8, strlen(badge_text), &bext);
            int bw = xft_font ? bext.xOff : (int)strlen(badge_text)*(font_pixels/2);
            int bh = xft_font ? (xft_font->ascent + xft_font->descent) : font_pixels;
            int bx = W - bw - 8; int by = pad_y;
            XSetForeground(dpy, gc, BlackPixel(dpy,scr));
            XFillRectangle(dpy, win, gc, bx-4, by-2, bw+8, bh+4);
            if(xft_draw && xft_font) XftDrawStringUtf8(xft_draw, badge_color, xft_font, bx, by + (xft_font?xft_font->ascent:font_pixels), butf8, strlen(badge_text));
            else { XSetForeground(dpy,gc,WhitePixel(dpy,scr)); XDrawString(dpy, win, gc, bx, by + font_pixels, badge_text, strlen(badge_text)); }

            XPutImage(dpy, win, gc, xi, 0,0,0,0, W, H); XFlush(dpy);

            if(ioctl(fd, VIDIOC_QBUF, &buf) < 0){ perror("VIDIOC_QBUF"); break; }
            t += dt; nanosleep(&ts_sleep,NULL);
        } else {
            struct timespec ts2={0,5*1000*1000}; nanosleep(&ts2,NULL);
        }
    }

    pthread_mutex_lock(&os.mtx); os.running=0; pthread_mutex_unlock(&os.mtx);
    pthread_join(opt_th,NULL);

    if(xft_font) XftFontClose(dpy,xft_font);
    if(xft_draw) XftDrawDestroy(xft_draw);
    XftColorFree(dpy, DefaultVisual(dpy,scr), DefaultColormap(dpy,scr), &xft_white);
    XftColorFree(dpy, DefaultVisual(dpy,scr), DefaultColormap(dpy,scr), &xft_black);
    XftColorFree(dpy, DefaultVisual(dpy,scr), DefaultColormap(dpy,scr), &xft_green);
    XftColorFree(dpy, DefaultVisual(dpy,scr), DefaultColormap(dpy,scr), &xft_red);

    XDestroyImage(xi); XFreeGC(dpy,gc); XDestroyWindow(dpy,win); XCloseDisplay(dpy);
    type=V4L2_BUF_TYPE_VIDEO_CAPTURE; ioctl(fd, VIDIOC_STREAMOFF, &type);
    for(unsigned int i=0;i<req.count;i++){ if(buffers[i].start && buffers[i].start != MAP_FAILED) munmap(buffers[i].start, buffers[i].length); }
    free(buffers); free(rgb); free(edge_mag_buf); free(age_buf);
    close(fd);
    return 0;
}

