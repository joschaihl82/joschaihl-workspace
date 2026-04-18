// qterm1.cpp
// Single-file Qt6 terminal emulator (minimal VT100 subset, PTY-backed).
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

// Cell attributes
struct CellAttr {
    uint8_t fg; // 1..16 (0 => default)
    uint8_t bg; // 1..8  (0 => default)
    bool bold;
    bool underline;
    bool blink;
    bool reverse;
    bool invisible;
    CellAttr() : fg(DEF_FG+1), bg(DEF_BG+1), bold(false), underline(false), blink(false), reverse(false), invisible(false) {}
};

struct Cell {
    uint8_t ch; // byte 0..255
    CellAttr a;
    Cell() : ch(' '), a() {}
};

// Minimal parser states
enum ParserState { PS_GROUND, PS_ESC, PS_CSI, PS_OSC };

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
      tweakx(0), tweaky(0)
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

void TerminalWidget::setSGR(const QByteArray &params)
{
    if (params.isEmpty()) {
        currentAttr = CellAttr();
        return;
    }
    QList<QByteArray> parts = params.split(';');
    for (const QByteArray &p : parts) {
        int v = p.toInt();
        if (v == 0) currentAttr = CellAttr();
        else if (v == 1) currentAttr.bold = true;
        else if (v == 4) currentAttr.underline = true;
        else if (v == 5) currentAttr.blink = true;
        else if (v == 7) currentAttr.reverse = true;
        else if (v == 8) currentAttr.invisible = true;
        else if (v >= 30 && v <= 37) currentAttr.fg = (v - 30) + 1;
        else if (v >= 40 && v <= 47) currentAttr.bg = (v - 40) % 8 + 1;
        else if (v >= 90 && v <= 97) currentAttr.fg = (v - 90) + 8 + 1;
        else if (v >= 100 && v <= 107) currentAttr.bg = (v - 100) + 8 + 1;
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
                if (ch == 0x1b) { pstate = PS_ESC; csiBuf.clear(); oscBuf.clear(); }
                else if (ch < 0x20) {
                    if (ch == '\r') putByte('\r');
                    else if (ch == '\n') putByte('\n');
                    else if (ch == '\b') putByte('\b');
                    else if (ch == '\t') putByte('\t');
                } else {
                    putByte(ch);
                }
                break;
            case PS_ESC:
                if (ch == '[') { pstate = PS_CSI; csiBuf.clear(); }
                else if (ch == ']') { pstate = PS_OSC; oscBuf.clear(); }
                else { pstate = PS_GROUND; }
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

    // Draw cells
    bool anyDirty = false;
    if ((int)dirty.size() == rows) {
        for (int y = 0; y < rows; ++y) if (dirty[y]) { anyDirty = true; break; }
    } else {
        anyDirty = true;
    }

    if (!anyDirty) {
        // Nothing marked dirty — still paint everything once to avoid blank screen.
        dirty = QVector<char>(rows, 1);
    }

    for (int y = 0; y < rows; ++y) {
        if (!dirty[y]) continue;
        for (int x = 0; x < cols; ++x) {
            const Cell &cell = screen[y][x];
            int fg = (cell.a.fg ? (cell.a.fg - 1) : DEF_FG);
            int bg = (cell.a.bg ? (cell.a.bg - 1) : DEF_BG);
            QRect dst(x * cellW + tweakx, y * cellH + tweaky, cellW, cellH);
            p.fillRect(dst, bgcolors[bg]);
            p.setPen(colors[fg]);
            // Invisible handling
            if (!cell.a.invisible) {
                // Draw glyph centered vertically
                QString s = QString::fromLatin1(QByteArray(1, (char)cell.ch));
                int tx = dst.x();
                int ty = dst.y() + fm->ascent();
                p.drawText(tx, ty, s);
            }
            if (cell.a.underline) {
                p.fillRect(dst.x(), dst.y() + cellH - 2, cellW, 2, colors[fg]);
            }
        }
        dirty[y] = 0;
    }

    // Cursor
    if (cursorEnabled && cursorVisible) {
        int fg = (screen[cursorY][cursorX].a.fg ? (screen[cursorY][cursorX].a.fg - 1) : DEF_FG);
        QRect cur(cursorX * cellW + tweakx, cursorY * cellH + cellH - 2 + tweaky, cellW, 2);
        p.fillRect(cur, colors[fg]);
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
