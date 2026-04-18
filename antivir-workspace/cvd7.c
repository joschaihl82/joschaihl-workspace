#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include <errno.h>
#include <ctype.h>

#define HEADER_SIZE 512
#define OUTPUT_DIR "extracted"

// TAR header struct
struct tar_header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
};

// octal → integer
static size_t oct2int(const char *str, size_t size) {
    size_t v=0;
    for (size_t i=0;i<size && str[i];i++)
        if (str[i]>='0' && str[i]<='7') v=(v<<3)+(str[i]-'0');
    return v;
}

// GZIP decompression
unsigned char *gunzip_memory(const unsigned char *src, size_t src_len, size_t *out_len) {
    z_stream s; memset(&s,0,sizeof(s));
    if (inflateInit2(&s, 15 + 32) != Z_OK) return NULL;

    size_t cap = src_len * 4;
    unsigned char *out = malloc(cap);
    if (!out) return NULL;

    s.next_in = (Bytef*)src;
    s.avail_in = src_len;
    size_t total=0;

    for (;;) {
        if (total==cap) { cap*=2; out=realloc(out,cap); if(!out) return NULL; }
        s.next_out = out+total;
        s.avail_out = cap-total;
        int ret = inflate(&s, Z_NO_FLUSH);
        total = cap - s.avail_out;
        if (ret==Z_STREAM_END) break;
        if (ret!=Z_OK) { free(out); return NULL; }
    }

    inflateEnd(&s);
    *out_len = total;
    return out;
}

// ensure directories exist
void mkdirs(const char *path) {
    char buf[512]; strcpy(buf,path);
    for (char *p=buf+1;*p;p++) {
        if (*p=='/') { *p=0; mkdir(buf,0777); *p='/'; }
    }
}

// HTML escape helper
void html_escape(FILE *f, const char *s) {
    for (;*s;s++) {
        if (*s=='<') fputs("&lt;",f);
        else if (*s=='>') fputs("&gt;",f);
        else if (*s=='&') fputs("&amp;",f);
        else fputc(*s,f);
    }
}

// Check if buffer is likely text
int likely_text(const unsigned char *buf, size_t n) {
    if(n==0) return 1;
    size_t bad=0;
    for(size_t i=0;i<n;i++){
        unsigned char c=buf[i];
        if(c==9||c==10||c==13) continue;
        if(c>=32 && c<=126) continue;
        if(c>=0xC2 && c<=0xF4) continue;
        if(c==0) { bad+=4; continue; }
        bad++;
    }
    return bad*10<=n;
}

// TAR extraction + virus HTML generation
int process_tar(const unsigned char *tar, size_t len, FILE *html) {
    size_t offset=0;
    unsigned long long virus_count=0;

    while(offset+512<=len) {
        struct tar_header *h=(struct tar_header *)(tar+offset);

        // End of archive
        int zero=1; for(int i=0;i<512;i++){ if(tar[offset+i]!=0){ zero=0; break; } }
        if(zero) break;

        size_t fsize=oct2int(h->size,sizeof(h->size));
        unsigned char *filebuf = (unsigned char*)tar+offset+512;
        int text = likely_text(filebuf,fsize);

        virus_count++;

        fprintf(html,"<h1>Virus %llu (%s)</h1>\n",virus_count,text?"text":"binary");
        fprintf(html,"<h2>");
        html_escape(html,h->name);
        fprintf(html,"</h2>\n");

        fprintf(html,"<p>");
        if(text) {
            size_t snippet = fsize<256?fsize:256;
            for(size_t i=0;i<snippet;i++){
                if(filebuf[i]=='\n') fputc('<',html), fputs("br>",html);
                else html_escape(html,(char[]){filebuf[i],0});
            }
            if(fsize>snippet) fputs("...",html);
        } else fputs("Binary content (not displayed)",html);
        fprintf(html,"</p>\n");

        // next file
        size_t blocks=(fsize+511)/512;
        offset+=512+blocks*512;
    }

    return 0;
}

int main(void){
    printf("Reading CVD file: main.cvd\n");

    FILE *fp=fopen("main.cvd","rb");
    if(!fp){ perror("open"); return 1; }

    fseek(fp,0,SEEK_END);
    size_t size=ftell(fp);
    fseek(fp,0,SEEK_SET);

    unsigned char *data=malloc(size);
    fread(data,1,size,fp);
    fclose(fp);

    // Skip 512-byte header
    unsigned char *gzptr = data+HEADER_SIZE;
    size_t gzlen = size-HEADER_SIZE;

    if(!(gzptr[0]==0x1F && gzptr[1]==0x8B)){ fprintf(stderr,"No gzip payload\n"); free(data); return 1; }

    size_t tar_len;
    unsigned char *tar_data = gunzip_memory(gzptr,gzlen,&tar_len);
    free(data);
    if(!tar_data){ fprintf(stderr,"Gzip decompression failed\n"); return 1; }

    mkdir(OUTPUT_DIR,0777);

    FILE *html=fopen("virus_catalog.html","w");
    if(!html){ fprintf(stderr,"Cannot create HTML file\n"); free(tar_data); return 1; }

    fprintf(html,"<!DOCTYPE html>\n<html lang=\"en\"><head><meta charset=\"UTF-8\"><title>Virus Catalog</title></head><body>\n");
    fprintf(html,"<h1>ClamAV Virus Catalog</h1>\n");

    process_tar(tar_data,tar_len,html);

    fprintf(html,"</body></html>\n");
    fclose(html);

    free(tar_data);

    printf("✓ HTML virus catalog generated: virus_catalog.html\n");

    return 0;
}

