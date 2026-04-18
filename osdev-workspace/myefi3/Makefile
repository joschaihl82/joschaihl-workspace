# Makefile - build pure x86_64 kernel (no 32-bit relocs), create EFI ISO and run with QEMU+OVMF
# Usage:
#   make				-> LowHalf build (VMA 0x00100000) and create ISO
#   make HIGH=1		 -> HighHalf build (VMA 0xFFFF800000010000) and create ISO
#   make run			-> build, create ISO, then run QEMU with OVMF
#   make HIGH=1 run	 -> HighHalf build, create ISO, run QEMU
#
# Requirements: clang, ld.lld, llvm-objcopy, mkfs.vfat (dosfstools), mtools (mmd/mcopy),
# xorriso, qemu-system-x86_64, OVMF.fd placed at boot/OVMF.fd (or set OVMF variable).

# --- Tools ---
CC	  := clang
LD	  := ld.lld
OBJCOPY := llvm-objcopy

# --- Build mode: HIGH=1 enables High-Half kernel (use -mcmodel=kernel) ---
HIGH ?= 0

# --- Flags ---
COMMON_CFLAGS := -target x86_64-unknown-elf -ffreestanding -mno-red-zone -Wall
COMMON_LDFLAGS := -nostdlib -z max-page-size=0x1000

ifeq ($(HIGH),1)
  KERNEL_CFLAGS := $(COMMON_CFLAGS) -fno-stack-protector -fno-pie -fno-pic \
				   -fno-common -fno-builtin -mcmodel=kernel -Wa,-mcmodel=kernel
  KERNEL_LDFLAGS := $(COMMON_LDFLAGS) -T kernel/linker_high.ld \
					-Wl,--image-base=0xFFFF800000010000
else
  KERNEL_CFLAGS := $(COMMON_CFLAGS) -fno-stack-protector -fno-pie -fno-pic \
				   -fno-common -fno-builtin
  KERNEL_LDFLAGS := $(COMMON_LDFLAGS) -T kernel/linker_low.ld
endif

# UEFI (PE) build flags for boot/ (UEFI app)
EFI_CFLAGS	:= -target x86_64-unknown-windows -ffreestanding -fshort-wchar \
				 -mno-red-zone -fno-stack-protector -Wall -I/usr/include/efi
EFI_LDFLAGS   := -target x86_64-unknown-windows -nostdlib -Wl,-entry:efiMain \
				 -Wl,-subsystem:efi_application -fuse-ld=lld

# --- Paths ---
BOOT_SRC	  := $(wildcard boot/*.c)
BOOT_OBJ	  := $(BOOT_SRC:.c=.o)
KERNEL_SRC	:= $(wildcard kernel/*.c)
KERNEL_OBJ	:= $(KERNEL_SRC:.c=.o)

BOOT_TARGET   := esp/EFI/BOOT/BOOTX64.EFI
KERNEL_TARGET := esp/EFI/BOOT/kernel.elf
KERNEL_ELF	:= kernel/kernel.elf
KERNEL_BIN	:= kernel/kernel.bin
ISO_TARGET	:= FatOS-x86_64.iso
EFI_IMG	   := esp/efi.img

# OVMF firmware path for QEMU run (adjust if needed)
OVMF := boot/OVMF.fd

# QEMU settings
QEMU := qemu-system-x86_64
QEMU_OPTS := -m 512M -serial stdio -no-reboot

# --- Rules ---
.PHONY: all clean run install-esp check-relocs

all: $(ISO_TARGET)

# Build and run: create ISO then boot with OVMF
run: all
	@if [ ! -f $(OVMF) ]; then \
	  echo "OVMF firmware not found at $(OVMF). Place OVMF.fd there or set OVMF variable."; exit 1; \
	fi
	@echo "Starting QEMU with OVMF and ISO..."
	$(QEMU) $(QEMU_OPTS) -bios $(OVMF) -drive file=$(ISO_TARGET),format=raw -net none

# ISO creation: build BOOTX64.EFI and kernel.elf into esp image and make ISO
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

# Ensure esp directory exists
install-esp:
	@mkdir -p esp/EFI/BOOT

# Bootloader (UEFI PE Binary)
$(BOOT_TARGET): $(BOOT_OBJ) | install-esp
	@mkdir -p $(dir $@)
	$(CC) $(EFI_LDFLAGS) $^ -o $@

boot/%.o: boot/%.c
	@mkdir -p $(dir $@)
	$(CC) $(EFI_CFLAGS) -c $< -o $@

# Kernel (ELF Binary) -> link into kernel/ then copy into esp
$(KERNEL_TARGET): $(KERNEL_ELF) | install-esp
	@mkdir -p $(dir $@)
	cp $(KERNEL_ELF) $(KERNEL_TARGET)

$(KERNEL_ELF): $(KERNEL_OBJ)
	@mkdir -p $(dir $@)
	$(LD) $(KERNEL_LDFLAGS) $^ -o $@
	@if command -v $(OBJCOPY) >/dev/null 2>&1; then \
	  $(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN) || true; \
	fi

kernel/%.o: kernel/%.c
	@mkdir -p $(dir $@)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

# Optional: check for problematic 32-bit relocations in object files
check-relocs:
	@echo "Checking relocations for 32-bit types in kernel objects..."
	@for f in $(KERNEL_OBJ); do \
	  echo "---- $$f ----"; \
	  readelf -r $$f | egrep 'R_X86_64_32S|R_X86_64_32' || echo "no 32-bit relocs in $$f"; \
	done
	@echo "Checking relocations in final ELF..."
	@readelf -r $(KERNEL_ELF) | egrep 'R_X86_64_32S|R_X86_64_32' || echo "no 32-bit relocs in final ELF"

clean:
	rm -rf esp $(ISO_TARGET) $(EFI_IMG) $(KERNEL_ELF) $(KERNEL_BIN) kernel/*.o boot/*.o

