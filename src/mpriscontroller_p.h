// -*- c++ -*-

/*!
 *
 * Copyright (C) 2015 Jolla Ltd.
 *
 * Contact: Valerio Valerio <valerio.valerio@jolla.com>
 * Author: Andres Gomez <andres.gomez@jolla.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


//
//  W A R N I N G
//  -------------
//
// This file is not part of the public API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//


#ifndef MPRISCONTROLLER_P_H
#define MPRISCONTROLLER_P_H

#include <DBusExtendedAbstractInterface>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.mpris.MediaPlayer2
 */
class MprisRootInterface: public DBusExtendedAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.mpris.MediaPlayer2"; }

public:
    MprisRootInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~MprisRootInterface();

    Q_PROPERTY(bool CanQuit READ canQuit NOTIFY canQuitChanged)
    inline bool canQuit() const
    { return qvariant_cast< bool >(property("CanQuit")); }

    Q_PROPERTY(bool CanRaise READ canRaise NOTIFY canRaiseChanged)
    inline bool canRaise() const
    { return qvariant_cast< bool >(property("CanRaise")); }

    Q_PROPERTY(bool CanSetFullscreen READ canSetFullscreen NOTIFY canSetFullscreenChanged)
    inline bool canSetFullscreen() const
    { return qvariant_cast< bool >(property("CanSetFullscreen")); }

    Q_PROPERTY(QString DesktopEntry READ desktopEntry NOTIFY desktopEntryChanged)
    inline QString desktopEntry() const
    { return qvariant_cast< QString >(property("DesktopEntry")); }

    Q_PROPERTY(bool Fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)
    inline bool fullscreen() const
    { return qvariant_cast< bool >(property("Fullscreen")); }
    inline void setFullscreen(bool value)
    { setProperty("Fullscreen", QVariant::fromValue(value)); }

    Q_PROPERTY(bool HasTrackList READ hasTrackList NOTIFY hasTrackListChanged)
    inline bool hasTrackList() const
    { return qvariant_cast< bool >(property("HasTrackList")); }

    Q_PROPERTY(QString Identity READ identity NOTIFY identityChanged)
    inline QString identity() const
    { return qvariant_cast< QString >(property("Identity")); }

    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes NOTIFY supportedMimeTypesChanged)
    inline QStringList supportedMimeTypes() const
    { return qvariant_cast< QStringList >(property("SupportedMimeTypes")); }

    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes NOTIFY supportedUriSchemesChanged)
    inline QStringList supportedUriSchemes() const
    { return qvariant_cast< QStringList >(property("SupportedUriSchemes")); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> Quit()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Quit"), argumentList);
    }

    inline QDBusPendingReply<> Raise()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Raise"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void canQuitChanged(bool canQuit);
    void canRaiseChanged(bool canRaise);
    void canSetFullscreenChanged(bool canSetFullscreen);
    void desktopEntryChanged(const QString &desktopEntry);
    void fullscreenChanged(bool fullscreen);
    void hasTrackListChanged(bool hasTrackList);
    void identityChanged(const QString &identity);
    void supportedMimeTypesChanged(const QStringList &supportedMimeTypes);
    void supportedUriSchemesChanged(const QStringList &supportedUriSchemes);

private Q_SLOTS:
    void onPropertyChanged(const QString &propertyName, const QVariant &value);
};

namespace org {
    namespace mpris {
        typedef ::MprisRootInterface MediaPlayer2;
    }
}


/*
 * Proxy class for interface org.mpris.MediaPlayer2
 */
class MprisPlayerInterface: public DBusExtendedAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.mpris.MediaPlayer2.Player"; }

public:
    MprisPlayerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~MprisPlayerInterface();

    Q_PROPERTY(bool CanControl READ canControl)
    inline bool canControl() const
    { return qvariant_cast< bool >(property("CanControl")); }

    Q_PROPERTY(bool CanGoNext READ canGoNext NOTIFY canGoNextChanged)
    inline bool canGoNext() const
    { return qvariant_cast< bool >(property("CanGoNext")); }

    Q_PROPERTY(bool CanGoPrevious READ canGoPrevious NOTIFY canGoPreviousChanged)
    inline bool canGoPrevious() const
    { return qvariant_cast< bool >(property("CanGoPrevious")); }

    Q_PROPERTY(bool CanPause READ canPause NOTIFY canPauseChanged)
    inline bool canPause() const
    { return qvariant_cast< bool >(property("CanPause")); }

    Q_PROPERTY(bool CanPlay READ canPlay NOTIFY canPlayChanged)
    inline bool canPlay() const
    { return qvariant_cast< bool >(property("CanPlay")); }

    Q_PROPERTY(bool CanSeek READ canSeek NOTIFY canSeekChanged)
    inline bool canSeek() const
    { return qvariant_cast< bool >(property("CanSeek")); }

    Q_PROPERTY(QString LoopStatus READ loopStatus WRITE setLoopStatus NOTIFY loopStatusChanged)
    inline QString loopStatus() const
    { return qvariant_cast< QString >(property("LoopStatus")); }
    inline void setLoopStatus(const QString &value)
    { setProperty("LoopStatus", QVariant::fromValue(value)); }

    Q_PROPERTY(double MaximumRate READ maximumRate NOTIFY maximumRateChanged)
    inline double maximumRate() const
    { return qvariant_cast< double >(property("MaximumRate")); }

    Q_PROPERTY(QVariantMap Metadata READ metadata NOTIFY metadataChanged)
    inline QVariantMap metadata() const
    { return qvariant_cast< QVariantMap >(property("Metadata")); }

    Q_PROPERTY(double MinimumRate READ minimumRate NOTIFY minimumRateChanged)
    inline double minimumRate() const
    { return qvariant_cast< double >(property("MinimumRate")); }

    Q_PROPERTY(QString PlaybackStatus READ playbackStatus NOTIFY playbackStatusChanged)
    inline QString playbackStatus() const
    { return qvariant_cast< QString >(property("PlaybackStatus")); }

    Q_PROPERTY(qlonglong Position READ position)
    inline qlonglong position() const
    { return qvariant_cast< qlonglong >(property("Position")); }

    Q_PROPERTY(double Rate READ rate WRITE setRate NOTIFY rateChanged)
    inline double rate() const
    { return qvariant_cast< double >(property("Rate")); }
    inline void setRate(double value)
    { setProperty("Rate", QVariant::fromValue(value)); }

    Q_PROPERTY(bool Shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged)
    inline bool shuffle() const
    { return qvariant_cast< bool >(property("Shuffle")); }
    inline void setShuffle(bool value)
    { setProperty("Shuffle", QVariant::fromValue(value)); }

    Q_PROPERTY(double Volume READ volume WRITE setVolume NOTIFY volumeChanged)
    inline double volume() const
    { return qvariant_cast< double >(property("Volume")); }
    inline void setVolume(double value)
    { setProperty("Volume", QVariant::fromValue(value)); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> Next()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Next"), argumentList);
    }

    inline QDBusPendingReply<> OpenUri(const QString &Uri)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(Uri);
        return asyncCallWithArgumentList(QLatin1String("OpenUri"), argumentList);
    }

    inline QDBusPendingReply<> Pause()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Pause"), argumentList);
    }

    inline QDBusPendingReply<> Play()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Play"), argumentList);
    }

    inline QDBusPendingReply<> PlayPause()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("PlayPause"), argumentList);
    }

    inline QDBusPendingReply<> Previous()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Previous"), argumentList);
    }

    inline QDBusPendingReply<> Seek(qlonglong Offset)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(Offset);
        return asyncCallWithArgumentList(QLatin1String("Seek"), argumentList);
    }

    inline QDBusPendingReply<> SetPosition(const QDBusObjectPath &TrackId, qlonglong Position)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(TrackId) << QVariant::fromValue(Position);
        return asyncCallWithArgumentList(QLatin1String("SetPosition"), argumentList);
    }

    inline QDBusPendingReply<> Stop()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Stop"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void canGoNextChanged(bool canGoNext);
    void canGoPreviousChanged(bool canGoPrevious);
    void canPauseChanged(bool canPause);
    void canPlayChanged(bool canPlay);
    void canSeekChanged(bool canSeek);
    void loopStatusChanged(const QString &loopStatus);
    void maximumRateChanged(double maximumRate);
    void metadataChanged(const QVariantMap &metadata);
    void minimumRateChanged(double minimumRate);
    void playbackStatusChanged(const QString &playbackStatus);
    void rateChanged(double rate);
    void shuffleChanged(bool shuffle);
    void volumeChanged(double volume);
    void seeked(qlonglong Position);

private Q_SLOTS:
    void onPropertyChanged(const QString &propertyName, const QVariant &value);
};

namespace MediaPlayer2 {
    typedef ::MprisPlayerInterface Player;
}

#endif /* MPRISROOTINTERFACE_P_H */
