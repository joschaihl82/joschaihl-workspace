// notepad1_fixed_highlighted.c
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <richedit.h>
#include <shlwapi.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdbool.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Shlwapi.lib")

/* IDs */
#define IDM_FILE_NEW      101
#define IDM_FILE_OPEN     102
#define IDM_FILE_SAVE     103
#define IDM_FILE_SAVEAS   104
#define IDM_FILE_EXIT     105
#define IDM_EDIT_CUT      201
#define IDM_EDIT_COPY     202
#define IDM_EDIT_PASTE    203
#define IDM_BUILD_COMPILE 301
#define IDM_HELP_ABOUT    401

#define TB_NEW_BTN    1001
#define TB_OPEN_BTN   1002
#define TB_SAVE_BTN   1003
#define TB_BUILD_BTN  1004

static HWND hEdit;
static HWND hLineNums;
static HWND hMainWnd;
static HWND hStatus;
static HFONT hMonoFont = NULL;
static wchar_t currentFile[MAX_PATH] = L"";
static const wchar_t gClassName[] = L"MiniNotepadClassFixed";

/* Font file handling */
static wchar_t g_privateFontPath[MAX_PATH] = L"";
static bool g_privateFontRegistered = false;
static const wchar_t *PRIVATE_FONT_FILENAME = L"SourceCodePro-Regular.ttf";
static const wchar_t *PRIVATE_FACE_NAME = L"Source Code Pro";

/* Colors */
enum {
    COLOR_DEFAULT = RGB(0,0,0),
    COLOR_KEYWORD = RGB(0,0,255),
    COLOR_STRING  = RGB(163,21,21),
    COLOR_COMMENT = RGB(0,128,0),
    COLOR_NUMBER  = RGB(128,0,128),
    COLOR_LINENUM = RGB(0,0,255)
};

/* Keywords */
static const wchar_t *c_keywords[] = {
    L"auto", L"break", L"case", L"char", L"const", L"continue", L"default", L"do",
    L"double", L"else", L"enum", L"extern", L"float", L"for", L"goto", L"if",
    L"inline", L"int", L"long", L"register", L"restrict", L"return", L"short",
    L"signed", L"sizeof", L"static", L"struct", L"switch", L"typedef", L"union",
    L"unsigned", L"void", L"volatile", L"while", L"bool", L"true", L"false", NULL
};

static bool is_ident_char(wchar_t c) {
    return (c == L'_') || (c >= L'0' && c <= L'9') || (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') || (c >= 0x80);
}

static bool is_keyword(const wchar_t *s, int len) {
    for (const wchar_t **k = c_keywords; *k; ++k) {
        if ((int)wcslen(*k) == len && wcsncmp(*k, s, len) == 0) return true;
    }
    return false;
}

/* Prevent re-entrant highlighting */
static volatile bool g_in_highlight = false;

/* Load private font from EXE dir (required) */
static bool LoadPrivateFontFromExeDir(void) {
    wchar_t exePath[MAX_PATH];
    if (!GetModuleFileNameW(NULL, exePath, MAX_PATH)) return false;
    PathRemoveFileSpecW(exePath);
    if (!PathCombineW(g_privateFontPath, exePath, PRIVATE_FONT_FILENAME)) {
        g_privateFontPath[0] = L'\0';
        return false;
    }
    if (!PathFileExistsW(g_privateFontPath)) {
        g_privateFontPath[0] = L'\0';
        return false;
    }
    int added = AddFontResourceExW(g_privateFontPath, FR_PRIVATE, 0);
    if (added == 0) {
        g_privateFontPath[0] = L'\0';
        return false;
    }
    g_privateFontRegistered = true;
    return true;
}

static void UnloadPrivateFont(void) {
    if (hMonoFont) {
        DeleteObject(hMonoFont);
        hMonoFont = NULL;
    }
    if (g_privateFontRegistered && g_privateFontPath[0]) {
        RemoveFontResourceExW(g_privateFontPath, FR_PRIVATE, 0);
        g_privateFontPath[0] = L'\0';
        g_privateFontRegistered = false;
    }
}

/* Set format for [start,end) */
static void set_format_range(HWND edit, int start, int end, COLORREF color, bool bold) {
    if (start >= end) return;
    CHARFORMAT2W cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR;
    if (bold) cf.dwMask |= CFM_BOLD;
    cf.crTextColor = color;
    cf.dwEffects = bold ? CFE_BOLD : 0;
    SendMessageW(edit, EM_SETSEL, (WPARAM)start, (LPARAM)end);
    SendMessageW(edit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

/* Set default format for [start,end) (preserve face/size by including CFM_FACE/CFM_SIZE if needed) */
static void set_default_format_range(HWND edit, int start, int end) {
    if (start >= end) return;
    CHARFORMAT2W cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = COLOR_DEFAULT;
    SendMessageW(edit, EM_SETSEL, (WPARAM)start, (LPARAM)end);
    SendMessageW(edit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

/* Highlight a specific range by tokenizing only that range */
static void HighlightRange(HWND edit, int start, int end) {
    if (g_in_highlight) return;
    if (start >= end) return;
    g_in_highlight = true;

    CHARRANGE oldRange;
    SendMessageW(edit, EM_EXGETSEL, 0, (LPARAM)&oldRange);
    SendMessageW(edit, WM_SETREDRAW, FALSE, 0);

    set_default_format_range(edit, start, end);

    int len = end - start;
    wchar_t *buf = (wchar_t*)GlobalAlloc(GPTR, (len + 1) * sizeof(wchar_t));
    if (buf) {
        TEXTRANGEW tr;
        tr.chrg.cpMin = start;
        tr.chrg.cpMax = end;
        tr.lpstrText = buf;
        SendMessageW(edit, EM_GETTEXTRANGE, 0, (LPARAM)&tr);

        int i = 0;
        while (i < len) {
            wchar_t c = buf[i];

            // // comment
            if (c == L'/' && i + 1 < len && buf[i+1] == L'/') {
                int s = start + i;
                int j = i + 2;
                while (j < len && buf[j] != L'\r' && buf[j] != L'\n') j++;
                set_format_range(edit, s, start + j, COLOR_COMMENT, false);
                i = j;
                continue;
            }

            // /* comment */
            if (c == L'/' && i + 1 < len && buf[i+1] == L'*') {
                int s = start + i;
                int j = i + 2;
                while (j + 1 < len && !(buf[j] == L'*' && buf[j+1] == L'/')) j++;
                if (j + 1 < len) j += 2;
                set_format_range(edit, s, start + j, COLOR_COMMENT, false);
                i = j;
                continue;
            }

            // string
            if (c == L'"') {
                int s = start + i;
                int j = i + 1;
                while (j < len) {
                    if (buf[j] == L'\\' && j + 1 < len) { j += 2; continue; }
                    if (buf[j] == L'"') { j++; break; }
                    j++;
                }
                set_format_range(edit, s, start + j, COLOR_STRING, false);
                i = j;
                continue;
            }

            // char literal
            if (c == L'\'') {
                int s = start + i;
                int j = i + 1;
                while (j < len) {
                    if (buf[j] == L'\\' && j + 1 < len) { j += 2; continue; }
                    if (buf[j] == L'\'') { j++; break; }
                    j++;
                }
                set_format_range(edit, s, start + j, COLOR_STRING, false);
                i = j;
                continue;
            }

            // number
            if ((c >= L'0' && c <= L'9') || (c == L'.' && i + 1 < len && buf[i+1] >= L'0' && buf[i+1] <= L'9')) {
                int s = start + i;
                int j = i;
                if (buf[j] == L'0' && j + 1 < len && (buf[j+1] == L'x' || buf[j+1] == L'X')) {
                    j += 2;
                    while (j < len && ((buf[j] >= L'0' && buf[j] <= L'9') || (buf[j] >= L'a' && buf[j] <= L'f') || (buf[j] >= L'A' && buf[j] <= L'F'))) j++;
                } else {
                    while (j < len && ((buf[j] >= L'0' && buf[j] <= L'9') || buf[j] == L'.' || buf[j] == L'e' || buf[j] == L'E' || buf[j] == L'+' || buf[j] == L'-')) j++;
                }
                set_format_range(edit, s, start + j, COLOR_NUMBER, false);
                i = j;
                continue;
            }

            // identifier / keyword
            if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') || c == L'_') {
                int s = start + i;
                int j = i + 1;
                while (j < len && is_ident_char(buf[j])) j++;
                int toklen = j - i;
                if (is_keyword(&buf[i], toklen)) {
                    set_format_range(edit, s, start + j, COLOR_KEYWORD, true);
                }
                i = j;
                continue;
            }

            i++;
        }

        GlobalFree(buf);
    }

    SendMessageW(edit, EM_EXSETSEL, 0, (LPARAM)&oldRange);
    SendMessageW(edit, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(edit, NULL, FALSE);
    g_in_highlight = false;
}

/* Highlight only the line where the caret is */
static void HighlightLineAtCaret(HWND edit) {
    if (g_in_highlight) return;
    DWORD selStart = 0, selEnd = 0;
    SendMessageW(edit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
    int pos = (int)selStart;

    int line = (int)SendMessageW(edit, EM_LINEFROMCHAR, (WPARAM)pos, 0);
    int lineStart = (int)SendMessageW(edit, EM_LINEINDEX, (WPARAM)line, 0);
    if (lineStart < 0) lineStart = 0;
    int lineLen = (int)SendMessageW(edit, EM_LINELENGTH, (WPARAM)pos, 0);
    if (lineLen < 0) lineLen = 0;
    int lineEnd = lineStart + lineLen;
    if (lineEnd < lineStart) lineEnd = lineStart;

    HighlightRange(edit, lineStart, lineEnd);
}

/* Full-document highlighting (used on load/paste) */
static void HighlightSyntax(HWND edit) {
    if (g_in_highlight) return;
    g_in_highlight = true;

    int len = GetWindowTextLengthW(edit);
    wchar_t *text = (wchar_t*)GlobalAlloc(GPTR, (len + 1) * sizeof(wchar_t));
    if (!text) { g_in_highlight = false; return; }
    GetWindowTextW(edit, text, len + 1);

    CHARRANGE oldRange;
    SendMessageW(edit, EM_EXGETSEL, 0, (LPARAM)&oldRange);
    SendMessageW(edit, WM_SETREDRAW, FALSE, 0);

    // Default color for all text
    {
        CHARFORMAT2W cf;
        ZeroMemory(&cf, sizeof(cf));
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_COLOR;
        cf.crTextColor = COLOR_DEFAULT;
        SendMessageW(edit, EM_SETSEL, 0, len);
        SendMessageW(edit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    }

    int i = 0;
    while (i < len) {
        wchar_t c = text[i];

        if (c == L'/' && i + 1 < len && text[i+1] == L'/') {
            int start = i;
            i += 2;
            while (i < len && text[i] != L'\r' && text[i] != L'\n') i++;
            set_format_range(edit, start, i, COLOR_COMMENT, false);
            continue;
        }

        if (c == L'/' && i + 1 < len && text[i+1] == L'*') {
            int start = i;
            i += 2;
            while (i + 1 < len && !(text[i] == L'*' && text[i+1] == L'/')) i++;
            if (i + 1 < len) i += 2;
            set_format_range(edit, start, i, COLOR_COMMENT, false);
            continue;
        }

        if (c == L'"') {
            int start = i++;
            while (i < len) {
                if (text[i] == L'\\' && i + 1 < len) { i += 2; continue; }
                if (text[i] == L'"') { i++; break; }
                i++;
            }
            set_format_range(edit, start, i, COLOR_STRING, false);
            continue;
        }

        if (c == L'\'') {
            int start = i++;
            while (i < len) {
                if (text[i] == L'\\' && i + 1 < len) { i += 2; continue; }
                if (text[i] == L'\'') { i++; break; }
                i++;
            }
            set_format_range(edit, start, i, COLOR_STRING, false);
            continue;
        }

        if ((c >= L'0' && c <= L'9') || (c == L'.' && i + 1 < len && text[i+1] >= L'0' && text[i+1] <= L'9')) {
            int start = i;
            if (c == L'0' && i + 1 < len && (text[i+1] == L'x' || text[i+1] == L'X')) {
                i += 2;
                while (i < len && ((text[i] >= L'0' && text[i] <= L'9') || (text[i] >= L'a' && text[i] <= L'f') || (text[i] >= L'A' && text[i] <= L'F'))) i++;
            } else {
                while (i < len && ((text[i] >= L'0' && text[i] <= L'9') || text[i] == L'.' || text[i] == L'e' || text[i] == L'E' || text[i] == L'+' || text[i] == L'-')) i++;
            }
            set_format_range(edit, start, i, COLOR_NUMBER, false);
            continue;
        }

        if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') || c == L'_') {
            int start = i++;
            while (i < len && is_ident_char(text[i])) i++;
            int toklen = i - start;
            if (is_keyword(&text[start], toklen)) {
                set_format_range(edit, start, i, COLOR_KEYWORD, true);
            }
            continue;
        }

        i++;
    }

    SendMessageW(edit, EM_EXSETSEL, 0, (LPARAM)&oldRange);
    SendMessageW(edit, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(edit, NULL, FALSE);
    GlobalFree(text);
    g_in_highlight = false;
}

/* Update line numbers: build single buffer and set once */
static void UpdateLineNumbers() {
    int lines = (int)SendMessageW(hEdit, EM_GETLINECOUNT, 0, 0);
    // allocate enough: 8 chars per line approx
    int bufSize = (lines + 2) * 8;
    wchar_t *buf = (wchar_t*)GlobalAlloc(GPTR, bufSize * sizeof(wchar_t));
    if (!buf) return;
    buf[0] = L'\0';
    wchar_t tmp[32];
    for (int i = 0; i < lines; ++i) {
        wsprintfW(tmp, L"%4d\r\n", i + 1);
        wcscat_s(buf, bufSize, tmp);
    }
    SetWindowTextW(hLineNums, buf);

    // Ensure line numbers are blue and use the monospace face/size
    CHARFORMAT2W cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE;
    cf.crTextColor = COLOR_LINENUM;
    cf.yHeight = 18 * 20; // 18px approx -> twips
    wcsncpy_s(cf.szFaceName, LF_FACESIZE, PRIVATE_FACE_NAME, _TRUNCATE);
    SendMessageW(hLineNums, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);

    GlobalFree(buf);
}

/* Update status bar */
static void UpdateStatusBar() {
    DWORD selStart = 0, selEnd = 0;
    SendMessageW(hEdit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
    int pos = (int)selStart;
    int line = (int)SendMessageW(hEdit, EM_LINEFROMCHAR, (WPARAM)pos, 0);
    int lineIndex = (int)SendMessageW(hEdit, EM_LINEINDEX, (WPARAM)line, 0);
    int col = pos - lineIndex;
    wchar_t status[512];
    wchar_t fname[MAX_PATH];
    if (currentFile[0]) wcscpy_s(fname, MAX_PATH, currentFile); else wcscpy_s(fname, MAX_PATH, L"(untitled)");
    BOOL modified = (BOOL)SendMessageW(hEdit, EM_GETMODIFY, 0, 0);
    if (modified) wsprintfW(status, L"* %s    Ln %d, Col %d", fname, line + 1, col + 1);
    else wsprintfW(status, L"%s    Ln %d, Col %d", fname, line + 1, col + 1);
    SendMessageW(hStatus, SB_SETTEXTW, (WPARAM)0, (LPARAM)status);
}

/* Save/Open functions (unchanged logic) */
static bool SaveFileAs(HWND hwnd, const wchar_t *filename, bool utf8) {
    int len = GetWindowTextLengthW(hEdit);
    wchar_t *buf = (wchar_t*)GlobalAlloc(GPTR, (len + 1) * sizeof(wchar_t));
    if (!buf) return false;
    GetWindowTextW(hEdit, buf, len + 1);
    HANDLE hFile = CreateFileW(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) { GlobalFree(buf); return false; }
    DWORD written;
    if (utf8) {
        BYTE bom[3] = {0xEF,0xBB,0xBF};
        WriteFile(hFile, bom, 3, &written, NULL);
        int needed = WideCharToMultiByte(CP_UTF8, 0, buf, len, NULL, 0, NULL, NULL);
        char *mb = (char*)GlobalAlloc(GPTR, needed);
        WideCharToMultiByte(CP_UTF8, 0, buf, len, mb, needed, NULL, NULL);
        WriteFile(hFile, mb, needed, &written, NULL);
        GlobalFree(mb);
    } else {
        WORD bom = 0xFEFF;
        WriteFile(hFile, &bom, sizeof(bom), &written, NULL);
        WriteFile(hFile, buf, len * sizeof(wchar_t), &written, NULL);
    }
    CloseHandle(hFile);
    GlobalFree(buf);
    SendMessageW(hEdit, EM_SETMODIFY, FALSE, 0);
    UpdateStatusBar();
    return true;
}

static bool OpenFileToEditor(HWND hwnd, const wchar_t *filename) {
    HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    DWORD size = GetFileSize(hFile, NULL);
    BYTE *buf = (BYTE*)GlobalAlloc(GPTR, size + 4);
    if (!buf) { CloseHandle(hFile); return false; }
    DWORD read = 0;
    ReadFile(hFile, buf, size, &read, NULL);
    CloseHandle(hFile);

    if (read >= 3 && buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) {
        int needed = MultiByteToWideChar(CP_UTF8, 0, (char*)(buf + 3), read - 3, NULL, 0);
        wchar_t *wbuf = (wchar_t*)GlobalAlloc(GPTR, (needed + 1) * sizeof(wchar_t));
        MultiByteToWideChar(CP_UTF8, 0, (char*)(buf + 3), read - 3, wbuf, needed);
        wbuf[needed] = 0;
        SetWindowTextW(hEdit, wbuf);
        GlobalFree(wbuf);
    } else if (read >= 2 && buf[0] == 0xFF && buf[1] == 0xFE) {
        SetWindowTextW(hEdit, (wchar_t*)(buf + 2));
    } else {
        int needed = MultiByteToWideChar(CP_ACP, 0, (char*)buf, read, NULL, 0);
        wchar_t *wbuf = (wchar_t*)GlobalAlloc(GPTR, (needed + 1) * sizeof(wchar_t));
        MultiByteToWideChar(CP_ACP, 0, (char*)buf, read, wbuf, needed);
        wbuf[needed] = 0;
        SetWindowTextW(hEdit, wbuf);
        GlobalFree(wbuf);
    }
    GlobalFree(buf);
    HighlightSyntax(hEdit);
    SendMessageW(hEdit, EM_SETMODIFY, FALSE, 0);
    UpdateLineNumbers();
    UpdateStatusBar();
    return true;
}

/* Prompt Save If Modified */
static int PromptSaveIfModified(HWND hwnd) {
    LRESULT modified = SendMessageW(hEdit, EM_GETMODIFY, 0, 0);
    if (!modified) return 1;
    int res = MessageBoxW(hwnd, L"Die Datei wurde geändert. Möchten Sie die Änderungen speichern?", L"Speichern", MB_YESNOCANCEL | MB_ICONQUESTION);
    if (res == IDCANCEL) return 0;
    if (res == IDNO) return 1;
    if (res == IDYES) {
        if (!currentFile[0]) {
            wchar_t szFile[MAX_PATH] = L"";
            OPENFILENAMEW ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"C Source\0*.c;*.h\0All Files\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST;
            ofn.lpstrDefExt = L"c";
            if (!GetSaveFileNameW(&ofn)) return 0;
            wcscpy_s(currentFile, MAX_PATH, szFile);
        }
        if (!SaveFileAs(hwnd, currentFile, true)) {
            MessageBoxW(hwnd, L"Fehler beim Speichern.", L"Fehler", MB_OK | MB_ICONERROR);
            return 0;
        }
        return 2;
    }
    return 0;
}

/* Simple toolbar: create buttons as children */
static void CreateSimpleToolbar(HWND parent, HINSTANCE hInst) {
    int btnW = 80;
    int btnH = 28;
    int x = 6;
    int y = 6;
    CreateWindowExW(0, L"BUTTON", L"New", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, btnW, btnH, parent, (HMENU)TB_NEW_BTN, hInst, NULL);
    x += btnW + 6;
    CreateWindowExW(0, L"BUTTON", L"Open", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, btnW, btnH, parent, (HMENU)TB_OPEN_BTN, hInst, NULL);
    x += btnW + 6;
    CreateWindowExW(0, L"BUTTON", L"Save", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, btnW, btnH, parent, (HMENU)TB_SAVE_BTN, hInst, NULL);
    x += btnW + 6;
    CreateWindowExW(0, L"BUTTON", L"Build", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, btnW, btnH, parent, (HMENU)TB_BUILD_BTN, hInst, NULL);
}

/* Window procedure */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        LoadLibraryW(L"Msftedit.dll");

        // Create HFONT using private face (WinMain ensured registration)
        LOGFONTW lf;
        ZeroMemory(&lf, sizeof(lf));
        HDC hdc = GetDC(hwnd);
        // 18px requested: convert px to logical units using device DPI (approx)
        lf.lfHeight = -MulDiv(18, GetDeviceCaps(hdc, LOGPIXELSY), 96);
        ReleaseDC(hwnd, hdc);
        lf.lfWeight = FW_NORMAL;
        lf.lfCharSet = DEFAULT_CHARSET;
        wcsncpy_s(lf.lfFaceName, LF_FACESIZE, PRIVATE_FACE_NAME, _TRUNCATE);
        hMonoFont = CreateFontIndirectW(&lf);
        if (!hMonoFont) {
            MessageBoxW(hwnd, L"Fehler: Schrift konnte nicht erstellt werden.", L"Fehler", MB_ICONERROR);
            PostQuitMessage(1);
            return -1;
        }

        // Toolbar
        CreateSimpleToolbar(hwnd, ((LPCREATESTRUCT)lParam)->hInstance);

        // Line numbers control
        hLineNums = CreateWindowExW(0, MSFTEDIT_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_BORDER,
            6, 40, 80, 0, hwnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        SendMessageW(hLineNums, WM_SETFONT, (WPARAM)hMonoFont, TRUE);
        SendMessageW(hLineNums, EM_SETREADONLY, TRUE, 0);

        // Main editor
        hEdit = CreateWindowExW(0, MSFTEDIT_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            92, 40, 0, 0, hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        SendMessageW(hEdit, WM_SETFONT, (WPARAM)hMonoFont, TRUE);

        // Force CHARFORMAT for editor (face + size)
        {
            CHARFORMAT2W cf;
            ZeroMemory(&cf, sizeof(cf));
            cf.cbSize = sizeof(cf);
            cf.dwMask = CFM_FACE | CFM_SIZE | CFM_COLOR;
            cf.yHeight = 18 * 20; // 18px approx -> twips
            cf.crTextColor = COLOR_DEFAULT;
            wcsncpy_s(cf.szFaceName, LF_FACESIZE, PRIVATE_FACE_NAME, _TRUNCATE);
            SendMessageW(hEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
        }

        // Set line numbers initial format (blue)
        {
            CHARFORMAT2W cf;
            ZeroMemory(&cf, sizeof(cf));
            cf.cbSize = sizeof(cf);
            cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE;
            cf.crTextColor = COLOR_LINENUM;
            cf.yHeight = 18 * 20;
            wcsncpy_s(cf.szFaceName, LF_FACESIZE, PRIVATE_FACE_NAME, _TRUNCATE);
            SendMessageW(hLineNums, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
        }

        // Status bar
        hStatus = CreateWindowExW(0, STATUSCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        // Menu
        HMENU hMenubar = CreateMenu();
        HMENU hFile = CreatePopupMenu();
        HMENU hEditMenu = CreatePopupMenu();
        HMENU hBuild = CreatePopupMenu();
        HMENU hHelp = CreatePopupMenu();

        AppendMenuW(hFile, MF_STRING, IDM_FILE_NEW, L"&Neu\tCtrl+N");
        AppendMenuW(hFile, MF_STRING, IDM_FILE_OPEN, L"&Öffnen...\tCtrl+O");
        AppendMenuW(hFile, MF_STRING, IDM_FILE_SAVE, L"&Speichern\tCtrl+S");
        AppendMenuW(hFile, MF_STRING, IDM_FILE_SAVEAS, L"Speichern &unter...");
        AppendMenuW(hFile, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hFile, MF_STRING, IDM_FILE_EXIT, L"&Beenden");

        AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_CUT, L"Ausschneiden\tCtrl+X");
        AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_COPY, L"Kopieren\tCtrl+C");
        AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_PASTE, L"Einfügen\tCtrl+V");

        AppendMenuW(hBuild, MF_STRING, IDM_BUILD_COMPILE, L"&Kompilieren\tCtrl+Shift+B");

        AppendMenuW(hHelp, MF_STRING, IDM_HELP_ABOUT, L"&Über");

        AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"&Datei");
        AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hEditMenu, L"&Bearbeiten");
        AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hBuild, L"&Build");
        AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hHelp, L"&Hilfe");

        SetMenu(hwnd, hMenubar);
        break;
    }
    case WM_SIZE: {
        RECT rc;
        GetClientRect(hwnd, &rc);
        int toolbarHeight = 40;
        int statusHeight = 24;
        MoveWindow(hStatus, 0, rc.bottom - statusHeight, rc.right, statusHeight, TRUE);
        MoveWindow(hLineNums, 6, toolbarHeight, 80, rc.bottom - toolbarHeight - statusHeight, TRUE);
        MoveWindow(hEdit, 92, toolbarHeight, rc.right - 92, rc.bottom - toolbarHeight - statusHeight, TRUE);
        break;
    }
    case WM_COMMAND:
        // Toolbar buttons
        if (LOWORD(wParam) == TB_NEW_BTN) { SendMessageW(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_NEW,0), 0); break; }
        if (LOWORD(wParam) == TB_OPEN_BTN) { SendMessageW(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_OPEN,0), 0); break; }
        if (LOWORD(wParam) == TB_SAVE_BTN) { SendMessageW(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_SAVE,0), 0); break; }
        if (LOWORD(wParam) == TB_BUILD_BTN) { SendMessageW(hwnd, WM_COMMAND, MAKELONG(IDM_BUILD_COMPILE,0), 0); break; }

        // EN_CHANGE from editor
        if (HIWORD(wParam) == EN_CHANGE && (HWND)lParam == hEdit) {
            if (!g_in_highlight) {
                UpdateLineNumbers();
                HighlightLineAtCaret(hEdit);
                UpdateStatusBar();
            }
            break;
        }

        switch (LOWORD(wParam)) {
        case IDM_FILE_NEW:
            SetWindowTextW(hEdit, L"");
            currentFile[0] = 0;
            SendMessageW(hEdit, EM_SETMODIFY, FALSE, 0);
            UpdateStatusBar();
            break;
        case IDM_FILE_OPEN: {
            OPENFILENAMEW ofn; wchar_t szFile[MAX_PATH] = L"";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"C Source\0*.c;*.h\0All Files\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            ofn.lpstrDefExt = L"c";
            if (GetOpenFileNameW(&ofn)) {
                wcscpy_s(currentFile, MAX_PATH, szFile);
                OpenFileToEditor(hwnd, szFile);
                UpdateStatusBar();
            }
            break;
        }
        case IDM_FILE_SAVE: {
            if (!currentFile[0]) {
                OPENFILENAMEW ofn; wchar_t szFile[MAX_PATH] = L"";
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFilter = L"C Source\0*.c;*.h\0All Files\0*.*\0";
                ofn.Flags = OFN_PATHMUSTEXIST;
                ofn.lpstrDefExt = L"c";
                if (GetSaveFileNameW(&ofn)) {
                    wcscpy_s(currentFile, MAX_PATH, szFile);
                    SaveFileAs(hwnd, currentFile, true);
                }
            } else {
                SaveFileAs(hwnd, currentFile, true);
            }
            UpdateStatusBar();
            break;
        }
        case IDM_FILE_SAVEAS: {
            OPENFILENAMEW ofn; wchar_t szFile[MAX_PATH] = L"";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"C Source\0*.c;*.h\0All Files\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST;
            ofn.lpstrDefExt = L"c";
            if (GetSaveFileNameW(&ofn)) {
                wcscpy_s(currentFile, MAX_PATH, szFile);
                SaveFileAs(hwnd, currentFile, true);
            }
            UpdateStatusBar();
            break;
        }
        case IDM_FILE_EXIT:
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
            break;
        case IDM_EDIT_CUT:
            SendMessageW(hEdit, WM_CUT, 0, 0);
            break;
        case IDM_EDIT_COPY:
            SendMessageW(hEdit, WM_COPY, 0, 0);
            break;
        case IDM_EDIT_PASTE:
            SendMessageW(hEdit, WM_PASTE, 0, 0);
            // After paste, re-highlight whole document (paste may change many lines)
            HighlightSyntax(hEdit);
            UpdateLineNumbers();
            UpdateStatusBar();
            break;
        case IDM_BUILD_COMPILE:
            //BuildProject(hwnd);
            break;
        case IDM_HELP_ABOUT:
            MessageBoxW(hwnd, L"Mini Notepad Editor\nC-Quellcodeditor mit Source Code Pro, Zeilennummern, Toolbar und Echtzeit-Highlighting", L"Über", MB_OK | MB_ICONINFORMATION);
            break;
        default:
            break;
        }
        break;
    case WM_CLOSE: {
        int prompt = PromptSaveIfModified(hwnd);
        if (prompt == 0) break;
        DestroyWindow(hwnd);
        break;
    }
    case WM_DESTROY:
        UnloadPrivateFont();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/* WinMain */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    LoadLibraryW(L"Msftedit.dll");

    INITCOMMONCONTROLSEX icex;
    ZeroMemory(&icex, sizeof(icex));
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_BAR_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    /* Require the font file in the EXE directory */
    if (!LoadPrivateFontFromExeDir()) {
        MessageBoxW(NULL,
            L"Die erforderliche Schriftdatei 'SourceCodePro-Regular.ttf' wurde im EXE-Verzeichnis nicht gefunden oder konnte nicht geladen.\n\nBitte kopiere 'SourceCodePro-Regular.ttf' in dasselbe Verzeichnis wie diese EXE und starte das Programm erneut.",
            L"Fehlende Schriftdatei",
            MB_ICONERROR | MB_OK);
        return 1;
    }

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = gClassName;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"Registrierung der Fensterklasse fehlgeschlagen.", L"Fehler", MB_ICONERROR);
        UnloadPrivateFont();
        return 0;
    }

    hMainWnd = CreateWindowExW(0, gClassName, L"Mini Notepad Editor", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700, NULL, NULL, hInstance, NULL);

    if (!hMainWnd) {
        MessageBoxW(NULL, L"Fenster konnte nicht erstellt werden.", L"Fehler", MB_ICONERROR);
        UnloadPrivateFont();
        return 0;
    }

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnloadPrivateFont();
    return (int)msg.wParam;
}
