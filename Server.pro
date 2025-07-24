QT -= gui
QT += core network

CONFIG += c++14 console
CONFIG -= app_bundle

TARGET = Server
TEMPLATE = app

SOURCES += \
    src/AudioRecorder.cpp \
    src/Core.cpp \
    src/Handler.cpp \
    src/main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/AudioRecorder.h \
    src/Core.h \
    src/Handler.h

INCLUDEPATH += src

LIBS += -lboost_system \
        -lboost_filesystem
