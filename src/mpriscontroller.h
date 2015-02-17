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


#ifndef MPRISCONTROLLER_H
#define MPRISCONTROLLER_H

#include <MprisQt>
#include <Mpris>

#include <QDBusConnection>
#include <QDBusObjectPath>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>


class MprisRootInterface;
class MprisPlayerInterface;
class QDBusPendingCallWatcher;
class MPRIS_QT_EXPORT MprisController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString service READ service)

    // Mpris2 Root Interface
    Q_PROPERTY(bool canQuit READ canQuit NOTIFY canQuitChanged)
    Q_PROPERTY(bool canRaise READ canRaise NOTIFY canRaiseChanged)
    Q_PROPERTY(bool canSetFullscreen READ canSetFullscreen NOTIFY canSetFullscreenChanged)
    Q_PROPERTY(QString desktopEntry READ desktopEntry NOTIFY desktopEntryChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(bool hasTrackList READ hasTrackList NOTIFY hasTrackListChanged)
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(QStringList supportedUriSchemes READ supportedUriSchemes NOTIFY supportedUriSchemesChanged)
    Q_PROPERTY(QStringList supportedMimeTypes READ supportedMimeTypes NOTIFY supportedMimeTypesChanged)

    // Mpris2 Player Interface
    Q_PROPERTY(bool canControl READ canControl NOTIFY canControlChanged)
    Q_PROPERTY(bool canGoNext READ canGoNext NOTIFY canGoNextChanged)
    Q_PROPERTY(bool canGoPrevious READ canGoPrevious NOTIFY canGoPreviousChanged)
    Q_PROPERTY(bool canPause READ canPause NOTIFY canPauseChanged)
    Q_PROPERTY(bool canPlay READ canPlay NOTIFY canPlayChanged)
    Q_PROPERTY(bool canSeek READ canSeek NOTIFY canSeekChanged)
    Q_PROPERTY(Mpris::LoopStatus loopStatus READ loopStatus WRITE setLoopStatus NOTIFY loopStatusChanged)
    Q_PROPERTY(double maximumRate READ maximumRate NOTIFY maximumRateChanged)
    Q_PROPERTY(QVariantMap metadata READ metadata NOTIFY metadataChanged)
    Q_PROPERTY(double minimumRate READ minimumRate NOTIFY minimumRateChanged)
    Q_PROPERTY(Mpris::PlaybackStatus playbackStatus READ playbackStatus NOTIFY playbackStatusChanged)
    Q_PROPERTY(qlonglong position READ position)
    Q_PROPERTY(double rate READ rate WRITE setRate NOTIFY rateChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged)
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:

    MprisController(const QString &service, const QDBusConnection &connection, QObject *parent = 0);
    ~MprisController();

    bool isValid() const;

    // Mpris2 Root Interface
    bool quit();
    bool raise();

    // Mpris2 Player Interface
    bool next();
    bool openUri(const QUrl &uri);
    bool pause();
    bool play();
    bool playPause();
    bool previous();
    bool seek(qlonglong offset);
    bool setPosition(qlonglong position);
    bool setPosition(const QString &aTrackId, qlonglong position);
    bool stop();

public slots:

    QString service() const;

    // Mpris2 Root Interface
    bool canQuit() const;

    bool canRaise() const;

    bool canSetFullscreen() const;

    QString desktopEntry() const;

    bool fullscreen() const;
    void setFullscreen(bool fullscreen);

    bool hasTrackList() const;

    QString identity() const;

    QStringList supportedUriSchemes() const;

    QStringList supportedMimeTypes() const;

    // Mpris2 Player Interface
    bool canControl() const;

    bool canGoNext() const;

    bool canGoPrevious() const;

    bool canPause() const;

    bool canPlay() const;

    bool canSeek() const;

    Mpris::LoopStatus loopStatus() const;
    void setLoopStatus(Mpris::LoopStatus loopStatus);

    double maximumRate() const;

    QVariantMap metadata() const;

    double minimumRate() const;

    Mpris::PlaybackStatus playbackStatus() const;

    qlonglong position() const;

    double rate() const;
    void setRate(double rate);

    bool shuffle() const;
    void setShuffle(bool shuffle);

    double volume() const;
    void setVolume(double volume);

signals:

    // Mpris2 Root Interface
    void canQuitChanged();
    void canRaiseChanged();
    void canSetFullscreenChanged();
    void desktopEntryChanged();
    void fullscreenChanged();
    void hasTrackListChanged();
    void identityChanged();
    void supportedUriSchemesChanged();
    void supportedMimeTypesChanged();

    // Mpris2 Player Interface
    void canControlChanged();
    void canGoNextChanged();
    void canGoPreviousChanged();
    void canPauseChanged();
    void canPlayChanged();
    void canSeekChanged();
    void loopStatusChanged();
    void maximumRateChanged();
    void metadataChanged();
    void minimumRateChanged();
    void playbackStatusChanged();
    void rateChanged();
    void shuffleChanged();
    void volumeChanged();
    void seeked(qlonglong position);

protected Q_SLOTS:
    void onCanQuitChanged(bool aCanQuit);
    void onCanRaiseChanged(bool aCanRaise);
    void onCanSetFullscreenChanged(bool aCanSetFullscreen);
    void onDesktopEntryChanged(const QString &aDesktopEntry);
    void onFullscreenChanged(bool aFullscreen);
    void onHasTrackListChanged(bool aHasTrackList);
    void onIdentityChanged(const QString &aIdentity);
    void onSupportedUriSchemesChanged(const QStringList &aSupportedUriSchemes);
    void onSupportedMimeTypesChanged(const QStringList &aSupportedMimeTypes);
    void onCanGoNextChanged(bool aCanGoNext);
    void onCanGoPreviousChanged(bool aCanGoPrevious);
    void onCanPauseChanged(bool aCanPause);
    void onCanPlayChanged(bool aCanPlay);
    void onCanSeekChanged(bool aCanSeek);
    void onLoopStatusChanged(const QString &aLoopStatus);
    void onMaximumRateChanged(double aMaximumRate);
    void onMetadataChanged(const QVariantMap &aMetadata);
    void onMinimumRateChanged(double aMinimumRate);
    void onPlaybackStatusChanged(const QString &aPlaybackStatus);
    void onRateChanged(double aRate);
    void onShuffleChanged(bool aShuffle);
    void onVolumeChanged(double aVolume);
    void initProperties();
    void onFinishedPendingCall(QDBusPendingCallWatcher *call);

private:
    void initCanControl(bool aCanControl);

    MprisRootInterface *m_mprisRootInterface;
    MprisPlayerInterface *m_mprisPlayerInterface;

    mutable bool m_validated;

    // Mpris2 Root Interface
    bool m_canQuit;
    bool m_canRaise;
    bool m_canSetFullscreen;
    QString m_desktopEntry;
    bool m_fullscreen;
    bool m_hasTrackList;
    QString m_identity;
    QStringList m_supportedUriSchemes;
    QStringList m_supportedMimeTypes;

    // Mpris2 Player Interface
    bool m_canControl;
    bool m_canGoNext;
    bool m_canGoPrevious;
    bool m_canPause;
    bool m_canPlay;
    bool m_canSeek;
    Mpris::LoopStatus m_loopStatus;
    double m_maximumRate;
    QVariantMap m_metadata;
    QVariantMap m_typedMetadata;
    double m_minimumRate;
    Mpris::PlaybackStatus m_playbackStatus;
    double m_rate;
    bool m_shuffle;
    double m_volume;
};

#endif /* MPRISCONTROLLER_H */
