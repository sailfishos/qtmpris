TEMPLATE = lib
CONFIG += qt staticlib

QT = core dbus

TARGET = dbusextended-qt5

DEFINES += QT_DBUS_EXTENDED_LIBRARY

SOURCES += \
    dbusextendedabstractinterface.cpp \
    dbusextendedpendingcallwatcher.cpp

HEADERS += \
    dbusextended.h \
    dbusextendedabstractinterface.h \
    dbusextendedpendingcallwatcher_p.h
