TEMPLATE = lib
CONFIG += plugin debug_and_release

QT += core gui widgets

TARGET = ClonePlugin
TARGET_EXT = $(QT_LIB_EXT)

# In Debian/Ubuntu werden die Header oft in Unterverzeichnissen von /usr/include/qtcreator/ abgelegt
# oder die Umgebungsvariable QTC_BUILD wird verwendet.
# Wir setzen die INCLUDEPATH auf die erwarteten Standardpfade:
INCLUDEPATH += /usr/include/qtcreator/core
INCLUDEPATH += /usr/include/qtcreator/extensionSystem
INCLUDEPATH += /usr/include/qtcreator/projectexplorer
# Hinzufügen des Hauptverzeichnisses
INCLUDEPATH += /usr/include/qtcreator

INCLUDEPATH += /usr/include/qtcreator/core
INCLUDEPATH += /usr/include/qtcreator/extensionSystem
INCLUDEPATH += /usr/include/qtcreator/projectexplorer
INCLUDEPATH += /usr/include/qtcreator

# Die LIBs müssen auf die Qt Creator Libraries verweisen.
# Unter Debian/Ubuntu wird die Bibliothek meist unter /usr/lib/x86_64-linux-gnu/ abgelegt.
# Die Hauptbibliothek für das Plugin-System ist oft QtCreator.
# **Achtung:** Dies ist der schwierigste Teil. Falls das nicht funktioniert,
# müssen Sie den genauen Namen und Pfad der Bibliothek auf Ihrem System finden.

LIBS += -L/usr/lib/x86_64-linux-gnu/ -lQtCreator
# Alternativ könnte die Bibliothek direkt in /usr/lib/ oder in einem Qt-spezifischen Pfad liegen.

SOURCES += main.cpp

QTC_PLUGIN_CLASS = ClonePlugin::ClonePlugin

# Versuch, die Standard-QTCREATOR Umgebungsvariablen zu nutzen (häufig in *.prl oder ähnlichen Dateien definiert)
!isEmpty(QTCREATOR_INCDIR): INCLUDEPATH += $$QTCREATOR_INCDIR/app/core
!isEmpty(QTCREATOR_LIBDIR): LIBS += -L$$QTCREATOR_LIBDIR -lQtCreator
