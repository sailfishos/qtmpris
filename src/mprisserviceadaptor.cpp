/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -l MprisServicePrivate -a mediaplayer2_adaptor org.mpris.MediaPlayer2.xml
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "mprisserviceadaptor_p.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include "mprisservice_p.h"

/*
 * Implementation of adaptor class MprisServiceAdaptor
 */

MprisServiceAdaptor::MprisServiceAdaptor(MprisServicePrivate *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

MprisServiceAdaptor::~MprisServiceAdaptor()
{
    // destructor
}

MprisServicePrivate *MprisServiceAdaptor::parent() const
{
    return static_cast<MprisServicePrivate *>(QObject::parent());
}

bool MprisServiceAdaptor::canQuit() const
{
    // get the value of property CanQuit
    return parent()->m_canQuit;
}

bool MprisServiceAdaptor::canRaise() const
{
    // get the value of property CanRaise
    return parent()->m_canRaise;
}

bool MprisServiceAdaptor::canSetFullscreen() const
{
    // get the value of property CanSetFullscreen
    return parent()->m_canSetFullscreen;
}

QString MprisServiceAdaptor::desktopEntry() const
{
    // get the value of property DesktopEntry
    return parent()->m_desktopEntry;
}

bool MprisServiceAdaptor::fullscreen() const
{
    // get the value of property Fullscreen
    return parent()->m_fullscreen;
}

void MprisServiceAdaptor::setFullscreen(bool value)
{
    // set the value of property Fullscreen
    parent()->setProperty("Fullscreen", QVariant::fromValue(value));
}

bool MprisServiceAdaptor::hasTrackList() const
{
    // get the value of property HasTrackList
    return parent()->m_hasTrackList;
}

QString MprisServiceAdaptor::identity() const
{
    // get the value of property Identity
    return parent()->m_identity;
}

QStringList MprisServiceAdaptor::supportedMimeTypes() const
{
    // get the value of property SupportedMimeTypes
    return parent()->m_supportedMimeTypes;
}

QStringList MprisServiceAdaptor::supportedUriSchemes() const
{
    // get the value of property SupportedUriSchemes
    return parent()->m_supportedUriSchemes;
}

void MprisServiceAdaptor::Quit()
{
    // handle method call org.mpris.MediaPlayer2.Quit
    parent()->quit();
}

void MprisServiceAdaptor::Raise()
{
    // handle method call org.mpris.MediaPlayer2.Raise
    parent()->raise();
}
