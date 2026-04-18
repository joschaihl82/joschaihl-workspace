#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    MessageBox(
        NULL,                             // Kein Besitzer-Fenster
        L"Hello, World from Win32 API!",  // Textinhalt
        L"Win32 Message",                 // Titel des Fensters
        MB_OK | MB_ICONINFORMATION        // Buttons (OK) und Icon (Info)
    );
    return 0;
}
