// gitpush1.cpp
// Qt6 + libgit2 single-file tool with GUI (auto-start, login dialog, save credentials, progress, instantaneous speed, detailed logging)
// Implements explicit git workflow using libgit2 and checks for existing GitHub repo before creating it.
// On successful push the program exits with code 0.
//
// IMPORTANT: File must be named gitpush1.cpp so AUTOMOC generates gitpush1.moc correctly.
// Single moc include is at the very end of this file.

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QTextEdit>
#include <QCommandLineParser>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QElapsedTimer>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QPushButton>

#include <git2.h>
#include <cstring>
#include <atomic>

// ---------- Robust HTTP helpers (POST/GET) ----------
static bool postJsonSyncWithDebug(QNetworkAccessManager &mgr,
                                  const QUrl &url,
                                  const QByteArray &json,
                                  const QByteArray &authHeader, // e.g. "token <PAT>"
                                  int timeoutMs,
                                  int &httpStatus,
                                  QByteArray &responseBody,
                                  QString &networkErrorString)
{
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!authHeader.isEmpty()) req.setRawHeader("Authorization", authHeader);
    req.setRawHeader("User-Agent", QByteArray("gitpush/1.0"));

    QNetworkReply *reply = mgr.post(req, json);
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();

    if (!timer.isActive()) {
        reply->abort();
        responseBody = reply->readAll();
        httpStatus = 0;
        networkErrorString = QStringLiteral("Timeout beim Warten auf Antwort");
        reply->deleteLater();
        return false;
    }

    responseBody = reply->readAll();
    QVariant status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    httpStatus = status.isValid() ? status.toInt() : 0;

    if (reply->error() != QNetworkReply::NoError) {
        networkErrorString = reply->errorString();
    } else {
        networkErrorString.clear();
    }

    reply->deleteLater();
    return (httpStatus >= 200 && httpStatus < 300);
}

static bool getJsonSyncWithDebug(QNetworkAccessManager &mgr,
                                 const QUrl &url,
                                 const QByteArray &authHeader,
                                 int timeoutMs,
                                 int &httpStatus,
                                 QByteArray &responseBody,
                                 QString &networkErrorString)
{
    QNetworkRequest req(url);
    if (!authHeader.isEmpty()) req.setRawHeader("Authorization", authHeader);
    req.setRawHeader("User-Agent", QByteArray("gitpush/1.0"));

    QNetworkReply *reply = mgr.get(req);
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();

    if (!timer.isActive()) {
        reply->abort();
        responseBody = reply->readAll();
        httpStatus = 0;
        networkErrorString = QStringLiteral("Timeout beim Warten auf Antwort");
        reply->deleteLater();
        return false;
    }

    responseBody = reply->readAll();
    QVariant status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    httpStatus = status.isValid() ? status.toInt() : 0;

    if (reply->error() != QNetworkReply::NoError) {
        networkErrorString = reply->errorString();
    } else {
        networkErrorString.clear();
    }

    reply->deleteLater();
    return (httpStatus >= 200 && httpStatus < 300);
}

// ---------- libgit2 credential callback ----------
struct CredContext {
    QByteArray token;
};

static int credentials_cb(git_cred **out,
                          const char *url,
                          const char *username_from_url,
                          unsigned int allowed_types,
                          void *payload)
{
    Q_UNUSED(url);
    Q_UNUSED(username_from_url);
    CredContext *ctx = static_cast<CredContext*>(payload);

    if (allowed_types & GIT_CREDTYPE_USERPASS_PLAINTEXT) {
        const char *user = "x-access-token";
        const char *pass = ctx->token.constData();
        return git_cred_userpass_plaintext_new(out, user, pass);
    }
    return GIT_PASSTHROUGH;
}

// ---------- Login dialog ----------
class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("GitHub Login");
        setModal(true);
        QFormLayout *form = new QFormLayout(this);
        m_userEdit = new QLineEdit(this);
        m_tokenEdit = new QLineEdit(this);
        m_tokenEdit->setEchoMode(QLineEdit::Password);
        m_saveCheck = new QCheckBox("Credentials speichern", this);
        m_saveCheck->setChecked(true);
        form->addRow("GitHub Benutzername:", m_userEdit);
        form->addRow("Personal Access Token:", m_tokenEdit);
        form->addRow("", m_saveCheck);
        QHBoxLayout *h = new QHBoxLayout;
        QPushButton *ok = new QPushButton("Login", this);
        QPushButton *cancel = new QPushButton("Abbrechen", this);
        h->addWidget(ok);
        h->addWidget(cancel);
        form->addRow(h);
        connect(ok, &QPushButton::clicked, this, &LoginDialog::onOk);
        connect(cancel, &QPushButton::clicked, this, &LoginDialog::reject);
    }

    QString username() const { return m_userEdit->text().trimmed(); }
    QString token() const { return m_tokenEdit->text(); }
    bool saveCredentials() const { return m_saveCheck->isChecked(); }

    void setUsername(const QString &u) { m_userEdit->setText(u); }
    void setToken(const QString &t) { m_tokenEdit->setText(t); }

private slots:
    void onOk() {
        if (username().isEmpty() || token().isEmpty()) {
            QMessageBox::warning(this, "Fehler", "Benutzername und Token dürfen nicht leer sein.");
            return;
        }
        accept();
    }

private:
    QLineEdit *m_userEdit;
    QLineEdit *m_tokenEdit;
    QCheckBox *m_saveCheck;
};

// Forward declaration for CombinedPayload used in callbacks
struct CombinedPayload;

// ---------- Worker (runs in background thread) ----------
class GitPushWorker : public QObject {
    Q_OBJECT
public:
    GitPushWorker(const QString &dirPath,
                  const QString &githubUser,
                  const QString &token,
                  bool isPrivate)
        : m_dirPath(dirPath), m_githubUser(githubUser), m_token(token), m_isPrivate(isPrivate),
          m_lastBytes(0), m_firstProgress(true)
    {
        m_speedTimer.invalidate();
    }

signals:
    void progressUpdated(qint64 transferred, qint64 total, double bytesPerSec);
    void message(const QString &text);
    void finished(int exitCode);

public slots:
    void run() {
        emit message(QStringLiteral("[%1] START: gitpush for '%2'").arg(timestamp(), m_dirPath));

        // Prepare network manager and auth header
        QNetworkAccessManager mgr;
        QByteArray authHeader = QString("token %1").arg(m_token).toUtf8();
        QString repoName = QFileInfo(m_dirPath).fileName();
        QUrl repoCheckUrl(QStringLiteral("https://api.github.com/repos/%1/%2").arg(m_githubUser, repoName));

        // 0) Check if repository already exists on GitHub. If yes, skip creation.
        int checkStatus = 0;
        QByteArray checkResp;
        QString checkNetErr;
        bool repoExists = false;
        if (getJsonSyncWithDebug(mgr, repoCheckUrl, authHeader, 10000, checkStatus, checkResp, checkNetErr)) {
            // 2xx -> exists
            repoExists = true;
            emit message(QStringLiteral("[%1] Repository already exists on GitHub: %2 (HTTP %3)").arg(timestamp()).arg(repoName).arg(QString::number(checkStatus)));
        } else {
            // Not 2xx: if 404 then it doesn't exist; otherwise log and continue to attempt creation
            if (checkStatus == 404) {
                emit message(QStringLiteral("[%1] Repository does not exist yet on GitHub: %2").arg(timestamp()).arg(repoName));
                repoExists = false;
            } else {
                // network error or other status; log and proceed to attempt creation
                QString details = QStringLiteral("HTTP %1; network: %2; body: %3")
                                      .arg(QString::number(checkStatus))
                                      .arg(checkNetErr)
                                      .arg(QString::fromUtf8(checkResp));
                emit message(QStringLiteral("[%1] Warning checking repo existence: %2").arg(timestamp()).arg(details));
                repoExists = false;
            }
        }

        // 1) create repo on GitHub (use robust helper) only if it doesn't exist
        if (!repoExists) {
            emit message(QStringLiteral("[%1] Creating repository on GitHub: %2").arg(timestamp()).arg(repoName));
            QUrl createUrl("https://api.github.com/user/repos");
            QJsonObject body;
            body["name"] = repoName;
            body["private"] = m_isPrivate;
            body["auto_init"] = false;
            QByteArray json = QJsonDocument(body).toJson();

            int httpStatus = 0;
            QByteArray resp;
            QString netErr;
            if (!postJsonSyncWithDebug(mgr, createUrl, json, authHeader, 30000, httpStatus, resp, netErr)) {
                QString details = QStringLiteral("HTTP %1; network: %2; body: %3")
                                      .arg(QString::number(httpStatus))
                                      .arg(netErr)
                                      .arg(QString::fromUtf8(resp));
                emit message(QStringLiteral("[%1] ERROR: Repository creation failed: %2").arg(timestamp()).arg(details));
                emit finished(4);
                return;
            }
            emit message(QStringLiteral("[%1] Repository created on GitHub: %2 (HTTP %3)").arg(timestamp()).arg(repoName).arg(QString::number(httpStatus)));
        }

        // 2) libgit2 init/open
        emit message(QStringLiteral("[%1] Initializing/opening local repository...").arg(timestamp()));
        git_libgit2_init();
        git_repository *repo = nullptr;
        int error = git_repository_open(&repo, m_dirPath.toUtf8().constData());
        if (error == GIT_ENOTFOUND) {
            // git init
            error = git_repository_init(&repo, m_dirPath.toUtf8().constData(), 0);
            if (error) {
                emit message(QStringLiteral("[%1] ERROR: git_repository_init failed: %2").arg(timestamp()).arg(QString::number(error)));
                git_libgit2_shutdown();
                emit finished(5);
                return;
            }
            emit message(QStringLiteral("[%1] Local repository initialized.").arg(timestamp()));
        } else if (error) {
            emit message(QStringLiteral("[%1] ERROR: git_repository_open failed: %2").arg(timestamp()).arg(QString::number(error)));
            git_libgit2_shutdown();
            emit finished(5);
            return;
        } else {
            emit message(QStringLiteral("[%1] Local repository opened.").arg(timestamp()));
        }

        // 3) add files to index (git add .)
        emit message(QStringLiteral("[%1] Adding files to index...").arg(timestamp()));
        git_index *index = nullptr;
        error = git_repository_index(&index, repo);
        if (error) {
            emit message(QStringLiteral("[%1] ERROR: git_repository_index: %2").arg(timestamp()).arg(QString::number(error)));
            git_repository_free(repo);
            git_libgit2_shutdown();
            emit finished(6);
            return;
        }

        int added = 0;
        const QString repoBaseName = QFileInfo(m_dirPath).fileName();
        QDirIterator it(m_dirPath, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString filePath = it.next();
            // compute path relative to repo root
            QString rel = QDir(m_dirPath).relativeFilePath(filePath);

            // If relativeFilePath returned something that still starts with the repo folder name,
            // strip it (happens if user passed parent dir by mistake)
            if (rel.startsWith(repoBaseName + QDir::separator())) {
                rel = rel.mid(repoBaseName.length() + 1);
            }

            // Normalize separators to forward slash for git
            rel.replace(QDir::separator(), '/');

            // Skip common build/generated directories and files
            if (rel.startsWith(".git/") ||
                rel.contains("/.git/") ||
                rel.contains("/build/") ||
                rel.contains("/cmake.run") ||
                rel.contains("/CMakeFiles/") ||
                rel.contains("/autogen/") ||
                rel.contains("_autogen/") ||
                rel.contains("/moc_") ||
                rel.contains("/mocs_compilation") ||
                rel.endsWith(".o") ||
                rel.endsWith(".so") ||
                rel.endsWith(".a") ||
                rel.endsWith(".obj") ||
                rel.contains("/node_modules/") ||
                rel.contains("/.cache/") )
            {
                emit message(QStringLiteral("[%1] Skipping generated/build file: %2").arg(timestamp(), rel));
                continue;
            }

            // Finally add to index
            QByteArray relUtf8 = rel.toUtf8();
            error = git_index_add_bypath(index, relUtf8.constData());
            if (error) {
                const git_error *gerr = git_error_last();
                QString gitMsg = gerr ? QString::fromUtf8(gerr->message) : QString();
                emit message(QStringLiteral("[%1] ERROR: git_index_add_bypath('%2'): code=%3 msg=%4")
                                 .arg(timestamp(), rel, QString::number(error), gitMsg));
                git_index_free(index);
                git_repository_free(repo);
                git_libgit2_shutdown();
                emit finished(6);
                return;
            }
            ++added;
        }
        error = git_index_write(index);
        git_index_free(index);
        if (error) {
            emit message(QStringLiteral("[%1] ERROR: git_index_write: %2").arg(timestamp()).arg(QString::number(error)));
            git_repository_free(repo);
            git_libgit2_shutdown();
            emit finished(6);
            return;
        }
        emit message(QStringLiteral("[%1] Added %2 files to index.").arg(timestamp()).arg(QString::number(added)));

        // 4) commit if needed (git commit -m "first commit")
        emit message(QStringLiteral("[%1] Creating commit if needed...").arg(timestamp()));
        git_index *idx2 = nullptr;
        error = git_repository_index(&idx2, repo);
        if (error) {
            emit message(QStringLiteral("[%1] ERROR: git_repository_index: %2").arg(timestamp()).arg(QString::number(error)));
            git_repository_free(repo);
            git_libgit2_shutdown();
            emit finished(7);
            return;
        }

        bool createdCommit = false;
        git_oid commit_oid;
        if (git_index_entrycount(idx2) > 0) {
            git_oid tree_oid;
            error = git_index_write_tree(&tree_oid, idx2);
            if (error) { git_index_free(idx2); emit message(QStringLiteral("[%1] ERROR write_tree: %2").arg(timestamp()).arg(QString::number(error))); git_repository_free(repo); git_libgit2_shutdown(); emit finished(7); return; }
            git_tree *tree = nullptr;
            error = git_tree_lookup(&tree, repo, &tree_oid);
            if (error) { git_index_free(idx2); emit message(QStringLiteral("[%1] ERROR tree_lookup: %2").arg(timestamp()).arg(QString::number(error))); git_repository_free(repo); git_libgit2_shutdown(); emit finished(7); return; }

            git_signature *sig = nullptr;
            error = git_signature_now(&sig, "gitpush", "gitpush@example.com");
            if (error) { git_tree_free(tree); git_index_free(idx2); emit message(QStringLiteral("[%1] ERROR signature: %2").arg(timestamp()).arg(QString::number(error))); git_repository_free(repo); git_libgit2_shutdown(); emit finished(7); return; }

            // parent if exists
            git_reference *head_ref = nullptr;
            git_commit *parent = nullptr;
            if (git_reference_lookup(&head_ref, repo, "HEAD") == 0) {
                git_oid parent_oid;
                if (git_reference_name_to_id(&parent_oid, repo, "HEAD") == 0) {
                    if (git_commit_lookup(&parent, repo, &parent_oid) != 0) parent = nullptr;
                }
            }

            const git_commit *parents[1];
            int parent_count = 0;
            if (parent) { parents[0] = parent; parent_count = 1; }

            // create commit on HEAD (or create initial commit)
            error = git_commit_create_v(&commit_oid, repo, "HEAD", sig, sig, nullptr, "first commit", tree, parent_count, parent_count ? parents : nullptr);
            git_signature_free(sig);
            if (parent) git_commit_free(parent);
            if (head_ref) git_reference_free(head_ref);
            git_tree_free(tree);
            git_index_free(idx2);

            if (error) {
                emit message(QStringLiteral("[%1] ERROR creating commit: %2").arg(timestamp()).arg(QString::number(error)));
                git_repository_free(repo);
                git_libgit2_shutdown();
                emit finished(8);
                return;
            } else {
                char oid_str[GIT_OID_HEXSZ+1] = {0};
                git_oid_tostr(oid_str, sizeof(oid_str), &commit_oid);
                emit message(QStringLiteral("[%1] Commit created: %2").arg(timestamp()).arg(QString::fromUtf8(oid_str)));
                createdCommit = true;
            }
        } else {
            git_index_free(idx2);
            emit message(QStringLiteral("[%1] No changes to commit.").arg(timestamp()));
        }

        // 4b) ensure branch main exists and HEAD points to it (git branch -M main)
        if (createdCommit) {
            // Try to create refs/heads/main pointing to commit_oid (force overwrite if exists)
            git_reference *ref = nullptr;
            int rc = git_reference_create(&ref, repo, "refs/heads/main", &commit_oid, 1, "create main branch");
            if (rc == GIT_EEXISTS) {
                // if exists, move it using git_reference_set_target
                git_reference *old = nullptr;
                if (git_reference_lookup(&old, repo, "refs/heads/main") == 0) {
                    git_reference *new_ref = nullptr;
                    int rc2 = git_reference_set_target(&new_ref, old, &commit_oid, "move main to new commit");
                    git_reference_free(old);
                    if (rc2 != 0) {
                        const git_error *ge = git_error_last();
                        QString gm = ge ? QString::fromUtf8(ge->message) : QStringLiteral("unknown");
                        emit message(QStringLiteral("[%1] ERROR moving refs/heads/main: %2").arg(timestamp()).arg(gm));
                        if (new_ref) git_reference_free(new_ref);
                        git_repository_free(repo);
                        git_libgit2_shutdown();
                        emit finished(9);
                        return;
                    }
                    if (new_ref) git_reference_free(new_ref);
                } else {
                    emit message(QStringLiteral("[%1] WARNING: could not lookup existing refs/heads/main to move").arg(timestamp()));
                }
            } else if (rc != 0) {
                const git_error *ge = git_error_last();
                QString gm = ge ? QString::fromUtf8(ge->message) : QStringLiteral("unknown");
                emit message(QStringLiteral("[%1] ERROR creating refs/heads/main: %2").arg(timestamp()).arg(gm));
                if (ref) git_reference_free(ref);
                git_repository_free(repo);
                git_libgit2_shutdown();
                emit finished(9);
                return;
            }
            if (ref) git_reference_free(ref);

            // set HEAD to refs/heads/main
            error = git_repository_set_head(repo, "refs/heads/main");
            if (error) {
                emit message(QStringLiteral("[%1] ERROR setting HEAD to refs/heads/main: %2").arg(timestamp()).arg(QString::number(error)));
                git_repository_free(repo);
                git_libgit2_shutdown();
                emit finished(9);
                return;
            }
            emit message(QStringLiteral("[%1] Branch 'main' created and set as HEAD.").arg(timestamp()));
        } else {
            // No new commit created; try to set main from existing HEAD if possible
            git_reference *head = nullptr;
            if (git_repository_head(&head, repo) == 0) {
                const git_oid *head_oid = git_reference_target(head);
                if (head_oid) {
                    git_reference *ref = nullptr;
                    int rc = git_reference_create(&ref, repo, "refs/heads/main", head_oid, 1, "create main branch from HEAD");
                    if (rc == 0 && ref) {
                        git_reference_free(ref);
                        git_repository_set_head(repo, "refs/heads/main");
                        emit message(QStringLiteral("[%1] Branch 'main' created from existing HEAD.").arg(timestamp()));
                    } else if (rc == GIT_EEXISTS) {
                        // move existing main to HEAD
                        git_reference *old = nullptr;
                        if (git_reference_lookup(&old, repo, "refs/heads/main") == 0) {
                            git_reference *new_ref = nullptr;
                            int rc2 = git_reference_set_target(&new_ref, old, head_oid, "move main to HEAD");
                            git_reference_free(old);
                            if (rc2 == 0 && new_ref) {
                                git_reference_free(new_ref);
                                git_repository_set_head(repo, "refs/heads/main");
                                emit message(QStringLiteral("[%1] Branch 'main' moved to existing HEAD.").arg(timestamp()));
                            } else {
                                const git_error *ge = git_error_last();
                                QString gm = ge ? QString::fromUtf8(ge->message) : QStringLiteral("unknown");
                                emit message(QStringLiteral("[%1] WARNING: could not move main to HEAD: %2").arg(timestamp()).arg(gm));
                            }
                        }
                    } else {
                        const git_error *ge = git_error_last();
                        QString gm = ge ? QString::fromUtf8(ge->message) : QStringLiteral("unknown");
                        emit message(QStringLiteral("[%1] WARNING: could not create main from HEAD: %2").arg(timestamp()).arg(gm));
                    }
                }
                git_reference_free(head);
            }
        }

        // 5) remote add origin (git remote add origin https://github.com/user/DIRNAME.git)
        emit message(QStringLiteral("[%1] Adding remote origin...").arg(timestamp()));
        QString remoteUrl = QString("https://github.com/%1/%2.git").arg(m_githubUser, repoName);
        git_remote *existing = nullptr;
        if (git_remote_lookup(&existing, repo, "origin") == 0) {
            // remove existing and recreate
            git_remote_delete(repo, "origin");
            if (existing) git_remote_free(existing);
            emit message(QStringLiteral("[%1] Removed existing origin").arg(timestamp()));
        }
        git_remote *remote = nullptr;
        error = git_remote_create(&remote, repo, "origin", remoteUrl.toUtf8().constData());
        if (error) {
            emit message(QStringLiteral("[%1] ERROR creating remote origin: %2").arg(timestamp()).arg(QString::number(error)));
            git_repository_free(repo);
            git_libgit2_shutdown();
            emit finished(9);
            return;
        }
        emit message(QStringLiteral("[%1] Remote origin set to %2").arg(timestamp(), remoteUrl));

        // 6) push -u origin main (push and set upstream)
        emit message(QStringLiteral("[%1] Starting push...").arg(timestamp()));

        // prepare callbacks
        CredContext credCtx;
        credCtx.token = m_token.toUtf8();

        struct CombinedPayload {
            CredContext *cred;
            GitPushWorker *worker;
        } combined;
        combined.cred = &credCtx;
        combined.worker = this;

        git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
        callbacks.credentials = [](git_cred **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)->int {
            CombinedPayload *cp = static_cast<CombinedPayload*>(payload);
            return credentials_cb(out, url, username_from_url, allowed_types, cp->cred);
        };
        callbacks.transfer_progress = [](const git_transfer_progress *stats, void *payload)->int {
            CombinedPayload *cp = static_cast<CombinedPayload*>(payload);
            if (!cp || !cp->worker) return 0;
            qint64 received = (qint64)stats->received_bytes;
            qint64 total = 0;
            if (stats->total_deltas > 0) total = (qint64)stats->total_deltas;
            else total = (qint64)stats->total_objects;
            QMetaObject::invokeMethod(cp->worker, "onLibgit2Progress", Qt::DirectConnection,
                                      Q_ARG(qint64, received), Q_ARG(qint64, total));
            return 0;
        };

        git_push_options push_opts;
        error = git_push_init_options(&push_opts, GIT_PUSH_OPTIONS_VERSION);
        if (error) {
            emit message(QStringLiteral("[%1] ERROR push options init: %2").arg(timestamp()).arg(QString::number(error)));
            git_remote_free(remote);
            git_repository_free(repo);
            git_libgit2_shutdown();
            emit finished(10);
            return;
        }
        push_opts.callbacks = callbacks;
        push_opts.callbacks.payload = &combined;

        // refspec to push local main to remote main
        const char *refs_main[] = { "refs/heads/main:refs/heads/main" };
        git_strarray refspecs_main;
        refspecs_main.strings = const_cast<char**>(refs_main);
        refspecs_main.count = 1;

        // perform push
        error = git_remote_push(remote, &refspecs_main, &push_opts);
        if (error) {
            // try alternative: push HEAD:refs/heads/main
            const char *refs_head[] = { "HEAD:refs/heads/main" };
            git_strarray refspecs_head;
            refspecs_head.strings = const_cast<char**>(refs_head);
            refspecs_head.count = 1;
            error = git_remote_push(remote, &refspecs_head, &push_opts);
            if (error) {
                const git_error *e = git_error_last();
                QString errMsg = e ? QString::fromUtf8(e->message) : QStringLiteral("Unknown libgit2 error");
                emit message(QStringLiteral("[%1] ERROR push failed: %2").arg(timestamp()).arg(errMsg));
                git_remote_free(remote);
                git_repository_free(repo);
                git_libgit2_shutdown();
                emit finished(12);
                return;
            }
        }

        // set upstream: local branch main -> origin/main
        git_reference *local_branch = nullptr;
        if (git_branch_lookup(&local_branch, repo, "main", GIT_BRANCH_LOCAL) == 0) {
            int rc = git_branch_set_upstream(local_branch, "origin/main");
            if (rc != 0) {
                const git_error *e = git_error_last();
                QString errMsg = e ? QString::fromUtf8(e->message) : QStringLiteral("Unknown libgit2 error");
                emit message(QStringLiteral("[%1] WARNING: could not set upstream for main: %2").arg(timestamp()).arg(errMsg));
            } else {
                emit message(QStringLiteral("[%1] Set upstream: main -> origin/main").arg(timestamp()));
            }
            git_reference_free(local_branch);
        } else {
            emit message(QStringLiteral("[%1] WARNING: local branch 'main' not found to set upstream").arg(timestamp()));
        }

        git_remote_free(remote);
        emit message(QStringLiteral("[%1] Push completed successfully.").arg(timestamp()));
        git_repository_free(repo);
        git_libgit2_shutdown();

        // finished successfully: emit finished(0)
        emit finished(0);
    }

    // Slot invoked from libgit2 callback (DirectConnection). Compute instantaneous speed.
public slots:
    void onLibgit2Progress(qint64 received, qint64 total) {
        qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
        if (m_firstProgress) {
            m_lastBytes = received;
            m_lastTimeMs = nowMs;
            m_firstProgress = false;
            emit progressUpdated(received, total, 0.0);
            emit message(QStringLiteral("[%1] Progress: %2/%3").arg(timestamp()).arg(QString::number(received)).arg(QString::number(total)));
            return;
        }
        qint64 deltaBytes = received - m_lastBytes;
        qint64 deltaMs = nowMs - m_lastTimeMs;
        double bytesPerSec = 0.0;
        if (deltaMs > 0) bytesPerSec = (double)deltaBytes / (deltaMs / 1000.0);
        if (bytesPerSec < 0) bytesPerSec = 0.0;
        m_lastBytes = received;
        m_lastTimeMs = nowMs;
        emit progressUpdated(received, total, bytesPerSec);
        emit message(QStringLiteral("[%1] Progress: %2/%3 — %4 B/s").arg(timestamp()).arg(QString::number(received)).arg(QString::number(total)).arg(QString::number(bytesPerSec, 'f', 2)));
    }

private:
    QString timestamp() const {
        return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    }

    QString m_dirPath;
    QString m_githubUser;
    QString m_token;
    bool m_isPrivate;

    // speed calculation state
    qint64 m_lastBytes;
    qint64 m_lastTimeMs;
    bool m_firstProgress;
    QElapsedTimer m_speedTimer;
};

// ---------- Small GUI (auto-start, logging text area, 1s speed calc) ----------
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(const QString &dirPath, const QString &user, const QString &token, bool isPrivate)
        : m_dirPath(dirPath), m_user(user), m_token(token), m_isPrivate(isPrivate),
          m_latestTransferred(0), m_latestTotal(0), m_prevTransferred(0)
    {
        setWindowTitle("gitpush");
        resize(640, 360);

        QVBoxLayout *l = new QVBoxLayout(this);
        m_label = new QLabel("Bereit", this);
        m_progress = new QProgressBar(this);
        m_progress->setRange(0, 100);
        m_log = new QTextEdit(this);
        m_log->setReadOnly(true);
        m_log->setLineWrapMode(QTextEdit::NoWrap);

        l->addWidget(m_label);
        l->addWidget(m_progress);
        l->addWidget(new QLabel("Log:", this));
        l->addWidget(m_log);

        // Timer for per-second speed calculation
        m_speedTimer = new QTimer(this);
        m_speedTimer->setInterval(1000);
        connect(m_speedTimer, &QTimer::timeout, this, &MainWindow::updateSpeed);
        m_speedTimer->start();

        // auto-start after widget is shown
        QTimer::singleShot(0, this, &MainWindow::startWorker);
    }

public slots:
    void startWorker() {
        appendLog(QStringLiteral("[%1] UI: starting worker").arg(timestamp()));
        GitPushWorker *worker = new GitPushWorker(m_dirPath, m_user, m_token, m_isPrivate);
        QThread *t = new QThread;
        worker->moveToThread(t);

        connect(t, &QThread::started, worker, &GitPushWorker::run);
        connect(worker, &GitPushWorker::progressUpdated, this, &MainWindow::onProgress);
        connect(worker, &GitPushWorker::message, this, &MainWindow::onMessage);
        connect(worker, &GitPushWorker::finished, this, [this, t, worker](int code){
            // cleanup UI and then exit on success
            if (code == 0) {
                appendLog(QStringLiteral("[%1] DONE: exit code 0").arg(timestamp()));
                m_label->setText("Fertig");
            } else {
                appendLog(QStringLiteral("[%1] FINISHED with code %2").arg(timestamp()).arg(QString::number(code)));
                m_label->setText(QStringLiteral("Beendet mit Code %1").arg(QString::number(code)));
            }
            t->quit();
            t->wait();
            worker->deleteLater();
            t->deleteLater();

            // If success, exit application with code 0
            if (code == 0) {
                QCoreApplication::exit(0);
            }
        });

        t->start();
    }

    void onProgress(qint64 transferred, qint64 total, double /*bytesPerSec*/) {
        // store latest values for per-second calculation
        m_latestTransferred = transferred;
        m_latestTotal = total;

        int percent = 0;
        if (total > 0) percent = int((transferred * 100) / total);
        m_progress->setValue(percent);
    }

    void onMessage(const QString &txt) {
        appendLog(txt);
        // also set label to last short message
        QString shortMsg = txt;
        if (shortMsg.size() > 120) shortMsg = shortMsg.left(120) + "...";
        m_label->setText(shortMsg);
    }

private slots:
    void updateSpeed() {
        qint64 delta = m_latestTransferred - m_prevTransferred;
        if (delta < 0) delta = 0;
        m_prevTransferred = m_latestTransferred;

        double bytesPerSec = static_cast<double>(delta); // bytes per second
        QString speedStr;
        if (bytesPerSec > 1024.0*1024.0) speedStr = QString::asprintf("%.2f MB/s", bytesPerSec / (1024.0*1024.0));
        else if (bytesPerSec > 1024.0) speedStr = QString::asprintf("%.2f KB/s", bytesPerSec / 1024.0);
        else speedStr = QString::asprintf("%.0f B/s", bytesPerSec);

        appendLog(QStringLiteral("[%1] Upload speed: %2").arg(timestamp()).arg(speedStr));
        m_label->setText(QStringLiteral("Upload: %1").arg(speedStr));
    }

private:
    QString timestamp() const {
        return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    }

    void appendLog(const QString &line) {
        m_log->append(line);
        QTextCursor c = m_log->textCursor();
        c.movePosition(QTextCursor::End);
        m_log->setTextCursor(c);
    }

    QString m_dirPath;
    QString m_user;
    QString m_token;
    bool m_isPrivate;

    QLabel *m_label;
    QProgressBar *m_progress;
    QTextEdit *m_log;

    // per-second speed calculation
    qint64 m_latestTransferred;
    qint64 m_latestTotal;
    qint64 m_prevTransferred;
    QTimer *m_speedTimer;
};

// ---------- Helper: authenticate with GitHub (GET /user) using debug helper ----------
static bool tryAuthenticate(const QString &username, const QString &token, QString &errorOut)
{
    QNetworkAccessManager mgr;
    QUrl url("https://api.github.com/user");
    QByteArray authHeader = QString("token %1").arg(token).toUtf8();

    int status = 0;
    QByteArray resp;
    QString netErr;
    if (!getJsonSyncWithDebug(mgr, url, authHeader, 10000, status, resp, netErr)) {
        if (!netErr.isEmpty()) errorOut = QStringLiteral("Netzwerkfehler: %1").arg(netErr);
        else errorOut = QStringLiteral("Authentifizierung fehlgeschlagen. HTTP Status: %1; Body: %2").arg(QString::number(status)).arg(QString::fromUtf8(resp));
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(resp);
    if (!doc.isObject()) {
        errorOut = QStringLiteral("Ungültige Antwort von GitHub.");
        return false;
    }
    QJsonObject obj = doc.object();
    QString login = obj.value("login").toString();
    if (!login.isEmpty() && login.compare(username, Qt::CaseInsensitive) != 0) {
        errorOut = QStringLiteral("Token gültig, aber GitHub-Login '%1' stimmt nicht mit eingegebenem Benutzer '%2' überein.").arg(login, username);
        return true;
    }
    return true;
}

// ---------- main ----------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("gitpush");

    QCommandLineParser parser;
    parser.setApplicationDescription("gitpush - create GitHub repo and push directory (GUI)");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("directory", "Lokales Verzeichnis, das gepusht werden soll");
    parser.addOption({{"p","private"}, "Repository privat erstellen", ""});
    parser.process(QCoreApplication::arguments());

    const QStringList posArgs = parser.positionalArguments();
    if (posArgs.isEmpty()) {
        parser.showHelp(1);
    }

    QString dirPath = posArgs.first();
    QFileInfo fi(dirPath);
    if (!fi.exists() || !fi.isDir()) {
        qCritical() << "Verzeichnis existiert nicht:" << dirPath;
        return 3;
    }

    // Config path
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    if (configDir.isEmpty()) configDir = QDir::homePath() + "/.config";
    QDir().mkpath(configDir + "/gitpush");
    QString configFile = configDir + "/gitpush/config.ini";
    QSettings settings(configFile, QSettings::IniFormat);

    QString savedUser = settings.value("github/username").toString();
    QString savedToken = settings.value("github/token").toString();
    bool useSaved = false;
    QString authError;

    if (!savedUser.isEmpty() && !savedToken.isEmpty()) {
        // try authenticate with saved credentials
        if (tryAuthenticate(savedUser, savedToken, authError)) {
            // warn if mismatch but proceed
            if (!authError.isEmpty()) {
                QMessageBox::warning(nullptr, "Warnung", authError);
            }
            useSaved = true;
        } else {
            settings.remove("github/username");
            settings.remove("github/token");
            settings.sync();
            QMessageBox::information(nullptr, "Login fehlgeschlagen", "Gespeicherte Anmeldedaten sind ungültig. Bitte erneut anmelden.");
        }
    }

    QString githubUser;
    QString githubToken;
    bool isPrivate = parser.isSet("private");

    if (useSaved) {
        githubUser = savedUser;
        githubToken = savedToken;
    } else {
        // show login dialog
        LoginDialog dlg;
        if (!savedUser.isEmpty()) dlg.setUsername(savedUser);
        if (!savedToken.isEmpty()) dlg.setToken(savedToken);
        bool ok = false;
        while (true) {
            if (dlg.exec() != QDialog::Accepted) {
                return 1; // user cancelled
            }
            QString user = dlg.username();
            QString token = dlg.token();
            QString err;
            if (tryAuthenticate(user, token, err)) {
                if (!err.isEmpty()) QMessageBox::warning(nullptr, "Warnung", err);
                githubUser = user;
                githubToken = token;
                if (dlg.saveCredentials()) {
                    settings.setValue("github/username", githubUser);
                    settings.setValue("github/token", githubToken);
                    settings.sync();
                }
                ok = true;
                break;
            } else {
                QMessageBox::critical(nullptr, "Login fehlgeschlagen", err);
            }
        }
        if (!ok) return 1;
    }

    MainWindow w(dirPath, githubUser, githubToken, isPrivate);
    w.show();
    return app.exec();
}

// single moc include for this source file (must be exactly this name)
#include "gitpush1.moc"
