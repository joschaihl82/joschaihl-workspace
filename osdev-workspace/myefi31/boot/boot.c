// boot/boot.c
// Patched UEFI bootloader: loads kernel.elf, extracts .symtab if present,
// builds KernelBootInfo (with KernelGOPInfo pointer), and passes it to kernel.
// Fixes: Elf64_Shdr definition if missing, SHT_SYMTAB constant, and uses
// local EFI_GUID variables for Open/CloseProtocol calls (no &MACRO initializer).

#include <efi.h>
#include <efilib.h> /* optional helper macros if available */
#include "elf.h"

#define PT_LOAD 1

#define PF_X	0x1
#define PF_W	0x2
#define PF_R	0x4

/* Provide Elf64_Shdr if elf.h doesn't define it */
#ifndef Elf64_Shdr
typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} Elf64_Shdr;
#endif

#ifndef SHT_SYMTAB
#define SHT_SYMTAB 2
#endif

/* Simple memcpy/memset for loader (no runtime libs) */
void *memcpy(void *dest, const void *src, uint64_t n) {
	uint8_t *d = (uint8_t*)dest;
	const uint8_t *s = (const uint8_t*)src;
	for (uint64_t i = 0; i < n; i++) d[i] = s[i];
	return dest;
}

void *memset(void *dest, uint64_t value, uint64_t n) {
	uint8_t *d = (uint8_t*)dest;
	uint8_t v = (uint8_t)value;
	for (uint64_t i = 0; i < n; i++) d[i] = v;
	return dest;
}

/* Kernel GOP info (kept as in your kernel) */
typedef struct {
	UINT32 Width;
	UINT32 Height;
	UINT32 PixelsPerScanLine;
	UINT64 FrameBufferBase;
} KernelGOPInfo;

/* Full boot info structure passed to kernel */
typedef struct {
	/* basic kernel image bookkeeping */
	UINT64 text_start;
	UINT64 text_end;

	/* optional symbol table (if available) */
	UINT64 symtab_addr;   /* pointer (virtual) to copied symtab buffer */
	UINT64 symtab_count;  /* number of Elf64_Sym entries */

	/* pointer to GOP info (allocated in pool) */
	KernelGOPInfo *gop;

	/* memory map info (pointer to buffer returned by GetMemoryMap) */
	EFI_MEMORY_DESCRIPTOR *memmap;
	UINTN memmap_size;
	UINTN map_key;
	UINTN descriptor_size;
	UINT32 descriptor_version;
} KernelBootInfo;

/* Minimal kernel symbol type (optional) */
typedef struct {
	UINT64 addr;
	const char *name;
} KernelSymbol;

/* Helper to print a message to console (UTF-16) */
static void printw(EFI_SYSTEM_TABLE *ST, const CHAR16 *s) {
    if (ST && ST->ConOut) ST->ConOut->OutputString(ST->ConOut, (CHAR16*)s);
}

/* Entry point */
EFI_STATUS EFIAPI efiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
	EFI_BOOT_SERVICES *bs = ST->BootServices;
	EFI_STATUS status;

	/* Clear screen */
	if (ST->ConOut) ST->ConOut->ClearScreen(ST->ConOut);

	/* Open Loaded Image protocol for this image */
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
		printw(ST, L"OpenProtocol(LIP) failed\r\n");
		return status;
	}

	/* Open Simple File System protocol on the device that loaded this image */
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
		printw(ST, L"OpenProtocol(SFSP) failed\r\n");
		return status;
	}

	/* Open root volume and kernel file */
	EFI_FILE_PROTOCOL *root = NULL;
	EFI_FILE_PROTOCOL *kernelFile = NULL;
	status = sfsp->OpenVolume(sfsp, &root);
	if (EFI_ERROR(status)) { printw(ST, L"OpenVolume failed\r\n"); return status; }

	status = root->Open(root, &kernelFile, L"\\EFI\\BOOT\\kernel.elf", EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(status)) { printw(ST, L"Open kernel.elf failed\r\n"); return status; }

	/* Read ELF header */
	Elf64_Ehdr *ehdr = NULL;
	UINTN ehdr_size = sizeof(Elf64_Ehdr);
	status = bs->AllocatePool(EfiLoaderData, ehdr_size, (VOID **)&ehdr);
	if (EFI_ERROR(status)) return status;
	status = kernelFile->Read(kernelFile, &ehdr_size, (VOID *)ehdr);
	if (EFI_ERROR(status)) return status;

	/* Read program headers */
	Elf64_Phdr *phdr_table = NULL;
	UINTN phdr_table_size = (UINTN)ehdr->e_phnum * (UINTN)ehdr->e_phentsize;
	status = bs->AllocatePool(EfiLoaderData, phdr_table_size, (VOID **)&phdr_table);
	if (EFI_ERROR(status)) return status;
	status = kernelFile->SetPosition(kernelFile, ehdr->e_phoff);
	if (EFI_ERROR(status)) return status;
	status = kernelFile->Read(kernelFile, &phdr_table_size, (VOID *)phdr_table);
	if (EFI_ERROR(status)) return status;

	/* Track loaded image bounds */
	UINT64 loaded_min = (UINT64)-1;
	UINT64 loaded_max = 0;

	/* Load PT_LOAD segments */
	for (uint32_t i = 0; i < ehdr->e_phnum; i++) {
		if (phdr_table[i].p_type != PT_LOAD) continue;

		UINTN memsize = (UINTN)phdr_table[i].p_memsz;
		UINTN filesize = (UINTN)phdr_table[i].p_filesz;
		EFI_PHYSICAL_ADDRESS vaddr = (EFI_PHYSICAL_ADDRESS)phdr_table[i].p_vaddr;
		VOID *p_vaddr = (VOID *)(uintptr_t)vaddr;

		EFI_MEMORY_TYPE memType = (phdr_table[i].p_flags & PF_X) ? EfiLoaderCode : EfiLoaderData;

		status = bs->AllocatePages(AllocateAddress, memType, EFI_SIZE_TO_PAGES(memsize), &vaddr);
		if (EFI_ERROR(status)) { printw(ST, L"AllocatePages failed\r\n"); return status; }

		status = kernelFile->SetPosition(kernelFile, phdr_table[i].p_offset);
		if (EFI_ERROR(status)) return status;
		UINTN read_size = filesize;
		status = kernelFile->Read(kernelFile, &read_size, p_vaddr);
		if (EFI_ERROR(status)) return status;

		if (memsize > filesize) {
			UINT8 *bss_start = (UINT8*)p_vaddr + filesize;
			memset((VOID*)bss_start, 0, memsize - filesize);
		}

		if ((UINT64)phdr_table[i].p_vaddr < loaded_min) loaded_min = (UINT64)phdr_table[i].p_vaddr;
		if ((UINT64)(phdr_table[i].p_vaddr + phdr_table[i].p_memsz) > loaded_max)
			loaded_max = (UINT64)(phdr_table[i].p_vaddr + phdr_table[i].p_memsz);
	}

	/* --- Section headers and .symtab extraction (optional) --- */
	Elf64_Shdr *shdr_table = NULL;
	UINTN shdr_table_size = 0;
	if (ehdr->e_shoff && ehdr->e_shnum) {
		shdr_table_size = (UINTN)ehdr->e_shnum * (UINTN)ehdr->e_shentsize;
		status = bs->AllocatePool(EfiLoaderData, shdr_table_size, (VOID **)&shdr_table);
		if (!EFI_ERROR(status)) {
			status = kernelFile->SetPosition(kernelFile, ehdr->e_shoff);
			if (EFI_ERROR(status)) { bs->FreePool(shdr_table); shdr_table = NULL; }
			else {
				UINTN tmp = shdr_table_size;
				status = kernelFile->Read(kernelFile, &tmp, shdr_table);
				if (EFI_ERROR(status)) { bs->FreePool(shdr_table); shdr_table = NULL; }
			}
		} else {
			shdr_table = NULL;
		}
	}

	UINT64 symtab_addr = 0;
	UINT64 symtab_count = 0;

	if (shdr_table) {
		for (uint16_t si = 0; si < ehdr->e_shnum; ++si) {
			if (shdr_table[si].sh_type == SHT_SYMTAB) {
				UINT64 sym_size = shdr_table[si].sh_size;
				UINT64 sym_entsize = shdr_table[si].sh_entsize;
				if (sym_size && sym_entsize) {
					VOID *symbuf = NULL;
					status = bs->AllocatePool(EfiLoaderData, (UINTN)sym_size, &symbuf);
					if (!EFI_ERROR(status) && symbuf) {
						status = kernelFile->SetPosition(kernelFile, shdr_table[si].sh_offset);
						if (!EFI_ERROR(status)) {
							UINTN read = (UINTN)sym_size;
							status = kernelFile->Read(kernelFile, &read, symbuf);
							if (!EFI_ERROR(status)) {
								symtab_addr = (UINT64)(uintptr_t)symbuf;
								symtab_count = (UINT64)(sym_size / sym_entsize);
							} else {
								bs->FreePool(symbuf);
							}
						} else {
							bs->FreePool(symbuf);
						}
					}
				}
				break;
			}
		}
		/* free section header copy */
		bs->FreePool(shdr_table);
		shdr_table = NULL;
	}

	/* Close files and protocols using local GUID variables (do NOT take address of GUID macro) */
	kernelFile->Close(kernelFile);
	root->Close(root);

	/* Close protocols opened earlier using the local GUID variables */
	status = bs->CloseProtocol(lip->DeviceHandle, &sfsp_guid, ImageHandle, NULL);
	(void)status;
	status = bs->CloseProtocol(ImageHandle, &lip_guid, ImageHandle, NULL);
	(void)status;

	/* --- Graphics (GOP) --- */
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
	EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	status = bs->LocateProtocol(&gop_guid, NULL, (VOID **)&gop);
	/* non-fatal if GOP not present */

	KernelGOPInfo local_kgi;
	memset(&local_kgi, 0, sizeof(local_kgi));
	if (gop && gop->Mode && gop->Mode->Info) {
		local_kgi.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;
		local_kgi.FrameBufferBase = gop->Mode->FrameBufferBase;
		local_kgi.Width = gop->Mode->Info->HorizontalResolution;
		local_kgi.Height = gop->Mode->Info->VerticalResolution;
	}

	/* Kernel entry point expects KernelBootInfo* */
	VOID *entry = (VOID *)(uintptr_t)ehdr->e_entry;
	typedef VOID (*KERNEL_ENTRY)(KernelBootInfo *info);
	KERNEL_ENTRY kernel_entry = (KERNEL_ENTRY)entry;

	/* --- Memory map --- */
	UINTN MemMapSize = 0;
	UINTN MapKey = 0;
	UINTN DescriptorSize = 0;
	UINT32 DescriptorVersion = 0;
	EFI_MEMORY_DESCRIPTOR *MemMap = NULL;

	/* Query size */
	status = bs->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
	/* Allocate buffer with slack */
	MemMapSize += 5 * DescriptorSize;
	status = bs->AllocatePool(EfiLoaderData, MemMapSize, (VOID **)&MemMap);
	if (EFI_ERROR(status)) { printw(ST, L"AllocatePool(MemMap) failed\r\n"); return status; }

	/* Get memory map */
	status = bs->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
	if (EFI_ERROR(status)) { printw(ST, L"GetMemoryMap failed\r\n"); return status; }

	/* --- Prepare KernelBootInfo and KernelGOPInfo in pool --- */
	KernelBootInfo *kbi = NULL;
	KernelGOPInfo *kgi = NULL;
	status = bs->AllocatePool(EfiLoaderData, sizeof(KernelBootInfo), (VOID **)&kbi);
	if (EFI_ERROR(status) || !kbi) { printw(ST, L"AllocatePool(KernelBootInfo) failed\r\n"); return status; }
	status = bs->AllocatePool(EfiLoaderData, sizeof(KernelGOPInfo), (VOID **)&kgi);
	if (EFI_ERROR(status) || !kgi) { printw(ST, L"AllocatePool(KernelGOPInfo) failed\r\n"); return status; }

	/* Copy GOP info into pool */
	*kgi = local_kgi;

	/* Fill boot info */
	kbi->text_start = loaded_min;
	kbi->text_end = loaded_max;
	kbi->symtab_addr = symtab_addr;
	kbi->symtab_count = symtab_count;
	kbi->gop = kgi;

	kbi->memmap = MemMap;
	kbi->memmap_size = MemMapSize;
	kbi->map_key = MapKey;
	kbi->descriptor_size = DescriptorSize;
	kbi->descriptor_version = DescriptorVersion;

	/* --- Exit boot services --- */
	status = bs->ExitBootServices(ImageHandle, MapKey);
	if (EFI_ERROR(status)) {
		/* Try to refresh map and retry once */
		status = bs->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
		if (!EFI_ERROR(status)) status = bs->ExitBootServices(ImageHandle, MapKey);
		if (EFI_ERROR(status)) {
			printw(ST, L"ExitBootServices failed\r\n");
			for (;;) { __asm__ volatile ("hlt"); }
		}
	}

	/* Call kernel */
	kernel_entry(kbi);

	/* Should not return */
	return EFI_SUCCESS;
}

