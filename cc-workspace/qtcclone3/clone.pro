# clone.pro
TEMPLATE = lib
CONFIG += plugin c++11
QT += widgets core gui

TARGET = cloneprj

SOURCES += \
    main.cpp

DEFINES += QT_PLUGIN

# Install to per-user Qt Creator plugins folder on Unix-like systems
unix:!macx {
    # Primary install using qmake INSTALLS
    plugin.path = "~/.local/share/QtProject/qtcreator/plugins"
    # Typical library name produced by qmake is lib<TARGET>.so
    plugin.files = "$$DESTDIR/lib$$TARGET.so"
    INSTALLS += plugin

    # Fallback: copy after build (handles cases where DESTDIR is empty or name differs)
    QMAKE_POST_LINK += $$escape_expand(\\
        if [ -f "$$DESTDIR/lib$$TARGET.so" ]; then \\
            cp -f "$$DESTDIR/lib$$TARGET.so" "~/.local/share/QtProject/qtcreator/plugins/"; \\
        elif [ -f "lib$$TARGET.so" ]; then \\
            cp -f "lib$$TARGET.so" "~/.local/share/QtProject/qtcreator/plugins/"; \\
        fi \\
    )
}

# macOS and Windows: adjust install path as needed (left commented)
# macx {
#     plugin.path = $$HOME/Library/Application Support/QtProject/qtcreator/plugins
#     plugin.files = $$DESTDIR/lib$$TARGET.dylib
#     INSTALLS += plugin
# }
# win32 {
#     plugin.path = $$HOME/AppData/Roaming/QtProject/qtcreator/plugins
#     plugin.files = $$DESTDIR/$$TARGET.dll
#     INSTALLS += plugin
# }

# Optional: debug suffix handling (if you build with debug suffixes)
# CONFIG(debug, debug|release) {
#     # If your build produces libcloneprj_debug.so, uncomment and adapt:
#     # plugin.files = $$DESTDIR/lib$$TARGET\_debug.so
# }

# Convenience: ensure plugin directory exists before install (best-effort)
QMAKE_EXTRA_TARGETS += ensure_plugin_dir
ensure_plugin_dir.target = ensure_plugin_dir
ensure_plugin_dir.commands = mkdir -p $$HOME/.local/share/QtProject/qtcreator/plugins
PRE_TARGETDEPS += ensure_plugin_dir

