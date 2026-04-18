/* make_usb.c - Einfaches, destruktives Hilfsprogramm zum Erstellen eines EFI-USB
   Achtung: führt system() Befehle aus und löscht das Zielgerät. Nur als root. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int run(const char *cmd) {
    int rc = system(cmd);
    if (rc != 0) {
        fprintf(stderr, "Befehl fehlgeschlagen: %s\n", cmd);
        exit(1);
    }
    return rc;
}

int main(int argc, char **argv) {
    if (getuid() != 0) {
        fprintf(stderr, "Bitte als root ausführen.\n");
        return 1;
    }
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <device> <kernel_path> [initramfs_path]\n", argv[0]);
        return 1;
    }
    const char *dev = argv[1];
    const char *kernel = argv[2];
    const char *initrd = (argc >=4) ? argv[3] : NULL;
    char conf[1024];

    printf("Zielgerät: %s\n", dev);
    printf("Kernel: %s\n", kernel);
    if (initrd) printf("Initramfs: %s\n", initrd);
    printf("Fortfahren? Tippe 'yes' und Enter: ");
    char ans[8];
    if (!fgets(ans, sizeof(ans), stdin) || strcmp(ans, "yes\n") != 0) {
        printf("Abgebrochen.\n"); return 0;
    }

    /* Partitionieren */
    snprintf(conf, sizeof(conf),
        "parted --script %s mklabel gpt && "
        "parted --script %s mkpart primary fat32 1MiB 513MiB && "
        "parted --script %s set 1 boot on && "
        "parted --script %s mkpart primary ext4 513MiB 100%%",
        dev, dev, dev, dev);
    run(conf);

    /* Device nodes (einfache Annahme: /dev/sda1 /dev/sda2) */
    char esp[64], root[64];
    snprintf(esp, sizeof(esp), "%s1", dev);
    snprintf(root, sizeof(root), "%s2", dev);

    /* Dateisysteme */
    snprintf(conf, sizeof(conf), "mkfs.vfat -F32 -n EFI %s", esp); run(conf);
    snprintf(conf, sizeof(conf), "mkfs.ext4 -F -L root %s", root); run(conf);

    /* Mounten und kopieren */
    run("mkdir -p /mnt/usb_esp /mnt/usb_root");
    snprintf(conf, sizeof(conf), "mount %s /mnt/usb_root", root); run(conf);
    snprintf(conf, sizeof(conf), "mkdir -p /mnt/usb_root/boot"); run(conf);
    snprintf(conf, sizeof(conf), "cp %s /mnt/usb_root/boot/vmlinuz-linux", kernel); run(conf);
    if (initrd) {
        snprintf(conf, sizeof(conf), "cp %s /mnt/usb_root/boot/initramfs-linux.img", initrd); run(conf);
    }
    snprintf(conf, sizeof(conf), "mount %s /mnt/usb_esp", esp); run(conf);
    snprintf(conf, sizeof(conf), "mkdir -p /mnt/usb_esp/EFI/BOOT"); run(conf);

    /* rEFInd herunterladen und entpacken (SourceForge latest) */
    run("tmpd=$(mktemp -d) && cd \"$tmpd\" && wget -O refind.zip \"https://sourceforge.net/projects/refind/files/latest/download\" && unzip refind.zip >/dev/null && REF=$(find . -maxdepth 2 -type d -name \"refind*\" | head -n1) && cp -r \"$REF\"/refind /mnt/usb_esp/EFI/ && cp /mnt/usb_esp/EFI/refind/refind_x64.efi /mnt/usb_esp/EFI/BOOT/BOOTX64.EFI || true");

    /* einfache refind.conf */
    run("cat >/mnt/usb_esp/EFI/refind/refind.conf <<'EOF'\nscanfor internal,manual\nmenuentry \"Linux kernel on USB\" {\n  loader /boot/vmlinuz-linux\n  initrd /boot/initramfs-linux.img\n  options \"root=/dev/sda2 rw\"\n}\nEOF");

    run("sync && umount /mnt/usb_esp || true && umount /mnt/usb_root || true");
    printf("Fertig. USB-Stick vorbereitet.\n");
    return 0;
}
