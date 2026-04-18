#ifndef QTCCREATOR3PLUGIN_H
#define QTCCREATOR3PLUGIN_H

#include "qtccreator3_global.h"

#include <extensionsystem/iplugin.h>

namespace Qtccreator3 {
namespace Internal {

class Qtccreator3Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Qtccreator3.json")

public:
    Qtccreator3Plugin();
    ~Qtccreator3Plugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace Qtccreator3

#endif // QTCCREATOR3PLUGIN_H
