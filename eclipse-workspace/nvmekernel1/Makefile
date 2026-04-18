# Makefile for building a PE/COFF UEFI application with mingw-w64 cross toolchain
# Usage:
#   make            -> builds BOOTX64.EFI
#   make clean

CROSS      := x86_64-w64-mingw32-
CC         := $(CROSS)gcc
OBJCOPY    := $(CROSS)objcopy
RM         := rm -f

SRCS       := bootx64.c
OBJS       := $(SRCS:.c=.o)
OUT        := BOOTX64.EFI
LDSCRIPT   := linker.ld

CFLAGS     := -I/usr/include/efi -I/usr/include/efi/x86_64 \
              -ffreestanding -fshort-wchar -mno-red-zone -Wall -O2 \
              -fno-stack-protector -fno-builtin -fno-exceptions
# Use -fno-pic/-fno-pie if your toolchain defaults to PIE; ensure object is suitable for PE/COFF
#CFLAGS    += -fno-pie

# Linker flags:
#  -nostdlib : avoid linking CRT
#  -Wl,-T,link.ld : use custom linker script
#  -Wl,-e,efi_main : set entry symbol to efi_main
#  -Wl,--subsystem,10 : set PE subsystem to EFI_APPLICATION (10)
LDFLAGS    := -nostdlib -Wl,-T,$(LDSCRIPT) -Wl,-e,efi_main -Wl,--subsystem,10

.PHONY: all clean

all: $(OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(OBJS) -o $(OUT)

clean:
	$(RM) $(OBJS) $(OUT)
