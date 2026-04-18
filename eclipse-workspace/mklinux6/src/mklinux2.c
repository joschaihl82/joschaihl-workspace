/*
 make_image_dynamic_esp.c
 Erstellt image.raw (256 MiB Standard) ohne Argumente.
 - Dynamische ESP-Größe: passt ESP an Kernel+Initramfs-Größe an (min 64MiB).
 - Partition 1: FAT32 EFI (dynamisch)
 - Partition 2: ext4 root (Rest)
 - Kopiert Kernel und Initramfs (falls vorhanden) in die FAT-ESP.
 - Benötigte Tools: dd, mkfs.vfat, mkfs.ext4, mtools (mcopy/mmd), wget, unzip
 - Erzeugung des Image benötigt keine root-UID; Schreiben auf Gerät erfordert Schreibrechte.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

static void die(const char *msg) {
    perror(msg);
    exit(1);
}
static void run_or_die(const char *cmd) {
    int rc = system(cmd);
    if (rc != 0) {
        fprintf(stderr, "Befehl fehlgeschlagen (%d): %s\n", rc, cmd);
        exit(1);
    }
}
static int file_exists(const char *path) {
    return access(path, F_OK) == 0;
}
static int file_readable(const char *path) {
    return access(path, R_OK) == 0;
}
static off_t file_size(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;
    return st.st_size;
}

/* round up to multiple of x */
static unsigned long round_up(unsigned long v, unsigned long x) {
    return ((v + x - 1) / x) * x;
}

int main(void) {
    const char *out_img = "image.raw";
    unsigned long IMG_MB = 256UL; /* default total image size in MiB */
    const unsigned long SECTOR = 512UL;
    unsigned long IMG_BYTES = IMG_MB * 1024UL * 1024UL;
    const unsigned long GAP_BYTES = 1UL * 1024UL * 1024UL; /* 1MiB gap before first partition */
    unsigned long ESP_BYTES_MIN = 64UL * 1024UL * 1024UL;  /* minimal 64MiB */
    const unsigned long ESP_MARGIN = 4UL * 1024UL * 1024UL; /* 4MiB safety margin */

    /* Kandidaten für Kernel/Initrd */
    const char *kernel_candidates[] = {
        "/boot/vmlinuz",
        "/boot/vmlinuz-linux",
        "/vmlinuz",
        "./vmlinuz",
        NULL
    };
    const char *initrd_candidates[] = {
        "/boot/initrd.img-6.17.0-19-generic",
        "/boot/initramfs-linux.img",
        "/boot/initramfs.img",
        "./initramfs.img",
        NULL
    };

    const char *kernel = NULL;
    const char *initrd = NULL;
    for (int i = 0; kernel_candidates[i]; ++i) {
        if (file_exists(kernel_candidates[i])) { kernel = kernel_candidates[i]; break; }
    }
    for (int i = 0; initrd_candidates[i]; ++i) {
        if (file_exists(initrd_candidates[i])) { initrd = initrd_candidates[i]; break; }
    }

    printf("Erzeuge Image %s (Standard %lu MiB)\n", out_img, IMG_MB);
    if (kernel) printf("Gefundener Kernel (existiert): %s\n", kernel); else printf("Kein Kernel in Standardpfaden gefunden.\n");
    if (initrd) printf("Gefundene Initramfs (existiert): %s\n", initrd); else printf("Keine Initramfs in Standardpfaden gefunden.\n");

    /* Ermittle Größen von Kernel und Initramfs (falls vorhanden) */
    off_t kernel_sz = 0, initrd_sz = 0;
    if (kernel && file_exists(kernel)) kernel_sz = file_size(kernel);
    if (initrd && file_exists(initrd)) initrd_sz = file_size(initrd);

    if (kernel_sz < 0) kernel_sz = 0;
    if (initrd_sz < 0) initrd_sz = 0;

    printf("Kernel-Größe: %lld bytes\n", (long long)kernel_sz);
    printf("Initramfs-Größe: %lld bytes\n", (long long)initrd_sz);

    /* Bestimme ESP-Größe: mindestens ESP_BYTES_MIN, sonst genug Platz für kernel+initrd + margin */
    unsigned long esp_needed = ESP_BYTES_MIN;
    unsigned long combined = (unsigned long)(kernel_sz + initrd_sz);
    if (combined + ESP_MARGIN > esp_needed) {
        /* runde auf MiB */
        unsigned long needed_mb = (combined + ESP_MARGIN + (1024UL*1024UL - 1)) / (1024UL*1024UL);
        esp_needed = needed_mb * 1024UL * 1024UL;
        /* setze auch ein vernünftiges Maximum: falls image zu klein, vergrößere image */
        unsigned long min_total = GAP_BYTES + esp_needed + (10UL * 1024UL * 1024UL);
        if (IMG_BYTES < min_total) {
            /* vergrößere Gesamtimage auf min_total rounded up to MiB */
            unsigned long new_img_mb = (min_total + (1024UL*1024UL - 1)) / (1024UL*1024UL);
            printf("Gesamtimage (%lu MiB) zu klein für ESP; vergrößere auf %lu MiB.\n", IMG_MB, new_img_mb);
            IMG_MB = new_img_mb;
            IMG_BYTES = IMG_MB * 1024UL * 1024UL;
        }
    }
    printf("Verwende ESP-Größe: %lu MiB\n", esp_needed / (1024UL*1024UL));

    /* Dateinamen für Partition-Images */
    const char *esp_img = "esp.img";
    const char *root_img = "root.img";

    unsigned long root_bytes = IMG_BYTES - GAP_BYTES - esp_needed;
    if (root_bytes < (10UL*1024UL*1024UL)) {
        fprintf(stderr, "Root-Partition zu klein nach Anpassung. Erhöhe Gesamtimage-Größe.\n");
        return 1;
    }

    char cmd[1024];

    /* 1) Erzeuge esp.img und root.img */
    snprintf(cmd, sizeof(cmd), "dd if=/dev/zero of=%s bs=1M count=%lu status=none",
             esp_img, esp_needed / (1024UL*1024UL));
    run_or_die(cmd);
    snprintf(cmd, sizeof(cmd), "dd if=/dev/zero of=%s bs=1M count=%lu status=none",
             root_img, root_bytes / (1024UL*1024UL));
    run_or_die(cmd);

    /* 2) Formatiere die Partition-Images */
    snprintf(cmd, sizeof(cmd), "mkfs.vfat -F32 -n EFI %s >/dev/null 2>&1", esp_img);
    run_or_die(cmd);
    snprintf(cmd, sizeof(cmd), "mkfs.ext4 -F -L root %s >/dev/null 2>&1", root_img);
    run_or_die(cmd);

    /* 3) Lade rEFInd herunter und extrahiere (best-effort) */
    run_or_die("tmpd=$(mktemp -d) && cd \"$tmpd\" && wget -q -O refind.zip \"https://sourceforge.net/projects/refind/files/latest/download\" || true && unzip -q refind.zip || true; cd - >/dev/null");
    run_or_die("tmpd=$(mktemp -d); cd \"$tmpd\"; wget -q -O refind.zip \"https://sourceforge.net/projects/refind/files/latest/download\" || true; unzip -q refind.zip || true; REF=$(find . -maxdepth 2 -type d -name \"refind*\" | head -n1); if [ -n \"$REF\" ]; then rm -rf /tmp/refind_copy 2>/dev/null || true; cp -r \"$REF\"/refind /tmp/refind_copy; fi; cd - >/dev/null");

    /* 4) Erstelle Verzeichnisse in FAT-Image und kopiere rEFInd */
    snprintf(cmd, sizeof(cmd), "mmd -i %s ::/EFI || true", esp_img); run_or_die(cmd);
    snprintf(cmd, sizeof(cmd), "mmd -i %s ::/EFI/BOOT || true", esp_img); run_or_die(cmd);
    snprintf(cmd, sizeof(cmd), "mmd -i %s ::/EFI/refind || true", esp_img); run_or_die(cmd);
    snprintf(cmd, sizeof(cmd), "if [ -d /tmp/refind_copy ]; then mcopy -i %s -s /tmp/refind_copy/* ::/EFI/refind/ || true; fi", esp_img);
    run_or_die(cmd);
    snprintf(cmd, sizeof(cmd), "if [ -f /tmp/refind_copy/refind_x64.efi ]; then mcopy -i %s /tmp/refind_copy/refind_x64.efi ::/EFI/BOOT/BOOTX64.EFI; fi", esp_img);
    run_or_die(cmd);

    /* 5) Kopiere Kernel/Initrd in FAT-Image; prüfe vorher Platz */
    /* Ermittle freien Platz in esp.img (grob): benutze dd um Größe zu prüfen ist kompliziert; wir prüfen gegen esp_needed */
    unsigned long esp_capacity = esp_needed;
    unsigned long used_estimate = 0;
    if (kernel_sz > 0) used_estimate += (unsigned long)kernel_sz;
    if (initrd_sz > 0) used_estimate += (unsigned long)initrd_sz;
    /* add overhead für Verzeichnisstrukturen ~1MiB */
    used_estimate += (1UL * 1024UL * 1024UL);

    if (used_estimate > esp_capacity) {
        fprintf(stderr, "Interner Fehler: berechneter Platzbedarf (%lu) größer als ESP-Kapazität (%lu).\n", used_estimate, esp_capacity);
        fprintf(stderr, "Bitte vergrößere das Image manuell.\n");
        return 1;
    }

    snprintf(cmd, sizeof(cmd), "mmd -i %s ::/boot || true", esp_img);
    run_or_die(cmd);

    if (kernel && file_readable(kernel)) {
        snprintf(cmd, sizeof(cmd), "mcopy -i %s %s ::/boot/vmlinuz-linux", esp_img, kernel);
        run_or_die(cmd);
        printf("Kernel %s in EFI-Image kopiert.\n", kernel);
    } else if (kernel) {
        fprintf(stderr, "Kernel %s nicht lesbar; überspringe Kopie. Lege ./vmlinuz an oder führe mit passenden Rechten aus.\n", kernel);
    }

    if (initrd && file_readable(initrd)) {
        snprintf(cmd, sizeof(cmd), "mcopy -i %s %s ::/boot/initramfs-linux.img", esp_img, initrd);
        run_or_die(cmd);
        printf("Initramfs %s in EFI-Image kopiert.\n", initrd);
    } else if (initrd) {
        fprintf(stderr, "Initramfs %s nicht lesbar; überspringe Kopie. Lege ./initramfs.img an oder führe mit passenden Rechten aus.\n", initrd);
    }

    /* 6) Schreibe einfache refind.conf */
    const char *tmpconf = "/tmp/refind.conf.tmp";
    FILE *f = fopen(tmpconf, "w");
    if (!f) die("fopen tmpconf");
    fprintf(f,
"scanfor internal,manual\n"
"menuentry \"Linux kernel on USB\" {\n"
"  icon /EFI/refind/icons/os_linux.png\n"
"  loader /boot/vmlinuz-linux\n"
"  initrd /boot/initramfs-linux.img\n"
"  options \"root=/dev/sda2 rw\"\n"
"}\n");
    fclose(f);
    snprintf(cmd, sizeof(cmd), "mcopy -i %s %s ::/EFI/refind/refind.conf", esp_img, tmpconf);
    run_or_die(cmd);
    unlink(tmpconf);

    /* 7) Erzeuge finale image.raw mit MBR und schreibe Partition-Images an Offsets */
    int fd = open(out_img, O_CREAT | O_RDWR, 0666);
    if (fd < 0) die("open imagefile");
    if (ftruncate(fd, IMG_BYTES) != 0) die("ftruncate imagefile");

    unsigned char mbr[512];
    memset(mbr, 0, sizeof(mbr));
    unsigned long total_sectors = IMG_BYTES / SECTOR;
    unsigned long p1_start = GAP_BYTES / SECTOR; /* 1MiB offset */
    unsigned long p1_sectors = esp_needed / SECTOR;
    unsigned long p2_start = p1_start + p1_sectors;
    unsigned long p2_sectors = total_sectors - p2_start;

    unsigned char *e = mbr + 446;
    e[0] = 0x00;
    e[4] = 0x0C; /* FAT32 LBA */
    e[8]  = (unsigned char)(p1_start & 0xFF);
    e[9]  = (unsigned char)((p1_start >> 8) & 0xFF);
    e[10] = (unsigned char)((p1_start >> 16) & 0xFF);
    e[11] = (unsigned char)((p1_start >> 24) & 0xFF);
    e[12] = (unsigned char)(p1_sectors & 0xFF);
    e[13] = (unsigned char)((p1_sectors >> 8) & 0xFF);
    e[14] = (unsigned char)((p1_sectors >> 16) & 0xFF);
    e[15] = (unsigned char)((p1_sectors >> 24) & 0xFF);

    unsigned char *e2 = e + 16;
    e2[0] = 0x00;
    e2[4] = 0x83; /* Linux */
    e2[8]  = (unsigned char)(p2_start & 0xFF);
    e2[9]  = (unsigned char)((p2_start >> 8) & 0xFF);
    e2[10] = (unsigned char)((p2_start >> 16) & 0xFF);
    e2[11] = (unsigned char)((p2_start >> 24) & 0xFF);
    e2[12] = (unsigned char)(p2_sectors & 0xFF);
    e2[13] = (unsigned char)((p2_sectors >> 8) & 0xFF);
    e2[14] = (unsigned char)((p2_sectors >> 16) & 0xFF);
    e2[15] = (unsigned char)((p2_sectors >> 24) & 0xFF);

    mbr[510] = 0x55;
    mbr[511] = 0xAA;

    if (pwrite(fd, mbr, sizeof(mbr), 0) != sizeof(mbr)) die("pwrite mbr");

    /* Schreibe esp.img an Offset p1_start*SECTOR */
    int fesp = open(esp_img, O_RDONLY);
    if (fesp < 0) die("open esp.img");
    off_t off = (off_t)p1_start * SECTOR;
    ssize_t r;
    char buf[65536];
    off_t pos = off;
    while ((r = read(fesp, buf, sizeof(buf))) > 0) {
        if (pwrite(fd, buf, r, pos) != r) die("pwrite esp");
        pos += r;
    }
    close(fesp);

    /* Schreibe root.img an Offset p2_start*SECTOR */
    int froot = open(root_img, O_RDONLY);
    if (froot < 0) die("open root.img");
    pos = (off_t)p2_start * SECTOR;
    while ((r = read(froot, buf, sizeof(buf))) > 0) {
        if (pwrite(fd, buf, r, pos) != r) die("pwrite root");
        pos += r;
    }
    close(froot);
    close(fd);

    /* Aufräumen */
    unlink(esp_img);
    unlink(root_img);
    run_or_die("rm -rf /tmp/refind_copy || true");

    printf("Fertig: %s erstellt (%lu MiB).\n", out_img, IMG_MB);
    printf("Zum Schreiben auf Stick: sudo sh -c 'cat %s > /dev/sdX && sync'\n", out_img);
    return 0;
}
