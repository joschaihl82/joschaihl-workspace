// vmm_ovmf_sdl.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/kvm.h>
#include <assert.h>

#include <SDL2/SDL.h>

// ---------------- Config ----------------
#define GUEST_RAM_SIZE      (512ULL * 1024 * 1024)   // 512 MiB
#define FWCFG_SEL_PORT      0x510
#define FWCFG_DATA_PORT     0x511
#define WINDOW_W            800
#define WINDOW_H            600

// fw_cfg control keys (subset)
#define FW_CFG_SIGNATURE    0x0000
#define FW_CFG_ID           0x0001
#define FW_CFG_NB_CPUS      0x0005
#define FW_CFG_E820_TABLE   0x0013
#define FW_CFG_FILE_DIR     0x0019

// Directory selector space (use >= 0x2000 per convention)
#define FW_CFG_SELECT_BASE  0x2000
#define FW_CFG_KEY_RAMFB    (FW_CFG_SELECT_BASE + 0x10) // arbitrary unique

// e820 types
#define E820_RAM       1
#define E820_RESERVED  2

// Ramfb descriptor as used by OVMF/QEMU (little-endian fields)
struct fw_cfg_ramfb {
    uint64_t addr;    // Guest physical address of the framebuffer
    uint32_t fourcc;  // 'XR24' (XRGB8888)
    uint32_t flags;   // reserved (0)
    uint32_t width;
    uint32_t height;
    uint32_t stride;  // bytes per scanline
} __attribute__((packed));

struct e820_entry {
    uint64_t addr;
    uint64_t size;
    uint32_t type;
} __attribute__((packed));

// fw_cfg file directory entry (QEMU-compatible layout; sizes in big-endian)
struct fwcfg_file {
    uint32_t size_be;      // big-endian
    uint16_t select_be;    // big-endian
    uint16_t reserved;
    char name[56];         // NUL-terminated or padded
} __attribute__((packed));

struct fwcfg_item {
    uint16_t key;
    uint8_t *data;
    uint32_t len;
    int writable;          // 1 if guest can write to this item
};

struct fwcfg_state {
    uint16_t current_key;
    uint32_t offset;
    struct fwcfg_item *items;
    size_t n_items;

    // live ramfb state
    struct fw_cfg_ramfb ramfb;
    int ramfb_ready;
};

// ---------------- Helpers ----------------
static void die(const char *msg) {
    perror(msg);
    exit(1);
}

static uint64_t rom_base_for_size(uint64_t rom_size) {
    const uint64_t top = 0x100000000ULL; // 4 GiB
    return top - rom_size;
}

static uint32_t be32(uint32_t v) {
    return ((v & 0x000000FFu) << 24) |
           ((v & 0x0000FF00u) << 8)  |
           ((v & 0x00FF0000u) >> 8)  |
           ((v & 0xFF000000u) >> 24);
}

static uint16_t be16(uint16_t v) {
    return ((v & 0x00FFu) << 8) | ((v & 0xFF00u) >> 8);
}

static void fwcfg_add(struct fwcfg_state *fw, uint16_t key, const void *buf, uint32_t len, int writable) {
    fw->items = realloc(fw->items, (fw->n_items + 1) * sizeof(*fw->items));
    fw->items[fw->n_items].key = key;
    fw->items[fw->n_items].data = malloc(len);
    memcpy(fw->items[fw->n_items].data, buf, len);
    fw->items[fw->n_items].len = len;
    fw->items[fw->n_items].writable = writable;
    fw->n_items++;
}

static struct fwcfg_item* fwcfg_find(struct fwcfg_state *fw, uint16_t key) {
    for (size_t i = 0; i < fw->n_items; i++)
        if (fw->items[i].key == key) return &fw->items[i];
    return NULL;
}

static uint8_t fwcfg_read_byte(struct fwcfg_state *fw) {
    struct fwcfg_item* it = fwcfg_find(fw, fw->current_key);
    if (!it) return 0;
    if (fw->offset >= it->len) return 0;
    uint8_t b = it->data[fw->offset];
    fw->offset++;
    return b;
}

static void fwcfg_write_byte(struct fwcfg_state *fw, uint8_t b) {
    struct fwcfg_item* it = fwcfg_find(fw, fw->current_key);
    if (!it || !it->writable) return;
    if (fw->offset < it->len) {
        it->data[fw->offset] = b;
        fw->offset++;
    }
}

static void fwcfg_select(struct fwcfg_state *fw, uint16_t key) {
    fw->current_key = key;
    fw->offset = 0;
}

static void fwcfg_sync_ramfb(struct fwcfg_state *fw) {
    // The ramfb item buffer holds the descriptor; copy into live struct
    struct fwcfg_item* it = fwcfg_find(fw, FW_CFG_KEY_RAMFB);
    if (!it || it->len < sizeof(struct fw_cfg_ramfb)) return;
    memcpy(&fw->ramfb, it->data, sizeof(struct fw_cfg_ramfb));
    // Validate fourcc 'XR24' (ASCII) in little-endian: 'X' 'R' '2' '4'
    const uint32_t XR24 = ('X') | ('R'<<8) | ('2'<<16) | ('4'<<24);
    if (fw->ramfb.fourcc != XR24 || fw->ramfb.width == 0 || fw->ramfb.height == 0)
        return;
    fw->ramfb_ready = 1;
}

// Map guest phys to host pointer if inside a mapped RAM slot
static inline uint8_t* gpa_to_host(uint64_t gpa, void *ram_base, uint64_t ram_size) {
    if (gpa < ram_size) return (uint8_t*)ram_base + gpa;
    return NULL;
}

// ---------------- Main ----------------
int main(int argc, char **argv) {
    char *firmware_path = "/usr/share/ovmf/OVMF.fd";

    if (argc >= 2) {
        firmware_path = argv[1];
    }

    // Load OVMF firmware
    int fwfd = open(firmware_path, O_RDONLY);
    if (fwfd < 0) die("open OVMF.fd");
    off_t fwsize = lseek(fwfd, 0, SEEK_END);
    if (fwsize <= 0) { close(fwfd); die("firmware size"); }
    lseek(fwfd, 0, SEEK_SET);
    uint8_t *fwbuf = malloc(fwsize);
    if (!fwbuf) die("malloc fwbuf");
    ssize_t rd = read(fwfd, fwbuf, fwsize);
    if (rd != fwsize) die("read firmware");
    close(fwfd);

    // KVM setup
    int kvm = open("/dev/kvm", O_RDWR | O_CLOEXEC);
    if (kvm < 0) die("open /dev/kvm");
    int api = ioctl(kvm, KVM_GET_API_VERSION, 0);
    if (api != KVM_API_VERSION) { fprintf(stderr, "KVM API mismatch\n"); return 1; }

    int vm = ioctl(kvm, KVM_CREATE_VM, 0);
    if (vm < 0) die("KVM_CREATE_VM");

    // IRQ chip + PIT, TSS and identity map required for in-kernel IRQ chip
    if (ioctl(vm, KVM_SET_TSS_ADDR, 0xFFFFD000) < 0) die("KVM_SET_TSS_ADDR");
    uint64_t identity_map = 0xFFFFC000ULL;
    if (ioctl(vm, KVM_SET_IDENTITY_MAP_ADDR, &identity_map) < 0) die("KVM_SET_IDENTITY_MAP_ADDR");
    if (ioctl(vm, KVM_CREATE_IRQCHIP, 0) < 0) die("KVM_CREATE_IRQCHIP");
    struct kvm_pit_config pit = { .flags = 0 };
    if (ioctl(vm, KVM_CREATE_PIT2, &pit) < 0) die("KVM_CREATE_PIT2");

    // Guest RAM
    void *ram = mmap(NULL, GUEST_RAM_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ram == MAP_FAILED) die("mmap ram");
    struct kvm_userspace_memory_region ram_region = {
        .slot = 0,
        .flags = 0,
        .guest_phys_addr = 0x00000000ULL,
        .memory_size = GUEST_RAM_SIZE,
        .userspace_addr = (uint64_t)ram
    };
    if (ioctl(vm, KVM_SET_USER_MEMORY_REGION, &ram_region) < 0) die("SET RAM");

    // Firmware ROM at top of 4GiB
    uint64_t rom_base = rom_base_for_size((uint64_t)fwsize);
    void *rom = mmap(NULL, fwsize, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (rom == MAP_FAILED) die("mmap rom");
    memcpy(rom, fwbuf, fwsize);
    struct kvm_userspace_memory_region rom_region = {
        .slot = 1,
        .flags = KVM_MEM_READONLY,
        .guest_phys_addr = rom_base,
        .memory_size = fwsize,
        .userspace_addr = (uint64_t)rom
    };
    if (ioctl(vm, KVM_SET_USER_MEMORY_REGION, &rom_region) < 0) die("SET ROM");

    // vCPU
    int vcpu = ioctl(vm, KVM_CREATE_VCPU, 0);
    if (vcpu < 0) die("KVM_CREATE_VCPU");
    int run_size = ioctl(kvm, KVM_GET_VCPU_MMAP_SIZE, 0);
    if (run_size <= 0) die("KVM_GET_VCPU_MMAP_SIZE");
    struct kvm_run *run = mmap(NULL, run_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu, 0);
    if (run == MAP_FAILED) die("mmap kvm_run");

    // Optional: fetch/set sregs to ensure sane reset state
    struct kvm_sregs sregs;
    if (ioctl(vcpu, KVM_GET_SREGS, &sregs) < 0) die("GET_SREGS");
    if (ioctl(vcpu, KVM_SET_SREGS, &sregs) < 0) die("SET_SREGS");

    // ------------- fw_cfg -------------
    struct fwcfg_state fw = {0};

    // Signature "QEMU" in big-endian 32-bit
    uint32_t sig_be = be32(0x51454D55u);
    fwcfg_add(&fw, FW_CFG_SIGNATURE, &sig_be, sizeof(sig_be), 0);

    // ID: bit0 set to indicate basic features (value arbitrary)
    uint32_t id = 0x00000001u;
    fwcfg_add(&fw, FW_CFG_ID, &id, sizeof(id), 0);

    // CPUs: 1
    uint32_t nb_cpus = 1;
    fwcfg_add(&fw, FW_CFG_NB_CPUS, &nb_cpus, sizeof(nb_cpus), 0);

    // E820: RAM + reserved ROM
    struct e820_entry e820[2];
    e820[0].addr = 0;
    e820[0].size = GUEST_RAM_SIZE;
    e820[0].type = E820_RAM;
    e820[1].addr = rom_base;
    e820[1].size = (uint64_t)fwsize;
    e820[1].type = E820_RESERVED;
    fwcfg_add(&fw, FW_CFG_E820_TABLE, &e820[0], sizeof(e820), 0);

    // Ramfb descriptor buffer (writable by guest)
    struct fw_cfg_ramfb ramfb_init = {0};
    fwcfg_add(&fw, FW_CFG_KEY_RAMFB, &ramfb_init, sizeof(ramfb_init), 1);

    // Build fw_cfg file directory:
    // One file "etc/ramfb" pointing to selector FW_CFG_KEY_RAMFB
    struct fwcfg_file dir;
    memset(&dir, 0, sizeof(dir));
    dir.size_be   = be32(sizeof(struct fw_cfg_ramfb));
    dir.select_be = be16(FW_CFG_KEY_RAMFB);
    snprintf(dir.name, sizeof(dir.name), "etc/ramfb");
    fwcfg_add(&fw, FW_CFG_FILE_DIR, &dir, sizeof(dir), 0);

    printf("OVMF size: %ld bytes, ROM at [0x%lx..0x%lx)\n",
           (long)fwsize, (unsigned long)rom_base, (unsigned long)(rom_base + fwsize));

    // ------------- SDL2 setup -------------
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow("OVMF Ramfb",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!ren) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Texture *tex = NULL;
    int tex_w = 0, tex_h = 0;

    // ------------- Main loop -------------
    int quit = 0;
    while (!quit) {
        // Drive vCPU until exits
        if (ioctl(vcpu, KVM_RUN, 0) < 0) die("KVM_RUN");

        switch (run->exit_reason) {
            case KVM_EXIT_HLT:
                printf("Guest halted.\n");
                quit = 1;
                break;

            case KVM_EXIT_IO: {
                uint16_t port = run->io.port;
                uint8_t *datap = (uint8_t*)run + run->io.data_offset;

                if (run->io.size == 1 && run->io.count == 1) {
                    if (run->io.direction == KVM_EXIT_IO_OUT && port == FWCFG_SEL_PORT) {
                        uint16_t key = *(uint16_t*)datap;
                        fwcfg_select(&fw, key);
                    } else if (run->io.direction == KVM_EXIT_IO_IN && port == FWCFG_DATA_PORT) {
                        *datap = fwcfg_read_byte(&fw);
                    } else if (run->io.direction == KVM_EXIT_IO_OUT && port == FWCFG_DATA_PORT) {
                        fwcfg_write_byte(&fw, *datap);
                        // If guest finished writing ramfb descriptor, sync it
                        if (fw.current_key == FW_CFG_KEY_RAMFB && fw.offset >= sizeof(struct fw_cfg_ramfb)) {
                            fwcfg_sync_ramfb(&fw);
                        }
                    } else {
                        // Log unknown I/O and ignore
                        printf("I/O: dir=%s port=0x%x size=%u\n",
                               run->io.direction ? "in" : "out", port, run->io.size);
                    }
                } else {
                    printf("I/O multi/size!=1: dir=%s port=0x%x size=%u count=%u\n",
                           run->io.direction ? "in" : "out", port, run->io.size, run->io.count);
                }
                break;
            }

            case KVM_EXIT_FAIL_ENTRY:
                fprintf(stderr, "KVM_EXIT_FAIL_ENTRY reason=0x%llx\n",
                        (unsigned long long)run->fail_entry.hardware_entry_failure_reason);
                quit = 1;
                break;

            case KVM_EXIT_INTERNAL_ERROR:
                fprintf(stderr, "KVM internal error suberror=0x%x\n", run->internal.suberror);
                quit = 1;
                break;

            case KVM_EXIT_SHUTDOWN:
                printf("Guest shutdown.\n");
                quit = 1;
                break;

            default:
                // Many exits can occur early (MMIO, debug). Log minimally.
                // You can extend with more handlers as you grow devices.
                // printf("Unhandled exit %d\n", run->exit_reason);
                break;
        }

        // Handle SDL events (close, resize)
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = 1;
        }

        // If ramfb is ready, render it
        if (fw.ramfb_ready) {
            uint8_t *fb_host = gpa_to_host(fw.ramfb.addr, ram, GUEST_RAM_SIZE);
            if (fb_host && fw.ramfb.width && fw.ramfb.height && fw.ramfb.stride) {
                // Recreate texture if size changed
                if (!tex || tex_w != (int)fw.ramfb.width || tex_h != (int)fw.ramfb.height) {
                    if (tex) SDL_DestroyTexture(tex);
                    tex_w = (int)fw.ramfb.width;
                    tex_h = (int)fw.ramfb.height;
                    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_XRGB8888,
                                            SDL_TEXTUREACCESS_STREAMING, tex_w, tex_h);
                    if (!tex) fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
                    SDL_SetWindowSize(win, tex_w, tex_h);
                }
                if (tex) {
                    // Update texture from guest framebuffer
                    SDL_UpdateTexture(tex, NULL, fb_host, fw.ramfb.stride);
                    SDL_RenderClear(ren);
                    SDL_RenderCopy(ren, tex, NULL, NULL);
                    SDL_RenderPresent(ren);
                }
            }
        } else {
            // No ramfb yet; simple heartbeat
            SDL_Delay(5);
        }
    }

    if (tex) SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
