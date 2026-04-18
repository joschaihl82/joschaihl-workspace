#include <efi.h>
#include "elf.h"

#define PT_LOAD 1

#define PF_X    0x1
#define PF_W    0x2
#define PF_R    0x4

void *memcpy(void *dest, void *src, uint64_t n) {
    uint8_t *d = (uint8_t*)dest;
    uint8_t *s = (uint8_t*)src;

    for (uint64_t i = 0; i < n; i++)
        d[i] = s[i];

    return dest;
}

void *memset(void *dest, uint64_t value, uint64_t n) {
    uint8_t *d = (uint8_t*)dest;
    uint8_t v = (uint8_t)value;

    for (uint64_t i = 0; i < n; i++)
        d[i] = v;

    return dest;
}

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 PixelsPerScanLine;
    UINT64 FrameBufferBase;
} KernelGOPInfo;

EFI_STATUS EFIAPI efiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
    EFI_BOOT_SERVICES *bs = ST->BootServices;

    ST->ConOut->ClearScreen(ST->ConOut);

    EFI_STATUS status = EFI_SUCCESS;
    EFI_LOADED_IMAGE_PROTOCOL *lip = NULL;
    EFI_GUID lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    status = bs->OpenProtocol(
            ImageHandle,
            &lip_guid,
            (VOID **)&lip,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"OpenProtocol(LIP) failed\r\n");
        return status;
    }

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp = NULL;
    EFI_GUID sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    status = bs->OpenProtocol(
            lip->DeviceHandle,
            &sfsp_guid,
            (VOID **)&sfsp,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"OpenProtocol(SFS) failed\r\n");
        return status;
    }

    EFI_FILE_PROTOCOL *root = NULL;
    EFI_FILE_PROTOCOL *kernelFile = NULL;
    status = sfsp->OpenVolume(sfsp, &root);

    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"OpenVolume\r\n");
        return status;
    }

    status = root->Open(
            root,
            &kernelFile,
            L"\\EFI\\BOOT\\kernel.elf",
            EFI_FILE_MODE_READ,
            0);

    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"Open kernel.elf failed\r\n");
        return status;
    }

    /* Read ELF header */
    Elf64_Ehdr *ehdr = NULL;
    UINTN ehdr_size = sizeof(Elf64_Ehdr);
    status = bs->AllocatePool(EfiLoaderData, ehdr_size, (VOID **)&ehdr);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"AllocatePool(ehdr) failed\r\n");
        return status;
    }
    status = kernelFile->Read(kernelFile, &ehdr_size, (VOID *)ehdr);
    if (EFI_ERROR(status) || ehdr_size != sizeof(Elf64_Ehdr)) {
        ST->ConOut->OutputString(ST->ConOut, L"Read ehdr failed\r\n");
        return status;
    }

    /* Read program header table */
    Elf64_Phdr *phdr_table = NULL;
    UINTN phdr_table_size = ehdr->e_phnum * ehdr->e_phentsize;
    status = bs->AllocatePool(EfiLoaderData, phdr_table_size, (VOID **)&phdr_table);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"AllocatePool(phdr) failed\r\n");
        return status;
    }
    status = kernelFile->SetPosition(kernelFile, ehdr->e_phoff);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"SetPosition(phdr) failed\r\n");
        return status;
    }
    status = kernelFile->Read(kernelFile, &phdr_table_size, (VOID *)phdr_table);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"Read phdr failed\r\n");
        return status;
    }

    /* Load PT_LOAD segments at their p_vaddr and zero BSS */
    for (uint32_t i = 0; i < ehdr->e_phnum; i++) {
        if (phdr_table[i].p_type != PT_LOAD) continue;

        UINTN memsize = (UINTN)phdr_table[i].p_memsz;
        UINTN filesize = (UINTN)phdr_table[i].p_filesz;
        EFI_PHYSICAL_ADDRESS vaddr = (EFI_PHYSICAL_ADDRESS)phdr_table[i].p_vaddr;
        VOID *p_vaddr = (VOID *)(uintptr_t)vaddr;

        EFI_MEMORY_TYPE memType;
        memType = (phdr_table[i].p_flags & PF_X) ? EfiLoaderCode : EfiLoaderData;

        /* Allocate pages at the requested virtual address */
        UINTN pages = EFI_SIZE_TO_PAGES(memsize);
        status = bs->AllocatePages(
                AllocateAddress,
                memType,
                pages,
                &vaddr);
        if (EFI_ERROR(status)) {
            ST->ConOut->OutputString(ST->ConOut, L"AllocatePages failed\r\n");
            return status;
        }

        /* Read file data into p_vaddr */
        status = kernelFile->SetPosition(kernelFile, phdr_table[i].p_offset);
        if (EFI_ERROR(status)) {
            ST->ConOut->OutputString(ST->ConOut, L"SetPosition(segment) failed\r\n");
            return status;
        }
        UINTN read_size = filesize;
        status = kernelFile->Read(kernelFile, &read_size, p_vaddr);
        if (EFI_ERROR(status) || read_size != filesize) {
            ST->ConOut->OutputString(ST->ConOut, L"Read segment failed\r\n");
            return status;
        }

        /* Zero BSS if needed */
        if (memsize > filesize) {
            UINT8 *bss_start = (UINT8*)p_vaddr + filesize;
            memset((VOID*)bss_start, 0, memsize - filesize);
        }
    }

    kernelFile->Close(kernelFile);
    root->Close(root);

    /* Close protocols we opened */
    bs->CloseProtocol(
            lip->DeviceHandle,
            &sfsp_guid,
            ImageHandle,
            NULL);

    bs->CloseProtocol(
            ImageHandle,
            &lip_guid,
            ImageHandle,
            NULL);

    /* Locate GOP and prepare boot info */
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    status = bs->LocateProtocol(&gop_guid, NULL, (VOID **)&gop);
    if (EFI_ERROR(status) || gop == NULL) {
        ST->ConOut->OutputString(ST->ConOut, L"GOP not found\r\n");
        /* continue anyway; kernel may handle no framebuffer */
    }

    KernelGOPInfo kgi;
    kgi.PixelsPerScanLine = gop ? gop->Mode->Info->PixelsPerScanLine : 0;
    kgi.FrameBufferBase = gop ? gop->Mode->FrameBufferBase : 0;
    kgi.Width = gop ? gop->Mode->Info->HorizontalResolution : 0;
    kgi.Height = gop ? gop->Mode->Info->VerticalResolution : 0;

    /* Prepare kernel entry pointer */
    VOID *entry = (VOID *)(uintptr_t)ehdr->e_entry;
    typedef VOID (*KERNEL_ENTRY)(KernelGOPInfo *kgi);
    KERNEL_ENTRY kernel_entry = (KERNEL_ENTRY)entry;

    /* Allocate a kernel stack BEFORE ExitBootServices (so we can use it after) */
    EFI_PHYSICAL_ADDRESS stack_addr = 0;
    UINTN stack_pages = 8; /* 8 pages = 32 KiB stack */
    status = bs->AllocatePages(AllocateAnyPages, EfiLoaderData, stack_pages, &stack_addr);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"AllocatePages(stack) failed\r\n");
        return status;
    }
    UINT64 stack_top = (UINT64)stack_addr + (stack_pages * 4096);

    /* --- Proper GetMemoryMap sequence --- */
    UINTN MemMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemMap = NULL;
    UINTN MapKey = 0;
    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;

    /* First call to get required size */
    status = bs->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (status != EFI_BUFFER_TOO_SMALL) {
        /* Some firmwares may return EFI_SUCCESS with zero size; handle conservatively */
        /* If status is success and size is zero, still allocate a small buffer */
        if (status == EFI_SUCCESS && MemMapSize == 0) {
            MemMapSize = 4096;
        } else if (status != EFI_BUFFER_TOO_SMALL) {
            /* Unexpected error */
            ST->ConOut->OutputString(ST->ConOut, L"GetMemoryMap(initial) failed\r\n");
            return status;
        }
    }

    /* Allocate pool for memory map (add some slack) */
    MemMapSize += 2 * DescriptorSize;
    status = bs->AllocatePool(EfiLoaderData, MemMapSize, (VOID **)&MemMap);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"AllocatePool(MemMap) failed\r\n");
        return status;
    }

    /* Retrieve the memory map */
    status = bs->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"GetMemoryMap(second) failed\r\n");
        return status;
    }

    /* Exit boot services */
    status = bs->ExitBootServices(ImageHandle, MapKey);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"ExitBootServices failed\r\n");
        return status;
    }

    /* Trampoline: switch to kernel stack, set up System V arg (RDI), clear volatile regs, call kernel entry.
     * We use inline assembly to:
     *   - move kernel_entry into RAX
     *   - move &kgi into RDI (first System V arg)
     *   - set RSP to stack_top (aligned)
     *   - clear RBP and volatile regs
     *   - call *RAX
     *
     * Note: after ExitBootServices we cannot call BootServices anymore.
     */
    {
        void *entry_ptr = (void*)kernel_entry;
        KernelGOPInfo *kgi_ptr = &kgi;
        void *stack_top_ptr = (void*)(uintptr_t)stack_top;

        asm volatile (
            "mov %0, %%rax\n"        /* kernel entry */
            "mov %1, %%rdi\n"        /* arg: pointer to kgi */
            "mov %2, %%rsp\n"        /* set stack */
            "and $-16, %%rsp\n"      /* align stack to 16 bytes */
            "xor %%rbp, %%rbp\n"
            "xor %%rsi, %%rsi\n"
            "xor %%rdx, %%rdx\n"
            "xor %%rcx, %%rcx\n"
            "xor %%r8,  %%r8\n"
            "xor %%r9,  %%r9\n"
            "call *%%rax\n"
            :
            : "r"(entry_ptr), "r"(kgi_ptr), "r"(stack_top_ptr)
            : "rax", "rdi", "rsi", "rdx", "rcx", "r8", "r9", "memory"
        );
    }

    /* Should not return; if it does, halt */
    for (;;) __asm__ volatile ("hlt");

    return EFI_SUCCESS;
}

