include(../common.pri)

TEMPLATE = lib
CONFIG += qt link_pkgconfig no_keywords

QT = core dbus

TARGET = $${MPRISQTLIB}

QMAKE_SUBSTITUTES = $${TARGET}.prf.in

DEFINES += MPRIS_QT_LIBRARY

use_system_dbus {
    DEFINES += USE_SYSTEM_DBUS
}

DEPENDPATH += ../qtdbusextended
INCLUDEPATH += ../qtdbusextended
LIBS += -L../qtdbusextended -ldbusextended-qt$${QT_MAJOR_VERSION}

# Generate pkg-config support by default
# Note that we HAVE TO also create prl config as QMake implementation
# mixes both of them together.
CONFIG += create_pc create_prl no_install_prl

SOURCES += \
    mpris.cpp \
    mprisrootadaptor.cpp \
    mprisplayeradaptor.cpp \
    mprisplayer.cpp \
    mpriscontroller.cpp \
    mprismanager.cpp \
    mprisplayerinterface.cpp \
    mprisrootinterface.cpp

HEADERS += \
    mprisqt.h \
    mpris.h \
    mprisplayer.h \
    mprisplayer_p.h \
    mpriscontroller.h \
    mpriscontroller_p.h \
    mprismanager.h

INSTALL_HEADERS = \
    Mpris \
    MprisPlayer \
    MprisController \
    MprisManager \
    mprisqt.h \
    mpris.h \
    mprisplayer.h \
    mpriscontroller.h \
    mprismanager.h

OTHER_FILES += org.mpris.MediaPlayer2.xml \
    org.mpris.MediaPlayer2.Player.xml

target.path = $$[QT_INSTALL_LIBS]
headers.files = $$INSTALL_HEADERS
headers.path = $$[QT_INSTALL_HEADERS]/MprisQt
prf.files = $${TARGET}.prf
prf.path = $$[QMAKE_MKSPECS]/features
INSTALLS += target headers prf

QMAKE_PKGCONFIG_REQUIRES = Qt$${QT_MAJOR_VERSION}Core Qt$${QT_MAJOR_VERSION}DBus
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_NAME = MprisQt
