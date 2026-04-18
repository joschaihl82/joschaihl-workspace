#include <efi.h>
#include "elf.h"

#define PT_LOAD 1

#define PF_X    0x1
#define PF_W    0x2
#define PF_R    0x4

/* Virtual base the kernel should map the framebuffer to. */
#define FB_VIRTUAL_BASE 0xFFFF800000000000ULL

/* Page pool size for page table pages (adjust as needed) */
#define PAGE_POOL_PAGES 128
#define PAGE_SIZE_4K 0x1000ULL
#define PAGE_SIZE_2M 0x200000ULL

/* PTE flags */
#define PTE_PRESENT   (1ULL << 0)
#define PTE_RW        (1ULL << 1)
#define PTE_USER      (1ULL << 2)
#define PTE_PWT       (1ULL << 3)
#define PTE_PCD       (1ULL << 4)
#define PTE_PS        (1ULL << 7)
#define PTE_NX        (1ULL << 63)

void *memcpy(void *dest, void *src, uint64_t n) {
    uint8_t *d = (uint8_t*)dest;
    uint8_t *s = (uint8_t*)src;
    for (uint32_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

void *memset(void *dest, uint64_t value, uint64_t n) {
    uint8_t *d = (uint8_t*)dest;
    uint8_t v = (uint8_t)value;
    for (uint32_t i = 0; i < n; i++) d[i] = v;
    return dest;
}

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 PixelsPerScanLine;
    UINT64 FrameBufferBase;      /* physical framebuffer base (from GOP) */
    UINT64 FrameBufferVirtual;   /* virtual address kernel should use */
} KernelGOPInfo;

/* ------------------ Page pool allocator for page table pages ------------------ */
static uint8_t *page_pool = NULL;      /* virtual pointer to pool (identity assumed) */
static size_t page_pool_pages = 0;
static size_t page_pool_next = 0;

static void init_page_pool(void *buf, size_t pages) {
    page_pool = (uint8_t*)buf;
    page_pool_pages = pages;
    page_pool_next = 0;
}

static void *alloc_pt_page(void) {
    if (!page_pool || page_pool_next >= page_pool_pages) return NULL;
    void *p = page_pool + (page_pool_next * PAGE_SIZE_4K);
    page_pool_next++;
    for (size_t i = 0; i < PAGE_SIZE_4K; ++i) ((uint8_t*)p)[i] = 0;
    return p;
}

/* ------------------ Helpers to build page tables (map 2MiB + 4KiB) ------------------ */
static inline uint16_t pml4_index(uint64_t va) { return (va >> 39) & 0x1FF; }
static inline uint16_t pdpt_index(uint64_t va) { return (va >> 30) & 0x1FF; }
static inline uint16_t pd_index(uint64_t va)   { return (va >> 21) & 0x1FF; }
static inline uint16_t pt_index(uint64_t va)   { return (va >> 12) & 0x1FF; }

/* Ensure PDPT exists for a PML4 entry. root_pml4 is a pointer to the PML4 page (identity). */
static uint64_t *ensure_pdpt(uint64_t *root_pml4, uint16_t i4) {
    uint64_t e = root_pml4[i4];
    if (e & PTE_PRESENT) return (uint64_t*)(e & ~0xFFFULL);
    uint64_t *pdpt = (uint64_t*)alloc_pt_page();
    if (!pdpt) return NULL;
    root_pml4[i4] = ((uint64_t)pdpt) | PTE_PRESENT | PTE_RW;
    return pdpt;
}

/* Ensure PD exists for a PDPT entry. */
static uint64_t *ensure_pd(uint64_t *pdpt, uint16_t i3) {
    uint64_t e = pdpt[i3];
    if (e & PTE_PRESENT) return (uint64_t*)(e & ~0xFFFULL);
    uint64_t *pd = (uint64_t*)alloc_pt_page();
    if (!pd) return NULL;
    pdpt[i3] = ((uint64_t)pd) | PTE_PRESENT | PTE_RW;
    return pd;
}

/* Ensure PT exists for a PD entry (used for 4K fallback). */
static uint64_t *ensure_pt(uint64_t *pd, uint16_t i2) {
    uint64_t e = pd[i2];
    if (e & PTE_PRESENT) {
        if (e & PTE_PS) return NULL; /* conflict */
        return (uint64_t*)(e & ~0xFFFULL);
    }
    uint64_t *pt = (uint64_t*)alloc_pt_page();
    if (!pt) return NULL;
    pd[i2] = ((uint64_t)pt) | PTE_PRESENT | PTE_RW;
    return pt;
}

/* Map a single 4 KiB page into the provided root PML4 */
static int map_4k_into(uint64_t *root_pml4, uint64_t virt, uint64_t phys, uint64_t flags) {
    uint16_t i4 = pml4_index(virt);
    uint16_t i3 = pdpt_index(virt);
    uint16_t i2 = pd_index(virt);
    uint16_t i1 = pt_index(virt);

    uint64_t *pdpt = ensure_pdpt(root_pml4, i4);
    if (!pdpt) return -1;
    uint64_t *pd = ensure_pd(pdpt, i3);
    if (!pd) return -2;
    uint64_t *pt = ensure_pt(pd, i2);
    if (!pt) return -3;

    if (pt[i1] & PTE_PRESENT) {
        uint64_t existing = pt[i1] & ~0xFFFULL;
        if (existing == (phys & ~0xFFFULL)) return 0;
        return -4;
    }

    pt[i1] = (phys & ~0xFFFULL) | (flags & 0xFFFULL) | PTE_PRESENT | PTE_NX;
    return 0;
}

/* Map a single 2 MiB page by writing a PD entry with PS bit */
static int map_2m_into(uint64_t *root_pml4, uint64_t virt, uint64_t phys, uint64_t flags) {
    if ((virt & (PAGE_SIZE_2M - 1)) || (phys & (PAGE_SIZE_2M - 1))) return -1;

    uint16_t i4 = pml4_index(virt);
    uint16_t i3 = pdpt_index(virt);
    uint16_t i2 = pd_index(virt);

    uint64_t *pdpt = ensure_pdpt(root_pml4, i4);
    if (!pdpt) return -2;
    uint64_t *pd = ensure_pd(pdpt, i3);
    if (!pd) return -3;

    uint64_t existing = pd[i2];
    if (existing & PTE_PRESENT) {
        if ((existing & PTE_PS) && ((existing & ~0x1FFFFFULL) == (phys & ~0x1FFFFFULL))) return 0;
        return -4;
    }

    pd[i2] = (phys & ~0x1FFFFFULL) | (flags & 0xFFFULL) | PTE_PRESENT | PTE_PS | PTE_NX;
    return 0;
}

/* Map a contiguous range [phys_base, phys_base+size) to [virt_base, virt_base+size) using 2MiB where possible */
static int map_range_into(uint64_t *root_pml4, uint64_t virt_base, uint64_t phys_base, uint64_t size, uint64_t flags) {
    if (size == 0) return 0;
    uint64_t phys = phys_base;
    uint64_t virt = virt_base;
    uint64_t end = phys_base + size;

    /* head 4K until aligned */
    while (phys < end && ((phys & (PAGE_SIZE_2M - 1)) || (virt & (PAGE_SIZE_2M - 1)))) {
        int r = map_4k_into(root_pml4, virt, phys, flags);
        if (r != 0) return r;
        phys += PAGE_SIZE_4K;
        virt += PAGE_SIZE_4K;
    }

    /* middle 2M */
    while (phys + PAGE_SIZE_2M <= end) {
        int r = map_2m_into(root_pml4, virt, phys, flags);
        if (r != 0) return r;
        phys += PAGE_SIZE_2M;
        virt += PAGE_SIZE_2M;
    }

    /* tail 4K */
    while (phys < end) {
        int r = map_4k_into(root_pml4, virt, phys, flags);
        if (r != 0) return r;
        phys += PAGE_SIZE_4K;
        virt += PAGE_SIZE_4K;
    }

    return 0;
}

/* ------------------ Memory map helper: check if a physical address lies in a "safe" region */
static BOOLEAN phys_in_safe_region(EFI_BOOT_SERVICES *bs, uint64_t phys) {
    UINTN mapSize = 0;
    UINTN mapKey = 0;
    UINTN descSize = 0;
    UINT32 descVersion = 0;
    EFI_MEMORY_DESCRIPTOR *map = NULL;

    bs->GetMemoryMap(&mapSize, map, &mapKey, &descSize, &descVersion);
    if (mapSize == 0 || descSize == 0) return FALSE;

    mapSize += 2 * descSize;
    if (bs->AllocatePool(EfiLoaderData, mapSize, (VOID **)&map) != EFI_SUCCESS) return FALSE;
    if (bs->GetMemoryMap(&mapSize, map, &mapKey, &descSize, &descVersion) != EFI_SUCCESS) {
        bs->FreePool(map);
        return FALSE;
    }

    BOOLEAN found = FALSE;
    UINT8 *iter = (UINT8*)map;
    UINTN descCount = mapSize / descSize;
    for (UINTN i = 0; i < descCount; ++i) {
        EFI_MEMORY_DESCRIPTOR *d = (EFI_MEMORY_DESCRIPTOR*)iter;
        UINT64 start = d->PhysicalStart;
        UINT64 end = start + (d->NumberOfPages * 4096ULL);
        if (phys >= start && phys < end) {
            if (d->Type == EfiLoaderData || d->Type == EfiBootServicesData || d->Type == EfiConventionalMemory) {
                found = TRUE;
            }
            break;
        }
        iter += descSize;
    }

    bs->FreePool(map);
    return found;
}

/* ------------------ Create a new PML4 in the page pool, map kernel segments + framebuffer, then switch CR3.
   Returns EFI_SUCCESS on success (CR3 switched), otherwise an EFI error and no CR3 switch performed. */
static EFI_STATUS create_new_pml4_and_switch_cr3(EFI_BOOT_SERVICES *bs,
                                                 Elf64_Ehdr *ehdr,
                                                 Elf64_Phdr *phdr_table,
                                                 UINTN phnum,
                                                 uint64_t fb_phys,
                                                 uint64_t fb_size_rounded,
                                                 uint64_t fb_virt) {
    /* Allocate root PML4 page from pool */
    uint64_t *new_pml4 = (uint64_t*)alloc_pt_page();
    if (!new_pml4) return EFI_OUT_OF_RESOURCES;

    /* Map kernel load segments: we assume kernel was loaded at p_vaddr (identity before paging).
       For each PT_LOAD segment, map virtual p_vaddr -> physical p_vaddr for the segment size. */
    uint64_t flags = PTE_RW; /* kernel RW; NX set by mapper */
    for (UINTN i = 0; i < phnum; ++i) {
        if (phdr_table[i].p_type != PT_LOAD) continue;
        uint64_t seg_v = phdr_table[i].p_vaddr;
        uint64_t seg_p = phdr_table[i].p_vaddr; /* loader allocated at vaddr physical */
        uint64_t seg_sz = phdr_table[i].p_memsz;
        /* Round seg_sz up to pages */
        uint64_t pages = (seg_sz + PAGE_SIZE_4K - 1) / PAGE_SIZE_4K;
        uint64_t seg_size_rounded = pages * PAGE_SIZE_4K;
        if (map_range_into(new_pml4, seg_v, seg_p, seg_size_rounded, flags) != 0) {
            return EFI_OUT_OF_RESOURCES;
        }
    }

    /* Map framebuffer virtual region */
    uint64_t fb_flags = PTE_RW | PTE_PWT; /* request WC via PWT; kernel should configure PAT/MTRR */
    if (map_range_into(new_pml4, fb_virt, fb_phys, fb_size_rounded, fb_flags) != 0) {
        return EFI_OUT_OF_RESOURCES;
    }

    /* Optionally map a small identity region for low memory (e.g., first 2 MiB) so firmware services still work.
       Map first 2 MiB identity (0..2MiB) as RW, NX. Adjust as needed. */
    uint64_t low_identity_size = PAGE_SIZE_2M;
    if (map_range_into(new_pml4, 0x0ULL, 0x0ULL, low_identity_size, PTE_RW) != 0) {
        /* Not fatal; continue */
    }

    /* All page table pages were allocated from page_pool and filled. Now switch CR3 to new PML4.
       We must provide the physical address of new_pml4. We assume page_pool is identity-mapped
       (pointer == physical). The caller must ensure pool_addr is safe before calling this function. */
    uint64_t new_pml4_phys = (uint64_t)(uintptr_t)new_pml4 & ~0xFFFULL;

    /* Mask new_cr3 low bits (should be zero) and load it */
    uint64_t new_cr3 = new_pml4_phys & ~0xFFFULL;
    asm volatile("mov %0, %%cr3" :: "r"(new_cr3) : "memory");

    return EFI_SUCCESS;
}

/* ------------------ Main efiMain ------------------ */

EFI_STATUS EFIAPI efiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
    EFI_BOOT_SERVICES *bs = ST->BootServices;
    ST->ConOut->ClearScreen(ST->ConOut);

    EFI_STATUS status = EFI_SUCCESS;
    EFI_LOADED_IMAGE_PROTOCOL *lip = NULL;
    EFI_GUID lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    bs->OpenProtocol(ImageHandle, &lip_guid, (VOID **)&lip, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp = NULL;
    EFI_GUID sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    bs->OpenProtocol(lip->DeviceHandle, &sfsp_guid, (VOID **)&sfsp, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    EFI_FILE_PROTOCOL *root = NULL;
    EFI_FILE_PROTOCOL *kernelFile = NULL;
    status = sfsp->OpenVolume(sfsp, &root);
    if (EFI_ERROR(status)) ST->ConOut->OutputString(ST->ConOut, L"OpenVolume\r\n");

    status = root->Open(root, &kernelFile, L"\\EFI\\BOOT\\kernel.elf", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) ST->ConOut->OutputString(ST->ConOut, L"Open\r\n");

    Elf64_Ehdr *ehdr = NULL;
    UINTN ehdr_size = sizeof(Elf64_Ehdr);
    bs->AllocatePool(EfiLoaderData, ehdr_size, (VOID **)&ehdr);
    kernelFile->Read(kernelFile, &ehdr_size, (VOID *)ehdr);

    Elf64_Phdr *phdr_table = NULL;
    UINTN phdr_table_size = ehdr->e_phnum * ehdr->e_phentsize;
    bs->AllocatePool(EfiLoaderData, phdr_table_size, (VOID **)&phdr_table);
    kernelFile->SetPosition(kernelFile, ehdr->e_phoff);
    kernelFile->Read(kernelFile, &phdr_table_size, (VOID *)phdr_table);

    for (uint32_t i = 0; i < ehdr->e_phnum; i++) {
        UINTN memsize = phdr_table[i].p_memsz;
        UINTN filesize = phdr_table[i].p_filesz;
        EFI_PHYSICAL_ADDRESS vaddr = phdr_table[i].p_vaddr;
        VOID *p_vaddr = (VOID *)(uintptr_t)vaddr;

        EFI_MEMORY_TYPE memType;
        memType = (phdr_table[i].p_flags & PF_X) ? EfiLoaderCode : EfiLoaderData;

        bs->AllocatePages(AllocateAddress, memType, EFI_SIZE_TO_PAGES(memsize), &vaddr);
        kernelFile->SetPosition(kernelFile, phdr_table[i].p_offset);
        kernelFile->Read(kernelFile, &filesize, p_vaddr);

        if (memsize > filesize) {
            UINT8 *bss_start = (UINT8*)p_vaddr + filesize;
            memset((VOID*)bss_start, 0, memsize - filesize);
        }
    }

    kernelFile->Close(kernelFile);
    root->Close(root);

    bs->CloseProtocol(lip->DeviceHandle, &sfsp_guid, ImageHandle, NULL);
    bs->CloseProtocol(ImageHandle, &lip_guid, ImageHandle, NULL);

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    bs->LocateProtocol(&gop_guid, NULL, (VOID **)&gop);

    KernelGOPInfo kgi;
    kgi.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;
    kgi.FrameBufferBase = gop->Mode->FrameBufferBase;
    kgi.Width = gop->Mode->Info->HorizontalResolution;
    kgi.Height = gop->Mode->Info->VerticalResolution;

    uint64_t fb_phys = kgi.FrameBufferBase;
    uint64_t fb_size = (uint64_t)kgi.Height * (uint64_t)kgi.PixelsPerScanLine * 4ULL;
    uint64_t fb_pages = (fb_size + PAGE_SIZE_4K - 1) / PAGE_SIZE_4K;
    uint64_t fb_size_rounded = fb_pages * PAGE_SIZE_4K;
    kgi.FrameBufferVirtual = FB_VIRTUAL_BASE;

    /* Try to reserve framebuffer physical pages (optional) */
    {
        EFI_PHYSICAL_ADDRESS reserve_addr = fb_phys;
        (void)bs->AllocatePages(AllocateAddress, EfiLoaderData, (UINTN)fb_pages, &reserve_addr);
    }

    /* Allocate page pool for page table pages */
    EFI_PHYSICAL_ADDRESS pool_addr = 0;
    EFI_STATUS pool_status = bs->AllocatePages(AllocateAnyPages, EfiLoaderData, (UINTN)PAGE_POOL_PAGES, &pool_addr);
    if (EFI_ERROR(pool_status)) {
        ST->ConOut->OutputString(ST->ConOut, L"Failed to allocate page pool\r\n");
        /* fallback: do not switch CR3; kernel will map framebuffer */
    } else {
        /* Check that pool_addr and current PML4 physical page are in safe regions */
        uint64_t cr3 = 0;
        asm volatile("mov %%cr3, %0" : "=r"(cr3));
        uint64_t pml4_phys = cr3 & ~0xFFFULL;

        BOOLEAN pool_safe = phys_in_safe_region(bs, (uint64_t)pool_addr);
        BOOLEAN pml4_safe = phys_in_safe_region(bs, pml4_phys);

        if (!pool_safe) {
            ST->ConOut->OutputString(ST->ConOut, L"Page pool not in safe region; skipping CR3 switch.\r\n");
        } else {
            /* Initialize pool and create new PML4, then switch CR3 */
            init_page_pool((void*)(uintptr_t)pool_addr, PAGE_POOL_PAGES);

            EFI_STATUS r = create_new_pml4_and_switch_cr3(bs, ehdr, phdr_table, ehdr->e_phnum, fb_phys, fb_size_rounded, kgi.FrameBufferVirtual);
            if (EFI_ERROR(r)) {
                ST->ConOut->OutputString(ST->ConOut, L"Failed to create/switch PML4; kernel will map later.\r\n");
            } else {
                ST->ConOut->OutputString(ST->ConOut, L"Switched to new PML4 successfully.\r\n");
            }
        }
    }

    /* Get memory map and ExitBootServices (as before) */
    UINTN MemMapSize = 0;
    UINTN MapKey = 0;
    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;
    EFI_MEMORY_DESCRIPTOR *MemMap = NULL;

    bs->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    MemMapSize += 5 * DescriptorSize;
    bs->AllocatePool(EfiLoaderData, MemMapSize, (VOID **)&MemMap);
    bs->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    while(1);
    bs->ExitBootServices(ImageHandle, MapKey);

    /* Jump to kernel entry */
    VOID *entry = (VOID *)(uintptr_t)ehdr->e_entry;
    typedef VOID (*KERNEL_ENTRY)(KernelGOPInfo *kgi);
    KERNEL_ENTRY kernel_entry = (KERNEL_ENTRY)entry;

    kernel_entry(&kgi);

    return EFI_SUCCESS;
}

