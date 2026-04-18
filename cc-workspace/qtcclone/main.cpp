//#include <extensionsystem/iplugin.h>
#include <QAction>
#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QString>
#include <QDir>
#include <QFileInfo>

// Qt Creator Framework Includes
#include <core/actionmanager/actionmanager.h>
#include <core/actionmanager/command.h>
#include <core/actionmanager/actioncontainer.h>
#include <core/coreconstants.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectmanager.h>


namespace ClonePlugin {

using namespace Core;
using namespace ProjectExplorer;

// ==========================================================
// HIER WIRD DIE KLASSE UND IHRE IMPLEMENTIERUNG AMALGAMIERT
// ==========================================================

class ClonePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreator.Plugin" FILE "ClonePlugin.json")

public:
    ClonePlugin() {}
    ~ClonePlugin() override {}

    bool initialize(const QStringList &arguments, QString *errorMessage) override;
    void extensionsInitialized() override {}
    ShutdownResult aboutToShutdown() override { return SynchronousShutdown; }

private slots:
    void triggerCloneProject();

private:
    QAction *m_cloneAction = nullptr;

    // Hilfsfunktion zur Implementierung der Klon-Logik
    bool cloneProjectFiles(const QString &originalProjectPath, const QString &newProjectDir, const QString &newProjectName);

    // Hilfsfunktion zum rekursiven Kopieren und Ersetzen von Dateiinhalten
    bool copyAndReplaceFile(const QString &sourceFilePath, const QString &destFilePath, const QString &oldName, const QString &newName);
};

// ==========================================================
// Implementierung der Methoden
// ==========================================================

bool ClonePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    m_cloneAction = new QAction(tr("Projekt klonen... (ClonePlugin)"), this);
    connect(m_cloneAction, &QAction::triggered, this, &ClonePlugin::triggerCloneProject);

    Id actionId("ClonePlugin.CloneProject");
    Command *cmd = ActionManager::registerAction(m_cloneAction, actionId, Context(Core::Constants::C_PROJECTEXPLORER));
    cmd->setAttribute(Command::CA_UpdateText);

    // Fügt Aktion dem Menü "Datei" hinzu
    ActionContainer *fileMenu = ActionManager::actionContainer(Core::Constants::M_FILE);
    if (fileMenu) {
        fileMenu->addAction(cmd, Core::Constants::G_FILE_SAVE);
    }

    // Fügt Aktion dem Projekt-Kontextmenü hinzu
    ActionContainer *projectContext = ActionManager::actionContainer(ProjectExplorer::Constants::M_PROJECTCONTEXT);
    if (projectContext) {
        projectContext->addAction(cmd, ProjectExplorer::Constants::G_PROJECTEXPLORER_NEW);
    }

    return true;
}

void ClonePlugin::triggerCloneProject()
{
    Project *currentProject = ProjectExplorer::ProjectExplorer::instance()->currentProject();

    if (!currentProject) {
        qDebug() << "ClonePlugin: Fehler - Kein aktives Projekt zum Klonen gefunden.";
        return;
    }

    QString originalPath = currentProject->projectDirectory();
    QString originalName = QFileInfo(currentProject->projectFilePath()).baseName(); // Name des .pro-Files ohne Pfad

    // --- Simulierte Eingabe für das neue Projekt ---
    // Im echten Plugin müssten Sie hier QInputDialog/QFileDialog verwenden
    QString newProjectName = originalName + "4";
    QString newProjectDir = originalPath + "_Clone"; // Neues Verzeichnis neben dem Original

    qDebug() << "ClonePlugin: Starte Klonen von:" << originalPath
             << "nach:" << newProjectDir << "mit neuem Namen:" << newProjectName;

    if (cloneProjectFiles(originalPath, newProjectDir, newProjectName)) {
        qDebug() << "ClonePlugin: Projekt erfolgreich geklont. Versuche, neues Projekt zu öffnen.";
        ProjectManager::instance()->openProject(newProjectDir + "/" + newProjectName + ".pro");
    } else {
        qDebug() << "ClonePlugin: Klonen fehlgeschlagen. Details siehe oben.";
    }
}

bool ClonePlugin::copyAndReplaceFile(const QString &sourceFilePath, const QString &destFilePath,
                                     const QString &oldName, const QString &newName)
{
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ClonePlugin: Fehler beim Lesen der Quelldatei:" << sourceFilePath;
        return false;
    }

    QString content = sourceFile.readAll();
    sourceFile.close();

    // Ersetze alte Projektnamen durch den neuen Namen (Groß-/Kleinschreibung beachtend)
    content.replace(oldName, newName);

    QFile destFile(destFilePath);
    if (!destFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug() << "ClonePlugin: Fehler beim Schreiben der Zieldatei:" << destFilePath;
        return false;
    }

    QTextStream out(&destFile);
    out << content;
    destFile.close();
    return true;
}

bool ClonePlugin::cloneProjectFiles(const QString &originalProjectPath, const QString &newProjectDir, const QString &newProjectName)
{
    QDir originalDir(originalProjectPath);

    if (!originalDir.exists()) {
        qDebug() << "ClonePlugin: Originalverzeichnis nicht gefunden:" << originalProjectPath;
        return false;
    }

    // Erstelle das neue Zielverzeichnis
    QDir targetDir;
    if (targetDir.exists(newProjectDir)) {
        qDebug() << "ClonePlugin: Zielverzeichnis existiert bereits, Klonen abgebrochen:" << newProjectDir;
        return false;
    }
    if (!targetDir.mkpath(newProjectDir)) {
        qDebug() << "ClonePlugin: Zielverzeichnis konnte nicht erstellt werden:" << newProjectDir;
        return false;
    }

    QString originalProjectFileName = QFileInfo(originalDir.absolutePath()).fileName();

    // Iteriere über alle Dateien und Unterverzeichnisse
    for (const QFileInfo &entryInfo : originalDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        QString originalPath = entryInfo.absoluteFilePath();
        QString relativePath = originalDir.relativeFilePath(originalPath);
        QString newPath = newProjectDir + QDir::separator() + relativePath;

        if (entryInfo.isDir()) {
            // Rekursiv Unterverzeichnisse klonen (vereinfacht, benötigt eigentlich eigene rekursive Funktion)
            // Hier nur das Erstellen des Verzeichnisses simulieren.
            if (!targetDir.mkpath(newPath)) {
                 qDebug() << "ClonePlugin: Fehler beim Erstellen des Unterverzeichnisses:" << newPath;
                 return false;
            }
        } else if (entryInfo.isFile()) {

            // Spezielles Handling für die .pro Datei (muss umbenannt werden)
            if (entryInfo.fileName() == originalProjectFileName + ".pro") {
                newPath = newProjectDir + QDir::separator() + newProjectName + ".pro";
            }

            // Kopiere die Datei und ersetze den Projektnamen im Inhalt
            if (!copyAndReplaceFile(originalPath, newPath, originalProjectFileName, newProjectName)) {
                return false;
            }
        }
    }

    return true;
}

} // namespace ClonePlugin
