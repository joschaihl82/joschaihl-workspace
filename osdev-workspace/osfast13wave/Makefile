# --- Tools ---
CC      := clang
LD      := ld.lld
OBJCOPY := llvm-objcopy

# --- Flags ---
EFI_CFLAGS    := -target x86_64-unknown-windows -ffreestanding -fshort-wchar \
                 -mno-red-zone -fno-stack-protector -Wall -I/usr/include/efi
EFI_LDFLAGS   := -target x86_64-unknown-windows -nostdlib -Wl,-entry:efiMain \
                 -Wl,-subsystem:efi_application -fuse-ld=lld

KERNEL_CFLAGS := -target x86_64-unknown-elf -ffreestanding -mno-red-zone \
                 -fno-stack-protector -fno-pie -Wall -ffast-math -O3
KERNEL_LDFLAGS:= -nostdlib -z max-page-size=0x1000 -T kernel/kernel.ld

# --- Pfade ---
BOOT_SRC      := $(wildcard boot/*.c)
BOOT_OBJ      := $(BOOT_SRC:.c=.o)
KERNEL_SRC    := $(wildcard kernel/*.c)
KERNEL_OBJ    := $(KERNEL_SRC:.c=.o)

BOOT_TARGET   := esp/EFI/BOOT/BOOTX64.EFI
KERNEL_TARGET := esp/EFI/BOOT/kernel.elf
ISO_TARGET    := FatOS-x86_64.iso
EFI_IMG       := esp/efi.img
NVME_IMG      := nvme_disk.img

# --- Regeln ---

.PHONY: all clean run

all: $(ISO_TARGET) $(NVME_IMG)

# Create a dummy 128MB file to act as the NVMe storage if it doesn't exist
$(NVME_IMG):
	dd if=/dev/zero of=$(NVME_IMG) bs=1M count=128

# QEMU Execution
# 1. -drive ... index=0 is your "CD-ROM" boot device.
# 2. -drive ... id=nvm ... + -device nvme ... simulates the NVMe hardware.
run: all
	qemu-system-x86_64 -bios boot/OVMF.fd \
		-drive file=$(ISO_TARGET),format=raw,index=0,media=cdrom \
		-drive file=$(NVME_IMG),if=none,id=nvm \
		-device nvme,serial=deadbeef,drive=nvm \
		-net none -m 512M -serial stdio

# ISO Erstellung
$(ISO_TARGET): $(BOOT_TARGET) $(KERNEL_TARGET)
	@mkdir -p esp
	dd if=/dev/zero of=$(EFI_IMG) bs=1M count=64
	/usr/sbin/mkfs.vfat -F 32 $(EFI_IMG)
	mmd -i $(EFI_IMG) ::/EFI
	mmd -i $(EFI_IMG) ::/EFI/BOOT
	mcopy -i $(EFI_IMG) $(BOOT_TARGET) ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i $(EFI_IMG) $(KERNEL_TARGET) ::/EFI/BOOT/kernel.elf
	xorriso -as mkisofs \
		-R -f -e efi.img \
		-no-emul-boot \
		-o $(ISO_TARGET) esp/

# Bootloader
$(BOOT_TARGET): $(BOOT_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(EFI_LDFLAGS) $^ -o $@

boot/%.o: boot/%.c
	$(CC) $(EFI_CFLAGS) -c $< -o $@

# Kernel
$(KERNEL_TARGET): $(KERNEL_OBJ)
	@mkdir -p $(dir $@)
	$(LD) $(KERNEL_LDFLAGS) $^ -o $@

kernel/%.o: kernel/%.c
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

clean:
	rm -rf esp $(ISO_TARGET) $(NVME_IMG) kernel/*.o boot/*.o

