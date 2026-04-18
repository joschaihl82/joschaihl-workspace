// x11raw.c
// Minimal raw X11 client that opens a local X0 unix socket and renders white-noise
// into a created window using raw X11 protocol requests.
//
// Compile: gcc x11raw.c -o x11raw
// Run: ./x11raw
//
// Notes:
// - This program speaks the raw X11 wire protocol to the local X server socket
//   at /tmp/.X11-unix/X0. It is intentionally minimal and does not implement
//   full error handling or all protocol features. Use at your own risk.

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <time.h>

int32_t GlobalId = 0;
int32_t GlobalIdBase = 0;
int32_t GlobalIdMask = 0;
int32_t GlobalRootWindow = 0;
int32_t GlobalRootVisualId = 0;

int32_t GlobalTextOffsetX = 10;
int32_t GlobalTextOffsetY = 20;

#define READ_BUFFER_SIZE (16*1024)

#define RESPONSE_STATE_FAILED 0
#define RESPONSE_STATE_SUCCESS 1
#define RESPONSE_STATE_AUTHENTICATE 2

#define X11_REQUEST_CREATE_WINDOW 1
#define X11_REQUEST_MAP_WINDOW 8
#define X11_REQUEST_IMAGE_TEXT_8 76
#define X11_REQUEST_OPEN_FONT 45
#define X11_REQUEST_CREATE_GC 55
#define X11_REQUEST_PUT_IMAGE 72

#define X11_EVENT_FLAG_KEY_PRESS 0x00000001
#define X11_EVENT_FLAG_KEY_RELEASE 0x00000002
#define X11_EVENT_FLAG_EXPOSURE 0x8000

#define WINDOWCLASS_COPYFROMPARENT 0
#define WINDOWCLASS_INPUTOUTPUT 1
#define WINDOWCLASS_INPUTONLY 2

#define X11_FLAG_BACKGROUND_PIXEL 0x00000002
#define X11_FLAG_WIN_EVENT 0x00000800

#define X11_FLAG_FG 0x00000004
#define X11_FLAG_BG 0x00000008
#define X11_FLAG_FONT 0x00004000
#define X11_FLAG_GC_EXPOSURE 0x00010000

#define PAD(N) ((4 - ((N) % 4)) % 4)

void VerifyOrDie(int IsSuccess, const char *Message) {
    if(!IsSuccess)  {
        fprintf(stderr, "%s\n", Message);
        exit(13);
    }
}

void VerifyOrDieWidthErrno(int IsSuccess, const char *Message) {
    if(!IsSuccess)  {
        perror(Message);
        exit(13);
    }
}

int32_t GetNextId() {
    int32_t Result = (GlobalIdMask & GlobalId) | GlobalIdBase;
    GlobalId += 1;
    return Result;
}

void PrintResponseError(char *Data, int32_t Size) {
    // Minimal error printing
    if(Size < 12) {
        fprintf(stderr, "Short error reply\n");
        return;
    }
    uint8_t ErrorCode = Data[1];
    uint16_t Minor = *((uint16_t*)&Data[8]);
    uint8_t Major = Data[10];

    const char *ErrorNames[] = {
        "Unknown Error",
        "Request",
        "Value",
        "Window",
        "Pixmap",
        "Atom",
        "Cursor",
        "Font",
        "Match",
        "Drawable",
        "Access",
        "Alloc",
        "Colormap",
        "GContext",
        "IDChoice",
        "Name",
        "Length",
        "Implementation",
    };

    const char* ErrorName = "Unknown error";
    if(ErrorCode < (sizeof(ErrorNames)/sizeof(ErrorNames[0]))) {
        ErrorName = ErrorNames[ErrorCode];
    }

    fprintf(stderr, "Response Error: [%u] %s  Minor: %u, Major: %u\n", ErrorCode, ErrorName, Minor, Major);
}

void PrintAndProcessEvent(char *Data, int32_t Size) {
    if(Size <= 0) return;
    uint8_t EventCode = Data[0];

    if(EventCode == 12) { // Expose
        uint16_t SequenceNumber = *((uint16_t*)&Data[2]);
        uint32_t Window = *((uint32_t*)&Data[4]);
        uint16_t X = *((uint16_t*)&Data[8]);
        uint16_t Y = *((uint16_t*)&Data[10]);
        uint16_t Width = *((uint16_t*)&Data[12]);
        uint16_t Height = *((uint16_t*)&Data[14]);
        uint16_t Count = *((uint16_t*)&Data[16]);
        printf("Expose: Seq %u, Win %u, X %u, Y %u, W %u, H %u, Count %u\n",
               SequenceNumber, Window, X, Y, Width, Height, Count);
    } else if(EventCode == 2) { // KeyPress
        uint8_t KeyCode = Data[1];
        uint16_t SequenceNumber = *((uint16_t*)&Data[2]);
        uint32_t TimeStamp = *((uint32_t*)&Data[4]);
        uint32_t RootWindow = *((uint32_t*)&Data[8]);
        uint32_t EventWindow = *((uint32_t*)&Data[12]);
        int16_t RootX = *((int16_t*)&Data[20]);
        int16_t RootY = *((int16_t*)&Data[22]);
        int16_t EventX = *((int16_t*)&Data[24]);
        int16_t EventY = *((int16_t*)&Data[26]);

        // Simple arrow-key handling (keycodes are X keycodes; may vary)
        int StepSize = 10;
        if(KeyCode == 25) { GlobalTextOffsetY += StepSize; } // 'Down' mapping in original hack
        if(KeyCode == 39) { GlobalTextOffsetY -= StepSize; } // 'Up'
        if(KeyCode == 38) { GlobalTextOffsetX -= StepSize; } // 'Left'
        if(KeyCode == 40) { GlobalTextOffsetX += StepSize; } // 'Right'

        printf("KeyPress: Code %u, Seq %u, Time %u, Root %u, Event %u, RX %d, RY %d, EX %d, EY %d\n",
               KeyCode, SequenceNumber, TimeStamp, RootWindow, EventWindow, RootX, RootY, EventX, EventY);
    } else {
        // Unknown or unhandled event
        // printf("Event code: %u\n", EventCode);
    }
}

void GetAndProcessReply(int Socket) {
    char Buffer[1024] = {};
    int32_t BytesRead = read(Socket, Buffer, sizeof(Buffer));
    if(BytesRead <= 0) return;
    uint8_t Code = Buffer[0];
    if(Code == 0) {
        PrintResponseError(Buffer, BytesRead);
    } else if (Code == 1) {
        // Reply - not expected in this minimal client
    } else {
        PrintAndProcessEvent(Buffer, BytesRead);
    }
}

int X_InitiateConnection(int Socket) {
    int SetupStatus = 1;
    char ReadBuffer[READ_BUFFER_SIZE] = {};
    uint8_t InitializationRequest[12] = {};
    InitializationRequest[0] = 'l'; // little-endian
    InitializationRequest[1] = 0;   // unused
    InitializationRequest[2] = 11;  // protocol major? (as in original)
    // send 12 bytes initialization request
    int BytesWritten = write(Socket, (char*)&InitializationRequest, sizeof(InitializationRequest));
    VerifyOrDie(BytesWritten == sizeof(InitializationRequest), "Wrong amount of bytes written during initialization");

    int BytesRead = read(Socket, ReadBuffer, 8);
    if(BytesRead < 8) {
        fprintf(stderr, "Short init response\n");
        return 1;
    }

    if(ReadBuffer[0] == RESPONSE_STATE_FAILED) {
        // read rest and dump
        int More = read(Socket, ReadBuffer + 8, READ_BUFFER_SIZE - 8);
        (void)More;
        fprintf(stderr, "Init failed\n");
    }
    else if(ReadBuffer[0] == RESPONSE_STATE_AUTHENTICATE) {
        fprintf(stderr, "Authentication required. This client does not support it.\n");
    }
    else if(ReadBuffer[0] == RESPONSE_STATE_SUCCESS) {
        // read remaining setup bytes
        BytesRead = read(Socket, ReadBuffer + 8, READ_BUFFER_SIZE - 8);
        (void)BytesRead;

        uint32_t ResourceIdBase = *((uint32_t*)&ReadBuffer[12]);
        uint32_t ResourceIdMask = *((uint32_t*)&ReadBuffer[16]);
        uint16_t LengthOfVendor = *((uint16_t*)&ReadBuffer[24]);
        uint8_t NumberOfFormats = ReadBuffer[29];

        int32_t VendorPad = PAD(LengthOfVendor);
        int32_t FormatByteLength = 8 * NumberOfFormats;
        int32_t ScreensStartOffset = 40 + LengthOfVendor + VendorPad + FormatByteLength;

        uint32_t RootWindow = *((uint32_t*)&ReadBuffer[ScreensStartOffset]);
        uint32_t RootVisualId = *((uint32_t*)&ReadBuffer[ScreensStartOffset + 32]);

        GlobalIdBase = ResourceIdBase;
        GlobalIdMask = ResourceIdMask;
        GlobalRootWindow = RootWindow;
        GlobalRootVisualId = RootVisualId;

        SetupStatus = 0;
    }

    return SetupStatus;
}

int X_CreateWindow(int Socket, int X, int Y, int Width, int Height) {
    char SendBuffer[16*1024] = {};
    int32_t WindowId = GetNextId();
    int32_t Depth = 0;
    uint32_t BorderWidth = 1;
    int32_t CreateWindowFlagCount = 2;
    int RequestLength = 8 + CreateWindowFlagCount;

    SendBuffer[0] = X11_REQUEST_CREATE_WINDOW;
    SendBuffer[1] = Depth;
    *((int16_t *)&SendBuffer[2]) = RequestLength;
    *((int32_t *)&SendBuffer[4]) = WindowId;
    *((int32_t *)&SendBuffer[8]) = GlobalRootWindow;
    *((int16_t *)&SendBuffer[12]) = X;
    *((int16_t *)&SendBuffer[14]) = Y;
    *((int16_t *)&SendBuffer[16]) = Width;
    *((int16_t *)&SendBuffer[18]) = Height;
    *((int16_t *)&SendBuffer[20]) = BorderWidth;
    *((int16_t *)&SendBuffer[22]) = WINDOWCLASS_INPUTOUTPUT;
    *((int32_t *)&SendBuffer[24]) = GlobalRootVisualId;
    *((int32_t *)&SendBuffer[28]) = X11_FLAG_WIN_EVENT | X11_FLAG_BACKGROUND_PIXEL;
    *((int32_t *)&SendBuffer[32]) = 0xff000000; // background pixel
    *((int32_t *)&SendBuffer[36]) = X11_EVENT_FLAG_EXPOSURE | X11_EVENT_FLAG_KEY_PRESS;

    write(Socket, (char *)&SendBuffer, RequestLength * 4);
    return WindowId;
}

int X_MapWindow(int Socket, int WindowId) {
    char SendBuffer[16*1024] = {};
    SendBuffer[0] = X11_REQUEST_MAP_WINDOW;
    SendBuffer[1] = 0;
    *((int16_t *)&SendBuffer[2]) = 2;
    *((int32_t *)&SendBuffer[4]) = WindowId;
    write(Socket, (char *)&SendBuffer, 2 * 4);
    return 0;
}

void X_OpenFont(int32_t Socket, char *FontName, int32_t FontId) {
    char SendBuffer[16*1024] = {};
    int32_t FontNameLength = strlen(FontName);
    int32_t Pad = PAD(FontNameLength);
    int RequestLength = (3 + (FontNameLength + Pad) / 4);

    SendBuffer[0] = X11_REQUEST_OPEN_FONT;
    SendBuffer[1] = 0;
    *((uint16_t *)&SendBuffer[2]) = RequestLength;
    *((uint32_t *)&SendBuffer[4]) = FontId;
    *((uint16_t *)&SendBuffer[8]) = FontNameLength;
    strncpy(SendBuffer + 12, FontName, FontNameLength);

    int32_t WriteSize = 12 + FontNameLength + Pad;
    write(Socket, (char *)&SendBuffer, WriteSize);
}

void X_CreateGC(int32_t Socket, int32_t GcId, int32_t FontId) {
    char SendBuffer[16*1024] = {};
    int32_t CreateGcFlagCount = 3;
    int RequestLength = 4 + CreateGcFlagCount;

    SendBuffer[0] = X11_REQUEST_CREATE_GC;
    SendBuffer[1] = 0;
    *((int16_t *)&SendBuffer[2]) = RequestLength;
    *((int32_t *)&SendBuffer[4]) = GcId;
    *((int32_t *)&SendBuffer[8]) = GlobalRootWindow;
    *((int32_t *)&SendBuffer[12]) = X11_FLAG_FG | X11_FLAG_BG | X11_FLAG_FONT;
    *((int32_t *)&SendBuffer[16]) = 0xFF00FF00; // Foreground
    *((int32_t *)&SendBuffer[20]) = 0xFF000000; // Background
    *((int32_t *)&SendBuffer[24]) = FontId; // Font

    write(Socket, (char *)&SendBuffer, RequestLength * 4);
}

// Write a ZPixmap PutImage request with random pixels (white noise).
// This is a minimal implementation and assumes 32-bit (4 bytes per pixel) alignment.
void WriteWhiteNoise(int Socket, int WindowId, int GCid, int Width, int Height) {
    // image parameters
    int depth = 24;      // bits per pixel (commonly 24)
    int format = 2;      // ZPixmap
    int bytesPerPixel = 4; // we'll send 32-bit pixels (pad to 4 bytes)
    int imageSize = Width * Height * bytesPerPixel;

    // request length in 4-byte units: header (6) + image data words
    int requestLength = 6 + ((imageSize + 3) / 4);
    if(requestLength * 4 > READ_BUFFER_SIZE) {
        // image too large for our buffer; skip
        return;
    }

    char Buffer[READ_BUFFER_SIZE];
    memset(Buffer, 0, sizeof(Buffer));

    Buffer[0] = X11_REQUEST_PUT_IMAGE;
    Buffer[1] = (uint8_t)format;
    *((uint16_t*)&Buffer[2]) = (uint16_t)requestLength;
    *((uint32_t*)&Buffer[4]) = (uint32_t)WindowId;
    *((uint32_t*)&Buffer[8]) = (uint32_t)GCid;
    *((uint16_t*)&Buffer[12]) = (uint16_t)Width;
    *((uint16_t*)&Buffer[14]) = (uint16_t)Height;
    *((int16_t*)&Buffer[16]) = 0; // dst-x
    *((int16_t*)&Buffer[18]) = 0; // dst-y
    Buffer[20] = (uint8_t)depth;  // depth

    // Pixel data starts at offset 24
    uint8_t *pixels = (uint8_t*)&Buffer[24];

    // Fill with random bytes. For 32-bit pixel order, we'll write as 0xRRGGBB (we'll set alpha to 0xFF)
    for(int i = 0; i < Width * Height; ++i) {
        uint8_t r = rand() & 0xFF;
        uint8_t g = rand() & 0xFF;
        uint8_t b = rand() & 0xFF;
        // store as 4 bytes per pixel: B G R A (order may vary by server; this is a common layout)
        int idx = i * bytesPerPixel;
        pixels[idx + 0] = b;
        pixels[idx + 1] = g;
        pixels[idx + 2] = r;
        pixels[idx + 3] = 0xFF; // alpha / padding
    }

    // write the request
    ssize_t Written = write(Socket, Buffer, requestLength * 4);
    (void)Written;
}

int main() {
    srand((unsigned)time(NULL));

    int Socket = socket(AF_UNIX, SOCK_STREAM, 0);
    VerifyOrDie(Socket > 0, "Couldn't open a socket(...)");

    struct sockaddr_un Address;
    memset(&Address, 0, sizeof(struct sockaddr_un));
    Address.sun_family = AF_UNIX;
    strncpy(Address.sun_path, "/tmp/.X11-unix/X0", sizeof(Address.sun_path)-1);

    int Status = connect(Socket, (struct sockaddr *)&Address, sizeof(Address));
    VerifyOrDieWidthErrno(Status == 0, "Couldn't connect to a unix socket with connect(...)");

    int SetupStatus = X_InitiateConnection(Socket);

    if(SetupStatus == 0) {
        int32_t X = 100;
        int32_t Y = 100;
        int32_t Width = 600;
        int32_t Height = 300;
        int WindowId = X_CreateWindow(Socket, X, Y, Width, Height);

        X_MapWindow(Socket, WindowId);

        int32_t FontId = GetNextId();
        X_OpenFont(Socket, (char *)"fixed", FontId);

        int32_t GcId = GetNextId();
        X_CreateGC(Socket, GcId, FontId);

        struct pollfd PollDescriptors[1];
        memset(PollDescriptors, 0, sizeof(PollDescriptors));
        PollDescriptors[0].fd = Socket;
        PollDescriptors[0].events = POLLIN;
        int32_t DescriptorCount = 1;
        int32_t IsProgramRunning = 1;

        while(IsProgramRunning) {
            int32_t EventCount = poll(PollDescriptors, DescriptorCount, 10); // short timeout to allow continuous updates

            if(EventCount > 0) {
                if(PollDescriptors[0].revents & POLLERR) {
                    fprintf(stderr, "Socket error\n");
                }
                if(PollDescriptors[0].revents & POLLHUP) {
                    fprintf(stderr, "Connection closed by server\n");
                    IsProgramRunning = 0;
                }
                if(PollDescriptors[0].revents & POLLIN) {
                    GetAndProcessReply(PollDescriptors[0].fd);
                }
            }

            // Render white noise into the window
            WriteWhiteNoise(Socket, WindowId, GcId, Width, Height);

            // Throttle updates to avoid overwhelming the server
            usleep(16000); // ~60 FPS
        }
    } else {
        fprintf(stderr, "Failed to initialize X connection\n");
    }

    close(Socket);
    return 0;
}
