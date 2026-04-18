CC := x86_64-w64-mingw32-gcc

CFLAGS := -O2 \
          -march=x86-64 -mtune=generic \
          -mno-sse -mno-sse2 -mno-sse3 -mno-sse4.1 -mno-sse4.2 -mno-avx -mno-avx2 -mno-mmx \
          -fno-tree-vectorize -fno-tree-slp-vectorize -fno-tree-loop-vectorize \
          -ffreestanding -fno-builtin -fno-exceptions -fno-stack-protector -mno-red-zone \
          -Wall -Wextra

LDFLAGS := -nostdlib -Wl,--subsystem,10 -Wl,-e,efi_main -Wl,--image-base,0x400000

SRC := main.c
OUT := bootx64.efi

all:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OUT) $(SRC)
