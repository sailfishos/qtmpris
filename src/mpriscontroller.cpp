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


#include "mpriscontroller.h"

#include "mpriscontroller_p.h"

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

static const QString mprisObjectPath = QStringLiteral("/org/mpris/MediaPlayer2");

MprisController::MprisController(const QString &service, const QDBusConnection &connection, QObject *parent)
    : QObject(parent)
    , m_mprisRootInterface(new MprisRootInterface(service, mprisObjectPath, connection, this))
    , m_mprisPlayerInterface(new MprisPlayerInterface(service, mprisObjectPath, connection, this))
    , m_validated(false)
    , m_canQuit(false)
    , m_canRaise(false)
    , m_canSetFullscreen(false)
    , m_desktopEntry()
    , m_fullscreen(false)
    , m_hasTrackList(false)
    , m_identity()
    , m_supportedUriSchemes()
    , m_supportedMimeTypes()
    , m_canControl(false)
    , m_canGoNext(false)
    , m_canGoPrevious(false)
    , m_canPause(false)
    , m_canPlay(false)
    , m_canSeek(false)
    , m_loopStatus(Mpris::None)
    , m_maximumRate(1)
    , m_metadata()
    , m_typedMetadata()
    , m_minimumRate(1)
    , m_playbackStatus(Mpris::Stopped)
    , m_rate(1)
    , m_shuffle(false)
    , m_volume(0)
{
    // Mpris Root Interface
    connect(m_mprisRootInterface, SIGNAL(canQuitChanged(bool)), this, SLOT(onCanQuitChanged(bool)));
    connect(m_mprisRootInterface, SIGNAL(canRaiseChanged(bool)), this, SLOT(onCanRaiseChanged(bool)));
    connect(m_mprisRootInterface, SIGNAL(canSetFullscreenChanged(bool)), this, SLOT(onCanSetFullscreenChanged(bool)));
    connect(m_mprisRootInterface, SIGNAL(desktopEntryChanged(QString)), this, SLOT(onDesktopEntryChanged(QString)));
    connect(m_mprisRootInterface, SIGNAL(fullscreenChanged(bool)), this, SLOT(onFullscreenChanged(bool)));
    connect(m_mprisRootInterface, SIGNAL(hasTrackListChanged(bool)), this, SLOT(onHasTrackListChanged(bool)));
    connect(m_mprisRootInterface, SIGNAL(identityChanged(QString)), this, SLOT(onIdentityChanged(QString)));
    connect(m_mprisRootInterface, SIGNAL(supportedMimeTypesChanged(QStringList)), this, SLOT(onSupportedMimeTypesChanged(QStringList)));
    connect(m_mprisRootInterface, SIGNAL(supportedUriSchemesChanged(QStringList)), this, SLOT(onSupportedUriSchemesChanged(QStringList)));

    // Mpris Player Interface
    connect(m_mprisPlayerInterface, SIGNAL(canGoNextChanged(bool)), this, SLOT(onCanGoNextChanged(bool)));
    connect(m_mprisPlayerInterface, SIGNAL(canGoPreviousChanged(bool)), this, SLOT(onCanGoPreviousChanged(bool)));
    connect(m_mprisPlayerInterface, SIGNAL(canPauseChanged(bool)), this, SLOT(onCanPauseChanged(bool)));
    connect(m_mprisPlayerInterface, SIGNAL(canPlayChanged(bool)), this, SLOT(onCanPlayChanged(bool)));
    connect(m_mprisPlayerInterface, SIGNAL(canSeekChanged(bool)), this, SLOT(onCanSeekChanged(bool)));
    connect(m_mprisPlayerInterface, SIGNAL(canSeekChanged(bool)), this, SLOT(onCanSeekChanged(bool)));
    connect(m_mprisPlayerInterface, SIGNAL(loopStatusChanged(QString)), this, SLOT(onLoopStatusChanged(QString)));
    connect(m_mprisPlayerInterface, SIGNAL(maximumRateChanged(double)), this, SLOT(onMaximumRateChanged(double)));
    connect(m_mprisPlayerInterface, SIGNAL(metadataChanged(QVariantMap)), this, SLOT(onMetadataChanged(QVariantMap)));
    connect(m_mprisPlayerInterface, SIGNAL(minimumRateChanged(double)), this, SLOT(onMinimumRateChanged(double)));
    connect(m_mprisPlayerInterface, SIGNAL(playbackStatusChanged(QString)), this, SLOT(onPlaybackStatusChanged(QString)));
    connect(m_mprisPlayerInterface, SIGNAL(rateChanged(double)), this, SLOT(onRateChanged(double)));
    connect(m_mprisPlayerInterface, SIGNAL(shuffleChanged(bool)), this, SLOT(onShuffleChanged(bool)));
    connect(m_mprisPlayerInterface, SIGNAL(volumeChanged(double)), this, SLOT(onVolumeChanged(double)));
    connect(m_mprisPlayerInterface, SIGNAL(seeked(qlonglong)), this, SIGNAL(seeked(qlonglong)));

    // This will initialize the properties, if needed
    isValid();
}

MprisController::~MprisController()
{
}

bool MprisController::isValid() const
{
    bool validNow = m_mprisRootInterface->isValid() && m_mprisPlayerInterface->isValid();
    if (m_validated != validNow) {
        m_validated = validNow;

        // We are calling this from properties' getters so we don't want
        // to emit a propertyChanged signals from there.
        QTimer::singleShot(0, this, SLOT(initProperties()));
    }

    return m_validated;
}

// Mpris2 Root Interface
bool MprisController::quit()
{
    if (!canQuit()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = m_mprisRootInterface->Quit();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}

bool MprisController::raise()
{
    if (!canRaise()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = m_mprisRootInterface->Raise();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}

// Mpris2 Player Interface
bool MprisController::next()
{
    if (!canGoNext()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = m_mprisPlayerInterface->Next();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}

bool MprisController::openUri(const QUrl &uri)
{
    if (!canControl()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    if (!uri.isValid()) {
        qDebug() << Q_FUNC_INFO << "The uri is invalid";
        return false;
    }

    if (!m_supportedUriSchemes.contains(uri.scheme())) {
        qDebug() << Q_FUNC_INFO << "The scheme is not supported";
        return false;
    }

    QMimeDatabase db;
    QMimeType mime;
    if (uri.isLocalFile()) {
        mime = db.mimeTypeForFile(uri.toLocalFile());
    } else {
        mime = db.mimeTypeForFile(uri.fileName(), QMimeDatabase::MatchExtension);
    }
    QStringList mimeNames = mime.aliases();
    mimeNames.prepend(mime.name());
    for (int i = 0; i < mimeNames.size(); i++) {
        if (m_supportedMimeTypes.contains(mimeNames[i])) {
            QDBusPendingReply<> reply = m_mprisPlayerInterface->OpenUri(uri.toString());
            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
            QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                             this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));
            return true;
        }
    }

    qDebug() << Q_FUNC_INFO << "The mime type is not supported";
    return false;
}

bool MprisController::pause()
{
    if (!canPause()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = m_mprisPlayerInterface->Pause();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}

bool MprisController::play()
{
    if (!canPlay()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = m_mprisPlayerInterface->Play();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}

bool MprisController::playPause()
{
    if (!canPause()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = m_mprisPlayerInterface->PlayPause();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}

bool MprisController::previous()
{
    if (!canGoPrevious()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = m_mprisPlayerInterface->Previous();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}

bool MprisController::seek(qlonglong offset)
{
    if (!canSeek()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = m_mprisPlayerInterface->Seek(offset);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}

bool MprisController::setPosition(qlonglong position)
{
    if (!canSeek()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QVariant trackId = m_metadata[Mpris::metadataToString(Mpris::TrackId)];
    if (!trackId.isValid()) {
        qDebug() << Q_FUNC_INFO << "Unknown trackId in which to set the position";
        return false;
    }

    return setPosition(trackId.value<QDBusObjectPath>().path(), position);
}

bool MprisController::setPosition(const QString &aTrackId, qlonglong position)
{
    if (!canSeek()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusObjectPath trackId(aTrackId);
    if (trackId.path().isEmpty()) {
        qDebug() << Q_FUNC_INFO << "trackId doesn't map to a valid DBus object path";
        return false;
    }

    QVariant length = m_metadata[Mpris::metadataToString(Mpris::Length)];
    if (!length.isValid()) {
        qlonglong reportedLength = length.toLongLong();

        if (position < 0 || position > reportedLength) {
            qDebug() << Q_FUNC_INFO << "Unknown trackId in which to set the position";
            return false;
        }
    }

    QDBusPendingReply<> reply = m_mprisPlayerInterface->SetPosition(trackId, position);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}

bool MprisController::stop()
{
    if (!canControl()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = m_mprisPlayerInterface->Stop();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onFinishedPendingCall(QDBusPendingCallWatcher*)));

    return true;
}


// Slots

QString MprisController::service() const
{
    return m_mprisRootInterface->service();
}

// Mpris2 Root Interface
bool MprisController::canQuit() const
{
    if (isValid()) {
        return m_canQuit;
    }

    return false;
}

bool MprisController::canRaise() const
{
    if (isValid()) {
        return m_canRaise;
    }

    return false;
}

bool MprisController::canSetFullscreen() const
{
    if (isValid()) {
        return m_canSetFullscreen;
    }

    return false;
}

QString MprisController::desktopEntry() const
{
    if (isValid()) {
        return m_desktopEntry;
    }

    return QString();
}

bool MprisController::fullscreen() const
{
    if (isValid()) {
        return m_fullscreen;
    }

    return false;
}

void MprisController::setFullscreen(bool fullscreen)
{
    if (isValid()) {
        m_mprisRootInterface->setFullscreen(fullscreen);
    }
}

bool MprisController::hasTrackList() const
{
    if (isValid()) {
        return m_hasTrackList;
    }

    return false;
}

QString MprisController::identity() const
{
    if (isValid()) {
        return m_identity;
    }

    return QString();
}

QStringList MprisController::supportedUriSchemes() const
{
    if (isValid()) {
        return m_supportedUriSchemes;
    }

    return QStringList();
}

QStringList MprisController::supportedMimeTypes() const
{
    if (isValid()) {
        return m_supportedMimeTypes;
    }

    return QStringList();
}

// Mpris2 Player Interface
bool MprisController::canControl() const
{
    if (isValid()) {
        return m_canControl;
    }

    return false;
}

bool MprisController::canGoNext() const
{
    if (isValid()) {
        return m_canGoNext;
    }

    return false;
}

bool MprisController::canGoPrevious() const
{
    if (isValid()) {
        return m_canGoPrevious;
    }

    return false;
}

bool MprisController::canPause() const
{
    if (isValid()) {
        return m_canPause;
    }

    return false;
}

bool MprisController::canPlay() const
{
    if (isValid()) {
        return m_canPlay;
    }

    return false;
}

bool MprisController::canSeek() const
{
    if (isValid()) {
        return m_canSeek;
    }

    return false;
}

Mpris::LoopStatus MprisController::loopStatus() const
{
    if (isValid()) {
        return m_loopStatus;
    }

    return Mpris::None;
}

void MprisController::setLoopStatus(Mpris::LoopStatus loopStatus)
{
    if (isValid()) {
        m_mprisPlayerInterface->setLoopStatus(Mpris::enumerationToString(loopStatus));
    }
}

double MprisController::maximumRate() const
{
    if (isValid()) {
        return m_maximumRate;
    }

    return 1;
}

QVariantMap MprisController::metadata() const
{
    if (isValid()) {
        return m_typedMetadata;
    }

    return QVariantMap();
}

double MprisController::minimumRate() const
{
    if (isValid()) {
        return m_minimumRate;
    }

    return 1;
}

Mpris::PlaybackStatus MprisController::playbackStatus() const
{
    if (isValid()) {
        return m_playbackStatus;
    }

    return Mpris::Stopped;
}

qlonglong MprisController::position() const
{
    if (isValid()) {
        return m_mprisPlayerInterface->position();
    }

    return -1;
}

double MprisController::rate() const
{
    if (isValid()) {
        return m_rate;
    }

    return 1;
}

void MprisController::setRate(double rate)
{
    if (isValid()) {
        m_mprisPlayerInterface->setRate(rate);
    }
}

bool MprisController::shuffle() const
{
    if (isValid()) {
        return m_shuffle;
    }

    return false;
}

void MprisController::setShuffle(bool shuffle)
{
    if (isValid()) {
        m_mprisPlayerInterface->setShuffle(shuffle);
    }
}

double MprisController::volume() const
{
    if (isValid()) {
        return m_volume;
    }

    return 0;
}

void MprisController::setVolume(double volume)
{
    if (isValid()) {
        m_mprisPlayerInterface->setVolume(volume);
    }
}


// Protected

void MprisController::onCanQuitChanged(bool aCanQuit)
{
    bool canQuit = m_validated ? aCanQuit : false;
    if (m_canQuit != canQuit) {
        m_canQuit = canQuit;
        emit canQuitChanged();
    }
}

void MprisController::onCanRaiseChanged(bool aCanRaise)
{
    bool canRaise = m_validated ? aCanRaise : false;
    if (m_canRaise != canRaise) {
        m_canRaise = canRaise;
        emit canRaiseChanged();
    }
}

void MprisController::onCanSetFullscreenChanged(bool aCanSetFullscreen)
{
    bool canSetFullscreen = m_validated ? aCanSetFullscreen : false;
    if (m_canSetFullscreen != canSetFullscreen) {
        m_canSetFullscreen = canSetFullscreen;
        emit canSetFullscreenChanged();
    }
}

void MprisController::onDesktopEntryChanged(const QString &aDesktopEntry)
{
    QString desktopEntry = m_validated ? aDesktopEntry : QString();
    if (m_desktopEntry != desktopEntry) {
        m_desktopEntry = desktopEntry;
        emit desktopEntryChanged();
    }
}

void MprisController::onFullscreenChanged(bool aFullscreen)
{
    bool fullscreen = m_validated ? aFullscreen : false;
    if (m_fullscreen != fullscreen) {
        m_fullscreen = fullscreen;
        emit fullscreenChanged();
    }
}

void MprisController::onHasTrackListChanged(bool aHasTrackList)
{
    bool hasTrackList = m_validated ? aHasTrackList : false;
    if (m_hasTrackList != hasTrackList) {
        m_hasTrackList = hasTrackList;
        emit hasTrackListChanged();
    }
}

void MprisController::onIdentityChanged(const QString &aIdentity)
{
    QString identity = m_validated ? aIdentity : QString();
    if (m_identity != identity) {
        m_identity = identity;
        emit identityChanged();
    }
}

void MprisController::onSupportedUriSchemesChanged(const QStringList &aSupportedUriSchemes)
{
    QStringList supportedUriSchemes = m_validated ? aSupportedUriSchemes : QStringList();
    if (m_supportedUriSchemes != supportedUriSchemes) {
        m_supportedUriSchemes = supportedUriSchemes;
        emit supportedUriSchemesChanged();
    }
}

void MprisController::onSupportedMimeTypesChanged(const QStringList &aSupportedMimeTypes)
{
    QStringList supportedMimeTypes = m_validated ? aSupportedMimeTypes : QStringList();
    if (m_supportedMimeTypes != supportedMimeTypes) {
        m_supportedMimeTypes = supportedMimeTypes;
        emit supportedMimeTypesChanged();
    }
}

void MprisController::onCanGoNextChanged(bool aCanGoNext)
{
    bool canGoNext = m_validated && m_canControl ? aCanGoNext : false;
    if (m_canGoNext != canGoNext) {
        m_canGoNext = canGoNext;
        emit canGoNextChanged();
    }
}

void MprisController::onCanGoPreviousChanged(bool aCanGoPrevious)
{
    bool canGoPrevious = m_validated && m_canControl ? aCanGoPrevious : false;
    if (m_canGoPrevious != canGoPrevious) {
        m_canGoPrevious = canGoPrevious;
        emit canGoPreviousChanged();
    }
}

void MprisController::onCanPauseChanged(bool aCanPause)
{
    bool canPause = m_validated && m_canControl ? aCanPause : false;
    if (m_canPause != canPause) {
        m_canPause = canPause;
        emit canPauseChanged();
    }
}

void MprisController::onCanPlayChanged(bool aCanPlay)
{
    bool canPlay = m_validated && m_canControl ? aCanPlay : false;
    if (m_canPlay != canPlay) {
        m_canPlay = canPlay;
        emit canPlayChanged();
    }
}

void MprisController::onCanSeekChanged(bool aCanSeek)
{
    bool canSeek = m_validated && m_canControl ? aCanSeek : false;
    if (m_canSeek != canSeek) {
        m_canSeek = canSeek;
        emit canSeekChanged();
    }
}

void MprisController::onLoopStatusChanged(const QString &aLoopStatus)
{
    Mpris::LoopStatus loopStatus = m_validated ? Mpris::enumerationFromString<Mpris::LoopStatus>(aLoopStatus) : Mpris::None;
    if (m_loopStatus != loopStatus) {
        m_loopStatus = loopStatus;
        emit loopStatusChanged();
    }
}

void MprisController::onMaximumRateChanged(double aMaximumRate)
{
    double maximumRate = m_validated ? aMaximumRate : 1;
    if (m_maximumRate != maximumRate) {
        m_maximumRate = maximumRate;
        emit maximumRateChanged();
    }
}

void MprisController::onMetadataChanged(const QVariantMap &aMetadata)
{
    QVariantMap metadata = m_validated ? aMetadata : QVariantMap();
    if (m_metadata != metadata) {
        m_metadata = metadata;
        emit metadataChanged();
    }
}

void MprisController::onMinimumRateChanged(double aMinimumRate)
{
    double minimumRate = m_validated ? aMinimumRate : 1;
    if (m_minimumRate != minimumRate) {
        m_minimumRate = minimumRate;
        emit minimumRateChanged();
    }
}

void MprisController::onPlaybackStatusChanged(const QString &aPlaybackStatus)
{
    Mpris::PlaybackStatus playbackStatus = m_validated ? Mpris::enumerationFromString<Mpris::PlaybackStatus>(aPlaybackStatus) : Mpris::Stopped;
    if (m_playbackStatus != playbackStatus) {
        m_playbackStatus = playbackStatus;
        emit playbackStatusChanged();
    }
}

void MprisController::onRateChanged(double aRate)
{
    double rate = m_validated ? aRate : 1;
    if (m_rate != rate) {
        m_rate = rate;
        emit rateChanged();
    }
}

void MprisController::onShuffleChanged(bool aShuffle)
{
    bool shuffle = m_validated ? aShuffle : false;
    if (m_shuffle != shuffle) {
        m_shuffle = shuffle;
        emit shuffleChanged();
    }
}

void MprisController::onVolumeChanged(double aVolume)
{
    double volume = m_validated ? aVolume : 0;
    if (m_volume != volume) {
        m_volume = volume;
        emit volumeChanged();
    }
}


// Private

void MprisController::initCanControl(bool aCanControl)
{
    bool canControl = m_validated ? aCanControl : false;
    if (m_canControl != canControl) {
        m_canControl = canControl;
        emit canControlChanged();
    }
}

void MprisController::initProperties()
{
    if (m_validated) {
        onCanQuitChanged(m_mprisRootInterface->canQuit());
        onCanRaiseChanged(m_mprisRootInterface->canRaise());
        onCanSetFullscreenChanged(m_mprisRootInterface->canSetFullscreen());
        onDesktopEntryChanged(m_mprisRootInterface->desktopEntry());
        onFullscreenChanged(m_mprisRootInterface->fullscreen());
        onHasTrackListChanged(m_mprisRootInterface->hasTrackList());
        onIdentityChanged(m_mprisRootInterface->identity());
        onSupportedUriSchemesChanged(m_mprisRootInterface->supportedUriSchemes());
        onSupportedMimeTypesChanged(m_mprisRootInterface->supportedMimeTypes());
        // Order matters
        initCanControl(m_mprisPlayerInterface->canControl());
        if (m_canControl) {
            onCanGoNextChanged(m_mprisPlayerInterface->canGoNext());
            onCanGoPreviousChanged(m_mprisPlayerInterface->canGoPrevious());
            onCanPauseChanged(m_mprisPlayerInterface->canPause());
            onCanPlayChanged(m_mprisPlayerInterface->canPlay());
            onCanSeekChanged(m_mprisPlayerInterface->canSeek());
        }
        onLoopStatusChanged(m_mprisPlayerInterface->loopStatus());
        onMaximumRateChanged(m_mprisPlayerInterface->maximumRate());
        onMetadataChanged(m_mprisPlayerInterface->metadata());
        onMinimumRateChanged(m_mprisPlayerInterface->minimumRate());
        onPlaybackStatusChanged(m_mprisPlayerInterface->playbackStatus());
        onRateChanged(m_mprisPlayerInterface->rate());
        onShuffleChanged(m_mprisPlayerInterface->shuffle());
        onVolumeChanged(m_mprisPlayerInterface->volume());
    } else {
        onCanQuitChanged(false);
        onCanRaiseChanged(false);
        onCanSetFullscreenChanged(false);
        onDesktopEntryChanged(QString());
        onFullscreenChanged(false);
        onHasTrackListChanged(false);
        onIdentityChanged(QString());
        onSupportedUriSchemesChanged(QStringList());
        onSupportedMimeTypesChanged(QStringList());
        m_canControl = false;
        onLoopStatusChanged(Mpris::enumerationToString(Mpris::None));
        onMaximumRateChanged(1);
        onMetadataChanged(QVariantMap());
        onMinimumRateChanged(1);
        onPlaybackStatusChanged(Mpris::enumerationToString(Mpris::Stopped));
        onRateChanged(1);
        onShuffleChanged(false);
        onVolumeChanged(0);
    }

    if (!m_canControl) {
        onCanGoNextChanged(false);
        onCanGoPreviousChanged(false);
        onCanPauseChanged(false);
        onCanPlayChanged(false);
        onCanSeekChanged(false);
    }
}

void MprisController::onFinishedPendingCall(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    if (reply.isError()) {
        qWarning() << Q_FUNC_INFO
                   <<"Error" << reply.error().name()
                   << "happened:" << reply.error().message();
    }

    call->deleteLater();
}
