include(../../common.pri)

TEMPLATE = app
TARGET = controller
CONFIG += qt link_pkgconfig

QT += qml quick dbus

DEPENDPATH += ../../src
INCLUDEPATH += ../../src
LIBS += -L../../src -l$${MPRISQTLIB}

SOURCES += \
    controller.cpp

target.path = /usr/bin

INSTALLS += target
