// qterm1.cpp
// Qt6 single-file terminal emulator (PTY-backed) with:
//  - ANSI/VT100 subset (CSI, OSC, private modes, alternate buffer)
//  - Full 256-color and truecolor SGR (38/48 ; 5;n and 2;r;g;b)
//  - DEC ACS (box/line drawing) support
//  - Vertical scrollbar with scrollback buffer
//  - Mouse/text selection and copy to system clipboard
//
// Build: link with Qt6 Widgets (CMakeLists.txt shown in earlier messages)

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
#include <deque>

// ----------------------------- Palette ------------------------------------
static uint32_t palette_raw[] = {
  0x171717, 0xa8203d, 0x3da820, 0xa88a20,
  0x203da8, 0x8a20a8, 0x20a88a, 0xbdbdbd,
  0x3b3b3b, 0xeb92a5, 0xa5eb92, 0xebd792,
  0x92a5eb, 0xd792eb, 0x92ebd7, 0xf1f1f1,
};
static uint32_t * bgpalette_raw = palette_raw;

static const char * DEFAULT_TITLE = "Lilt";
static const int DEF_FG = 15;
static const int DEF_BG = 0;

// ----------------------------- Cell types ---------------------------------
struct CellAttr {
    int fg_index; // 0..15
    int bg_index; // 0..7
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
        bold(false), underline(false), blink(false), reverse(false), invisible(false) {}
};

struct Cell {
    uint8_t ch;
    CellAttr a;
    Cell() : ch(' '), a() {}
};

// ----------------------------- Parser states ------------------------------
enum ParserState { PS_GROUND, PS_ESC, PS_ESC2, PS_CSI, PS_OSC };

// ----------------------------- TerminalWidget -----------------------------
class TerminalWidget : public QWidget {
    Q_OBJECT
public:
    TerminalWidget(QWidget *parent = nullptr);
    ~TerminalWidget() override;

    bool spawnPty(const QStringList &cmd = QStringList());
    void setFontFile(const QString &path, int size);

    // small helpers for main
    void allocScreenPublic(int c, int r) { allocScreen(c, r); }
    void setMouseModePublic(int m) { mouseMode = m; }
    int getCellW() const { return cellW; }
    int getCellH() const { return cellH; }

    // make scrollbar public as requested
    QScrollBar *vscroll;

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void inputMethodEvent(QInputMethodEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void wheelEvent(QWheelEvent *ev) override;

private slots:
    void onPtyReadable();
    void onCursorBlink();
    void onScrollBarChanged(int value);

private:
    // rendering
    QFont mono;
    QFontMetrics *fm;
    int cellW, cellH;
    int cols, rows;
    QVector<QVector<Cell>> screen;     // visible buffer
    QVector<char> dirty;               // per-line dirty flag

    // scrollback/history
    std::deque<QVector<Cell>> history; // each entry is a full row
    int history_limit = 10000;         // max lines to keep
    int scroll_offset;                 // 0 = bottom (live), >0 = scrolled up N lines

    // alternate buffer
    QVector<QVector<Cell>> alt_screen;
    bool using_alt;

    // saved state for DECSC/DECRC and buffer switching
    int saved_cursorX, saved_cursorY;
    CellAttr saved_attr;

    // palette
    QColor colors[16];
    QColor bgcolors[8];

    // pty
    int masterFd;
    pid_t childPid;
    QSocketNotifier *notifier;

    // parser
    ParserState pstate;
    QByteArray csiBuf;
    QByteArray oscBuf;
    CellAttr currentAttr;
    int cursorX, cursorY;
    bool cursorEnabled;
    bool cursorVisible;
    QTimer *blinkTimer;
    int blinkIntervalMs;

    // mouse
    int mouseMode; // -1 disabled, 9 X10, 1006
    bool mouseMotion;

    // layout tweaks
    int tweakx;
    int tweaky;

    // ACS (DEC Special Graphics)
    bool use_acs;
    QHash<char, QChar> acs_map;

    // ESC two-byte handling
    char esc_prefix;

    // selection
    bool selecting;
    QPoint sel_start; // (bufferLine, col)
    QPoint sel_end;
    bool selection_has_content;

    // helpers
    void initPalette();
    void initAcsMap();
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

    // modes & buffer switching
    void setPrivateMode(int mode);
    void unsetPrivateMode(int mode);
    void saveCursor();
    void restoreCursor();
    void switchToAltBuffer();
    void restoreFromAltBuffer();

    // color helpers
    QColor colorFromAnsi256(int idx);
    QColor colorFromCellAttrFg(const CellAttr &a);
    QColor colorFromCellAttrBg(const CellAttr &a);

    // history helpers
    void pushHistoryRow(const QVector<Cell> &row);
    int totalBufferLines() const; // history.size() + rows
    int visibleRowToBufferIndex(int y) const;

    // selection helpers
    void startSelectionAtBufferPos(int bufferLine, int col);
    void updateSelectionToBufferPos(int bufferLine, int col);
    QString getSelectionText() const;
    void copySelectionToClipboard();

    // utility
    static QString cellToString(const Cell &c, bool use_acs, const QHash<char,QChar> &acs_map);
};

// ----------------------------- Implementation ------------------------------
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
      esc_prefix(0),
      selecting(false),
      selection_has_content(false),
      vscroll(nullptr),
      scroll_offset(0)
{
    setWindowTitle(DEFAULT_TITLE);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled, true);

    mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    mono.setStyleHint(QFont::Monospace);
    mono.setPointSize(12);

    fm = new QFontMetrics(mono);
    cellW = fm->horizontalAdvance(QLatin1Char('W'));
    cellH = fm->height();

    initPalette();
    initAcsMap();
    allocScreen(cols, rows);

    // vertical scrollbar (public member)
    vscroll = new QScrollBar(Qt::Vertical, this);
    vscroll->setRange(0, 0);
    vscroll->setPageStep(rows);
    connect(vscroll, &QScrollBar::valueChanged, this, &TerminalWidget::onScrollBarChanged);

    // cursor blink
    blinkTimer = new QTimer(this);
    connect(blinkTimer, &QTimer::timeout, this, &TerminalWidget::onCursorBlink);
    if (blinkIntervalMs > 0) blinkTimer->start(blinkIntervalMs);
}

TerminalWidget::~TerminalWidget()
{
    if (notifier) delete notifier;
    if (masterFd >= 0) ::close(masterFd);
    if (fm) delete fm;
}

// ----------------------------- Palette & ACS -------------------------------
void TerminalWidget::initPalette()
{
    for (int i = 0; i < 16; ++i) {
        uint32_t v = palette_raw[i];
        colors[i] = QColor((v>>16)&0xff, (v>>8)&0xff, v&0xff);
    }
    for (int i = 0; i < 8; ++i) {
        uint32_t v = bgpalette_raw[i];
        int r = (v>>16)&0xff, g = (v>>8)&0xff, b = v&0xff;
        bgcolors[i] = QColor((r*90)/100, (g*90)/100, (b*90)/100);
    }
}

void TerminalWidget::initAcsMap()
{
    acs_map.clear();
    acs_map['l'] = QChar(0x250C); // ┌
    acs_map['m'] = QChar(0x2514); // └
    acs_map['k'] = QChar(0x2510); // ┐
    acs_map['j'] = QChar(0x2518); // ┘
    acs_map['q'] = QChar(0x2500); // ─
    acs_map['x'] = QChar(0x2502); // │
    acs_map['t'] = QChar(0x253C); // ┼
    acs_map['u'] = QChar(0x252C); // ┬
    acs_map['v'] = QChar(0x2534); // ┴
    acs_map['w'] = QChar(0x251C); // ├
    acs_map['n'] = QChar(0x2524); // ┤
    acs_map['s'] = QChar(0x00B7); // ·
    acs_map['f'] = QChar(0x00B0); // °
    acs_map['g'] = QChar(0x00B1); // ±
    acs_map['a'] = QChar(0x2666); // ♦
}

// ----------------------------- Screen & History ---------------------------
void TerminalWidget::allocScreen(int c, int r)
{
    cols = c; rows = r;
    screen.clear();
    screen.resize(rows);
    for (int y = 0; y < rows; ++y) screen[y].resize(cols);
    dirty = QVector<char>(rows, 1);

    alt_screen.clear();
    alt_screen.resize(rows);
    for (int y = 0; y < rows; ++y) alt_screen[y].resize(cols);

    cursorX = 0; cursorY = 0;
    scroll_offset = 0;
    if (vscroll) {
        vscroll->setPageStep(rows);
        vscroll->setRange(0, (int)history.size());
        vscroll->setValue(0);
    }
    updateGeometry();
    update();
}

void TerminalWidget::pushHistoryRow(const QVector<Cell> &row)
{
    if ((int)history.size() >= history_limit) history.pop_front();
    history.push_back(row);
    if (vscroll) {
        int maxScroll = qMax(0, (int)history.size());
        vscroll->setRange(0, maxScroll);
        if (scroll_offset == 0) vscroll->setValue(0);
        else vscroll->setValue(scroll_offset);
    }
}

int TerminalWidget::totalBufferLines() const
{
    return (int)history.size() + rows;
}

int TerminalWidget::visibleRowToBufferIndex(int y) const
{
    int bottomIndex = (int)history.size() + (rows - 1);
    return bottomIndex - y - scroll_offset;
}

// ----------------------------- Drawing ------------------------------------
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
    if (a.fg_index >= 0 && a.fg_index < 16) return colors[a.fg_index];
    return colors[DEF_FG];
}

QColor TerminalWidget::colorFromCellAttrBg(const CellAttr &a)
{
    if (a.bg_is_rgb) return QColor(a.bg_r, a.bg_g, a.bg_b);
    if (a.bg_index >= 0 && a.bg_index < 8) return bgcolors[a.bg_index];
    return bgcolors[DEF_BG];
}

QString TerminalWidget::cellToString(const Cell &c, bool use_acs, const QHash<char,QChar> &acs_map)
{
    if (use_acs && acs_map.contains((char)c.ch)) {
        return QString(acs_map[(char)c.ch]);
    } else {
        return QString::fromLatin1(QByteArray(1, (char)c.ch));
    }
}

void TerminalWidget::paintEvent(QPaintEvent * /*ev*/)
{
    QPainter p(this);
    p.setFont(mono);

    p.fillRect(rect(), bgcolors[DEF_BG]);

    int sbw = vscroll ? vscroll->sizeHint().width() : 0;
    if (vscroll) vscroll->setGeometry(width() - sbw, 0, sbw, height());
    int drawWidth = width() - sbw;

    for (int y = 0; y < rows; ++y) {
        int bufIndex = visibleRowToBufferIndex(y);
        QRect dst(0 + tweakx, y * cellH + tweaky, drawWidth, cellH);
        p.fillRect(dst, bgcolors[DEF_BG]);

        if (bufIndex < 0 || bufIndex >= totalBufferLines()) continue;

        QVector<Cell> rowCells;
        if (bufIndex < (int)history.size()) rowCells = history[bufIndex];
        else rowCells = screen[bufIndex - (int)history.size()];

        for (int x = 0; x < cols; ++x) {
            const Cell &cell = rowCells[x];
            QColor fg = colorFromCellAttrFg(cell.a);
            QColor bg = colorFromCellAttrBg(cell.a);
            QRect cellRect(x * cellW + tweakx, y * cellH + tweaky, cellW, cellH);
            p.fillRect(cellRect, bg);
            p.setPen(fg);

            if (!cell.a.invisible) {
                QString s = cellToString(cell, use_acs, acs_map);
                int tx = cellRect.x();
                int ty = cellRect.y() + fm->ascent();
                p.drawText(tx, ty, s);
            }
            if (cell.a.underline) {
                p.fillRect(cellRect.x(), cellRect.y() + cellH - 2, cellW, 2, fg);
            }
        }
    }

    // selection highlight
    if (selection_has_content) {
        int sLine = sel_start.x(), sCol = sel_start.y();
        int eLine = sel_end.x(), eCol = sel_end.y();
        if (sLine > eLine || (sLine == eLine && sCol > eCol)) {
            std::swap(sLine, eLine); std::swap(sCol, eCol);
        }
        QColor selColor = QColor(0x33, 0x99, 0xFF, 120);
        for (int line = sLine; line <= eLine; ++line) {
            int bottomIndex = (int)history.size() + (rows - 1);
            int y = bottomIndex - line - scroll_offset;
            if (y < 0 || y >= rows) continue;
            int sx = (line == sLine) ? sCol : 0;
            int ex = (line == eLine) ? eCol : (cols - 1);
            if (sx < 0) sx = 0;
            if (ex >= cols) ex = cols - 1;
            QRect selRect(sx * cellW + tweakx, y * cellH + tweaky, (ex - sx + 1) * cellW, cellH);
            p.fillRect(selRect, selColor);
        }
        // redraw text on top of selection
        for (int y = 0; y < rows; ++y) {
            int bufIndex = visibleRowToBufferIndex(y);
            if (bufIndex < 0 || bufIndex >= totalBufferLines()) continue;
            QVector<Cell> rowCells;
            if (bufIndex < (int)history.size()) rowCells = history[bufIndex];
            else rowCells = screen[bufIndex - (int)history.size()];
            for (int x = 0; x < cols; ++x) {
                const Cell &cell = rowCells[x];
                int line = bufIndex;
                bool selected = false;
                int sL = sel_start.x(), eL = sel_end.x();
                int sC = sel_start.y(), eC = sel_end.y();
                if (sL > eL || (sL == eL && sC > eC)) { std::swap(sL, eL); std::swap(sC, eC); }
                if (line > sL && line < eL) selected = true;
                else if (line == sL && line == eL && x >= sC && x <= eC) selected = true;
                else if (line == sL && line < eL && x >= sC) selected = true;
                else if (line > sL && line == eL && x <= eC) selected = true;
                if (!selected) continue;
                QColor fg = colorFromCellAttrFg(cell.a);
                QRect cellRect(x * cellW + tweakx, y * cellH + tweaky, cellW, cellH);
                if (!cell.a.invisible) {
                    QString s = cellToString(cell, use_acs, acs_map);
                    int tx = cellRect.x();
                    int ty = cellRect.y() + fm->ascent();
                    p.setPen(fg);
                    p.drawText(tx, ty, s);
                }
            }
        }
    }

    // cursor
    if (cursorEnabled && cursorVisible) {
        int bufIndex = (int)history.size() + cursorY;
        int bottomIndex = (int)history.size() + (rows - 1);
        int y = bottomIndex - bufIndex - scroll_offset;
        if (y >= 0 && y < rows) {
            QVector<Cell> rowCells = screen[cursorY];
            QColor curcol = colorFromCellAttrFg(rowCells[cursorX].a);
            QRect cur(cursorX * cellW + tweakx, y * cellH + cellH - 2 + tweaky, cellW, 2);
            p.fillRect(cur, curcol);
        }
    }
}

// ----------------------------- Input & Events -----------------------------
void TerminalWidget::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);
    int sbw = vscroll ? vscroll->sizeHint().width() : 0;
    int w = ev->size().width() - sbw;
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
    if ((ev->modifiers() & Qt::ControlModifier) && ev->key() == Qt::Key_C) {
        if (selection_has_content) copySelectionToClipboard();
        return;
    }

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
            if (!ev->text().isEmpty()) sendToPty(ev->text().toUtf8());
            return;
    }
    if (!seq.isEmpty()) sendToPty(seq);
}

void TerminalWidget::inputMethodEvent(QInputMethodEvent *ev)
{
    if (!ev->commitString().isEmpty()) sendToPty(ev->commitString().toUtf8());
    else QWidget::inputMethodEvent(ev);
}

void TerminalWidget::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        int x = (ev->position().x()) / cellW;
        int y = (ev->position().y()) / cellH;
        if (x < 0) x = 0; if (x >= cols) x = cols - 1;
        int bufIndex = visibleRowToBufferIndex(y);
        startSelectionAtBufferPos(bufIndex, x);
        selecting = true;
        update();
    } else if (ev->button() == Qt::RightButton) {
        if (selection_has_content) copySelectionToClipboard();
    }
}

void TerminalWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton && selecting) {
        int x = (ev->position().x()) / cellW;
        int y = (ev->position().y()) / cellH;
        if (x < 0) x = 0; if (x >= cols) x = cols - 1;
        int bufIndex = visibleRowToBufferIndex(y);
        updateSelectionToBufferPos(bufIndex, x);
        selecting = false;
        selection_has_content = true;
        update();
    }
}

void TerminalWidget::mouseMoveEvent(QMouseEvent *ev)
{
    if (!selecting) {
        if (mouseMode != -1) sendMouseEvent(ev, false);
        return;
    }
    int x = (ev->position().x()) / cellW;
    int y = (ev->position().y()) / cellH;
    if (x < 0) x = 0; if (x >= cols) x = cols - 1;
    int bufIndex = visibleRowToBufferIndex(y);
    updateSelectionToBufferPos(bufIndex, x);
    update();
}

void TerminalWidget::wheelEvent(QWheelEvent *ev)
{
    int delta = ev->angleDelta().y();
    if (delta == 0) return;
    int steps = delta / 120;
    int newVal = vscroll->value() + (-steps);
    newVal = qBound(vscroll->minimum(), newVal, vscroll->maximum());
    vscroll->setValue(newVal);
}

// ----------------------------- Selection ----------------------------------
void TerminalWidget::startSelectionAtBufferPos(int bufferLine, int col)
{
    sel_start = QPoint(bufferLine, col);
    sel_end = sel_start;
    selection_has_content = false;
}

void TerminalWidget::updateSelectionToBufferPos(int bufferLine, int col)
{
    sel_end = QPoint(bufferLine, col);
    selection_has_content = !(sel_start == sel_end);
}

QString TerminalWidget::getSelectionText() const
{
    if (!selection_has_content) return QString();

    int sLine = sel_start.x(), sCol = sel_start.y();
    int eLine = sel_end.x(), eCol = sel_end.y();
    if (sLine > eLine || (sLine == eLine && sCol > eCol)) {
        std::swap(sLine, eLine);
        std::swap(sCol, eCol);
    }

    QStringList lines;
    for (int line = sLine; line <= eLine; ++line) {
        if (line < 0 || line >= totalBufferLines()) continue;
        QVector<Cell> rowCells;
        if (line < (int)history.size()) rowCells = history[line];
        else rowCells = screen[line - (int)history.size()];
        int startCol = (line == sLine) ? sCol : 0;
        int endCol = (line == eLine) ? eCol : (cols - 1);
        if (startCol < 0) startCol = 0;
        if (endCol >= cols) endCol = cols - 1;
        QString rowText;
        for (int c = startCol; c <= endCol; ++c) rowText += cellToString(rowCells[c], use_acs, acs_map);
        lines << rowText;
    }
    return lines.join('\n');
}

void TerminalWidget::copySelectionToClipboard()
{
    QString txt = getSelectionText();
    if (txt.isEmpty()) return;
    QClipboard *cb = QGuiApplication::clipboard();
    cb->setText(txt);
}

// ----------------------------- PTY I/O & Parser ---------------------------
void TerminalWidget::onPtyReadable()
{
    if (masterFd < 0) return;
    char buf[4096];
    ssize_t n = ::read(masterFd, buf, sizeof(buf));
    if (n > 0) {
        feedParser(buf, n);
        if (scroll_offset == 0 && vscroll) vscroll->setValue(0);
        update();
    } else if (n == 0) {
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
    int master = -1;
    pid_t pid = forkpty(&master, nullptr, nullptr, nullptr);
    if (pid < 0) return false;
    if (pid == 0) {
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
    masterFd = master;
    childPid = pid;
    int flags = fcntl(masterFd, F_GETFL, 0);
    fcntl(masterFd, F_SETFL, flags | O_NONBLOCK);
    notifier = new QSocketNotifier(masterFd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &TerminalWidget::onPtyReadable);
    updateWindowSize();
    return true;
}

// ----------------------------- Parser helpers -----------------------------
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
    alt_screen = screen;
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x) screen[y][x].ch = ' ';
    using_alt = true;
    cursorX = 0; cursorY = 0;
    markLineDirty(cursorY);
    update();
}

void TerminalWidget::restoreFromAltBuffer()
{
    if (!using_alt) return;
    if (!alt_screen.isEmpty()) screen = alt_screen;
    alt_screen.clear();
    using_alt = false;
    markLineDirty(cursorY);
    update();
}

void TerminalWidget::setPrivateMode(int mode)
{
    switch (mode) {
        case 25: cursorEnabled = true; markLineDirty(cursorY); break;
        case 1000: mouseMode = 9; break;
        case 1002: mouseMotion = true; break;
        case 1006: mouseMode = 1006; break;
        case 1049:
        case 47:
        case 1047:
        case 1048:
            saveCursor(); switchToAltBuffer(); break;
        default: break;
    }
}

void TerminalWidget::unsetPrivateMode(int mode)
{
    switch (mode) {
        case 25: cursorEnabled = false; markLineDirty(cursorY); break;
        case 1000: if (mouseMode == 9) mouseMode = -1; break;
        case 1002: mouseMotion = false; break;
        case 1006: if (mouseMode == 1006) mouseMode = -1; break;
        case 1049:
        case 47:
        case 1047:
        case 1048:
            restoreFromAltBuffer(); restoreCursor(); break;
        default: break;
    }
}

void TerminalWidget::setSGR(const QByteArray &params)
{
    if (params.isEmpty()) { currentAttr = CellAttr(); return; }
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
        else if (v >= 30 && v <= 37) { currentAttr.fg_is_rgb = false; currentAttr.fg_index = v - 30; i++; }
        else if (v >= 40 && v <= 47) { currentAttr.bg_is_rgb = false; currentAttr.bg_index = (v - 40) % 8; i++; }
        else if (v >= 90 && v <= 97) { currentAttr.fg_is_rgb = false; currentAttr.fg_index = (v - 90) + 8; i++; }
        else if (v >= 100 && v <= 107) { currentAttr.bg_is_rgb = false; currentAttr.bg_index = (v - 100) + 8; i++; }
        else if (v == 38 || v == 48) {
            bool isFg = (v == 38);
            i++;
            if (i >= parts.size()) break;
            int mode = parts[i].toInt();
            if (mode == 5) {
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
                } else break;
            } else { i++; }
        } else { i++; }
    }
}

void TerminalWidget::putByte(uint8_t b)
{
    if (b == '\r') { cursorX = 0; return; }
    if (b == '\n') {
        QVector<Cell> row = screen[cursorY];
        pushHistoryRow(row);
        for (int r = 0; r < rows - 1; ++r) screen[r] = screen[r+1];
        screen[rows-1].fill(Cell());
        cursorX = 0;
        markLineDirty(rows-1);
        if (scroll_offset == 0 && vscroll) vscroll->setValue(0);
        return;
    }
    if (b == '\b') { if (cursorX > 0) cursorX--; return; }
    if (b == '\t') { int next = ((cursorX / 8) + 1) * 8; if (next >= cols) next = cols - 1; cursorX = next; return; }

    if (cursorX >= cols) { cursorX = 0; cursorY++; if (cursorY >= rows) {
            QVector<Cell> row = screen[rows-1];
            pushHistoryRow(row);
            for (int r = 0; r < rows - 1; ++r) screen[r] = screen[r+1];
            screen[rows-1].fill(Cell());
            cursorY = rows - 1;
        }
    }
    Cell &cell = screen[cursorY][cursorX];
    cell.ch = b;
    cell.a = currentAttr;
    markLineDirty(cursorY);
    cursorX++;
    if (cursorX >= cols) { cursorX = 0; cursorY++; if (cursorY >= rows) {
            QVector<Cell> row = screen[rows-1];
            pushHistoryRow(row);
            for (int r = 0; r < rows - 1; ++r) screen[r] = screen[r+1];
            screen[rows-1].fill(Cell());
            cursorY = rows - 1;
        }
    }
}

void TerminalWidget::handleCsi(const QByteArray &seq)
{
    if (seq.isEmpty()) return;
    bool is_private = (seq.size() > 0 && seq[0] == '?');
    if (is_private) {
        char final = seq.constData()[seq.size()-1];
        QByteArray params = seq.mid(1, seq.size()-2);
        if (final == 'h') {
            QList<QByteArray> parts = params.split(';');
            for (const QByteArray &p : parts) setPrivateMode(p.toInt());
            return;
        } else if (final == 'l') {
            QList<QByteArray> parts = params.split(';');
            for (const QByteArray &p : parts) unsetPrivateMode(p.toInt());
            return;
        }
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
                    putByte(ch);
                }
                break;
            case PS_ESC:
                if (ch == '[') { pstate = PS_CSI; csiBuf.clear(); }
                else if (ch == ']') { pstate = PS_OSC; oscBuf.clear(); }
                else if (ch == '(' || ch == ')') { esc_prefix = (char)ch; pstate = PS_ESC2; }
                else if (ch == '7') { saveCursor(); pstate = PS_GROUND; }
                else if (ch == '8') { restoreCursor(); pstate = PS_GROUND; }
                else pstate = PS_GROUND;
                break;
            case PS_ESC2:
                if (esc_prefix == '(') {
                    if (ch == '0') use_acs = true;
                    else if (ch == 'B') use_acs = false;
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
                if (ch == 0x07) { handleOsc(oscBuf); pstate = PS_GROUND; }
                else if (ch == 0x1b) { oscBuf.append((char)ch); }
                else if (ch == '\\' && !oscBuf.isEmpty() && oscBuf.endsWith('\x1b')) {
                    oscBuf.chop(1);
                    handleOsc(oscBuf);
                    pstate = PS_GROUND;
                } else oscBuf.append((char)ch);
                break;
        }
    }
}

// ----------------------------- Scrollbar ---------------------------------
void TerminalWidget::onScrollBarChanged(int value)
{
    scroll_offset = value;
    update();
}

// ----------------------------- Mouse reporting ---------------------------
void TerminalWidget::sendMouseEvent(const QMouseEvent *ev, bool isRelease)
{
    int x = (int)ev->position().x() - tweakx;
    int y = (int)ev->position().y() - tweaky;
    x = x / cellW; y = y / cellH;
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
        buf.append("\x1b[M"); buf.append(b1); buf.append(bx); buf.append(by);
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

// ----------------------------- Utility -----------------------------------
void TerminalWidget::markLineDirty(int y)
{
    if (y < 0 || y >= rows) return;
    if ((int)dirty.size() != rows) dirty = QVector<char>(rows, 0);
    dirty[y] = 1;
}

// ----------------------------- Missing functions implemented --------------
// clearLine: 0 = from cursor to end, 1 = from start to cursor, 2 = entire line
void TerminalWidget::clearLine(int mode)
{
    if (cursorY < 0 || cursorY >= rows) return;
    if (mode == 0) {
        for (int x = cursorX; x < cols; ++x) screen[cursorY][x] = Cell();
    } else if (mode == 1) {
        for (int x = 0; x <= cursorX; ++x) screen[cursorY][x] = Cell();
    } else {
        for (int x = 0; x < cols; ++x) screen[cursorY][x] = Cell();
    }
    markLineDirty(cursorY);
}

// clearScreen: 0 = from cursor to end, 1 = from start to cursor, 2 = entire screen
void TerminalWidget::clearScreen(int mode)
{
    if (mode == 0) {
        for (int y = cursorY; y < rows; ++y) {
            int start = (y == cursorY) ? cursorX : 0;
            for (int x = start; x < cols; ++x) screen[y][x] = Cell();
            markLineDirty(y);
        }
    } else if (mode == 1) {
        for (int y = 0; y <= cursorY; ++y) {
            int end = (y == cursorY) ? cursorX : (cols - 1);
            for (int x = 0; x <= end; ++x) screen[y][x] = Cell();
            markLineDirty(y);
        }
    } else {
        for (int y = 0; y < rows; ++y)
            for (int x = 0; x < cols; ++x) screen[y][x] = Cell();
        for (int y = 0; y < rows; ++y) markLineDirty(y);
    }
}

// moveCursorTo: 1-based coordinates per ANSI; clamp to screen
void TerminalWidget::moveCursorTo(int r, int c)
{
    if (r <= 0) r = 1;
    if (c <= 0) c = 1;
    cursorY = qBound(0, r - 1, rows - 1);
    cursorX = qBound(0, c - 1, cols - 1);
    markLineDirty(cursorY);
}

// setFontFile: load font file or fallback to system monospace and recalc metrics
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

// ----------------------------- Main --------------------------------------
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QString fontPath;
    int fontSize = 12;
    int colsArg = 80, rowsArg = 25;
    QString title = QString::fromUtf8(DEFAULT_TITLE);
    bool enableX10 = false, enable1006 = false;
    QStringList cmd;

    for (int i = 1; i < argc; ++i) {
        QString a = QString::fromLocal8Bit(argv[i]);
        if (a == "-F" && i+1 < argc) fontPath = argv[++i];
        else if (a == "-S" && i+1 < argc) fontSize = QString::fromLocal8Bit(argv[++i]).toInt();
        else if (a == "-d" && i+1 < argc) {
            QString d = QString::fromLocal8Bit(argv[++i]);
            QStringList parts = d.split('x');
            if (parts.size() == 2) { colsArg = parts[0].toInt(); rowsArg = parts[1].toInt(); }
        }
        else if (a == "-t" && i+1 < argc) title = QString::fromLocal8Bit(argv[++i]);
        else if (a == "-m") enableX10 = true;
        else if (a == "-M") enable1006 = true;
        else {
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

    int sbw = w.vscroll ? w.vscroll->sizeHint().width() : 0;
    w.resize(colsArg * w.getCellW() + sbw, rowsArg * w.getCellH());
    w.show();

    return app.exec();
}

#include "qterm1.moc"
