# Makefile — build EFI bootable ISO with kernel, NVMe image and QEMU run target

# --- Tools ---
CC	  := clang
LD	  := ld.lld
OBJCOPY := llvm-objcopy

# --- Flags ---
EFI_CFLAGS	:= -target x86_64-unknown-windows -ffreestanding -fshort-wchar \
				 -mno-red-zone -fno-stack-protector -Wall -I/usr/include/efi
EFI_LDFLAGS   := -target x86_64-unknown-windows -nostdlib -Wl,-entry:efiMain \
				 -Wl,-subsystem:efi_application -fuse-ld=lld

KERNEL_CFLAGS := -target x86_64-unknown-elf -ffreestanding -mno-red-zone \
				 -fno-stack-protector -fno-pie -Wall
KERNEL_LDFLAGS:= -nostdlib -z max-page-size=0x1000 -T kernel/kernel.ld

# --- Paths / Targets ---
BOOT_SRC	  := $(wildcard boot/*.c)
BOOT_OBJ	  := $(BOOT_SRC:.c=.o)
KERNEL_SRC	:= $(wildcard kernel/*.c)
KERNEL_OBJ	:= $(KERNEL_SRC:.c=.o)

BOOT_TARGET   := esp/EFI/BOOT/BOOTX64.EFI
KERNEL_TARGET := esp/EFI/BOOT/kernel.elf
ISO_TARGET	:= FatOS-x86_64.iso
EFI_IMG	   := esp/efi.img
NVME_IMG	  := nvme_disk.img

# --- Rules ---
.PHONY: all clean run

all: $(ISO_TARGET) $(NVME_IMG)

# Create a dummy 128MB file to act as the NVMe storage if it doesn't exist
$(NVME_IMG):
	dd if=/dev/zero of=$(NVME_IMG) bs=1M count=128

# QEMU Execution
# boots the produced ISO as UEFI CD and attaches NVMe image as an NVMe device
run: all
	qemu-system-x86_64 \
		-bios boot/OVMF.fd \
		-m 512M \
		-cdrom $(ISO_TARGET) \
		-drive if=none,file=$(NVME_IMG),format=raw,id=nvm \
		-device nvme,drive=nvm,serial=nvme0 \
		-serial stdio \
		-net none

# ISO creation
$(ISO_TARGET): $(BOOT_TARGET) $(KERNEL_TARGET)
	@mkdir -p esp
	# create a 64MB FAT image for the EFI system partition
	dd if=/dev/zero of=$(EFI_IMG) bs=1M count=64
	/usr/sbin/mkfs.vfat -F 32 $(EFI_IMG)
	# create directories inside the FAT image and copy EFI binaries
	mmd -i $(EFI_IMG) ::/EFI
	mmd -i $(EFI_IMG) ::/EFI/BOOT
	mcopy -i $(EFI_IMG) $(BOOT_TARGET) ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i $(EFI_IMG) $(KERNEL_TARGET) ::/EFI/BOOT/kernel.elf
	# produce an ISO with the FAT image as the EFI system image
	# NOTE: use -e efi.img (not esp/efi.img) because esp/ is used as the source root
	xorriso -as mkisofs \
		-R -f -e efi.img \
		-isohybrid-gpt-basdat \
		-no-emul-boot \
		-o $(ISO_TARGET) esp/

# Bootloader (EFI application)
$(BOOT_TARGET): $(BOOT_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(EFI_LDFLAGS) $^ -o $@

boot/%.o: boot/%.c
	$(CC) $(EFI_CFLAGS) -c $< -o $@

# Kernel (ELF)
$(KERNEL_TARGET): $(KERNEL_OBJ)
	@mkdir -p $(dir $@)
	$(LD) $(KERNEL_LDFLAGS) $^ -o $@

kernel/%.o: kernel/%.c
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

clean:
	rm -rf esp $(ISO_TARGET) $(NVME_IMG) kernel/*.o boot/*.o

