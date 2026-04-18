// qterm1.cpp
// Single-file Qt6 terminal emulator (minimal VT100 subset, PTY-backed).
// Extended features:
//  - Alternate screen buffer, private CSI modes
//  - Full 256-color and truecolor (24-bit) SGR support (38;5;n, 48;5;n, 38;2;r;g;b, 48;2;r;g;b)
//  - DEC line/box drawing (ACS) via ESC ( 0  and ESC ( B toggles
// Build: link with Qt6::Widgets
// Example CMakeLists.txt:
// cmake_minimum_required(VERSION 3.16)
// project(qterm1 LANGUAGES CXX)
// find_package(Qt6 REQUIRED COMPONENTS Widgets)
// set(CMAKE_CXX_STANDARD 17)
// set(CMAKE_AUTOMOC ON)
// add_executable(qterm1 qterm1.cpp)
// target_link_libraries(qterm1 PRIVATE Qt6::Widgets)

#include <QtWidgets>
#include <QSocketNotifier>
#include <QTimer>

#include <pty.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// Embedded palette (user-provided)
static uint32_t palette_raw[] = {
  0x171717, 0xa8203d, 0x3da820, 0xa88a20,
  0x203da8, 0x8a20a8, 0x20a88a, 0xbdbdbd,
  0x3b3b3b, 0xeb92a5, 0xa5eb92, 0xebd792,
  0x92a5eb, 0xd792eb, 0x92ebd7, 0xf1f1f1,
};
static uint32_t * bgpalette_raw = palette_raw;

// Defaults
static const char * DEFAULT_TITLE = "Lilt";
static const int DEF_FG = 15;
static const int DEF_BG = 0;

// Cell attributes extended to support palette indices and RGB colors
struct CellAttr {
    // palette indices (0..15 for fg, 0..7 for bg) stored as -1 if not used
    int fg_index; // -1 means not using palette index
    int bg_index; // -1 means not using palette index
    // truecolor flags and values
    bool fg_is_rgb;
    bool bg_is_rgb;
    uint8_t fg_r, fg_g, fg_b;
    uint8_t bg_r, bg_g, bg_b;

    bool bold;
    bool underline;
    bool blink;
    bool reverse;
    bool invisible;

    CellAttr()
      : fg_index(DEF_FG), bg_index(DEF_BG),
        fg_is_rgb(false), bg_is_rgb(false),
        fg_r(0), fg_g(0), fg_b(0),
        bg_r(0), bg_g(0), bg_b(0),
        bold(false), underline(false), blink(false), reverse(false), invisible(false)
    {}
};

// Single cell
struct Cell {
    uint8_t ch; // byte 0..255
    CellAttr a;
    Cell() : ch(' '), a() {}
};

// Minimal parser states
enum ParserState { PS_GROUND, PS_ESC, PS_ESC2, PS_CSI, PS_OSC };

// Terminal widget
class TerminalWidget : public QWidget {
    Q_OBJECT
public:
    TerminalWidget(QWidget *parent = nullptr);
    ~TerminalWidget() override;

    bool spawnPty(const QStringList &cmd = QStringList());
    void setFontFile(const QString &path, int size);

    // Expose a few members/methods for main() convenience
    void allocScreenPublic(int c, int r) { allocScreen(c, r); }
    void setMouseModePublic(int m) { mouseMode = m; }
    int getCellW() const { return cellW; }
    int getCellH() const { return cellH; }

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void inputMethodEvent(QInputMethodEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;

private slots:
    void onPtyReadable();
    void onCursorBlink();

private:
    // Rendering
    QFont mono;
    QFontMetrics *fm;
    int cellW, cellH;
    int cols, rows;
    QVector<QVector<Cell>> screen;
    QVector<char> dirty; // per-line dirty flag

    // Alternate buffer
    QVector<QVector<Cell>> alt_screen;
    bool using_alt;

    // Saved state for DECSC/DECRC and buffer switching
    int saved_cursorX, saved_cursorY;
    CellAttr saved_attr;

    // Palette
    QColor colors[16];
    QColor bgcolors[8];

    // PTY
    int masterFd;
    pid_t childPid;
    QSocketNotifier *notifier;

    // Parser
    ParserState pstate;
    QByteArray csiBuf;
    QByteArray oscBuf;
    CellAttr currentAttr;
    int cursorX, cursorY;
    bool cursorEnabled;
    bool cursorVisible;
    QTimer *blinkTimer;
    int blinkIntervalMs;

    // Mouse
    int mouseMode; // -1 disabled, 9 X10, 1006
    bool mouseMotion;

    // Layout tweaks (center)
    int tweakx;
    int tweaky;

    // ACS (DEC Special Graphics) support
    bool use_acs; // true when G0 is DEC Special Graphics (ESC ( 0)
    QHash<char, QChar> acs_map;

    // ESC two-byte handling
    char esc_prefix; // stores '(' or ')' when waiting for next char

    // Helpers
    void initPalette();
    void allocScreen(int c, int r);
    void markLineDirty(int y);
    void scrollUp(int n);
    void scrollDown(int n);
    void moveCursorTo(int r, int c);
    void clearScreen(int mode);
    void clearLine(int mode);
    void setSGR(const QByteArray &params);
    void putByte(uint8_t b);
    void handleCsi(const QByteArray &seq);
    void handleOsc(const QByteArray &seq);
    void feedParser(const char *buf, qint64 len);
    void updateWindowSize();
    void sendToPty(const QByteArray &data);
    void sendMouseEvent(const QMouseEvent *ev, bool isRelease);

    // New helpers for modes and buffer switching
    void setPrivateMode(int mode);
    void unsetPrivateMode(int mode);
    void saveCursor();
    void restoreCursor();
    void switchToAltBuffer();
    void restoreFromAltBuffer();
    QColor colorFromAnsi256(int idx);
    QColor colorFromCellAttrFg(const CellAttr &a);
    QColor colorFromCellAttrBg(const CellAttr &a);
    void initAcsMap();
};

// Implementation

TerminalWidget::TerminalWidget(QWidget *parent)
    : QWidget(parent),
      fm(nullptr),
      cellW(8), cellH(16),
      cols(80), rows(25),
      masterFd(-1), childPid(-1), notifier(nullptr),
      pstate(PS_GROUND),
      currentAttr(),
      cursorX(0), cursorY(0),
      cursorEnabled(true), cursorVisible(true),
      blinkTimer(nullptr),
      blinkIntervalMs(500),
      mouseMode(-1), mouseMotion(false),
      tweakx(0), tweaky(0),
      using_alt(false),
      saved_cursorX(0), saved_cursorY(0),
      use_acs(false),
      esc_prefix(0)
{
    setWindowTitle(DEFAULT_TITLE);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled, true);

    // Default monospace font
    mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    mono.setStyleHint(QFont::Monospace);
    mono.setPointSize(12);

    fm = new QFontMetrics(mono);
    cellW = fm->horizontalAdvance(QLatin1Char('W'));
    cellH = fm->height();

    initPalette();
    initAcsMap();
    allocScreen(cols, rows);

    // Cursor blink timer
    blinkTimer = new QTimer(this);
    connect(blinkTimer, &QTimer::timeout, this, &TerminalWidget::onCursorBlink);
    if (blinkIntervalMs > 0) blinkTimer->start(blinkIntervalMs);
}

TerminalWidget::~TerminalWidget()
{
    if (notifier) { delete notifier; notifier = nullptr; }
    if (masterFd >= 0) ::close(masterFd);
    if (fm) delete fm;
}

void TerminalWidget::initPalette()
{
    for (int i = 0; i < 16; ++i) {
        uint32_t v = palette_raw[i];
        int r = (v >> 16) & 0xff;
        int g = (v >> 8) & 0xff;
        int b = (v >> 0) & 0xff;
        colors[i] = QColor(r, g, b);
    }
    for (int i = 0; i < 8; ++i) {
        uint32_t v = bgpalette_raw[i];
        int r = (v >> 16) & 0xff;
        int g = (v >> 8) & 0xff;
        int b = (v >> 0) & 0xff;
        // darken background slightly (approx)
        int dr = (r * 90) / 100;
        int dg = (g * 90) / 100;
        int db = (b * 90) / 100;
        bgcolors[i] = QColor(dr, dg, db);
    }
}

void TerminalWidget::initAcsMap()
{
    // Map common ACS characters (ncurses ACS mapping) to Unicode box-drawing / symbols
    acs_map.clear();
    acs_map['l'] = QChar(0x250C); // upper left corner ┌
    acs_map['m'] = QChar(0x2514); // lower left corner └
    acs_map['k'] = QChar(0x2510); // upper right corner ┐
    acs_map['j'] = QChar(0x2518); // lower right corner ┘
    acs_map['q'] = QChar(0x2500); // horizontal line ─
    acs_map['x'] = QChar(0x2502); // vertical line │
    acs_map['t'] = QChar(0x253C); // ┼
    acs_map['u'] = QChar(0x252C); // ┬
    acs_map['v'] = QChar(0x2534); // ┴
    acs_map['w'] = QChar(0x251C); // ├
    acs_map['n'] = QChar(0x2524); // ┤
    acs_map['o'] = QChar(0x23BA); // overline? use horizontal bar
    acs_map['s'] = QChar(0x00B7); // middle dot ·
    acs_map['`'] = QChar(0x00A0); // non-breaking space
    acs_map['a'] = QChar(0x2666); // diamond ♦
    acs_map['f'] = QChar(0x00B0); // degree °
    acs_map['g'] = QChar(0x00B1); // plus/minus ±
    acs_map['~'] = QChar(0x2264); // ≤ (approx)
    // Add more mappings as needed
}

void TerminalWidget::allocScreen(int c, int r)
{
    cols = c;
    rows = r;
    screen.clear();
    screen.resize(rows);
    for (int y = 0; y < rows; ++y) {
        screen[y].resize(cols);
    }
    // Properly allocate and initialize the dirty vector
    dirty = QVector<char>(rows, 1);

    // Also ensure alt buffer has same size
    alt_screen.clear();
    alt_screen.resize(rows);
    for (int y = 0; y < rows; ++y) alt_screen[y].resize(cols);

    cursorX = 0;
    cursorY = 0;
    updateGeometry();
    update();
}

void TerminalWidget::markLineDirty(int y)
{
    if (y < 0 || y >= rows) return;
    if ((int)dirty.size() != rows) {
        // If dirty vector is unexpectedly sized, reinitialize it
        dirty = QVector<char>(rows, 0);
    }
    dirty[y] = 1;
}

void TerminalWidget::scrollUp(int n)
{
    if (n <= 0) return;
    if (n >= rows) {
        for (int y = 0; y < rows; ++y)
            for (int x = 0; x < cols; ++x) screen[y][x].ch = ' ';
    } else {
        for (int y = 0; y < rows - n; ++y)
            screen[y] = screen[y + n];
        for (int y = rows - n; y < rows; ++y)
            for (int x = 0; x < cols; ++x) screen[y][x].ch = ' ';
    }
    for (int y = 0; y < rows; ++y) markLineDirty(y);
}

void TerminalWidget::scrollDown(int n)
{
    if (n <= 0) return;
    if (n >= rows) {
        for (int y = 0; y < rows; ++y)
            for (int x = 0; x < cols; ++x) screen[y][x].ch = ' ';
    } else {
        for (int y = rows - 1; y >= n; --y)
            screen[y] = screen[y - n];
        for (int y = 0; y < n; ++y)
            for (int x = 0; x < cols; ++x) screen[y][x].ch = ' ';
    }
    for (int y = 0; y < rows; ++y) markLineDirty(y);
}

void TerminalWidget::moveCursorTo(int r, int c)
{
    if (r <= 0) r = 1;
    if (c <= 0) c = 1;
    cursorY = qBound(0, r - 1, rows - 1);
    cursorX = qBound(0, c - 1, cols - 1);
}

void TerminalWidget::clearScreen(int mode)
{
    if (mode == 0) {
        for (int y = cursorY; y < rows; ++y) {
            int start = (y == cursorY) ? cursorX : 0;
            for (int x = start; x < cols; ++x) screen[y][x].ch = ' ';
            markLineDirty(y);
        }
    } else if (mode == 1) {
        for (int y = 0; y <= cursorY; ++y) {
            int end = (y == cursorY) ? cursorX : cols - 1;
            for (int x = 0; x <= end; ++x) screen[y][x].ch = ' ';
            markLineDirty(y);
        }
    } else {
        for (int y = 0; y < rows; ++y)
            for (int x = 0; x < cols; ++x) screen[y][x].ch = ' ';
        for (int y = 0; y < rows; ++y) markLineDirty(y);
    }
}

void TerminalWidget::clearLine(int mode)
{
    if (mode == 0) {
        for (int x = cursorX; x < cols; ++x) screen[cursorY][x].ch = ' ';
    } else if (mode == 1) {
        for (int x = 0; x <= cursorX; ++x) screen[cursorY][x].ch = ' ';
    } else {
        for (int x = 0; x < cols; ++x) screen[cursorY][x].ch = ' ';
    }
    markLineDirty(cursorY);
}

// Map a 256-color index to QColor
QColor TerminalWidget::colorFromAnsi256(int idx)
{
    if (idx < 0) return QColor(0,0,0);
    if (idx < 16) {
        uint32_t v = palette_raw[idx];
        return QColor((v>>16)&0xff, (v>>8)&0xff, v&0xff);
    }
    if (idx >= 16 && idx <= 231) {
        int ci = idx - 16;
        int r = ci / 36;
        int g = (ci / 6) % 6;
        int b = ci % 6;
        auto conv = [](int v)->int { return v == 0 ? 0 : 55 + v * 40; };
        return QColor(conv(r), conv(g), conv(b));
    }
    if (idx >= 232 && idx <= 255) {
        int gray = 8 + (idx - 232) * 10;
        return QColor(gray, gray, gray);
    }
    return QColor(0,0,0);
}

QColor TerminalWidget::colorFromCellAttrFg(const CellAttr &a)
{
    if (a.fg_is_rgb) return QColor(a.fg_r, a.fg_g, a.fg_b);
    if (a.fg_index >= 0 && a.fg_index < 16) {
        return colors[a.fg_index];
    }
    return colors[DEF_FG];
}

QColor TerminalWidget::colorFromCellAttrBg(const CellAttr &a)
{
    if (a.bg_is_rgb) return QColor(a.bg_r, a.bg_g, a.bg_b);
    if (a.bg_index >= 0 && a.bg_index < 8) {
        return bgcolors[a.bg_index];
    }
    // fallback: use background palette index 0
    return bgcolors[DEF_BG];
}

void TerminalWidget::setSGR(const QByteArray &params)
{
    if (params.isEmpty()) {
        currentAttr = CellAttr();
        return;
    }

    // Support sequences like "38;5;n", "48;5;n", "38;2;r;g;b", "48;2;r;g;b"
    QList<QByteArray> parts = params.split(';');
    int i = 0;
    while (i < parts.size()) {
        int v = parts[i].toInt();
        if (v == 0) { currentAttr = CellAttr(); i++; }
        else if (v == 1) { currentAttr.bold = true; i++; }
        else if (v == 4) { currentAttr.underline = true; i++; }
        else if (v == 5) { currentAttr.blink = true; i++; }
        else if (v == 7) { currentAttr.reverse = true; i++; }
        else if (v == 8) { currentAttr.invisible = true; i++; }
        else if (v >= 30 && v <= 37) { currentAttr.fg_is_rgb = false; currentAttr.fg_index = (v - 30); i++; }
        else if (v >= 40 && v <= 47) { currentAttr.bg_is_rgb = false; currentAttr.bg_index = (v - 40) % 8; i++; }
        else if (v >= 90 && v <= 97) { currentAttr.fg_is_rgb = false; currentAttr.fg_index = (v - 90) + 8; i++; }
        else if (v >= 100 && v <= 107) { currentAttr.bg_is_rgb = false; currentAttr.bg_index = (v - 100) + 8; i++; }
        else if (v == 38 || v == 48) {
            bool isFg = (v == 38);
            i++;
            if (i >= parts.size()) break;
            int mode = parts[i].toInt();
            if (mode == 5) {
                // 256-color index
                i++;
                if (i >= parts.size()) break;
                int idx = parts[i].toInt();
                QColor c = colorFromAnsi256(idx);
                if (isFg) {
                    currentAttr.fg_is_rgb = true;
                    currentAttr.fg_r = c.red(); currentAttr.fg_g = c.green(); currentAttr.fg_b = c.blue();
                } else {
                    currentAttr.bg_is_rgb = true;
                    currentAttr.bg_r = c.red(); currentAttr.bg_g = c.green(); currentAttr.bg_b = c.blue();
                }
                i++;
            } else if (mode == 2) {
                // truecolor r;g;b
                if (i + 3 <= parts.size()) {
                    int r = parts[i+1].toInt();
                    int g = parts[i+2].toInt();
                    int b = parts[i+3].toInt();
                    if (isFg) {
                        currentAttr.fg_is_rgb = true;
                        currentAttr.fg_r = (uint8_t)r; currentAttr.fg_g = (uint8_t)g; currentAttr.fg_b = (uint8_t)b;
                    } else {
                        currentAttr.bg_is_rgb = true;
                        currentAttr.bg_r = (uint8_t)r; currentAttr.bg_g = (uint8_t)g; currentAttr.bg_b = (uint8_t)b;
                    }
                    i += 4;
                } else {
                    // malformed, break
                    break;
                }
            } else {
                // unknown mode, skip
                i++;
            }
        } else {
            // unknown param, skip
            i++;
        }
    }
}

void TerminalWidget::putByte(uint8_t b)
{
    if (b == '\r') { cursorX = 0; return; }
    if (b == '\n') { cursorX = 0; cursorY++; if (cursorY >= rows) { scrollUp(1); cursorY = rows - 1; } return; }
    if (b == '\b') { if (cursorX > 0) cursorX--; return; }
    if (b == '\t') { int next = ((cursorX / 8) + 1) * 8; if (next >= cols) next = cols - 1; cursorX = next; return; }

    if (cursorX >= cols) { cursorX = 0; cursorY++; if (cursorY >= rows) { scrollUp(1); cursorY = rows - 1; } }
    Cell &cell = screen[cursorY][cursorX];
    cell.ch = b;
    cell.a = currentAttr;
    markLineDirty(cursorY);
    cursorX++;
    if (cursorX >= cols) { cursorX = 0; cursorY++; if (cursorY >= rows) { scrollUp(1); cursorY = rows - 1; } }
}

void TerminalWidget::handleCsi(const QByteArray &seq)
{
    if (seq.isEmpty()) return;

    // Detect private-mode sequences starting with '?'
    bool is_private = (seq.size() > 0 && seq[0] == '?');
    if (is_private) {
        char final = seq.constData()[seq.size()-1];
        QByteArray params = seq.mid(1, seq.size()-2); // between '?' and final
        if (final == 'h') {
            QList<QByteArray> parts = params.split(';');
            for (const QByteArray &p : parts) {
                int mode = p.toInt();
                setPrivateMode(mode);
            }
            return;
        } else if (final == 'l') {
            QList<QByteArray> parts = params.split(';');
            for (const QByteArray &p : parts) {
                int mode = p.toInt();
                unsetPrivateMode(mode);
            }
            return;
        }
        // else fall through
    }

    char final = seq.constData()[seq.size()-1];
    QByteArray params = seq.left(seq.size()-1);
    if (final == 'A') { int n = params.toInt(); if (n <= 0) n = 1; cursorY = qMax(0, cursorY - n); }
    else if (final == 'B') { int n = params.toInt(); if (n <= 0) n = 1; cursorY = qMin(rows-1, cursorY + n); }
    else if (final == 'C') { int n = params.toInt(); if (n <= 0) n = 1; cursorX = qMin(cols-1, cursorX + n); }
    else if (final == 'D') { int n = params.toInt(); if (n <= 0) n = 1; cursorX = qMax(0, cursorX - n); }
    else if (final == 'H' || final == 'f') {
        int r = 1, c = 1;
        if (!params.isEmpty()) {
            QList<QByteArray> parts = params.split(';');
            if (parts.size() >= 1) r = parts[0].toInt();
            if (parts.size() >= 2) c = parts[1].toInt();
        }
        moveCursorTo(r, c);
    }
    else if (final == 'J') { int mode = params.toInt(); clearScreen(mode); }
    else if (final == 'K') { int mode = params.toInt(); clearLine(mode); }
    else if (final == 'm') { setSGR(params); }
    markLineDirty(cursorY);
}

void TerminalWidget::handleOsc(const QByteArray &seq)
{
    if (seq.startsWith("0;") || seq.startsWith("2;")) {
        QByteArray title = seq.mid(2);
        if (!title.isEmpty()) setWindowTitle(QString::fromUtf8(title));
    }
}

void TerminalWidget::feedParser(const char *buf, qint64 len)
{
    for (qint64 i = 0; i < len; ++i) {
        uint8_t ch = (uint8_t)buf[i];
        switch (pstate) {
            case PS_GROUND:
                if (ch == 0x1b) { pstate = PS_ESC; csiBuf.clear(); oscBuf.clear(); esc_prefix = 0; }
                else if (ch < 0x20) {
                    if (ch == '\r') putByte('\r');
                    else if (ch == '\n') putByte('\n');
                    else if (ch == '\b') putByte('\b');
                    else if (ch == '\t') putByte('\t');
                } else {
                    // If ACS is active, map certain characters to box-drawing
                    if (use_acs) {
                        // map only when drawing; store original byte but we will map at render time
                        putByte(ch);
                    } else {
                        putByte(ch);
                    }
                }
                break;
            case PS_ESC:
                if (ch == '[') { pstate = PS_CSI; csiBuf.clear(); }
                else if (ch == ']') { pstate = PS_OSC; oscBuf.clear(); }
                else if (ch == '(' || ch == ')') {
                    // Expect next char to select charset
                    esc_prefix = (char)ch;
                    pstate = PS_ESC2;
                }
                else if (ch == '7') { // DECSC - save cursor
                    saveCursor();
                    pstate = PS_GROUND;
                }
                else if (ch == '8') { // DECRC - restore cursor
                    restoreCursor();
                    pstate = PS_GROUND;
                }
                else {
                    // unsupported single-char escape; ignore
                    pstate = PS_GROUND;
                }
                break;
            case PS_ESC2:
                // handle charset selection after ESC ( or ESC )
                if (esc_prefix == '(') {
                    // G0 selection
                    if (ch == '0') {
                        // DEC Special Graphics
                        use_acs = true;
                    } else if (ch == 'B') {
                        // ASCII
                        use_acs = false;
                    } else {
                        // other charsets ignored
                    }
                } else if (esc_prefix == ')') {
                    // G1 selection (not used here)
                    if (ch == '0') {
                        // DEC Special Graphics in G1 (rare)
                        // We'll ignore G1 handling for simplicity
                    } else if (ch == 'B') {
                        // ASCII
                    }
                }
                esc_prefix = 0;
                pstate = PS_GROUND;
                break;
            case PS_CSI:
                csiBuf.append((char)ch);
                if (ch >= 0x40 && ch <= 0x7E) {
                    handleCsi(csiBuf);
                    pstate = PS_GROUND;
                }
                break;
            case PS_OSC:
                if (ch == 0x07) {
                    handleOsc(oscBuf);
                    pstate = PS_GROUND;
                } else if (ch == 0x1b) {
                    oscBuf.append((char)ch);
                } else if (ch == '\\' && !oscBuf.isEmpty() && oscBuf.endsWith('\x1b')) {
                    oscBuf.chop(1);
                    handleOsc(oscBuf);
                    pstate = PS_GROUND;
                } else {
                    oscBuf.append((char)ch);
                }
                break;
        }
    }
}

void TerminalWidget::paintEvent(QPaintEvent * /*ev*/)
{
    QPainter p(this);
    p.setFont(mono);

    // Background
    p.fillRect(rect(), bgcolors[DEF_BG]);

    // Draw all rows unconditionally (robust)
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            const Cell &cell = screen[y][x];

            // Determine foreground and background colors
            QColor fgcol = colorFromCellAttrFg(cell.a);
            QColor bgcol = colorFromCellAttrBg(cell.a);

            QRect dst(x * cellW + tweakx, y * cellH + tweaky, cellW, cellH);
            p.fillRect(dst, bgcol);
            p.setPen(fgcol);

            // Determine glyph to draw: if ACS active and mapping exists, map to Unicode
            QChar glyph;
            if (use_acs && acs_map.contains((char)cell.ch)) {
                glyph = acs_map[(char)cell.ch];
            } else {
                glyph = QChar::fromLatin1((char)cell.ch);
            }

            if (!cell.a.invisible) {
                int tx = dst.x();
                int ty = dst.y() + fm->ascent();
                p.drawText(tx, ty, QString(glyph));
            }
            if (cell.a.underline) {
                p.fillRect(dst.x(), dst.y() + cellH - 2, cellW, 2, fgcol);
            }
        }
        if ((int)dirty.size() == rows) dirty[y] = 0;
    }

    // Cursor
    if (cursorEnabled && cursorVisible) {
        // Use current cell fg color for cursor
        QColor curcol = colorFromCellAttrFg(screen[cursorY][cursorX].a);
        QRect cur(cursorX * cellW + tweakx, cursorY * cellH + cellH - 2 + tweaky, cellW, 2);
        p.fillRect(cur, curcol);
    }
}

void TerminalWidget::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);
    int w = ev->size().width();
    int h = ev->size().height();
    int newCols = qMax(5, w / cellW);
    int newRows = qMax(5, h / cellH);
    allocScreen(newCols, newRows);
    tweakx = (w % cellW) / 2;
    tweaky = (h % cellH) / 2;
    updateWindowSize();
}

void TerminalWidget::keyPressEvent(QKeyEvent *ev)
{
    // Special keys -> sequences
    QByteArray seq;
    switch (ev->key()) {
        case Qt::Key_Up: seq = "\x1b[A"; break;
        case Qt::Key_Down: seq = "\x1b[B"; break;
        case Qt::Key_Right: seq = "\x1b[C"; break;
        case Qt::Key_Left: seq = "\x1b[D"; break;
        case Qt::Key_Home: seq = "\x1b[H"; break;
        case Qt::Key_End: seq = "\x1b[F"; break;
        case Qt::Key_Backspace: seq = "\x7f"; break;
        case Qt::Key_Return: seq = "\r"; break;
        case Qt::Key_Tab: seq = "\t"; break;
        case Qt::Key_Escape: seq = "\x1b"; break;
        default:
            // For printable text, send the text() (UTF-8)
            if (!ev->text().isEmpty()) {
                QByteArray utf = ev->text().toUtf8();
                sendToPty(utf);
            }
            return;
    }
    if (!seq.isEmpty()) sendToPty(seq);
}

void TerminalWidget::inputMethodEvent(QInputMethodEvent *ev)
{
    if (!ev->commitString().isEmpty()) {
        sendToPty(ev->commitString().toUtf8());
    } else {
        QWidget::inputMethodEvent(ev);
    }
}

void TerminalWidget::mousePressEvent(QMouseEvent *ev)
{
    if (mouseMode != -1) sendMouseEvent(ev, false);
}

void TerminalWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if (mouseMode != -1) sendMouseEvent(ev, true);
}

void TerminalWidget::mouseMoveEvent(QMouseEvent *ev)
{
    if (mouseMode != -1 && mouseMotion) sendMouseEvent(ev, false);
}

void TerminalWidget::sendMouseEvent(const QMouseEvent *ev, bool isRelease)
{
    int x = (int)ev->position().x() - tweakx;
    int y = (int)ev->position().y() - tweaky;
    x = x / cellW;
    y = y / cellH;
    x = qBound(0, x, cols - 1);
    y = qBound(0, y, rows - 1);

    QByteArray buf;
    if (mouseMode == 9) {
        int button = 0;
        if (ev->type() == QEvent::MouseButtonPress) {
            if (ev->button() == Qt::LeftButton) button = 0;
            else if (ev->button() == Qt::MiddleButton) button = 1;
            else if (ev->button() == Qt::RightButton) button = 2;
        }
        char b1 = (char)(button + 32);
        char bx = (char)(x + 1 + 32);
        char by = (char)(y + 1 + 32);
        buf.append("\x1b[M");
        buf.append(b1);
        buf.append(bx);
        buf.append(by);
    } else if (mouseMode == 1006) {
        int button = 0;
        if (ev->type() == QEvent::MouseButtonPress) {
            if (ev->button() == Qt::LeftButton) button = 0;
            else if (ev->button() == Qt::MiddleButton) button = 1;
            else if (ev->button() == Qt::RightButton) button = 2;
        }
        char c = isRelease ? 'm' : 'M';
        QString s = QString("\x1b[<%1;%2;%3%4").arg(button).arg(x+1).arg(y+1).arg(QChar(c));
        buf = s.toUtf8();
    }
    if (!buf.isEmpty()) sendToPty(buf);
}

void TerminalWidget::onPtyReadable()
{
    if (masterFd < 0) return;
    char buf[4096];
    ssize_t n = ::read(masterFd, buf, sizeof(buf));
    if (n > 0) {
        feedParser(buf, n);
        update();
    } else if (n == 0) {
        // EOF: child exited
        ::close(masterFd);
        masterFd = -1;
        if (notifier) { delete notifier; notifier = nullptr; }
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
    }
}

void TerminalWidget::onCursorBlink()
{
    cursorVisible = !cursorVisible;
    markLineDirty(cursorY);
    update();
}

void TerminalWidget::updateWindowSize()
{
    if (masterFd < 0) return;
    struct winsize ws;
    ws.ws_row = rows;
    ws.ws_col = cols;
    ws.ws_xpixel = cols * cellW;
    ws.ws_ypixel = rows * cellH;
    ioctl(masterFd, TIOCSWINSZ, &ws);
}

void TerminalWidget::sendToPty(const QByteArray &data)
{
    if (masterFd < 0) return;
    ssize_t w = ::write(masterFd, data.constData(), data.size());
    (void)w;
}

bool TerminalWidget::spawnPty(const QStringList &cmd)
{
    // Use forkpty to create pty and child
    int master = -1;
    pid_t pid = forkpty(&master, nullptr, nullptr, nullptr);
    if (pid < 0) return false;
    if (pid == 0) {
        // Child: exec shell or provided command
        if (!cmd.isEmpty()) {
            QVector<char*> argv;
            for (const QString &s : cmd) argv.push_back(strdup(s.toLocal8Bit().constData()));
            argv.push_back(nullptr);
            execvp(argv[0], argv.data());
            _exit(1);
        } else {
            const char *shell = getenv("SHELL");
            if (!shell) shell = "/bin/sh";
            execlp(shell, shell, (char*)nullptr);
            _exit(1);
        }
    }
    // Parent
    masterFd = master;
    childPid = pid;
    // Non-blocking
    int flags = fcntl(masterFd, F_GETFL, 0);
    fcntl(masterFd, F_SETFL, flags | O_NONBLOCK);
    // QSocketNotifier
    notifier = new QSocketNotifier(masterFd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &TerminalWidget::onPtyReadable);
    updateWindowSize();
    return true;
}

void TerminalWidget::setFontFile(const QString &path, int size)
{
    QFont f;
    if (!path.isEmpty()) {
        int id = QFontDatabase::addApplicationFont(path);
        if (id >= 0) {
            QString family = QFontDatabase::applicationFontFamilies(id).value(0);
            f = QFont(family);
        } else {
            f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        }
    } else {
        f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    }
    f.setPointSize(size);
    f.setStyleHint(QFont::Monospace);
    mono = f;
    if (fm) delete fm;
    fm = new QFontMetrics(mono);
    cellW = fm->horizontalAdvance(QLatin1Char('W'));
    cellH = fm->height();
    allocScreen(cols, rows);
    update();
}

// Private-mode handling (CSI ? Pm h/l)
void TerminalWidget::setPrivateMode(int mode)
{
    switch (mode) {
        case 25: // show cursor
            cursorEnabled = true;
            markLineDirty(cursorY);
            break;
        case 1000: // X10 mouse
            mouseMode = 9;
            break;
        case 1002: // mouse-motion (drag)
            mouseMotion = true;
            break;
        case 1006: // SGR mouse
            mouseMode = 1006;
            break;
        case 1049: // alt buffer with cursor save/restore
        case 47:
        case 1047:
        case 1048:
            // Save cursor and switch to alt buffer
            saveCursor();
            switchToAltBuffer();
            break;
        default:
            // ignore unknown
            break;
    }
}

void TerminalWidget::unsetPrivateMode(int mode)
{
    switch (mode) {
        case 25: // hide cursor
            cursorEnabled = false;
            markLineDirty(cursorY);
            break;
        case 1000:
            if (mouseMode == 9) mouseMode = -1;
            break;
        case 1002:
            mouseMotion = false;
            break;
        case 1006:
            if (mouseMode == 1006) mouseMode = -1;
            break;
        case 1049:
        case 47:
        case 1047:
        case 1048:
            // restore buffer and cursor
            restoreFromAltBuffer();
            restoreCursor();
            break;
        default:
            break;
    }
}

void TerminalWidget::saveCursor()
{
    saved_cursorX = cursorX;
    saved_cursorY = cursorY;
    saved_attr = currentAttr;
}

void TerminalWidget::restoreCursor()
{
    cursorX = saved_cursorX;
    cursorY = saved_cursorY;
    currentAttr = saved_attr;
    markLineDirty(cursorY);
}

void TerminalWidget::switchToAltBuffer()
{
    if (using_alt) return;
    // Save main screen into alt_screen and clear main screen
    alt_screen = screen;
    // Clear main screen
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x) screen[y][x].ch = ' ';
    using_alt = true;
    // Reset cursor
    cursorX = 0; cursorY = 0;
    markLineDirty(cursorY);
    update();
}

void TerminalWidget::restoreFromAltBuffer()
{
    if (!using_alt) return;
    // Restore saved main screen
    if (!alt_screen.isEmpty()) {
        screen = alt_screen;
    }
    alt_screen.clear();
    using_alt = false;
    markLineDirty(cursorY);
    update();
}

// main
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Parse simple args: -F font -S size -d colsxrows -t title -m -M
    QString fontPath;
    int fontSize = 12;
    int colsArg = 80, rowsArg = 25;
    QString title = QString::fromUtf8(DEFAULT_TITLE);
    bool enableX10 = false, enable1006 = false;
    QStringList cmd;

    for (int i = 1; i < argc; ++i) {
        QString a = QString::fromLocal8Bit(argv[i]);
        if (a == "-F" && i+1 < argc) { fontPath = argv[++i]; }
        else if (a == "-S" && i+1 < argc) { fontSize = QString::fromLocal8Bit(argv[++i]).toInt(); }
        else if (a == "-d" && i+1 < argc) {
            QString d = QString::fromLocal8Bit(argv[++i]);
            QStringList parts = d.split('x');
            if (parts.size() == 2) { colsArg = parts[0].toInt(); rowsArg = parts[1].toInt(); }
        }
        else if (a == "-t" && i+1 < argc) { title = QString::fromLocal8Bit(argv[++i]); }
        else if (a == "-m") enableX10 = true;
        else if (a == "-M") enable1006 = true;
        else {
            // treat remaining as command
            for (int j = i; j < argc; ++j) cmd << QString::fromLocal8Bit(argv[j]);
            break;
        }
    }

    TerminalWidget w;
    w.setWindowTitle(title);
    w.setFontFile(fontPath, fontSize);
    w.allocScreenPublic(colsArg, rowsArg);
    if (enableX10) w.setMouseModePublic(9);
    if (enable1006) w.setMouseModePublic(1006);
    if (!cmd.isEmpty()) w.spawnPty(cmd);
    else w.spawnPty();

    w.resize(colsArg * w.getCellW(), rowsArg * w.getCellH());
    w.show();

    return app.exec();
}

#include "qterm1.moc"
