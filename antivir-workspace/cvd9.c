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
#include <dirent.h>

#define HEADER_SIZE 512

//--------------------------------------------------------------
// CONFIG
//--------------------------------------------------------------
static const char *CVD_FILE = "main.cvd";
static const char *OUTPUT_DIR = "extracted";
static const uid_t TARGET_UID = 1000;
static const gid_t TARGET_GID = 1000;

//--------------------------------------------------------------
// TAR header
//--------------------------------------------------------------
struct tar_header {
    char name[100]; char mode[8]; char uid[8]; char gid[8];
    char size[12]; char mtime[12]; char checksum[8]; char typeflag;
    char linkname[100]; char magic[6]; char version[2];
    char uname[32]; char gname[32]; char devmajor[8]; char devminor[8];
    char prefix[155]; char padding[12];
};

static size_t oct2int(const char *str, size_t size) {
    size_t v = 0;
    for (size_t i = 0; i < size && str[i]; i++)
        if (str[i] >= '0' && str[i] <= '7') v = (v << 3) + (str[i] - '0');
    return v;
}

//--------------------------------------------------------------
// GUNZIP
//--------------------------------------------------------------
unsigned char *gunzip_memory(const unsigned char *src, size_t src_len, size_t *out_len) {
    z_stream s; memset(&s, 0, sizeof(s));
    inflateInit2(&s, 15 + 32);

    size_t cap = src_len * 4;
    unsigned char *out = malloc(cap);
    if (!out) return NULL;

    s.next_in = (Bytef*)src;
    s.avail_in = src_len;
    size_t total = 0;

    for (;;) {
        if (total == cap) { cap *= 2; out = realloc(out, cap); if (!out) return NULL; }
        s.next_out = out + total;
        s.avail_out = cap - total;

        int ret = inflate(&s, Z_NO_FLUSH);
        total = cap - s.avail_out;

        if (ret == Z_STREAM_END) break;
        if (ret != Z_OK) { free(out); return NULL; }
    }

    inflateEnd(&s);
    *out_len = total;
    return out;
}

//--------------------------------------------------------------
// TAR extraction
//--------------------------------------------------------------
int extract_tar(const unsigned char *tar, size_t len, const char *outdir) {
    size_t offset = 0;

    while (offset + 512 <= len) {
        struct tar_header *h = (struct tar_header *)(tar + offset);

        int zero = 1;
        for (int i = 0; i < 512; i++) if (tar[offset + i] != 0) { zero = 0; break; }
        if (zero) break;

        char filename[512];
        snprintf(filename, sizeof(filename), "%s/%s", outdir, h->name);
        size_t fsize = oct2int(h->size, sizeof(h->size));

        if (h->typeflag == '5') {
            mkdir(filename, 0777);
            chown(filename, TARGET_UID, TARGET_GID);
            chmod(filename, 0777);
        } else if (h->typeflag == '0' || h->typeflag == '\0') {
            char pathbuf[512];
            strcpy(pathbuf, filename);
            for (char *p = pathbuf + 1; *p; p++) if (*p == '/') { *p = 0; mkdir(pathbuf, 0777); chown(pathbuf, TARGET_UID, TARGET_GID); chmod(pathbuf, 0777); *p = '/'; }
            int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd >= 0) { write(fd, tar + offset + 512, fsize); close(fd); chown(filename, TARGET_UID, TARGET_GID); chmod(filename, 0666); }
        }

        size_t blocks = (fsize + 511) / 512;
        offset += 512 + blocks * 512;
    }
    return 0;
}

//--------------------------------------------------------------
// HTML helpers
//--------------------------------------------------------------
void html_escape(FILE *f, const char *s) {
    for (; *s; s++) {
        if (*s=='<') fputs("&lt;",f);
        else if (*s=='>') fputs("&gt;",f);
        else if (*s=='&') fputs("&amp;",f);
        else fputc(*s,f);
    }
}

void html_add_hdb(FILE *html, const char *line, unsigned long long virus_index, unsigned long long *pattern_counter) {
    fprintf(html,"<h1>%llu. HDB Virus</h1>\n", virus_index);
    fprintf(html,"<h2>%llu. MD5</h2>\n", (*pattern_counter)++);
    fprintf(html,"<p>"); html_escape(html,line); fprintf(html,"</p>\n");
}

void html_add_ndb(FILE *html, const char *line, unsigned long long virus_index, unsigned long long *pattern_counter) {
    fprintf(html,"<h1>%llu. NDB Virus</h1>\n", virus_index);
    fprintf(html,"<h2>%llu. Regex</h2>\n", (*pattern_counter)++);
    fprintf(html,"<p>"); html_escape(html,line); fprintf(html,"</p>\n");
}

void html_add_ldb(FILE *html, const unsigned char *buf, size_t len, unsigned long long virus_index, unsigned long long *pattern_counter) {
    fprintf(html,"<h1>%llu. LDB Virus</h1>\n", virus_index);
    fprintf(html,"<h2>%llu. Hexdump / Binary</h2>\n", (*pattern_counter)++);
    fprintf(html,"<pre>\n");

    size_t offset = 0;
    while(offset < len) {
        size_t i;
        fprintf(html,"%08zx  ", offset);
        for(i=0;i<16;i++) {
            if(offset + i < len) fprintf(html,"%02x ", buf[offset+i]);
            else fprintf(html,"   ");
            if(i==7) fputc(' ', html);
        }
        fputc(' ', html);
        for(i=0;i<16 && offset+i < len;i++){
            unsigned char c = buf[offset+i];
            fputc((c>=32 && c<=126)?c:'.', html);
        }
        fputc('\n', html);
        offset += 16;
    }

    fprintf(html,"</pre>\n");
}

//--------------------------------------------------------------
// MAIN
//--------------------------------------------------------------
int main(void) {
    printf("Reading CVD file: %s\n", CVD_FILE);
    FILE *fp = fopen(CVD_FILE,"rb");
    if(!fp){ perror("open"); return 1; }

    fseek(fp,0,SEEK_END);
    size_t size = ftell(fp);
    fseek(fp,0,SEEK_SET);

    unsigned char *data = malloc(size);
    fread(data,1,size,fp);
    fclose(fp);

    unsigned char *gzptr = data + HEADER_SIZE;
    size_t gzlen = size - HEADER_SIZE;

    if(!(gzptr[0]==0x1F && gzptr[1]==0x8B)) { fprintf(stderr,"No gzip payload\n"); free(data); return 1; }

    size_t tar_len;
    unsigned char *tar_data = gunzip_memory(gzptr,gzlen,&tar_len);
    free(data);
    if(!tar_data){ fprintf(stderr,"Gzip decompression failed\n"); return 1; }

    mkdir(OUTPUT_DIR,0777);
    chown(OUTPUT_DIR,TARGET_UID,TARGET_GID);
    if(extract_tar(tar_data,tar_len,OUTPUT_DIR)!=0){ free(tar_data); return 1; }
    free(tar_data);

    FILE *html = fopen("virus_catalog.html","w");
    if(!html){ fprintf(stderr,"Cannot create HTML file\n"); return 1; }
    fprintf(html,"<!DOCTYPE html>\n<html lang=\"en\"><head><meta charset=\"UTF-8\"><title>Virus Catalog</title></head><body>\n");
    fprintf(html,"<h1>ClamAV Virus Catalog</h1>\n");

    DIR *d = opendir(OUTPUT_DIR);
    if(!d) return 1;

    struct dirent *de;
    char path[512];
    unsigned long long virus_index = 1;

    while((de=readdir(d))) {
        if(de->d_type != DT_REG) continue;
        snprintf(path,sizeof(path),"%s/%s",OUTPUT_DIR,de->d_name);

        FILE *f = fopen(path,"rb");
        if(!f) continue;

        fseek(f,0,SEEK_END);
        size_t flen = ftell(f);
        fseek(f,0,SEEK_SET);

        unsigned char *buf = malloc(flen+1);
        fread(buf,1,flen,f);
        fclose(f);
        buf[flen]=0;

        unsigned long long pattern_counter = 1;

        if(strstr(de->d_name,".hdb")) {
            char *saveptr=NULL, *line=strtok_r((char*)buf,"\n",&saveptr);
            while(line){ if(*line) html_add_hdb(html,line,virus_index,&pattern_counter); line=strtok_r(NULL,"\n",&saveptr);}
        } else if(strstr(de->d_name,".ndb")) {
            char *saveptr=NULL, *line=strtok_r((char*)buf,"\n",&saveptr);
            while(line){ if(*line) html_add_ndb(html,line,virus_index,&pattern_counter); line=strtok_r(NULL,"\n",&saveptr);}
        } else if(strstr(de->d_name,".ldb")) {
            html_add_ldb(html,buf,flen,virus_index,&pattern_counter);
        }

        free(buf);
        virus_index++;
    }

    closedir(d);
    fprintf(html,"</body></html>\n");
    fclose(html);

    printf("✓ HTML virus catalog generated: virus_catalog.html\n");
    return 0;
}

