#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>


int32_t GlobalId = 0;
int32_t GlobalIdBase = 0;
int32_t GlobalIdMask = 0;
int32_t GlobalRootWindow = 0;
int32_t GlobalRootVisualId = 0;

int32_t GlobalTextOffsetX = 10;
int32_t GlobalTextOffsetY = 20;

#define READ_BUFFER_SIZE 16*1024

#define RESPONSE_STATE_FAILED 0
#define RESPONSE_STATE_SUCCESS 1
#define RESPONSE_STATE_AUTHENTICATE 2

#define X11_REQUEST_CREATE_WINDOW 1
#define X11_REQUEST_MAP_WINDOW 8
#define X11_REQUEST_IMAGE_TEXT_8 76
#define X11_REQUEST_OPEN_FONT 45
#define X11_REQUEST_CREATE_GC 55


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

#define PAD(N) ((4 - (N % 4)) % 4)

void VerifyOrDie(int IsSuccess, const char *Message) {
    if(!IsSuccess)  {
        fprintf(stderr, "%s", Message);
        exit(13);
    }
}

void VerifyOrDieWidthErrno(int IsSuccess, const char *Message) {
    if(!IsSuccess)  {
        perror(Message);
        exit(13);
    }
}

void DumpResponseError(int Socket, char* ReadBuffer) {
        uint8_t ReasonLength = ReadBuffer[1];
        uint16_t MajorVersion = *((uint16_t*)&ReadBuffer[2]);
        uint16_t MinorVersion = *((uint16_t*)&ReadBuffer[4]);
        uint16_t AdditionalDataLength = *((uint16_t*)&ReadBuffer[6]); // Length in 4-byte units of "additional data"
        uint8_t *Message = (uint8_t*)&ReadBuffer[8];

        int BytesRead = read(Socket, ReadBuffer + 8, READ_BUFFER_SIZE-8);

        printf("State: %d\n", ReadBuffer[0]);
        printf("MajorVersion: %d\n", MajorVersion);
        printf("MinorVersion: %d\n", MinorVersion);
        printf("AdditionalDataLength: %d\n", AdditionalDataLength);
        printf("Reason: %s\n", Message);
}

void AuthenticateX11() {
    fprintf(stderr, "Current version of the app does not support authentication.\n");
    fprintf(stderr, "Please run 'xhost +local:' in your terminal to disable cookie based authentication\n");
    fprintf(stderr, "and allow local apps to communication with Xorg without it.");
}

int32_t GetNextId() {
    int32_t Result = (GlobalIdMask & GlobalId) | GlobalIdBase;
    GlobalId += 1;
    return Result;
}

void PrintResponseError(char *Data, int32_t Size) {
    char ErrorCode = Data[1];
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
    if(ErrorCode < sizeof(ErrorNames) / sizeof(ErrorNames[0])) {
        ErrorName = ErrorNames[ErrorCode];
    }


    uint16_t Minor = *((uint16_t*)&Data[8]);
    uint8_t Major = *((uint8_t*)&Data[10]);

    printf("\033[0;31m");
    printf("Response Error: [%d] %s", ErrorCode, ErrorName);
    printf("	Minor: %d, Major: %d", Minor, Major);
    printf("\033[0m\n");


}

void PrintAndProcessEvent(char *Data, int32_t Size) {
    char EventCode = Data[0];
    const char* EventNames[] = {
        "-- Wrong Event Code --",
        "-- Wrong Event Code --",
        "KeyPress",
        "KeyRelease",
        "ButtonPress",
        "ButtonRelease",
        "MotionNotify",
        "EnterNotify",
        "LeaveNotify",
        "FocusIn",
        "FocusOut",
        "KeymapNotify",
        "Expose",
        "GraphicsExposure",
        "NoExposure",
        "VisibilityNotify",
        "CreateNotify",
        "DestroyNotify",
        "UnmapNotify",
        "MapNotify",
        "MapRequest",
        "ReparentNotify",
        "ConfigureNotify",
        "ConfigureRequest",
        "GravityNotify",
        "ResizeRequest",
        "CirculateNotify",
        "CirculateRequest",
        "PropertyNotify",
        "SelectionClear",
        "SelectionRequest",
        "SelectionNotify",
        "ColormapNotify",
        "ClientMessage",
        "MappingNotify",
    };

#define REPLY_EVENT_CODE_KEY_PRESS 2
#define REPLY_EVENT_CODE_EXPOSE 12

const char* TERMINAL_TEXT_COLOR_RED = "\033[0;32m";
const char* TERMINAL_TEXT_COLOR_CLEAR = "\033[0m";

    if(EventCode == REPLY_EVENT_CODE_EXPOSE) {
        // NOTE: Exposure event
        const char *EventName = "Expose";
        uint16_t SequenceNumber = *((uint16_t*)&Data[2]);
        uint32_t Window = *((uint32_t*)&Data[4]);
        uint16_t X = *((uint16_t*)&Data[8]);
        uint16_t Y = *((uint16_t*)&Data[10]);
        uint16_t Width = *((uint16_t*)&Data[12]);
        uint16_t Height = *((uint16_t*)&Data[14]);
        uint16_t Count = *((uint16_t*)&Data[16]);

        printf(TERMINAL_TEXT_COLOR_RED);
            printf("%s: ", EventName);
        printf(TERMINAL_TEXT_COLOR_CLEAR);

        printf("Seq %d, ", SequenceNumber);
        printf("Win %d: ", Window);
        printf("X %d: ", X);
        printf("Y %d: ", Y);
        printf("Width %d: ", Width);
        printf("Height %d: ", Height);
        printf("Count %d: ", Count);
        printf("\n");
        /* printf("%s: Seq %d\n", EventName, SequenceNumber); */
    } else if(EventCode == REPLY_EVENT_CODE_KEY_PRESS) {
        const char *EventName = "KeyPress";
        char KeyCode = Data[1];
        uint16_t SequenceNumber = *((uint16_t*)&Data[2]);
        uint32_t TimeStamp = *((uint32_t*)&Data[4]);
        uint32_t RootWindow = *((uint32_t*)&Data[8]);
        uint32_t EventWindow = *((uint32_t*)&Data[12]);
        uint32_t ChildWindow = *((uint32_t*)&Data[16]); // NOTE: Always 0
        int16_t RootX = *((int16_t*)&Data[20]);
        int16_t RootY = *((int16_t*)&Data[22]);
        int16_t EventX = *((int16_t*)&Data[24]);
        int16_t EventY = *((int16_t*)&Data[26]);
        int16_t SetOfKeyButMask = *((int16_t*)&Data[28]);
        int8_t IsSameScreen = *((int8_t*)&Data[30]);

        printf(TERMINAL_TEXT_COLOR_RED);
            printf("%s: ", EventName);
        printf(TERMINAL_TEXT_COLOR_CLEAR);

        // NOTE: Temporary hack that will not work everywhere
        int StepSize = 10;
        if(KeyCode == 25) { GlobalTextOffsetY += StepSize; }
        if(KeyCode == 39) { GlobalTextOffsetY -= StepSize; }
        if(KeyCode == 38) { GlobalTextOffsetX -= StepSize; }
        if(KeyCode == 40) { GlobalTextOffsetX += StepSize; }

        printf("Code %u, ", (uint8_t)KeyCode);
        printf("Seq %d, ", SequenceNumber);
        printf("Time %d, ", TimeStamp);
        printf("Root %d, ", RootWindow);
        printf("EventW %d, ", EventWindow);
        printf("Child %d, ", ChildWindow);
        printf("RX %d, ", RootX);
        printf("RY %d, ", RootY);
        printf("EX %d, ", EventX);
        printf("EY %d, ", EventY);
        printf("\n");
    } else {
        const char* EventName = " - Unknown Event Code -";
        if(EventCode < sizeof(EventNames) / sizeof(EventNames[0])) {
            EventName = EventNames[EventCode];
        }
        // printf("-------------Event: %s\n", EventName);
        // for(int i = 0; i < Size; i++) {
            // printf("%c", Data[i]);
        // }
        // printf("\n");
    }

}

void GetAndProcessReply(int Socket) {
    char Buffer[1024] = {};
    int32_t BytesRead = read(Socket, Buffer, 1024);

    uint8_t Code = Buffer[0];

    if(Code == 0) {
        PrintResponseError(Buffer, BytesRead);
    } else if (Code == 1) {
        printf("---------------- Unexpected reply\n");
    } else {
        // NOTE: Event?
        PrintAndProcessEvent(Buffer, BytesRead);
    }
}

int X_InitiateConnection(int Socket) {
    // TODO: Remove global variables and put them into 'connection' struct.
    int SetupStatus = 1;
    char SendBuffer[16*1024] = {};
    char ReadBuffer[16*1024] = {};

    uint8_t InitializationRequest[12] = {};
    InitializationRequest[0] = 'l';
    InitializationRequest[1] = 0;
    InitializationRequest[2] = 11;

    int BytesWritten = write(Socket, (char*)&InitializationRequest, sizeof(InitializationRequest));
    VerifyOrDie(BytesWritten == sizeof(InitializationRequest), "Wrong amount of bytes written during initialization");

    int BytesRead = read(Socket, ReadBuffer, 8);

    if(ReadBuffer[0] == RESPONSE_STATE_FAILED) {
        DumpResponseError(Socket, ReadBuffer);
    }
    else if(ReadBuffer[0] == RESPONSE_STATE_AUTHENTICATE) {
        AuthenticateX11();
    }
    else if(ReadBuffer[0] == RESPONSE_STATE_SUCCESS) {
        printf("INIT Response SUCCESS. BytesRead: %d\n", BytesRead);

        BytesRead = read(Socket, ReadBuffer + 8, READ_BUFFER_SIZE-8);
        printf("---------------------------%d\n", BytesRead);

        /* -------------------------------------------------------------------------------- */
        uint8_t _Unused = ReadBuffer[1];
        uint16_t MajorVersion = *((uint16_t*)&ReadBuffer[2]);
        uint16_t MinorVersion = *((uint16_t*)&ReadBuffer[4]);
        uint16_t AdditionalDataLength = *((uint16_t*)&ReadBuffer[6]); // Length in 4-byte units of "additional data"

        uint32_t ResourceIdBase = *((uint32_t*)&ReadBuffer[12]);
        uint32_t ResourceIdMask = *((uint32_t*)&ReadBuffer[16]);
        uint16_t LengthOfVendor = *((uint16_t*)&ReadBuffer[24]);
        uint8_t NumberOfFormants = *((uint16_t*)&ReadBuffer[29]);
        uint8_t *Vendor = (uint8_t *)&ReadBuffer[40];

        int32_t VendorPad = PAD(LengthOfVendor);
        int32_t FormatByteLength = 8 * NumberOfFormants;
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

int X_CreatWindow(int Socket, int X, int Y, int Width, int Height) {
    // TODO: Put this into 'connection' struct
    char SendBuffer[16*1024] = {};
    char ReadBuffer[16*1024] = {};

    int32_t WindowId = GetNextId();
    int32_t Depth = 0;
    uint32_t BorderWidth = 1;
    int32_t CreateWindowFlagCount = 2;
    int RequestLength = 8+CreateWindowFlagCount;

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
    *((int32_t *)&SendBuffer[32]) = 0xff000000;
    *((int32_t *)&SendBuffer[36]) = X11_EVENT_FLAG_EXPOSURE | X11_EVENT_FLAG_KEY_PRESS;

    int BytesWritten = write(Socket, (char *)&SendBuffer, RequestLength*4);

    return WindowId;
}

int X_MapWindow(int Socket, int WindowId) {
    // TODO: Put this into 'connection' struct
    char SendBuffer[16*1024] = {};
    char ReadBuffer[16*1024] = {};

    SendBuffer[0] = X11_REQUEST_MAP_WINDOW;
    SendBuffer[1] = 0;
    *((int16_t *)&SendBuffer[2]) = 2;
    *((int32_t *)&SendBuffer[4]) = WindowId;

    int BytesWritten = write(Socket, (char *)&SendBuffer, 2*4);
    return 0;
}

void X_OpenFont(int32_t Socket, char *FontName, int32_t FontId) {
    char SendBuffer[16*1024] = {};
    char ReadBuffer[16*1024] = {};
    int BytesWritten = 0;
    int BytesRead = 0;

    int32_t FontNameLength = strlen((char *)FontName);
    int32_t Pad = PAD(FontNameLength);
    int RequestLength = (3 + (FontNameLength + Pad)/4);

    SendBuffer[0] = X11_REQUEST_OPEN_FONT;
    SendBuffer[1] = 0;
    *((uint16_t *)&SendBuffer[2]) = RequestLength;
    *((uint32_t *)&SendBuffer[4]) = FontId;
    *((uint16_t *)&SendBuffer[8]) = FontNameLength;
    strncpy(SendBuffer + 12, (char *)FontName, FontNameLength);

    int32_t WriteSize = 12 + FontNameLength + Pad;
    BytesWritten = write(Socket, (char *)&SendBuffer, WriteSize);
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

    write(Socket, (char *)&SendBuffer, RequestLength*4);
}

void WriteText(int Socket, int WindowId, int GCid, int16_t X, int16_t Y, const char *Text, int32_t TextLength) {
    char Buffer[16*1024] = {};

    uint32_t ContentLength = 4 + (TextLength + PAD(TextLength))/4;

    Buffer[0] = (uint8_t)X11_REQUEST_IMAGE_TEXT_8;
    Buffer[1] = TextLength;
    *((int16_t *)&Buffer[2]) = ContentLength;
    *((int32_t *)&Buffer[4]) = WindowId;
    *((int32_t *)&Buffer[8]) = GCid;
    *((int16_t *)&Buffer[12]) = X;
    *((int16_t *)&Buffer[14]) = Y;

    strncpy(&Buffer[16], (char *)Text, TextLength);
    int BytesWritten = write(Socket, (char *)&Buffer, ContentLength*4);
}

int main(){
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
        uint32_t Width = 600;
        uint32_t Height = 300;
        int WindowId = X_CreatWindow(Socket, X, Y, Width, Height);

        X_MapWindow(Socket, WindowId);

        int32_t FontId = GetNextId();
        X_OpenFont(Socket, (int8_t *)"fixed", FontId);

        int32_t GcId = GetNextId();
        X_CreateGC(Socket, GcId, FontId);

        struct pollfd PollDescriptors[1] = {};
        PollDescriptors[0].fd = Socket;
        PollDescriptors[0].events = POLLIN;
        int32_t DescriptorCount = 1;
        int32_t IsProgramRunning = 1;
        while(IsProgramRunning){
            int32_t EventCount = poll(PollDescriptors, DescriptorCount, -1);

            if(PollDescriptors[0].revents & POLLERR) {
                printf("------- Error\n");
            }

            if(PollDescriptors[0].revents & POLLHUP) {
                printf("---- Connection close\n");
                IsProgramRunning = 0;
            }

            char* t1 = "Hello, World!";
            char* t2 = "This is a test text directly written to X";
            char* t3 = "Whooha. Is this even legal? Let's keep a secret!";
            WriteText(Socket, WindowId, GcId, GlobalTextOffsetX, GlobalTextOffsetY, t1, strlen(t1));
            WriteText(Socket, WindowId, GcId, GlobalTextOffsetX, GlobalTextOffsetY + 15, t2, strlen(t2));
            WriteText(Socket, WindowId, GcId, GlobalTextOffsetX, GlobalTextOffsetY + 30, t3, strlen(t3));

            GetAndProcessReply(PollDescriptors[0].fd);
        }
    }

}
