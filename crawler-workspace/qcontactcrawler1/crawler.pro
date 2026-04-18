QT       += core gui widgets network
TARGET   = addresscrawler
TEMPLATE = app
SOURCES += addresscrawler.cpp

# linux/mac (pkg-config is usually easiest, or manual link)
LIBS += -lcurl -lxml2
INCLUDEPATH += /usr/include/libxml2
