// terminal.cpp
// Single-file Qt6 VT100-ish terminal with PTY (Linux/macOS).
// Build with Qt6 (qmake or CMake). Example (qmake):
//   QT += widgets
//   SOURCES += terminal.cpp
//
// Notes:
// - Requires forkpty (pty.h / util.h) available on Unix-like systems.
// - This is a minimal VT100 subset: SGR (colors) and CSI K (erase line).
// - Not a full-featured terminal emulator; extend parser for more features.

#include <QApplication>
#include <QPlainTextEdit>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QFont>
#include <QScrollBar>
#include <QSocketNotifier>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QDebug>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#if defined(__APPLE__)
  #include <util.h>
#else
  #include <pty.h>
#endif

// -----------------------------
// Vt100Terminal widget (single-file)
// -----------------------------
class Vt100Terminal : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit Vt100Terminal(QWidget *parent = nullptr)
        : QPlainTextEdit(parent), masterFd(-1)
    {
        setReadOnly(false); // we intercept keys and forward to PTY
        QFont f("Monospace");
        f.setStyleHint(QFont::Monospace);
        f.setPointSize(11);
        setFont(f);
        setLineWrapMode(QPlainTextEdit::NoWrap);
        curFormat = currentCharFormat();
        setMaximumBlockCount(10000); // keep document bounded
    }

    // Feed bytes from PTY into the terminal
    void feed(const QByteArray &data) {
        for (char c : data) processByte(c);
        flushText();
    }

    void setMaxLines(int lines) { setMaximumBlockCount(lines); }

    // Set PTY master fd so widget can forward keyboard input
    void setPtyMasterFd(int fd) { masterFd = fd; if(fd >= 0) setFocus(); }

protected:
    // Forward key presses to the PTY (basic handling)
    void keyPressEvent(QKeyEvent *event) override {
        if (masterFd < 0) {
            QPlainTextEdit::keyPressEvent(event);
            return;
        }

        QByteArray out;
        // If printable text, send event->text()
        QString txt = event->text();
        if (!txt.isEmpty()) {
            // Convert newline to CR for many shells
            if (txt == "\r" || txt == "\n") out.append('\r');
            else out.append(txt.toUtf8());
        } else {
            // Handle special keys
            switch (event->key()) {
                case Qt::Key_Backspace: out.append('\x7f'); break;
                case Qt::Key_Left:  out.append("\033[D"); break;
                case Qt::Key_Right: out.append("\033[C"); break;
                case Qt::Key_Up:    out.append("\033[A"); break;
                case Qt::Key_Down:  out.append("\033[B"); break;
                case Qt::Key_Home:  out.append("\033[H"); break;
                case Qt::Key_End:   out.append("\033[F"); break;
                case Qt::Key_Tab:   out.append('\t'); break;
                case Qt::Key_Return:
                case Qt::Key_Enter: out.append('\r'); break;
                default:
                    // For Ctrl+Key combos: send control codes
                    if (event->modifiers() & Qt::ControlModifier) {
                        int k = event->key();
                        if (k >= Qt::Key_A && k <= Qt::Key_Z) {
                            char ctrl = char(k - Qt::Key_A + 1);
                            out.append(ctrl);
                        }
                    }
                    break;
            }
        }

        if (!out.isEmpty()) {
            ssize_t w = write(masterFd, out.constData(), out.size());
            (void)w;
        }
    }

private:
    // Minimal parser state
    enum ParserState { Normal, Esc, Csi } state = Normal;
    QByteArray csiBuf;
    QString currentText;
    QTextCharFormat curFormat;
    int masterFd;

    void processByte(char c) {
        if (state == Normal) {
            if (c == '\033') { state = Esc; return; }
            if (c == '\r') { /* ignore CR, handle LF */ return; }
            if (c == '\n') { currentText += '\n'; return; }
            currentText += c;
        } else if (state == Esc) {
            if (c == '[') { state = Csi; csiBuf.clear(); return; }
            // unsupported single-letter escapes: ignore and return to normal
            state = Normal;
        } else if (state == Csi) {
            if ((unsigned char)c >= 0x40 && (unsigned char)c <= 0x7E) {
                QByteArray params = csiBuf;
                handleCsi(params, c);
                state = Normal;
            } else {
                csiBuf.append(c);
            }
        }
    }

    void handleCsi(const QByteArray &params, char final) {
        // Subset: 'K' erase to end of line, 'm' SGR (colors)
        if (final == 'K') {
            int pos = currentText.lastIndexOf('\n');
            if (pos < 0) currentText.clear();
            else currentText.truncate(pos + 1);
        } else if (final == 'm') {
            QList<int> nums;
            if (params.isEmpty()) nums.append(0);
            else {
                for (auto part : params.split(';')) nums.append(part.toInt());
            }
            applySgr(nums);
        }
        // other CSI sequences ignored in this minimal example
    }

    void applySgr(const QList<int> &params) {
        for (int p : params) {
            if (p == 0) { curFormat = QTextCharFormat(); continue; }
            if (p >= 30 && p <= 37) {
                static QColor fg[] = { Qt::black, Qt::red, Qt::green, Qt::yellow,
                                       Qt::blue, Qt::magenta, Qt::cyan, Qt::white };
                curFormat.setForeground(fg[p - 30]);
            } else if (p >= 40 && p <= 47) {
                static QColor bg[] = { Qt::black, Qt::red, Qt::green, Qt::yellow,
                                       Qt::blue, Qt::magenta, Qt::cyan, Qt::white };
                curFormat.setBackground(bg[p - 40]);
            } else if (p == 1) {
                curFormat.setFontWeight(QFont::Bold);
            } else if (p == 22) {
                curFormat.setFontWeight(QFont::Normal);
            } else if (p == 39) {
                curFormat.clearForeground();
            } else if (p == 49) {
                curFormat.clearBackground();
            }
        }
    }

    void flushText() {
        if (currentText.isEmpty()) return;
        QTextCursor c = textCursor();
        c.movePosition(QTextCursor::End);
        c.insertText(currentText, curFormat);
        setTextCursor(c);
        currentText.clear();
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }
};

// -----------------------------
// main: spawn PTY and shell, wire to widget
// -----------------------------
int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Simple Qt6 VT100 Terminal (PTY)");
    QVBoxLayout *layout = new QVBoxLayout(&window);

    Vt100Terminal *term = new Vt100Terminal;
    layout->addWidget(term);
    window.resize(900, 600);
    window.show();

    // Create PTY and fork a shell
    int masterFd = -1;
    pid_t pid = forkpty(&masterFd, nullptr, nullptr, nullptr);
    if (pid < 0) {
        qFatal("forkpty failed");
        return 1;
    }

    if (pid == 0) {
        // Child: replace with user's shell
        const char *shell = getenv("SHELL");
        if (!shell) shell = "/bin/bash";
        execlp(shell, shell, nullptr);
        _exit(1);
    }

    // Parent: set non-blocking on masterFd
    int flags = fcntl(masterFd, F_GETFL, 0);
    fcntl(masterFd, F_SETFL, flags | O_NONBLOCK);

    term->setPtyMasterFd(masterFd);

    // Notifier to read when PTY has data
    QSocketNotifier *notifier = new QSocketNotifier(masterFd, QSocketNotifier::Read, &window);
    QObject::connect(notifier, &QSocketNotifier::activated, [&](int fd){
        char buf[4096];
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            term->feed(QByteArray(buf, (int)n));
        } else if (n == 0) {
            // EOF: child exited
            notifier->setEnabled(false);
        } else {
            // EAGAIN or error: ignore
        }
    });

    // Optional: reap child when it exits (avoid zombies)
    QTimer *reaper = new QTimer(&window);
    reaper->setInterval(1000);
    QObject::connect(reaper, &QTimer::timeout, [&](){
        int status = 0;
        pid_t r = waitpid(pid, &status, WNOHANG);
        if (r == pid) {
            qDebug() << "Child exited, status" << status;
            reaper->stop();
            notifier->setEnabled(false);
        }
    });
    reaper->start();

    return app.exec();
}

#include "qterm1.moc"
