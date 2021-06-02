include(../common.pri)

TEMPLATE = lib
CONFIG += qt plugin link_pkgconfig
DEPENDPATH += ../src
INCLUDEPATH += ../src

QT = core dbus qml

LIBS += -L../src -l$${MPRISQTLIB}

EXAMPLE = ../example/declarative/*

OTHER_FILES += $${EXAMPLE}

TARGET = $${MPRISQTLIB}-qml-plugin
PLUGIN_IMPORT_PATH = Amber/Mpris

QMAKE_SUBSTITUTES = qmldir.in

SOURCES += \
    declarativemprisplayer.cpp \
    legacympris.cpp \
    legacympriscommon.cpp \
    legacymprismanager.cpp \
    legacymprisplayer.cpp \
    mprisplugin.cpp

HEADERS += \
    declarativemprisplayer_p.h \
    legacympris_p.h \
    legacympriscommon_p.h \
    legacymprismanager_p.h \
    legacymprisplayer_p.h \
    mprisplugin.h

target.path = $$[QT_INSTALL_QML]/$$PLUGIN_IMPORT_PATH

qml.files = \
    qmldir \
    plugins.qmltypes \
    MprisAudio.qml
qml.path = $$target.path
INSTALLS += target qml

qmltypes.commands = qmlplugindump -nonrelocatable Amber.Mpris 1.0 > $$PWD/plugins.qmltypes
QMAKE_EXTRA_TARGETS += qmltypes
