// main.cpp
// GleditorPro - single-file amalgam (C++17, Qt6)
// Qt6 fixes: QRegularExpression, LineNumberArea ordering, mutex mutable, naming fixes, etc.

#include <QApplication>
#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>
#include <QToolBar>
#include <QDockWidget>
#include <QAction>
#include <QPalette>
#include <QIcon>
#include <QTimer>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShader>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QTextBlock>
#include <QRegularExpression>
#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QStatusBar>
#include <QLabel>
#include <QMenuBar>
#include <QInputDialog>
#include <QDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QShortcut>
#include <QKeySequence>
#include <QFont>
#include <QScrollBar>
#include <QMouseEvent>
#include <QTextCursor>
#include <QTextStream>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFormLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QDateTime>
#include <QDir>
#include <cmath>

// -----------------------------
// ProjectManager (simple model + project serialization)
// -----------------------------
struct ProjectTab {
    QString title;
    QString source;
    QString path;
    bool isModified = false;
};

class ProjectManager : public QObject {
    Q_OBJECT
public:
    ProjectManager(QObject* parent = nullptr) : QObject(parent) {}
    int addTab(const QString& title, const QString& source, const QString& path) {
        ProjectTab t; t.title = title; t.source = source; t.path = path; t.isModified = false;
        tabs.append(t);
        return tabs.size()-1;
    }
    ProjectTab tab(int idx) const { return tabs.value(idx); }
    int count() const { return tabs.size(); }
    void setSource(int idx, const QString& src) { if(idx>=0 && idx<tabs.size()) tabs[idx].source = src; }
    void setPath(int idx, const QString& p) { if(idx>=0 && idx<tabs.size()) tabs[idx].path = p; }
    void setModified(int idx, bool m) { if(idx>=0 && idx<tabs.size()) tabs[idx].isModified = m; }

    QJsonObject toJson() const {
        QJsonObject root;
        QJsonArray arr;
        for(const auto &t : tabs) {
            QJsonObject o;
            o["title"] = t.title;
            o["source"] = t.source;
            o["path"] = t.path;
            o["isModified"] = t.isModified;
            arr.append(o);
        }
        root["tabs"] = arr;
        root["savedAt"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        return root;
    }
    bool fromJson(const QJsonObject &root) {
        if(!root.contains("tabs") || !root["tabs"].isArray()) return false;
        tabs.clear();
        QJsonArray arr = root["tabs"].toArray();
        for(const QJsonValue &v : arr) {
            if(!v.isObject()) continue;
            QJsonObject o = v.toObject();
            ProjectTab t;
            t.title = o["title"].toString();
            t.source = o["source"].toString();
            t.path = o["path"].toString();
            t.isModified = o["isModified"].toBool(false);
            tabs.append(t);
        }
        return true;
    }

private:
    QVector<ProjectTab> tabs;
};

// -----------------------------
// TemplatesManager (persistent templates)
// -----------------------------
class TemplatesManager : public QObject {
    Q_OBJECT
public:
    TemplatesManager(QObject* parent = nullptr) : QObject(parent) {
        QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dir);
        templatesFile = dir + "/templates.json";
        load();
    }

    QMap<QString, QString> all() const { return templates; }

    void addTemplate(const QString& name, const QString& code) {
        templates[name] = code;
        save();
    }
    void removeTemplate(const QString& name) {
        templates.remove(name);
        save();
    }
    QString get(const QString& name) const { return templates.value(name); }

    bool save() {
        QJsonObject root;
        for(auto it = templates.begin(); it != templates.end(); ++it) {
            root[it.key()] = it.value();
        }
        QJsonDocument doc(root);
        QFile f(templatesFile);
        if(!f.open(QIODevice::WriteOnly)) return false;
        f.write(doc.toJson(QJsonDocument::Indented));
        f.close();
        return true;
    }

    bool load() {
        QFile f(templatesFile);
        if(!f.exists()) {
            templates["Simple Gradient"] = R"(void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fragCoord / iResolution.xy;
    fragColor = vec4(uv, 0.5 + 0.5 * sin(iTime), 1.0);
})";
            templates["Moving Circle"] = R"(void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = (fragCoord - 0.5 * iResolution.xy) / iResolution.y;
    float r = length(uv - vec2(0.3*sin(iTime), 0.2*cos(iTime)));
    fragColor = vec4(vec3(1.0 - smoothstep(0.1, 0.11, r)), 1.0);
})";
            templates["Plasma"] = R"(void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fragCoord / iResolution.xy;
    float v = sin(uv.x*10.0 + iTime) + sin(uv.y*10.0 + iTime*1.3);
    fragColor = vec4(0.5 + 0.5*sin(v), 0.5 + 0.5*cos(v), 0.5 + 0.5*sin(v*0.5), 1.0);
})";
            save();
            return true;
        }
        if(!f.open(QIODevice::ReadOnly)) return false;
        QByteArray data = f.readAll();
        f.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if(!doc.isObject()) return false;
        QJsonObject root = doc.object();
        templates.clear();
        for(auto it = root.begin(); it != root.end(); ++it) {
            templates[it.key()] = it.value().toString();
        }
        return true;
    }

private:
    QString templatesFile;
    QMap<QString, QString> templates;
};

// -----------------------------
// ErrorPanel (clickable lines)
// -----------------------------
class ErrorPanel : public QWidget {
    Q_OBJECT
public:
    ErrorPanel(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* l = new QVBoxLayout(this);
        view = new QListWidget(this);
        view->setSelectionMode(QAbstractItemView::SingleSelection);
        l->addWidget(view);
        setLayout(l);
        connect(view, &QListWidget::itemClicked, this, &ErrorPanel::onItemClicked);
    }
    void setError(const QString& text) {
        view->clear();
        if(text.isEmpty()) return;
        QStringList lines = text.split('\n');
        for(const QString& ln : lines) {
            QListWidgetItem* it = new QListWidgetItem(ln);
            view->addItem(it);
        }
    }
    void addErrorLine(const QString& line) {
        view->addItem(new QListWidgetItem(line));
    }
signals:
    void lineSelected(int line);
private slots:
    void onItemClicked(QListWidgetItem* it) {
        if(!it) return;
        QString txt = it->text();
        QRegularExpression rx1("0:(\\d+)");
        QRegularExpression rx2("\\((\\d+)\\)");
        int line = -1;
        auto m1 = rx1.match(txt);
        if(m1.hasMatch()) line = m1.captured(1).toInt();
        else {
            auto m2 = rx2.match(txt);
            if(m2.hasMatch()) line = m2.captured(1).toInt();
        }
        if(line > 0) emit lineSelected(line);
    }
private:
    QListWidget* view;
};

// -----------------------------
// GLSL Syntax Highlighter (QRegularExpression)
// -----------------------------
class GLSLSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    GLSLSyntaxHighlighter(QTextDocument* parent = nullptr) : QSyntaxHighlighter(parent) {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(QColor("#7C4DFF"));
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywords = {"\\bvec2\\b","\\bvec3\\b","\\bvec4\\b","\\bmat4\\b","\\buniform\\b","\\bout\\b","\\bin\\b","\\bvoid\\b","\\bfor\\b","\\bif\\b","\\breturn\\b"};
        for(const QString& k: keywords) {
            Rule r;
            r.pattern = QRegularExpression(k);
            r.format = keywordFormat;
            rules.append(r);
        }

        QTextCharFormat builtinFormat;
        builtinFormat.setForeground(QColor("#4FC3F7"));
        QStringList builtins = {"\\bsin\\b","\\bcos\\b","\\btexture\\b","\\blength\\b","\\bnormalize\\b","\\bdot\\b","\\bcross\\b"};
        for(const QString& b: builtins) {
            Rule r;
            r.pattern = QRegularExpression(b);
            r.format = builtinFormat;
            rules.append(r);
        }

        QTextCharFormat numberFormat;
        numberFormat.setForeground(Qt::yellow);
        {
            Rule r;
            r.pattern = QRegularExpression("\\b[0-9]+(\\.[0-9]+)?\\b");
            r.format = numberFormat;
            rules.append(r);
        }

        QTextCharFormat commentFormat;
        commentFormat.setForeground(Qt::darkGreen);
        {
            Rule r;
            r.pattern = QRegularExpression("//[^\\n]*");
            r.format = commentFormat;
            rules.append(r);
        }
        {
            Rule r;
            r.pattern = QRegularExpression("/\\*.*\\*/");
            r.format = commentFormat;
            rules.append(r);
        }
    }
protected:
    void highlightBlock(const QString& text) override {
        for(const Rule& r: rules) {
            QRegularExpressionMatchIterator it = r.pattern.globalMatch(text);
            while(it.hasNext()) {
                QRegularExpressionMatch m = it.next();
                int index = m.capturedStart(0);
                int length = m.capturedLength(0);
                if(index >= 0 && length > 0) setFormat(index, length, r.format);
            }
        }
    }
private:
    struct Rule { QRegularExpression pattern; QTextCharFormat format; };
    QVector<Rule> rules;
};

// -----------------------------
// Forward declare ShaderEditor for LineNumberArea placement later
// -----------------------------
class ShaderEditor;

// -----------------------------
// ShaderEditor with line numbers and small UX features
// -----------------------------
class ShaderEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    ShaderEditor(QWidget* parent = nullptr) : QPlainTextEdit(parent) {
        // lineNumberArea will be created after class definition (see below)
        highlighter = new GLSLSyntaxHighlighter(document());

        QFont f("Consolas");
        f.setPointSize(12);
        setFont(f);
        setTabStopDistance(QFontMetricsF(f).horizontalAdvance(' ') * 4.0);

        connect(this, &QPlainTextEdit::blockCountChanged, this, &ShaderEditor::updateLineNumberAreaWidth);
        connect(this, &QPlainTextEdit::updateRequest, this, &ShaderEditor::updateLineNumberArea);
        connect(this, &QPlainTextEdit::textChanged, [&](){ debounceTimer.start(500); setModified(true); });
        debounceTimer.setSingleShot(true);
        connect(&debounceTimer, &QTimer::timeout, this, &ShaderEditor::onTextChangedDebounced);
        updateLineNumberAreaWidth(0);

        setContextMenuPolicy(Qt::DefaultContextMenu);
        new QShortcut(QKeySequence("Ctrl++"), this, SLOT(zoomInSlot()));
        new QShortcut(QKeySequence("Ctrl+-"), this, SLOT(zoomOutSlot()));
        new QShortcut(QKeySequence("F11"), this, SLOT(toggleFullScreenSlot()));
    }

    // create line number area after construction
    void createLineNumberArea() {
        if(!lineNumberArea) {
            lineNumberArea = new QWidget(this);
            lineNumberArea->setAttribute(Qt::WA_OpaquePaintEvent);
        }
    }

    void highlightLine(int line) {
        QList<QTextEdit::ExtraSelection> extras = extraSelections();
        QList<QTextEdit::ExtraSelection> kept;
        for(const auto& s : extras) {
            if(s.format.background().color() != QColor(255,100,100,120)) kept.append(s);
        }
        if(line > 0) {
            QTextEdit::ExtraSelection sel;
            QColor col = QColor(255, 100, 100, 120);
            sel.format.setBackground(col);
            QTextBlock block = document()->findBlockByNumber(line-1);
            sel.cursor = QTextCursor(block);
            sel.cursor.select(QTextCursor::LineUnderCursor);
            kept.append(sel);
            QTextCursor c = textCursor();
            c.setPosition(block.position());
            setTextCursor(c);
            centerCursor();
        }
        setExtraSelections(kept);
    }

    bool isModifiedFlag() const { return modifiedFlag; }
    void setModified(bool m) { modifiedFlag = m; }

signals:
    void requestCompile(const QString& code);
    void saveRequested();
    void zoomChanged(int delta);

public slots:
    void updateLineNumberAreaWidth(int /*unused*/) { setViewportMargins(lineNumberAreaWidth(), 0, 0, 0); }

protected:
    void resizeEvent(QResizeEvent* e) override {
        QPlainTextEdit::resizeEvent(e);
        if(lineNumberArea) {
            QRect cr = contentsRect();
            lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
        }
    }

    void keyPressEvent(QKeyEvent* e) override {
        if(e->key() == Qt::Key_Tab && e->modifiers() == Qt::NoModifier) {
            int tabWidth = 4;
            insertPlainText(QString(tabWidth, ' '));
            return;
        }
        if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
            QTextCursor c = textCursor();
            c.beginEditBlock();
            c.movePosition(QTextCursor::StartOfLine);
            c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
            QString line = c.selectedText();
            QRegularExpression rx("^\\s+");
            QString indent;
            auto m = rx.match(line);
            if(m.hasMatch()) indent = m.captured(0);
            QPlainTextEdit::keyPressEvent(e);
            insertPlainText(indent);
            c.endEditBlock();
            return;
        }
        QPlainTextEdit::keyPressEvent(e);
    }

    void contextMenuEvent(QContextMenuEvent* event) override {
        QMenu* menu = createStandardContextMenu();
        menu->addSeparator();
        QAction* zoomIn = menu->addAction("Zoom In (Ctrl++)");
        QAction* zoomOut = menu->addAction("Zoom Out (Ctrl+-)");
        connect(zoomIn, &QAction::triggered, this, &ShaderEditor::zoomInSlot);
        connect(zoomOut, &QAction::triggered, this, &ShaderEditor::zoomOutSlot);
        menu->exec(event->globalPos());
        delete menu;
    }

private slots:
    void onTextChangedDebounced() { emit requestCompile(toPlainText()); }
    void updateLineNumberArea(const QRect &rect, int dy) {
        if(lineNumberArea) {
            if(dy) lineNumberArea->scroll(0, dy);
            else lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
            if(rect.contains(viewport()->rect())) updateLineNumberAreaWidth(0);
        }
    }
    void zoomInSlot() { QFont f = font(); f.setPointSize(f.pointSize()+1); setFont(f); updateLineNumberAreaWidth(0); emit zoomChanged(+1); }
    void zoomOutSlot() { QFont f = font(); f.setPointSize(qMax(6, f.pointSize()-1)); setFont(f); updateLineNumberAreaWidth(0); emit zoomChanged(-1); }
    void toggleFullScreenSlot() { QWidget* w = window(); if(w->isFullScreen()) w->showNormal(); else w->showFullScreen(); }

public:
    int lineNumberAreaWidth() {
        int digits = 1;
        int max = qMax(1, blockCount());
        while(max >= 10) { max /= 10; ++digits; }
        int space = 6 + fontMetrics().horizontalAdvance(QString(digits, '9'));
        return space;
    }

    void paintLineNumbers(QPaintEvent* event) {
        if(!lineNumberArea) return;
        QPainter painter(lineNumberArea);
        painter.fillRect(event->rect(), QColor("#333333"));
        QTextBlock block = firstVisibleBlock();
        int blockNumber = block.blockNumber();
        int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
        int bottom = top + (int) blockBoundingRect(block).height();
        while(block.isValid() && top <= event->rect().bottom()) {
            if(block.isVisible() && bottom >= event->rect().top()) {
                QString number = QString::number(blockNumber + 1);
                painter.setPen(Qt::lightGray);
                painter.drawText(0, top, lineNumberArea->width()-4, fontMetrics().height(),
                                 Qt::AlignRight, number);
            }
            block = block.next();
            top = bottom;
            bottom = top + (int) blockBoundingRect(block).height();
            ++blockNumber;
        }
    }

private:
    QTimer debounceTimer;
    QWidget* lineNumberArea = nullptr; // created via createLineNumberArea()
    GLSLSyntaxHighlighter* highlighter;
    bool modifiedFlag = false;
};

// Now define LineNumberArea class (after ShaderEditor so ShaderEditor is a complete type)
class LineNumberArea : public QWidget {
public:
    LineNumberArea(ShaderEditor* editor) : QWidget(editor), codeEditor(editor) {
        setAttribute(Qt::WA_OpaquePaintEvent);
    }
    QSize sizeHint() const override { return QSize(codeEditor->lineNumberAreaWidth(), 0); }
protected:
    void paintEvent(QPaintEvent* event) override {
        codeEditor->paintLineNumbers(event);
    }
private:
    ShaderEditor* codeEditor;
};

// -----------------------------
// GLShaderView (OpenGL rendering) with accessors for textures
// -----------------------------
class GLShaderView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public:
    explicit GLShaderView(QWidget* parent = nullptr)
        : QOpenGLWidget(parent), vbo(QOpenGLBuffer::VertexBuffer) {
        setMinimumSize(400, 300);
        connect(&frameTimer, &QTimer::timeout, this, QOverload<>::of(&GLShaderView::update));
        frameTimer.start(1000/60);
        elapsed.start();
    }
    ~GLShaderView() override {
        makeCurrent();
        delete program;
        for(auto f: fbos) delete f;
        doneCurrent();
    }

    void setUserCode(const QString& code) {
        QMutexLocker locker(&mutex);
        userCode = code;
    }
    void requestCompile() {
        if(isValid()) {
            makeCurrent();
            compileShader();
            doneCurrent();
        }
    }
    void resetTime() { elapsed.restart(); frameCount = 0; }

    void setPaused(bool p) {
        paused = p;
        if(paused) frameTimer.stop();
        else frameTimer.start(1000/60);
    }
    bool isPaused() const { return paused; }

    float lastFPS() const { return fps; }
    int lastFrameCount() const { return frameCount; }

    QImage fboImage(int idx) {
        if(idx < 0 || idx >= fbos.size()) return QImage();
        QOpenGLFramebufferObject* f = fbos[idx];
        if(!f) return QImage();
        makeCurrent();
        QImage img = f->toImage();
        doneCurrent();
        return img;
    }

    QStringList parsedUniforms() const {
        QMutexLocker locker(&mutex);
        return uniformsList;
    }

    void setUniformValueByName(const QString& name, const QVariant& value) {
        if(!program || !compiled) return;
        makeCurrent();
        int loc = program->uniformLocation(name.toStdString().c_str());
        if(loc < 0) { doneCurrent(); return; }
        program->bind();
        if(value.canConvert<double>() || value.canConvert<int>()) {
            program->setUniformValue(name.toStdString().c_str(), (float)value.toDouble());
        } else if(value.canConvert<QVector3D>()) {
            program->setUniformValue(name.toStdString().c_str(), value.value<QVector3D>());
        } else if(value.canConvert<QVector4D>()) {
            program->setUniformValue(name.toStdString().c_str(), value.value<QVector4D>());
        }
        program->release();
        doneCurrent();
    }

signals:
    void compileFailed(const QString& error, int line);
    void compileSucceeded();
    void fpsUpdated(float fps);

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        buildQuad();
        buildFBOs();
        program = new QOpenGLShaderProgram(this);
        {
            QMutexLocker locker(&mutex);
            userCode = "void mainImage(out vec4 fragColor, in vec2 fragCoord) { vec2 uv = fragCoord / iResolution.xy; fragColor = vec4(uv, 0.5+0.5*sin(iTime),1.0); }";
        }
        compileShader();
        connect(&fpsTimer, &QTimer::timeout, this, &GLShaderView::updateFPS);
        fpsTimer.start(500);
    }

    void resizeGL(int w, int h) override {
        Q_UNUSED(w); Q_UNUSED(h);
        for(auto f: fbos) { delete f; }
        fbos.clear();
        buildFBOs();
    }

    void paintGL() override {
        float t = elapsed.elapsed() / 1000.0f;
        float delta = t - lastTime;
        lastTime = t;
        frameCount++;
        if(delta > 0.0f) {
            float currentFPS = 1.0f / delta;
            fpsHistory.push_back(currentFPS);
            if(fpsHistory.size() > 8) fpsHistory.pop_front();
        }

        if(!compiled) {
            glClear(GL_COLOR_BUFFER_BIT);
            return;
        }

        for(int i=0;i<fbos.size();++i){
            auto fbo = fbos[i];
            fbo->bind();
            glViewport(0,0,fbo->width(), fbo->height());
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            program->bind();
            program->setUniformValue("iTime", t);
            program->setUniformValue("iResolution", QVector3D(fbo->width(), fbo->height(), 1.0f));
            program->setUniformValue("iFrame", frameCount);

            vao.bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            vao.release();
            program->release();

            fbo->release();
        }

        glViewport(0,0,width(),height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program->bind();
        program->setUniformValue("iTime", t);
        program->setUniformValue("iResolution", QVector3D(width(), height(), 1.0f));
        program->setUniformValue("iFrame", frameCount);

        for(int i=0;i<fbos.size() && i<4;i++){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, fbos[i]->texture());
            program->setUniformValue(QString("iChannel%1").arg(i).toStdString().c_str(), i);
        }

        vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        vao.release();
        program->release();
    }

private:
    void buildQuad() {
        if(vao.isCreated()) vao.destroy();
        vao.create();
        vao.bind();

        if(vbo.isCreated()) vbo.destroy();
        vbo.create();
        vbo.bind();
        GLfloat verts[] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 1.0f, 1.0f
        };
        vbo.allocate(verts, sizeof(verts));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
        vbo.release();
        vao.release();
    }

    void buildFBOs() {
        int w = width() > 0 ? width() : 800;
        int h = height() > 0 ? height() : 600;
        for(int i=0;i<4;i++){
            QOpenGLFramebufferObjectFormat fmt;
            fmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            fmt.setTextureTarget(GL_TEXTURE_2D);
            fmt.setInternalTextureFormat(GL_RGBA8);
            fbos.append(new QOpenGLFramebufferObject(w, h, fmt));
        }
    }

    QString makeShaderFromUser(const QString& user) {
        QString header = R"(#version 330 core
precision highp float;
uniform float iTime;
uniform vec3 iResolution;
uniform vec4 iMouse;
uniform int iFrame;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
out vec4 FragColor;
)";
        QString footer = R"(
void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec4 color = vec4(0.0);
    mainImage(color, fragCoord);
    FragColor = color;
}
)";
        QMutexLocker locker(&mutex);
        return header + "\n" + user + "\n" + footer;
    }

    void compileShader() {
        if(program) {
            delete program;
            program = new QOpenGLShaderProgram(this);
        } else {
            program = new QOpenGLShaderProgram(this);
        }

        QString frag = makeShaderFromUser(userCode);
        bool ok = program->addShaderFromSourceCode(QOpenGLShader::Fragment, frag);
        if(!ok) {
            QString log = program->log();
            parseCompileLog(log);
            compiled = false;
            return;
        }
        const char* vs = R"(#version 330 core
layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
out vec2 vUV;
void main(){ vUV = uv; gl_Position = vec4(position,0.0,1.0); })";
        ok = program->addShaderFromSourceCode(QOpenGLShader::Vertex, vs);
        if(!ok){
            parseCompileLog(program->log());
            compiled = false;
            return;
        }
        if(!program->link()){
            parseCompileLog(program->log());
            compiled = false;
            return;
        }
        compiled = true;
        parseUniformsFromUserCode();
        emit compileSucceeded();
    }

    void parseCompileLog(const QString& log) {
        int line = -1;
        QRegularExpression rx1("0:(\\d+)");
        auto m1 = rx1.match(log);
        if(m1.hasMatch()) {
            line = m1.captured(1).toInt();
        } else {
            QRegularExpression rx2("\\((\\d+)\\)");
            auto m2 = rx2.match(log);
            if(m2.hasMatch()) line = m2.captured(1).toInt();
        }
        emit compileFailed(log, line);
    }

    void parseUniformsFromUserCode() {
        QMutexLocker locker(&mutex);
        uniformsList.clear();
        QRegularExpression rx("uniform\\s+(\\w+)\\s+(\\w+)\\s*;");
        QRegularExpressionMatchIterator it = rx.globalMatch(userCode);
        while(it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            QString type = m.captured(1);
            QString name = m.captured(2);
            uniformsList.append(QString("%1 %2").arg(type, name));
        }
    }

    void updateFPS() {
        if(fpsHistory.empty()) return;
        float sum = 0.0f;
        for(float v : fpsHistory) sum += v;
        fps = sum / fpsHistory.size();
        emit fpsUpdated(fps);
    }

private:
    QOpenGLShaderProgram* program = nullptr;
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QVector<QOpenGLFramebufferObject*> fbos;
    QString userCode;
    QElapsedTimer elapsed;
    QTimer frameTimer;
    QTimer fpsTimer;
    int frameCount = 0;
    float lastTime = 0.0f;
    bool compiled = false;
    mutable QMutex mutex; // mutable so const methods can lock
    bool paused = false;
    float fps = 0.0f;
    QVector<float> fpsHistory;
    QStringList uniformsList;
};

// -----------------------------
// Templates Dialog (uses TemplatesManager)
// -----------------------------
class TemplatesDialog : public QDialog {
    Q_OBJECT
public:
    TemplatesDialog(TemplatesManager* mgr, QWidget* parent = nullptr) : QDialog(parent), manager(mgr) {
        setWindowTitle("Templates");
        setModal(true);
        QVBoxLayout* l = new QVBoxLayout(this);
        list = new QListWidget(this);
        list->setSelectionMode(QAbstractItemView::SingleSelection);
        l->addWidget(list);

        QHBoxLayout* btnRow = new QHBoxLayout();
        QPushButton* addBtn = new QPushButton("Add", this);
        QPushButton* removeBtn = new QPushButton("Remove", this);
        QPushButton* editBtn = new QPushButton("Edit", this);
        btnRow->addWidget(addBtn);
        btnRow->addWidget(editBtn);
        btnRow->addWidget(removeBtn);
        l->addLayout(btnRow);

        QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        l->addWidget(bb);
        connect(bb, &QDialogButtonBox::accepted, this, &TemplatesDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, this, &TemplatesDialog::reject);
        connect(addBtn, &QPushButton::clicked, this, &TemplatesDialog::onAdd);
        connect(removeBtn, &QPushButton::clicked, this, &TemplatesDialog::onRemove);
        connect(editBtn, &QPushButton::clicked, this, &TemplatesDialog::onEdit);

        reloadList();
    }

    QString selectedTemplate() const {
        QString key = list->currentItem() ? list->currentItem()->text() : QString();
        return manager->all().value(key);
    }

private slots:
    void reloadList() {
        list->clear();
        for(auto it = manager->all().begin(); it != manager->all().end(); ++it) {
            list->addItem(it.key());
        }
    }
    void onAdd() {
        bool ok;
        QString name = QInputDialog::getText(this, "Template Name", "Name:", QLineEdit::Normal, QString(), &ok);
        if(!ok || name.isEmpty()) return;
        QString code = QInputDialog::getMultiLineText(this, "Template Code", "GLSL code:", QString(), &ok);
        if(!ok) return;
        manager->addTemplate(name, code);
        reloadList();
    }
    void onRemove() {
        QListWidgetItem* it = list->currentItem();
        if(!it) return;
        QString name = it->text();
        manager->removeTemplate(name);
        reloadList();
    }
    void onEdit() {
        QListWidgetItem* it = list->currentItem();
        if(!it) return;
        QString name = it->text();
        QString code = manager->get(name);
        bool ok;
        QString newCode = QInputDialog::getMultiLineText(this, "Edit Template", "GLSL code:", code, &ok);
        if(!ok) return;
        manager->addTemplate(name, newCode);
        reloadList();
    }

private:
    TemplatesManager* manager;
    QListWidget* list;
};

// -----------------------------
// Uniform Inspector
// -----------------------------
class UniformInspector : public QWidget {
    Q_OBJECT
public:
    UniformInspector(GLShaderView* view, QWidget* parent = nullptr) : QWidget(parent), gl(view) {
        QVBoxLayout* l = new QVBoxLayout(this);
        info = new QLabel("Uniforms", this);
        l->addWidget(info);
        form = new QFormLayout();
        l->addLayout(form);
        refreshBtn = new QPushButton("Refresh", this);
        l->addWidget(refreshBtn);
        setLayout(l);
        connect(refreshBtn, &QPushButton::clicked, this, &UniformInspector::refresh);
        refresh();
    }

public slots:
    void refresh() {
        QLayoutItem* child;
        while((child = form->takeAt(0)) != nullptr) {
            if(child->widget()) { delete child->widget(); }
            delete child;
        }
        if(!gl) return;
        QStringList list = gl->parsedUniforms();
        for(const QString& entry : list) {
            QStringList parts = entry.split(' ', Qt::SkipEmptyParts);
            if(parts.size() < 2) continue;
            QString type = parts[0];
            QString name = parts[1];
            if(type == "float" || type == "int") {
                QLineEdit* le = new QLineEdit("0.0", this);
                form->addRow(name, le);
                connect(le, &QLineEdit::editingFinished, this, [this, name, le](){
                    bool ok; double v = le->text().toDouble(&ok);
                    if(ok) gl->setUniformValueByName(name, QVariant(v));
                });
            } else if(type == "vec3") {
                QWidget* w = new QWidget(this);
                QHBoxLayout* hl = new QHBoxLayout(w);
                QLineEdit* x = new QLineEdit("0.0", w);
                QLineEdit* y = new QLineEdit("0.0", w);
                QLineEdit* z = new QLineEdit("0.0", w);
                x->setMaximumWidth(60); y->setMaximumWidth(60); z->setMaximumWidth(60);
                hl->addWidget(x); hl->addWidget(y); hl->addWidget(z);
                form->addRow(name, w);
                connect(x, &QLineEdit::editingFinished, this, [this, name, x, y, z](){ applyVec3(name, x, y, z); });
                connect(y, &QLineEdit::editingFinished, this, [this, name, x, y, z](){ applyVec3(name, x, y, z); });
                connect(z, &QLineEdit::editingFinished, this, [this, name, x, y, z](){ applyVec3(name, x, y, z); });
            } else if(type == "vec4") {
                QWidget* w = new QWidget(this);
                QHBoxLayout* hl = new QHBoxLayout(w);
                QLineEdit* a = new QLineEdit("0.0", w);
                QLineEdit* b = new QLineEdit("0.0", w);
                QLineEdit* c = new QLineEdit("0.0", w);
                QLineEdit* d = new QLineEdit("0.0", w);
                a->setMaximumWidth(50); b->setMaximumWidth(50); c->setMaximumWidth(50); d->setMaximumWidth(50);
                hl->addWidget(a); hl->addWidget(b); hl->addWidget(c); hl->addWidget(d);
                form->addRow(name, w);
                connect(a, &QLineEdit::editingFinished, this, [this, name, a, b, c, d](){ applyVec4(name, a, b, c, d); });
                connect(b, &QLineEdit::editingFinished, this, [this, name, a, b, c, d](){ applyVec4(name, a, b, c, d); });
                connect(c, &QLineEdit::editingFinished, this, [this, name, a, b, c, d](){ applyVec4(name, a, b, c, d); });
                connect(d, &QLineEdit::editingFinished, this, [this, name, a, b, c, d](){ applyVec4(name, a, b, c, d); });
            } else {
                QLabel* lbl = new QLabel(QString("%1 (%2)").arg(name, type), this);
                form->addRow(lbl);
            }
        }
    }

private:
    void applyVec3(const QString& name, QLineEdit* x, QLineEdit* y, QLineEdit* z) {
        bool ok1, ok2, ok3;
        float vx = x->text().toFloat(&ok1);
        float vy = y->text().toFloat(&ok2);
        float vz = z->text().toFloat(&ok3);
        if(ok1 && ok2 && ok3) {
            gl->setUniformValueByName(name, QVariant::fromValue(QVector3D(vx, vy, vz)));
        }
    }
    void applyVec4(const QString& name, QLineEdit* a, QLineEdit* b, QLineEdit* c, QLineEdit* d) {
        bool ok1, ok2, ok3, ok4;
        float v1 = a->text().toFloat(&ok1);
        float v2 = b->text().toFloat(&ok2);
        float v3 = c->text().toFloat(&ok3);
        float v4 = d->text().toFloat(&ok4);
        if(ok1 && ok2 && ok3 && ok4) {
            QVector4D vec(v1,v2,v3,v4);
            QVariant var; var.setValue(vec);
            gl->setUniformValueByName(name, var);
        }
    }

    GLShaderView* gl = nullptr;
    QLabel* info;
    QFormLayout* form;
    QPushButton* refreshBtn;
};

// -----------------------------
// Texture Viewer (shows FBO textures iChannel0..3)
// -----------------------------
class TextureViewer : public QWidget {
    Q_OBJECT
public:
    TextureViewer(GLShaderView* view, QWidget* parent = nullptr) : QWidget(parent), gl(view) {
        QVBoxLayout* l = new QVBoxLayout(this);
        QHBoxLayout* row = new QHBoxLayout();
        for(int i=0;i<4;i++){
            QLabel* lbl = new QLabel(QString("iChannel%1").arg(i), this);
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setMinimumSize(160, 120);
            lbl->setFrameShape(QFrame::Box);
            lbl->setStyleSheet("background:#111111; color:#ddd;");
            images.append(lbl);
            row->addWidget(lbl);
        }
        l->addLayout(row);
        QHBoxLayout* btnRow = new QHBoxLayout();
        QPushButton* refreshBtn = new QPushButton("Refresh", this);
        btnRow->addWidget(refreshBtn);
        QPushButton* saveBtn = new QPushButton("Save Selected", this);
        btnRow->addWidget(saveBtn);
        l->addLayout(btnRow);
        setLayout(l);
        connect(refreshBtn, &QPushButton::clicked, this, &TextureViewer::refresh);
        connect(saveBtn, &QPushButton::clicked, this, &TextureViewer::saveSelected);
        refresh();
    }

public slots:
    void refresh() {
        if(!gl) return;
        for(int i=0;i<4;i++){
            QImage img = gl->fboImage(i);
            if(img.isNull()) {
                images[i]->setText(QString("iChannel%1\n(empty)").arg(i));
                images[i]->setPixmap(QPixmap());
            } else {
                QPixmap pm = QPixmap::fromImage(img.scaled(images[i]->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                images[i]->setPixmap(pm);
                images[i]->setText(QString());
            }
        }
    }

    void saveSelected() {
        for(int i=0;i<4;i++){
            QImage img = gl->fboImage(i);
            if(!img.isNull()) {
                QString fn = QFileDialog::getSaveFileName(this, QString("Save iChannel%1").arg(i), QString(), "PNG Image (*.png)");
                if(fn.isEmpty()) return;
                img.save(fn);
                QMessageBox::information(this, "Saved", "Saved texture to " + fn);
                return;
            }
        }
        QMessageBox::warning(this, "Save", "No texture available to save.");
    }

private:
    GLShaderView* gl = nullptr;
    QVector<QLabel*> images;
};

// -----------------------------
// MainWindow
// -----------------------------
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        QPalette pal;
        pal.setColor(QPalette::Window, QColor("#2b2b2b"));
        pal.setColor(QPalette::WindowText, Qt::white);
        pal.setColor(QPalette::Base, QColor("#222222"));
        pal.setColor(QPalette::AlternateBase, QColor("#2b2b2b"));
        pal.setColor(QPalette::ToolTipBase, Qt::white);
        pal.setColor(QPalette::ToolTipText, Qt::white);
        pal.setColor(QPalette::Text, Qt::white);
        pal.setColor(QPalette::Button, QColor("#3c3c3c"));
        pal.setColor(QPalette::ButtonText, Qt::white);
        setPalette(pal);

        splitter = new QSplitter(Qt::Horizontal, this);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setTabsClosable(true);
        glView = new GLShaderView(splitter);
        splitter->addWidget(tabWidget);
        splitter->addWidget(glView);
        splitter->setStretchFactor(1, 1);
        setCentralWidget(splitter);

        projectManager = new ProjectManager(this);
        templatesManager = new TemplatesManager(this);
        errorPanel = new ErrorPanel(this);

        QMenu* fileMenu = menuBar()->addMenu("&File");
        QAction* openAct = fileMenu->addAction("Open...", QKeySequence::Open);
        QAction* saveAct = fileMenu->addAction("Save", QKeySequence::Save);
        QAction* saveAsAct = fileMenu->addAction("Save As...", QKeySequence::SaveAs);
        QAction* newAct = fileMenu->addAction("New", QKeySequence::New);
        fileMenu->addSeparator();
        QAction* saveProjectAct = fileMenu->addAction("Save Project...");
        QAction* loadProjectAct = fileMenu->addAction("Load Project...");
        QAction* autosaveToggle = fileMenu->addAction("Toggle Autosave");
        fileMenu->addSeparator();
        QAction* exitAct = fileMenu->addAction("Exit", QKeySequence::Quit);

        connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
        connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);
        connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveFileAs);
        connect(newAct, &QAction::triggered, this, &MainWindow::newTab);
        connect(exitAct, &QAction::triggered, this, &MainWindow::close);
        connect(saveProjectAct, &QAction::triggered, this, &MainWindow::saveProject);
        connect(loadProjectAct, &QAction::triggered, this, &MainWindow::loadProject);
        connect(autosaveToggle, &QAction::triggered, this, &MainWindow::toggleAutosave);

        QToolBar* tb = addToolBar("Main");
        tb->setMovable(false);
        playAction = tb->addAction(QIcon::fromTheme("media-playback-start"), "Play/Pause");
        playAction->setToolTip("Play/Pause (Space)");
        resetAction = tb->addAction(QIcon::fromTheme("view-refresh"), "Reset");
        templatesAction = tb->addAction(QIcon::fromTheme("document-new"), "Templates");
        QAction* openAction = tb->addAction(QIcon::fromTheme("document-open"), "Open");
        QAction* saveAction = tb->addAction(QIcon::fromTheme("document-save"), "Save");
        QAction* fullscreenAction = tb->addAction(QIcon::fromTheme("view-fullscreen"), "Fullscreen");

        playAction->setCheckable(true);
        playAction->setChecked(true);

        connect(playAction, &QAction::triggered, this, &MainWindow::togglePlayPause);
        connect(resetAction, &QAction::triggered, this, &MainWindow::resetTime);
        connect(templatesAction, &QAction::triggered, this, &MainWindow::openTemplates);
        connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
        connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
        connect(fullscreenAction, &QAction::triggered, this, &MainWindow::toggleFullScreen);

        QDockWidget* dock = new QDockWidget("Errors", this);
        dock->setWidget(errorPanel);
        dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
        addDockWidget(Qt::BottomDockWidgetArea, dock);

        uniformInspector = new UniformInspector(glView, this);
        QDockWidget* uniformDock = new QDockWidget("Uniforms", this);
        uniformDock->setWidget(uniformInspector);
        addDockWidget(Qt::RightDockWidgetArea, uniformDock);

        textureViewer = new TextureViewer(glView, this);
        QDockWidget* texDock = new QDockWidget("Textures", this);
        texDock->setWidget(textureViewer);
        addDockWidget(Qt::RightDockWidgetArea, texDock);

        connect(templatesAction, &QAction::triggered, this, &MainWindow::openTemplates);

        statusFPS = new QLabel("FPS: 0");
        statusCompile = new QLabel("Ready");
        statusFrame = new QLabel("Frame: 0");
        statusBar()->addPermanentWidget(statusFrame);
        statusBar()->addPermanentWidget(statusFPS);
        statusBar()->addPermanentWidget(statusCompile);

        ShaderEditor* editor = createEditorTab("untitled.glsl");
        editor->createLineNumberArea(); // create the line number area now that editor exists
        tabWidget->addTab(editor, "untitled.glsl");
        projectManager->addTab("untitled.glsl", "", QString());
        connect(editor, &ShaderEditor::requestCompile, this, &MainWindow::onRequestCompile);
        connect(glView, &GLShaderView::compileFailed, this, &MainWindow::onCompileFailed);
        connect(glView, &GLShaderView::compileSucceeded, this, &MainWindow::onCompileSucceeded);
        connect(glView, &GLShaderView::fpsUpdated, this, &MainWindow::onFPSUpdated);
        connect(errorPanel, &ErrorPanel::lineSelected, this, &MainWindow::onErrorLineSelected);
        connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
        connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);

        new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(togglePlayPause()));
        new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(toggleFullScreen()));

        editor->setPlainText(R"(// simple shader
void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fragCoord / iResolution.xy;
    fragColor = vec4(uv, 0.5 + 0.5 * sin(iTime), 1.0);
})");
        QTimer::singleShot(150, [this, editor](){ onRequestCompile(editor->toPlainText()); });

        autosaveEnabled = true;
        autosaveTimer = new QTimer(this);
        connect(autosaveTimer, &QTimer::timeout, this, &MainWindow::autosave);
        autosaveTimer->start(60000);

        resize(1400, 900);
    }

protected:
    void closeEvent(QCloseEvent* ev) override {
        if(autosaveEnabled) autosave();
        QMainWindow::closeEvent(ev);
    }

private slots:
    void onRequestCompile(const QString& code) {
        statusCompile->setText("Compiling...");
        glView->setUserCode(code);
        glView->requestCompile();
    }

    void onCompileFailed(const QString& error, int line) {
        statusCompile->setText("Compile Error");
        errorPanel->setError(error);
        ShaderEditor* ed = currentEditor();
        if(ed) ed->highlightLine(line);
    }

    void onCompileSucceeded() {
        statusCompile->setText("Compiled");
        errorPanel->setError(QString());
        uniformInspector->refresh();
        textureViewer->refresh();
    }

    void onFPSUpdated(float fps) {
        statusFPS->setText(QString("FPS: %1").arg(QString::number(fps, 'f', 1)));
        statusFrame->setText(QString("Frame: %1").arg(glView->lastFrameCount()));
    }

    void onErrorLineSelected(int line) {
        ShaderEditor* ed = currentEditor();
        if(ed) ed->highlightLine(line);
    }

    void onTabChanged(int idx) {
        Q_UNUSED(idx);
        ShaderEditor* ed = currentEditor();
        if(ed) {
            glView->setUserCode(ed->toPlainText());
            glView->requestCompile();
        }
    }

    void onTabCloseRequested(int idx) {
        QWidget* w = tabWidget->widget(idx);
        ShaderEditor* ed = qobject_cast<ShaderEditor*>(w);
        if(ed) {
            tabWidget->removeTab(idx);
            ed->deleteLater();
        }
    }

    void togglePlayPause() {
        bool was = glView->isPaused();
        glView->setPaused(!was);
        playAction->setChecked(!was);
        playAction->setIcon(QIcon::fromTheme(was ? "media-playback-start" : "media-playback-pause"));
        playAction->setText(was ? "Play" : "Pause");
    }

    void resetTime() { glView->resetTime(); }

    void openTemplates() {
        TemplatesDialog dlg(templatesManager, this);
        if(dlg.exec() == QDialog::Accepted) {
            QString templ = dlg.selectedTemplate();
            if(!templ.isEmpty()) {
                ShaderEditor* ed = currentEditor();
                if(ed) {
                    ed->setPlainText(templ);
                    onRequestCompile(templ);
                }
            }
        }
    }

    void openFile() {
        QString fn = QFileDialog::getOpenFileName(this, "Open GLSL", QString(), "GLSL Files (*.glsl *.frag);;All Files (*)");
        if(fn.isEmpty()) return;
        QFile f(fn);
        if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Open", "Cannot open file.");
            return;
        }
        QTextStream in(&f);
        QString content = in.readAll();
        ShaderEditor* ed = createEditorTab(QFileInfo(fn).fileName());
        ed->createLineNumberArea();
        int idx = tabWidget->addTab(ed, QFileInfo(fn).fileName());
        projectManager->addTab(QFileInfo(fn).fileName(), content, fn);
        ed->setPlainText(content);
        connect(ed, &ShaderEditor::requestCompile, this, &MainWindow::onRequestCompile);
        tabWidget->setCurrentIndex(idx);
        onRequestCompile(content);
    }

    void saveFile() {
        ShaderEditor* ed = currentEditor();
        if(!ed) return;
        int idx = tabWidget->currentIndex();
        ProjectTab pt = projectManager->tab(idx);
        if(pt.path.isEmpty()) {
            saveFileAs();
            return;
        }
        QFile f(pt.path);
        if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Save", "Cannot save file.");
            return;
        }
        QTextStream out(&f);
        out << ed->toPlainText();
        projectManager->setModified(idx, false);
        statusCompile->setText("Saved");
    }

    void saveFileAs() {
        ShaderEditor* ed = currentEditor();
        if(!ed) return;
        QString fn = QFileDialog::getSaveFileName(this, "Save GLSL", QString(), "GLSL Files (*.glsl *.frag);;All Files (*)");
        if(fn.isEmpty()) return;
        QFile f(fn);
        if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Save As", "Cannot save file.");
            return;
        }
        QTextStream out(&f);
        out << ed->toPlainText();
        int idx = tabWidget->currentIndex();
        projectManager->setPath(idx, fn);
        projectManager->setModified(idx, false);
        tabWidget->setTabText(idx, QFileInfo(fn).fileName());
        statusCompile->setText("Saved");
    }

    void newTab() {
        ShaderEditor* ed = createEditorTab("untitled.glsl");
        ed->createLineNumberArea();
        int idx = tabWidget->addTab(ed, "untitled.glsl");
        projectManager->addTab("untitled.glsl", "", QString());
        connect(ed, &ShaderEditor::requestCompile, this, &MainWindow::onRequestCompile);
        tabWidget->setCurrentIndex(idx);
    }

    void toggleFullScreen() {
        if(isFullScreen()) showNormal();
        else showFullScreen();
    }

    void saveProject() {
        QString fn = QFileDialog::getSaveFileName(this, "Save Project", QString(), "Gleditor Project (*.gproj);;JSON Files (*.json)");
        if(fn.isEmpty()) return;
        for(int i=0;i<tabWidget->count();++i) {
            QWidget* w = tabWidget->widget(i);
            ShaderEditor* ed = qobject_cast<ShaderEditor*>(w);
            if(ed) {
                projectManager->setSource(i, ed->toPlainText());
            }
        }
        QJsonObject root = projectManager->toJson();
        QJsonDocument doc(root);
        QFile f(fn);
        if(!f.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "Save Project", "Cannot save project file.");
            return;
        }
        f.write(doc.toJson(QJsonDocument::Indented));
        f.close();
        statusCompile->setText("Project saved");
    }

    void loadProject() {
        QString fn = QFileDialog::getOpenFileName(this, "Load Project", QString(), "Gleditor Project (*.gproj *.json)");
        if(fn.isEmpty()) return;
        QFile f(fn);
        if(!f.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Load Project", "Cannot open project file.");
            return;
        }
        QByteArray data = f.readAll();
        f.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if(!doc.isObject()) {
            QMessageBox::warning(this, "Load Project", "Invalid project file.");
            return;
        }
        if(!projectManager->fromJson(doc.object())) {
            QMessageBox::warning(this, "Load Project", "Failed to parse project.");
            return;
        }
        while(tabWidget->count() > 0) {
            QWidget* w = tabWidget->widget(0);
            tabWidget->removeTab(0);
            w->deleteLater();
        }
        for(int i=0;i<projectManager->count();++i) {
            ProjectTab pt = projectManager->tab(i);
            ShaderEditor* ed = createEditorTab(pt.title);
            ed->createLineNumberArea();
            tabWidget->addTab(ed, pt.title);
            ed->setPlainText(pt.source);
            connect(ed, &ShaderEditor::requestCompile, this, &MainWindow::onRequestCompile);
        }
        if(tabWidget->count() > 0) tabWidget->setCurrentIndex(0);
        statusCompile->setText("Project loaded");
    }

    void autosave() {
        if(!autosaveEnabled) return;
        for(int i=0;i<tabWidget->count();++i) {
            QWidget* w = tabWidget->widget(i);
            ShaderEditor* ed = qobject_cast<ShaderEditor*>(w);
            if(ed) {
                projectManager->setSource(i, ed->toPlainText());
            }
        }
        QJsonObject root = projectManager->toJson();
        QJsonDocument doc(root);
        QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dir);
        QString fn = dir + "/autosave.gproj";
        QFile f(fn);
        if(!f.open(QIODevice::WriteOnly)) return;
        f.write(doc.toJson(QJsonDocument::Indented));
        f.close();
        statusCompile->setText("Autosaved");
    }

    void toggleAutosave() {
        autosaveEnabled = !autosaveEnabled;
        if(autosaveEnabled) autosaveTimer->start(60000);
        else autosaveTimer->stop();
        statusCompile->setText(autosaveEnabled ? "Autosave ON" : "Autosave OFF");
    }

private:
    ShaderEditor* createEditorTab(const QString& title) {
        ShaderEditor* editor = new ShaderEditor(this);
        return editor;
    }

    ShaderEditor* currentEditor() {
        return qobject_cast<ShaderEditor*>(tabWidget->currentWidget());
    }

    QSplitter* splitter = nullptr;
    QTabWidget* tabWidget = nullptr;
    GLShaderView* glView = nullptr;
    ProjectManager* projectManager = nullptr;
    TemplatesManager* templatesManager = nullptr;
    ErrorPanel* errorPanel = nullptr;
    UniformInspector* uniformInspector = nullptr;
    TextureViewer* textureViewer = nullptr;
    QLabel* statusFPS = nullptr;
    QLabel* statusCompile = nullptr;
    QLabel* statusFrame = nullptr;
    QAction* playAction = nullptr;
    QAction* resetAction = nullptr;
    QAction* templatesAction = nullptr;

    QTimer* autosaveTimer = nullptr;
    bool autosaveEnabled = true;
};

// -----------------------------
// main
// -----------------------------
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("GleditorPro");
    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"
