TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lcrypto -lssl
SOURCES += \
        main.c
