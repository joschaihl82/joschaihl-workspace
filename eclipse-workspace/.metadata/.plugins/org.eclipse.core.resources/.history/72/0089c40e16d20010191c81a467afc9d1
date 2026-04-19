/* efi_app.c
   Minimal UEFI application in C without using the C runtime or EFI libraries.
   Entry point: efi_main(ImageHandle, SystemTable)
*/

#include <stdint.h>

/* basic uefi types we need */
typedef uint64_t EFI_STATUS;
typedef void* EFI_HANDLE;
typedef uint16_t CHAR16;
typedef uint64_t UINTN;
typedef uint64_t EFI_PHYSICAL_ADDRESS;

/* minimal subset of the simple text output protocol we call */
typedef struct {
    /* reset omitted */
    EFI_STATUS (*OutputString)(void* this, const CHAR16* string);
    /* other members omitted */
} SIMPLE_TEXT_OUTPUT_PROTOCOL;

/* minimal system table layout (only fields we use) */
typedef struct {
    void* hdr;
    CHAR16* firmwarevendor;
    uint32_t firmwarerevision;
    void* consoleinhandle;
    void* conin;
    void* consoleouthandle;
    SIMPLE_TEXT_OUTPUT_PROTOCOL* conout;
    void* standarderrorhandle;
    /* bootservices pointer and other fields omitted */
} EFI_SYSTEM_TABLE;

/* efi return codes */
#define EFI_SUCCESS 0

/*
 * utility function to convert a 64-bit unsigned integer to a utf-16 string (char16).
 * this is necessary because we cannot use standard library functions like sprintf.
 */
static void u64tochar16(uint64_t val, CHAR16* buffer, UINTN buffer_size) {
    if (buffer_size == 0) return;

    // maximum 20 digits for a 64-bit int + null terminator + crlf
    CHAR16 temp[24];
    int i = 0;

    // special case for 0
    if (val == 0) {
        temp[i++] = l'0';
    } else {
        while (val != 0) {
            temp[i++] = (CHAR16)(l'0' + (val % 10));
            val /= 10;
        }
    }

    // reverse the string and copy to buffer
    int j = 0;
    while (i > 0 && j < buffer_size - 3) { // reserve 3 spots for \r\n\0
        buffer[j++] = temp[--i];
    }

    // append carriage return, line feed, and null terminator
    buffer[j++] = l'\r';
    buffer[j++] = l'\n';
    buffer[j] = l'\0';
}

/* exported entry point for efi applications.
   signature: efi_status efiapi efi_main(efi_handle imagehandle, efi_system_table *systemtable);
   we use __attribute__((ms_abi)) to ensure correct calling convention for x64 efi.
*/
#if defined(__GNUC__) && defined(__x86_64__)
  #define EFIAPI __attribute__((ms_abi))
#else
  #define EFIAPI
#endif

EFI_STATUS EFIAPI efi_main(EFI_HANDLE imagehandle, EFI_SYSTEM_TABLE* systemtable) {
    (void)imagehandle;

    // output buffer: 20 digits + "\r\n" + null terminator = 23
    CHAR16 output_buffer[23];
    uint64_t counter = 0;

    // check for minimal required pointers
    if (!systemtable || !systemtable->conout || !systemtable->conout->outputstring) {
        // if conout is missing, we can't print anything
        return (EFI_STATUS)-1; // just return an error status
    }

    // the endless loop
    while (1) {
        // convert the current counter value to a utf-16 string
        u64tochar16(counter, output_buffer, sizeof(output_buffer) / sizeof(CHAR16));

        // print the counter value
        systemtable->conout->outputstring(systemtable->conout, output_buffer);

        // increment the counter
        counter++;

        // note: in a real uefi environment, this loop would spin incredibly fast.
        // in jitemu.c, the delay is determined by the emulator's execution speed.

        // check for overflow and reset (optional, but good practice)
        if (counter == 0) {
            counter = 1;
        }
    }

    // this line is unreachable but kept for completeness
    // return efi_success;
}

/* provide a weak alias for the symbol _start in case the toolchain expects it.
   this is important to prevent linker errors when using -nostdlib.
*/
__attribute__((weak, alias("efi_main")))
void _start(void);
