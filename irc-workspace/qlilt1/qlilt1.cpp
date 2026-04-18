// qlilt1.cpp
// Qt6 port of lilt_single.c (single-file terminal widget, PTY-backed).
// Adjusted to avoid name collisions and warnings (use ::close, capture write return, etc).

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QFont>
#include <QSocketNotifier>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>

#include <pty.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>

#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>

struct CellAttr {
    uint8_t fg; // 1..16 (0 default)
    uint8_t bg; // 1..8 (0 default)
    bool bold, underline, blink, reverse, invisible;
};
struct Cell {
    uint8_t c;
    CellAttr a;
};

static uint32_t palette_raw[] = {
    0x171717,0xa8203d,0x3da820,0xa88a20,0x203da8,0x8a20a8,0x20a88a,0xbdbdbd,
    0x3b3b3b,0xeb92a5,0xa5eb92,0xebd792,0x92a5eb,0xd792eb,0x92ebd7,0xf1f1f1
};

class TerminalWidget : public QWidget {
    Q_OBJECT
public:
    TerminalWidget(QString fontFamily = "DejaVu Sans Mono", int fontSize = 14, QWidget *parent = nullptr)
        : QWidget(parent),
          font(fontFamily, fontSize),
          glyphCount(256),
          atlasCols(16),
          masterFd(-1),
          childPid(-1),
          mouseMode(-1)
    {
        setFocusPolicy(Qt::StrongFocus);
        setAttribute(Qt::WA_InputMethodEnabled, true);

        // default size
        termW = 80; termH = 25;

        // palette -> QColor
        for (int i=0;i<16;++i) {
            uint32_t v = palette_raw[i];
            colors.push_back(QColor((v>>16)&0xff, (v>>8)&0xff, v&0xff));
        }
        for (int i=0;i<8;++i) {
            uint32_t v = palette_raw[i];
            int r = (v>>16)&0xff; int g = (v>>8)&0xff; int b = v&0xff;
            r = r*90/100; g = g*90/100; b = b*90/100;
            bgcolors.push_back(QColor(r,g,b));
        }

        currentAttr = { (uint8_t)(15+1), (uint8_t)(0+1), false,false,false,false,false };

        // allocate buffers
        allocScreen(termW, termH);

        // glyph metrics & atlas
        QFontMetrics fm(font);
        glyphH = fm.height();
        glyphW = fm.horizontalAdvance(QLatin1Char('M'));
        if (glyphW <= 0) glyphW = 8;
        if (glyphH <= 0) glyphH = 16;
        recreateGlyphAtlas();

        // spawn PTY child (shell)
        if (spawnPtyAndChild(nullptr) != 0) {
            qWarning() << "spawnPtyAndChild failed";
        }

        // notifier for PTY reads
        readNotifier = nullptr;
        if (masterFd >= 0) {
            readNotifier = new QSocketNotifier(masterFd, QSocketNotifier::Read, this);
            connect(readNotifier, &QSocketNotifier::activated, this, &TerminalWidget::onPtyReadable);
        }

        // blink timer
        blinkTimer = new QTimer(this);
        connect(blinkTimer, &QTimer::timeout, this, &TerminalWidget::onBlink);
        blinkTimer->start(500);

        setMinimumSize(glyphW*5, glyphH*5);
    }

    ~TerminalWidget() override {
        if (readNotifier) {
            readNotifier->setEnabled(false);
            delete readNotifier;
            readNotifier = nullptr;
        }
        if (masterFd >= 0) {
            ::close(masterFd); // use global close to avoid QWidget::close()
            masterFd = -1;
        }
        if (childPid > 0) {
            ::kill(childPid, SIGHUP);
            childPid = -1;
        }
        freeBuffers();
    }

protected:
    void paintEvent(QPaintEvent *ev) override {
        Q_UNUSED(ev);
        QPainter p(this);
        p.fillRect(rect(), bgcolors[0]);

        auto &buf = altActive && !altBuf.empty() ? altBuf : screenBuf;

        for (int r=0;r<termH;++r) {
            for (int c=0;c<termW;++c) {
                const Cell &cell = buf[r*termW + c];
                int fg = cell.a.fg ? (cell.a.fg - 1) : 15;
                int bg = cell.a.bg ? (cell.a.bg - 1) : 0;
                QRect dst(c*glyphW, r*glyphH, glyphW, glyphH);
                p.fillRect(dst, bgcolors[bg]);

                uint8_t cc = cell.c;
                if (cc >= glyphCount) cc = '?';
                if (!atlas.isNull()) {
                    int srcCol = cc % atlasCols;
                    int srcRow = cc / atlasCols;
                    QRect src(srcCol*glyphW, srcRow*glyphH, glyphW, glyphH);
                    QImage glyph = atlas.copy(src);
                    QImage colored(glyph.size(), QImage::Format_ARGB32);
                    colored.fill(Qt::transparent);
                    QPainter gp(&colored);
                    gp.setCompositionMode(QPainter::CompositionMode_Source);
                    gp.drawImage(0,0,glyph);
                    gp.setCompositionMode(QPainter::CompositionMode_SourceIn);
                    gp.fillRect(colored.rect(), colors[fg]);
                    gp.end();
                    p.drawImage(dst.topLeft(), colored);
                } else {
                    p.setPen(colors[fg]);
                    p.setFont(font);
                    QChar ch = (cc < 32) ? QChar(' ') : QChar(cc);
                    p.drawText(dst, Qt::AlignCenter, QString(ch));
                }

                if (cell.a.underline) {
                    p.fillRect(dst.x(), dst.y() + glyphH - 2, glyphW, 2, colors[fg]);
                }
            }
        }

        if (cursorEnabled && cursorVisible) {
            if (cursorY >=0 && cursorY < termH && cursorX >=0 && cursorX < termW) {
                int fg = 15;
                const Cell &c = (altActive && !altBuf.empty()) ? altBuf[cursorY*termW + cursorX] : screenBuf[cursorY*termW + cursorX];
                fg = c.a.fg ? (c.a.fg - 1) : 15;
                QRect cur(cursorX*glyphW, cursorY*glyphH + glyphH - 2, glyphW, 2);
                p.fillRect(cur, colors[fg]);
            }
        }
    }

    void keyPressEvent(QKeyEvent *ev) override {
        if (ev->modifiers() & Qt::ControlModifier) {
            if (!ev->text().isEmpty()) {
                QChar ch = ev->text().at(0).toLower();
                if (ch >= 'a' && ch <= 'z') {
                    char c = (char)(ch.unicode() - 'a' + 1);
                    writeToPty(&c, 1);
                    return;
                }
            }
        }

        switch (ev->key()) {
            case Qt::Key_Up: writeToPty("\x1b[A",3); break;
            case Qt::Key_Down: writeToPty("\x1b[B",3); break;
            case Qt::Key_Left: writeToPty("\x1b[D",3); break;
            case Qt::Key_Right: writeToPty("\x1b[C",3); break;
            case Qt::Key_Backspace: writeToPty("\x7f",1); break;
            case Qt::Key_Return: writeToPty("\r",1); break;
            case Qt::Key_Tab: writeToPty("\t",1); break;
            case Qt::Key_Escape: writeToPty("\x1b",1); break;
            default:
                if (!ev->text().isEmpty()) {
                    QByteArray ba = ev->text().toUtf8();
                    writeToPty(ba.constData(), ba.size());
                }
                break;
        }
    }

    void mousePressEvent(QMouseEvent *ev) override { handleMouse(ev, true); }
    void mouseReleaseEvent(QMouseEvent *ev) override { handleMouse(ev, false); }
    void mouseMoveEvent(QMouseEvent *ev) override { if (mouseMotionEnabled) handleMouse(ev, true); }

    void resizeEvent(QResizeEvent *ev) override {
        Q_UNUSED(ev);
        int newCols = width() / glyphW;
        int newRows = height() / glyphH;
        if (newCols < 5) newCols = 5;
        if (newRows < 5) newRows = 5;
        allocScreen(newCols, newRows);
        sendWinSize();
        update();
    }

private slots:
    void onPtyReadable() {
        if (masterFd < 0) return;
        char buf[4096];
        ssize_t n = ::read(masterFd, buf, sizeof(buf));
        if (n > 0) {
            feedBytesToParser(buf, n);
            update();
        } else if (n == 0) {
            ::close(masterFd);
            masterFd = -1;
            // reap child
            if (childPid > 0) {
                int status;
                waitpid(childPid, &status, WNOHANG);
                childPid = -1;
            }
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                qWarning() << "read error from pty:" << strerror(errno);
            }
        }
    }

    void onBlink() {
        cursorVisible = !cursorVisible;
        update();
    }

private:
    QFont font;
    int glyphW=8, glyphH=16;
    int glyphCount;
    int atlasCols;
    QImage atlas;
    std::vector<QColor> colors;
    std::vector<QColor> bgcolors;

    int termW, termH;
    std::vector<Cell> screenBuf;
    std::vector<Cell> altBuf;
    bool altActive = false;
    bool cursorEnabled = true;
    bool cursorVisible = true;
    int cursorX = 0, cursorY = 0;
    CellAttr currentAttr;

    int masterFd;
    pid_t childPid;
    QSocketNotifier *readNotifier;
    QTimer *blinkTimer;

    int mouseMode;
    bool mouseMotionEnabled = false;

    enum ParserState { PS_GROUND, PS_ESC, PS_CSI, PS_OSC } pstate = PS_GROUND;
    QByteArray csiBuf;
    QByteArray oscBuf;

    void allocScreen(int cols, int rows) {
        termW = cols; termH = rows;
        screenBuf.assign(termW*termH, Cell{(uint8_t)' ', currentAttr});
        if (!altBuf.empty()) {
            altBuf.assign(termW*termH, Cell{(uint8_t)' ', currentAttr});
        }
    }
    void freeBuffers() { screenBuf.clear(); altBuf.clear(); }

    void recreateGlyphAtlas() {
        int cols = atlasCols;
        int rows = (glyphCount + cols - 1) / cols;
        atlas = QImage(glyphW*cols, glyphH*rows, QImage::Format_ARGB32);
        atlas.fill(Qt::transparent);
        QPainter p(&atlas);
        p.setFont(font);
        p.setPen(Qt::white);
        for (int cc=0; cc<glyphCount; ++cc) {
            QChar ch = (cc < 32) ? QChar(' ') : QChar(cc);
            int col = cc % cols;
            int row = cc / cols;
            QRect dst(col*glyphW, row*glyphH, glyphW, glyphH);
            p.drawText(dst, Qt::AlignCenter, QString(ch));
        }
        p.end();
    }

    void writeToPty(const char *s, int len) {
        if (masterFd >= 0) {
            ssize_t __w = ::write(masterFd, s, (size_t)len);
            (void)__w;
        }
    }
    void writeToPty(const QByteArray &ba) { writeToPty(ba.constData(), ba.size()); }

    int spawnPtyAndChild(char **runCmd) {
        char *cshell = getenv("SHELL");
        const char *shell = cshell ? cshell : "/bin/sh";
        int master = -1;
        pid_t pid = forkpty(&master, nullptr, nullptr, nullptr);
        if (pid < 0) {
            qWarning() << "forkpty failed:" << strerror(errno);
            return -1;
        }
        if (pid == 0) {
            setenv("TERM", "xterm-256color", 1);
            if (runCmd && runCmd[0]) execvp(runCmd[0], runCmd);
            execlp(shell, shell, (char*)nullptr);
            perror("exec");
            _exit(127);
        }
        masterFd = master;
        childPid = pid;
        int flags = fcntl(masterFd, F_GETFL, 0);
        if (flags >= 0) fcntl(masterFd, F_SETFL, flags | O_NONBLOCK);
        return 0;
    }

    void sendWinSize() {
        if (masterFd < 0) return;
        struct winsize ws;
        ws.ws_row = termH;
        ws.ws_col = termW;
        ws.ws_xpixel = termW * glyphW;
        ws.ws_ypixel = termH * glyphH;
        ioctl(masterFd, TIOCSWINSZ, &ws);
    }

    void feedBytesToParser(const char *buf, ssize_t len) {
        for (ssize_t i=0;i<len;++i) {
            unsigned char ch = (unsigned char)buf[i];
            switch (pstate) {
                case PS_GROUND:
                    if (ch == 0x1b) { pstate = PS_ESC; csiBuf.clear(); oscBuf.clear(); }
                    else if (ch < 0x20) {
                        if (ch == '\r') putChar('\r');
                        else if (ch == '\n') putChar('\n');
                        else if (ch == '\b') putChar('\b');
                        else if (ch == '\t') putChar('\t');
                    } else putChar(ch);
                    break;
                case PS_ESC:
                    if (ch == '[') { pstate = PS_CSI; csiBuf.clear(); }
                    else if (ch == ']') { pstate = PS_OSC; oscBuf.clear(); }
                    else pstate = PS_GROUND;
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
                    else if (ch == '\\' && !oscBuf.isEmpty() && oscBuf.back() == 0x1b) {
                        oscBuf.chop(1);
                        handleOsc(oscBuf);
                        pstate = PS_GROUND;
                    } else oscBuf.append((char)ch);
                    break;
            }
        }
    }

    void putChar(unsigned char ch) {
        if (ch == '\r') { cursorX = 0; return; }
        if (ch == '\n') { cursorX = 0; cursorY++; if (cursorY >= termH) { scrollUp(1); cursorY = termH-1; } return; }
        if (ch == '\b') { if (cursorX>0) cursorX--; return; }
        if (ch == '\t') { int next = ((cursorX/8)+1)*8; if (next>=termW) next = termW-1; cursorX = next; return; }

        if (cursorX >= termW) { cursorX = 0; cursorY++; if (cursorY >= termH) { scrollUp(1); cursorY = termH-1; } }
        std::vector<Cell> &buf = altActive && !altBuf.empty() ? altBuf : screenBuf;
        buf[cursorY*termW + cursorX].c = ch;
        buf[cursorY*termW + cursorX].a = currentAttr;
        cursorX++;
        if (cursorX >= termW) { cursorX = 0; cursorY++; if (cursorY >= termH) { scrollUp(1); cursorY = termH-1; } }
    }

    void scrollUp(int n) {
        if (n<=0) return;
        std::vector<Cell> &buf = altActive && !altBuf.empty() ? altBuf : screenBuf;
        for (int r=0;r<termH-n;++r) {
            memcpy(&buf[r*termW], &buf[(r+n)*termW], termW*sizeof(Cell));
        }
        for (int r=termH-n;r<termH;++r) {
            for (int c=0;c<termW;++c) buf[r*termW + c].c = ' ';
        }
    }

    void handleCsi(const QByteArray &seq) {
        if (seq.isEmpty()) return;
        char final = seq.back();
        QByteArray params = seq.left(seq.size()-1);
        if (!params.isEmpty() && params[0] == '?') {
            QByteArray tmp = params.mid(1);
            QList<QByteArray> toks = tmp.split(';');
            for (auto t : toks) {
                int v = atoi(t.constData());
                if (final == 'h') {
                    if (v == 25) { cursorEnabled = true; cursorVisible = true; }
                    else if (v == 1049) { if (altBuf.empty()) altBuf.assign(termW*termH, Cell{(uint8_t)' ', currentAttr}); altActive = true; cursorX=0; cursorY=0; }
                } else if (final == 'l') {
                    if (v == 25) { cursorEnabled = true; cursorVisible = false; }
                    else if (v == 1049) { altActive = false; }
                }
            }
            return;
        }
        if (final == 'A') { int n = params.isEmpty() ? 1 : atoi(params.constData()); cursorY = qMax(0, cursorY - (n<=0?1:n)); }
        else if (final == 'B') { int n = params.isEmpty() ? 1 : atoi(params.constData()); cursorY = qMin(termH-1, cursorY + (n<=0?1:n)); }
        else if (final == 'C') { int n = params.isEmpty() ? 1 : atoi(params.constData()); cursorX = qMin(termW-1, cursorX + (n<=0?1:n)); }
        else if (final == 'D') { int n = params.isEmpty() ? 1 : atoi(params.constData()); cursorX = qMax(0, cursorX - (n<=0?1:n)); }
        else if (final == 'H' || final == 'f') {
            int r=1,c=1;
            if (!params.isEmpty()) sscanf(params.constData(), "%d;%d", &r, &c);
            moveCursorTo(r,c);
        }
        else if (final == 'J') { int m = params.isEmpty() ? 0 : atoi(params.constData()); clearScreen(m); }
        else if (final == 'K') { int m = params.isEmpty() ? 0 : atoi(params.constData()); clearLine(m); }
        else if (final == 'm') { setSgr(params.constData()); }
        else if (final == 's') { savedX = cursorX; savedY = cursorY; savedAttr = currentAttr; }
        else if (final == 'u') { cursorX = savedX; cursorY = savedY; currentAttr = savedAttr; }
        else if (final == 'r') {
            if (params.isEmpty()) { scrollTop = 0; scrollBottom = termH-1; }
            else { int top=0,bottom=0; if (sscanf(params.constData(), "%d;%d", &top, &bottom)==2) { scrollTop = qBound(0, top-1, termH-1); scrollBottom = qBound(0, bottom-1, termH-1); } }
        }
    }

    void handleOsc(const QByteArray &seq) {
        if (seq.startsWith("0;") || seq.startsWith("2;")) {
            QByteArray title = seq.mid(2);
            if (!title.isEmpty()) {
                if (window()) window()->setWindowTitle(QString::fromUtf8(title));
            }
        }
    }

    void moveCursorTo(int r, int c) {
        if (r<=0) r=1; if (c<=0) c=1;
        cursorY = qBound(0, r-1, termH-1);
        cursorX = qBound(0, c-1, termW-1);
    }

    void clearScreen(int mode) {
        std::vector<Cell> &buf = altActive && !altBuf.empty() ? altBuf : screenBuf;
        if (mode == 0) {
            for (int r=cursorY;r<termH;++r) {
                int start = (r==cursorY)?cursorX:0;
                for (int c=start;c<termW;++c) buf[r*termW + c].c = ' ';
            }
        } else if (mode == 1) {
            for (int r=0;r<=cursorY;++r) {
                int end = (r==cursorY)?cursorX:termW-1;
                for (int c=0;c<=end;++c) buf[r*termW + c].c = ' ';
            }
        } else {
            for (int i=0;i<termW*termH;++i) buf[i].c = ' ';
        }
    }

    void clearLine(int mode) {
        std::vector<Cell> &buf = altActive && !altBuf.empty() ? altBuf : screenBuf;
        if (mode == 0) {
            for (int c=cursorX;c<termW;++c) buf[cursorY*termW + c].c = ' ';
        } else if (mode == 1) {
            for (int c=0;c<=cursorX;++c) buf[cursorY*termW + c].c = ' ';
        } else {
            for (int c=0;c<termW;++c) buf[cursorY*termW + c].c = ' ';
        }
    }

    void setSgr(const char *params) {
        if (!params || params[0]==0) {
            currentAttr.fg = 15+1; currentAttr.bg = 0+1; currentAttr.bold = currentAttr.underline = currentAttr.blink = currentAttr.reverse = currentAttr.invisible = false;
            return;
        }
        char tmp[128]; strncpy(tmp, params, sizeof(tmp)-1); tmp[sizeof(tmp)-1]=0;
        char *tok = strtok(tmp, ";");
        while (tok) {
            int v = atoi(tok);
            if (v==0) { currentAttr.fg = 15+1; currentAttr.bg = 0+1; currentAttr.bold = currentAttr.underline = currentAttr.blink = currentAttr.reverse = currentAttr.invisible = false; }
            else if (v==1) currentAttr.bold = true;
            else if (v==4) currentAttr.underline = true;
            else if (v==5) currentAttr.blink = true;
            else if (v==7) currentAttr.reverse = true;
            else if (v==8) currentAttr.invisible = true;
            else if (v>=30 && v<=37) currentAttr.fg = (v-30)+1;
            else if (v>=40 && v<=47) currentAttr.bg = (v-40)%8 + 1;
            else if (v>=90 && v<=97) currentAttr.fg = (v-90)+8+1;
            else if (v>=100 && v<=107) currentAttr.bg = (v-100)+8+1;
            tok = strtok(nullptr, ";");
        }
    }

    void handleMouse(QMouseEvent *ev, bool down) {
        if (mouseMode == -1) return;
        int x = ev->position().x() / glyphW;
        int y = ev->position().y() / glyphH;
        x = qBound(0, x, termW-1);
        y = qBound(0, y, termH-1);
        if (mouseMode == 9) {
            int button = 0;
            if (ev->button() == Qt::LeftButton) button = 0;
            else if (ev->button() == Qt::MiddleButton) button = 1;
            else if (ev->button() == Qt::RightButton) button = 2;
            char buf[6];
            int len = snprintf(buf, sizeof(buf), "\x1b[M%c%c%c", (char)(button+32), (char)(x+1+32), (char)(y+1+32));
            writeToPty(buf, len);
        } else if (mouseMode == 1006) {
            int button = 0;
            if (ev->button() == Qt::LeftButton) button = 0;
            else if (ev->button() == Qt::MiddleButton) button = 1;
            else if (ev->button() == Qt::RightButton) button = 2;
            char c = down ? 'M' : 'm';
            char buf[64];
            int len = snprintf(buf, sizeof(buf), "\x1b[<%d;%d;%d%c", button, x+1, y+1, c);
            writeToPty(buf, len);
        }
    }

    int savedX=0, savedY=0;
    CellAttr savedAttr;
    int scrollTop=0, scrollBottom=0;
};

int main(int argc, char **argv) {
    QApplication a(argc, argv);
    TerminalWidget w("DejaVu Sans Mono", 14);
    w.resize(80*8, 25*16);
    w.show();
    return a.exec();
}

#include "qlilt1.moc"
