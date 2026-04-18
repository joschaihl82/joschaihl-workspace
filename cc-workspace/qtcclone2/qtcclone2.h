#ifndef QTCCLONE2PLUGIN_H
#define QTCCLONE2PLUGIN_H

#include "qtcclone2_global.h"

#include <extensionsystem/iplugin.h>

namespace Qtcclone2 {
namespace Internal {

class Qtcclone2Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Qtcclone2.json")

public:
    Qtcclone2Plugin();
    ~Qtcclone2Plugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace Qtcclone2

#endif // QTCCLONE2PLUGIN_H
