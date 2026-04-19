/* (same header-free GOP demo as provided earlier) */
#include <stdint.h>
#include <stddef.h>
typedef uint64_t UINTN;
typedef uint64_t EFI_STATUS;
typedef void *EFI_HANDLE;
typedef uint16_t CHAR16;
typedef uint64_t EFI_PHYSICAL_ADDRESS;
typedef void VOID;
#define EFI_SUCCESS 0
typedef struct {
	uint64_t Signature;
	uint32_t Revision;
	uint32_t HeaderSize;
	uint32_t CRC32;
	uint32_t Reserved;
} EFI_TABLE_HEADER;
struct _EFI_BOOT_SERVICES;
typedef struct {
	EFI_TABLE_HEADER Hdr;
	CHAR16 *FirmwareVendor;
	uint32_t FirmwareRevision;
	EFI_HANDLE ConsoleInHandle;
	VOID *ConIn;
	EFI_HANDLE ConsoleOutHandle;
	VOID *ConOut;
	EFI_HANDLE StandardErrorHandle;
	VOID *StdErr;
	struct _EFI_BOOT_SERVICES *BootServices;
	UINTN NumberOfTableEntries;
	VOID *ConfigurationTable;
} EFI_SYSTEM_TABLE;
typedef EFI_STATUS (*EFI_LOCATE_PROTOCOL)(const void *Protocol,
		VOID *Registration, VOID **Interface);
typedef struct _EFI_BOOT_SERVICES {
	EFI_TABLE_HEADER Hdr;
	VOID *RaiseTPL;
	VOID *RestoreTPL;
	EFI_LOCATE_PROTOCOL LocateProtocol;
} EFI_BOOT_SERVICES;
typedef enum {
	PixelRedGreenBlueReserved8BitPerColor = 0,
	PixelBlueGreenRedReserved8BitPerColor = 1,
	PixelBitMask = 2,
	PixelBltOnly = 3
} EFI_GRAPHICS_PIXEL_FORMAT;
typedef struct {
	uint32_t RedMask;
	uint32_t GreenMask;
	uint32_t BlueMask;
	uint32_t ReservedMask;
} EFI_PIXEL_BITMASK;
typedef struct {
	uint32_t Version;
	uint32_t HorizontalResolution;
	uint32_t VerticalResolution;
	EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
	EFI_PIXEL_BITMASK PixelInformation;
	uint32_t PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;
typedef struct {
	uint32_t MaxMode;
	uint32_t Mode;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
	size_t SizeOfInfo;
	EFI_PHYSICAL_ADDRESS FrameBufferBase;
	size_t FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
	void *QueryMode;
	void *SetMode;
	EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
	void *Blt;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;
static const struct {
	uint32_t a;
	uint16_t b, c;
	uint8_t d[8];
} EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID = { 0x9042A9DE, 0x23DC, 0x4A38, { 0x96,
		0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A } };
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	if (!SystemTable || !SystemTable->BootServices)
		return (EFI_STATUS) -1;
	EFI_BOOT_SERVICES *BS = (EFI_BOOT_SERVICES*) SystemTable->BootServices;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = NULL;
	void *gop_guid = (void*) &EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	if (BS->LocateProtocol(gop_guid, NULL, (void**) &Gop) != EFI_SUCCESS
			|| !Gop)
		return (EFI_STATUS) -2;
	EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *mode = Gop->Mode;
	uint32_t w = mode->Info->HorizontalResolution, h =
			mode->Info->VerticalResolution, stride =
			mode->Info->PixelsPerScanLine;
	uint8_t *fb = (uint8_t*) (uintptr_t) mode->FrameBufferBase;
	uint32_t bg = 0xFF202040;
	for (uint32_t y = 0; y < h; y++) {
		uint32_t *row = (uint32_t*) (fb + (size_t) y * (size_t) stride * 4);
		for (uint32_t x = 0; x < w; x++)
			row[x] = bg;
	}
	uint32_t rx0 = w / 4, rx1 = 3 * w / 4, ry0 = h / 4, ry1 = 3 * h / 4, color =
			0xFFFFFFFF;
	for (uint32_t y = ry0; y < ry1; y++) {
		uint32_t *row = (uint32_t*) (fb + (size_t) y * (size_t) stride * 4);
		for (uint32_t x = rx0; x < rx1; x++)
			row[x] = color;
	}
	for (;;) { /* keep visible */
	}
	return EFI_SUCCESS;
}
