/*
 make_image_nolosetup.c
 Erzeugt ein 256MiB raw image mit:
  - MBR (1MiB gap)
  - Partition 1: FAT32 EFI, 64MiB
  - Partition 2: ext4 root, rest
 Kopiert rEFInd + Kernel(+initramfs) in die FAT32-Partition using mtools.
 Benötigt: root, mkfs.vfat, mkfs.ext4, mtools (mcopy/mmd), wget, unzip.
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

int main(int argc, char **argv) {
    if (getuid() != 0) {
        fprintf(stderr, "Dieses Programm muss als root ausgeführt werden.\n");
        return 1;
    }
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <image-file> <kernel-path> [initramfs-path]\n", argv[0]);
        return 1;
    }

    const char *imgfile = argv[1];
    const char *kernel = argv[2];
    const char *initrd = (argc >= 4) ? argv[3] : NULL;

    /* Parameter */
    const unsigned long IMG_SIZE_MB = 256UL;
    const unsigned long SECTOR = 512UL;
    const unsigned long IMG_SIZE = IMG_SIZE_MB * 1024UL * 1024UL;
    const unsigned long FIRST_GAP_BYTES = 1UL * 1024UL * 1024UL; /* 1MiB */
    const unsigned long ESP_BYTES = 64UL * 1024UL * 1024UL;      /* 64MiB */
    const unsigned long ESP_START_SECTOR = FIRST_GAP_BYTES / SECTOR; /* 2048 */
    const unsigned long ESP_SECTORS = ESP_BYTES / SECTOR;

    if (IMG_SIZE < FIRST_GAP_BYTES + ESP_BYTES + (10UL*1024UL*1024UL)) {
        fprintf(stderr, "Image zu klein.\n");
        return 1;
    }

    printf("Erzeuge Image %s (%lu MiB)\n", imgfile, IMG_SIZE_MB);
    printf("Kernel: %s\n", kernel);
    if (initrd) printf("Initramfs: %s\n", initrd);

    /* 1) Erzeuge zwei temporäre Partition-Images: esp.img und root.img */
    const char *esp_img = "esp.img";
    const char *root_img = "root.img";

    unsigned long root_bytes = IMG_SIZE - FIRST_GAP_BYTES - ESP_BYTES;
    if (root_bytes < (10UL*1024UL*1024UL)) {
        fprintf(stderr, "Root-Partition zu klein.\n");
        return 1;
    }

    char cmd[1024];

    /* create esp.img */
    snprintf(cmd, sizeof(cmd), "dd if=/dev/zero of=%s bs=1M count=%lu status=none",
             esp_img, ESP_BYTES / (1024UL*1024UL));
    run_or_die(cmd);

    /* format FAT32 */
    snprintf(cmd, sizeof(cmd), "mkfs.vfat -F32 -n EFI %s >/dev/null", esp_img);
    run_or_die(cmd);

    /* create root.img */
    snprintf(cmd, sizeof(cmd), "dd if=/dev/zero of=%s bs=1M count=%lu status=none",
             root_img, root_bytes / (1024UL*1024UL));
    run_or_die(cmd);

    /* format ext4 */
    snprintf(cmd, sizeof(cmd), "mkfs.ext4 -F -L root %s >/dev/null", root_img);
    run_or_die(cmd);

    /* 2) Populate esp.img: rEFInd + kernel(+initrd) */
    /* Download rEFInd zip into temp dir */
    run_or_die("tmpd=$(mktemp -d) && cd \"$tmpd\" && "
               "wget -q -O refind.zip \"https://sourceforge.net/projects/refind/files/latest/download\" && "
               "unzip -q refind.zip || true && cd - >/dev/null");

    /* find refind dir (best effort) */
    run_or_die("tmpd=$(mktemp -d); cd \"$tmpd\"; wget -q -O refind.zip \"https://sourceforge.net/projects/refind/files/latest/download\"; unzip -q refind.zip || true; REF=$(find . -maxdepth 2 -type d -name \"refind*\" | head -n1); "
               "if [ -n \"$REF\" ]; then cp -r \"$REF\"/refind /tmp/refind_copy; fi; cd - >/dev/null");

    /* Ensure mtools config uses the image file directly (-i) */
    /* Create EFI directories inside esp.img using mtools */
    snprintf(cmd, sizeof(cmd), "mmd -i %s ::/EFI || true", esp_img);
    run_or_die(cmd);
    snprintf(cmd, sizeof(cmd), "mmd -i %s ::/EFI/BOOT || true", esp_img);
    run_or_die(cmd);

    /* Copy rEFInd files if available */
    snprintf(cmd, sizeof(cmd), "if [ -d /tmp/refind_copy ]; then mcopy -i %s -s /tmp/refind_copy/* ::/EFI/refind/ || true; fi", esp_img);
    run_or_die(cmd);

    /* Ensure BOOTX64.EFI exists (copy refind_x64.efi) */
    snprintf(cmd, sizeof(cmd),
             "if [ -f /tmp/refind_copy/refind_x64.efi ]; then mcopy -i %s /tmp/refind_copy/refind_x64.efi ::/EFI/BOOT/BOOTX64.EFI; fi",
             esp_img);
    run_or_die(cmd);

    /* Copy kernel and initrd into esp.img under /boot */
    snprintf(cmd, sizeof(cmd), "mmd -i %s ::/boot || true", esp_img);
    run_or_die(cmd);
    snprintf(cmd, sizeof(cmd), "mcopy -i %s %s ::/boot/vmlinuz-linux", esp_img, kernel);
    run_or_die(cmd);
    if (initrd) {
        snprintf(cmd, sizeof(cmd), "mcopy -i %s %s ::/boot/initramfs-linux.img", esp_img, initrd);
        run_or_die(cmd);
    }

    /* Create a simple refind.conf inside the refind dir on the FAT image */
    /* Write a temporary refind.conf and copy it in */
    const char *tmp_conf = "/tmp/refind.conf.tmp";
    FILE *f = fopen(tmp_conf, "w");
    if (!f) die("fopen tmp refind.conf");
    fprintf(f,
"scanfor internal,manual\n"
"menuentry \"Linux kernel on USB\" {\n"
"    icon /EFI/refind/icons/os_linux.png\n"
"    loader /boot/vmlinuz-linux\n"
"    initrd /boot/initramfs-linux.img\n"
"    options \"root=/dev/sda2 rw\"\n"
"}\n");
    fclose(f);
    snprintf(cmd, sizeof(cmd), "mcopy -i %s %s ::/EFI/refind/refind.conf", esp_img, tmp_conf);
    run_or_die(cmd);
    unlink(tmp_conf);

    /* 3) Create final image.raw and write MBR + partition contents at offsets */
    /* Create empty image file */
    int fd = open(imgfile, O_CREAT | O_RDWR, 0666);
    if (fd < 0) die("open imagefile");
    if (ftruncate(fd, IMG_SIZE) != 0) die("ftruncate imagefile");

    /* Build MBR (512 bytes) with partition table entries at offset 446 */
    unsigned char mbr[512];
    memset(mbr, 0, sizeof(mbr));

    /* Protective boot code can be left zero; set partition entries */
    /* Partition entry structure (16 bytes each):
       [0] boot flag
       [1..3] CHS start (we set zeros)
       [4] partition type
       [5..7] CHS end
       [8..11] LBA start (little endian)
       [12..15] number of sectors (little endian)
    */
    unsigned long total_sectors = IMG_SIZE / SECTOR;
    unsigned long p1_start = ESP_START_SECTOR;
    unsigned long p1_sectors = ESP_SECTORS;
    unsigned long p2_start = p1_start + p1_sectors;
    unsigned long p2_sectors = total_sectors - p2_start;

    /* Partition 1: FAT32 LBA type 0x0C */
    unsigned char *p = mbr + 446;
    p[0] = 0x00; /* not bootable */
    p[4] = 0x0C; /* FAT32 LBA */
    /* LBA start */
    p[8]  = (unsigned char)(p1_start & 0xFF);
    p[9]  = (unsigned char)((p1_start >> 8) & 0xFF);
    p[10] = (unsigned char)((p1_start >> 16) & 0xFF);
    p[11] = (unsigned char)((p1_start >> 24) & 0xFF);
    /* sectors */
    p[12] = (unsigned char)(p1_sectors & 0xFF);
    p[13] = (unsigned char)((p1_sectors >> 8) & 0xFF);
    p[14] = (unsigned char)((p1_sectors >> 16) & 0xFF);
    p[15] = (unsigned char)((p1_sectors >> 24) & 0xFF);

    /* Partition 2: Linux type 0x83 */
    unsigned char *p2 = mbr + 446 + 16;
    p2[0] = 0x00;
    p2[4] = 0x83;
    p2[8]  = (unsigned char)(p2_start & 0xFF);
    p2[9]  = (unsigned char)((p2_start >> 8) & 0xFF);
    p2[10] = (unsigned char)((p2_start >> 16) & 0xFF);
    p2[11] = (unsigned char)((p2_start >> 24) & 0xFF);
    p2[12] = (unsigned char)(p2_sectors & 0xFF);
    p2[13] = (unsigned char)((p2_sectors >> 8) & 0xFF);
    p2[14] = (unsigned char)((p2_sectors >> 16) & 0xFF);
    p2[15] = (unsigned char)((p2_sectors >> 24) & 0xFF);

    /* Boot signature */
    mbr[510] = 0x55;
    mbr[511] = 0xAA;

    /* Write MBR at offset 0 */
    if (pwrite(fd, mbr, sizeof(mbr), 0) != sizeof(mbr)) die("pwrite mbr");

    /* Write esp.img at offset p1_start * SECTOR */
    int fesp = open(esp_img, O_RDONLY);
    if (fesp < 0) die("open esp.img");
    off_t off = (off_t)p1_start * SECTOR;
    ssize_t r;
    char buf[65536];
    off_t pos = off;
    while ((r = read(fesp, buf, sizeof(buf))) > 0) {
        if (pwrite(fd, buf, r, pos) != r) die("pwrite esp to image");
        pos += r;
    }
    close(fesp);

    /* Write root.img at offset p2_start * SECTOR */
    int froot = open(root_img, O_RDONLY);
    if (froot < 0) die("open root.img");
    pos = (off_t)p2_start * SECTOR;
    while ((r = read(froot, buf, sizeof(buf))) > 0) {
        if (pwrite(fd, buf, r, pos) != r) die("pwrite root to image");
        pos += r;
    }
    close(froot);

    close(fd);

    /* Cleanup temporary files */
    unlink(esp_img);
    unlink(root_img);
    run_or_die("rm -rf /tmp/refind_copy || true");

    printf("Fertig: %s erstellt (%lu MiB).\n", imgfile, IMG_SIZE_MB);
    printf("Zum Schreiben auf Stick: cat %s > /dev/sdX ; sync\n", imgfile);
    printf("Hinweis: Dieses Image verwendet MBR mit einer FAT32 EFI-Partition.\n");
    return 0;
}
