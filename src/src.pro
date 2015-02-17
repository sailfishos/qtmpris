include(../common.pri)

TEMPLATE = lib
CONFIG += qt

QT = core dbus qml

TARGET = $${MPRISQTLIB}

QMAKE_SUBSTITUTES = $${TARGET}.prf.in

DEFINES += MPRIS_QT_LIBRARY

# Generate pkg-config support by default
# Note that we HAVE TO also create prl config as QMake implementation
# mixes both of them together.
CONFIG += create_pc create_prl no_install_prl

SOURCES += \
    mpris.cpp \
    mprisrootadaptor.cpp \
    mprisplayeradaptor.cpp \
    mprisplayer.cpp

HEADERS += \
    mprisqt.h \
    mpris.h \
    mprisplayer.h \
    mprisplayer_p.h

INSTALL_HEADERS = \
    MprisQt \
    Mpris \
    MprisPlayer \
    mprisqt.h \
    mpris.h \
    mprisplayer.h

OTHER_FILES += org.mpris.MediaPlayer2.xml \
    org.mpris.MediaPlayer2.Player.xml

target.path = $$[QT_INSTALL_LIBS]
headers.files = $$INSTALL_HEADERS
headers.path = $$[QT_INSTALL_HEADERS]/MprisQt
prf.files = $${TARGET}.prf
prf.path = $$[QMAKE_MKSPECS]/features
INSTALLS += target headers prf

QMAKE_PKGCONFIG_REQUIRES = Qt5Core Qt5DBus
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_NAME = MprisQt
