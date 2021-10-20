// -*- c++ -*-

/*!
 *
 * Copyright (C) 2015-2021 Jolla Ltd.
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
#include "mprismetadata_p.h"

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

namespace {
    const QString mprisObjectPath = QStringLiteral("/org/mpris/MediaPlayer2");
};

class MprisControllerPrivate : public QObject {
    Q_OBJECT

public:
    MprisControllerPrivate(const QString &service, const QDBusConnection &connection, MprisController *parent);

    ~MprisControllerPrivate();

public Q_SLOTS:
    void onAsyncGetAllRootPropertiesFinished();
    void onAsyncGetAllPlayerPropertiesFinished();
    void onAsyncPropertyFinished(const QString &propertyName);
    void onCanControlChanged();
    void onMetadataChanged();
    void onPositionChanged(qlonglong aPosition);
    void onFinishedPendingCall(QDBusPendingCallWatcher *call);
    void onPlaybackStatusChanged();
    void onSeeked(qlonglong aPosition);
    void onPositionTimeout();

public:
    MprisRootInterface m_mprisRootInterface;
    MprisPlayerInterface m_mprisPlayerInterface;

    MprisMetaData m_metaData;
    QTimer m_positionTimer;
    MprisController *parent() const;

    mutable bool m_initedRootInterface;
    mutable bool m_initedPlayerInterface;
    mutable bool m_requestedPosition;
    bool m_canControlReceived;
    int m_syncInterval;
    unsigned m_positionConnected;
    qlonglong m_lastPosition;
    qlonglong m_lastStamp;
};

MprisControllerPrivate::MprisControllerPrivate(const QString &service, const QDBusConnection &connection, MprisController *parent)
    : QObject(parent)
    , m_mprisRootInterface(service, mprisObjectPath, connection, this)
    , m_mprisPlayerInterface(service, mprisObjectPath, connection, this)
    , m_metaData(this)
    , m_positionTimer(this)
    , m_initedRootInterface(false)
    , m_initedPlayerInterface(false)
    , m_requestedPosition(false)
    , m_canControlReceived(false)
    , m_syncInterval(5000)
    , m_positionConnected(0)
    , m_lastPosition(-1)
    , m_lastStamp(-1)
{
    m_positionTimer.setInterval(1000);
    m_positionTimer.setSingleShot(false);
    connect(&m_positionTimer, &QTimer::timeout, this, &MprisControllerPrivate::onPositionTimeout);
    connect(&m_mprisPlayerInterface, &DBusExtendedAbstractInterface::asyncPropertyFinished, this, &MprisControllerPrivate::onAsyncPropertyFinished);
}

MprisControllerPrivate::~MprisControllerPrivate()
{
}

void MprisControllerPrivate::onPositionTimeout()
{
    if (QDateTime::currentMSecsSinceEpoch() - m_lastStamp > m_syncInterval) {
        parent()->requestPosition();
    } else {
        Q_EMIT parent()->positionChanged(parent()->position());
    }
}

MprisController *MprisControllerPrivate::parent() const
{
    return static_cast<MprisController *>(QObject::parent());
}

MprisController::MprisController(const QString &service, const QDBusConnection &connection, QObject *parent)
    : QObject(parent)
    , priv(new MprisControllerPrivate(service, connection, this))
{
    // Mpris Root Interface
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::asyncGetAllPropertiesFinished, priv, &MprisControllerPrivate::onAsyncGetAllRootPropertiesFinished);
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::canQuitChanged, this, &MprisController::canQuitChanged);
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::canRaiseChanged, this, &MprisController::canRaiseChanged);
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::canSetFullscreenChanged, this, &MprisController::canSetFullscreenChanged);
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::desktopEntryChanged, this, &MprisController::desktopEntryChanged);
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::fullscreenChanged, this, &MprisController::fullscreenChanged);
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::hasTrackListChanged, this, &MprisController::hasTrackListChanged);
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::identityChanged, this, &MprisController::identityChanged);
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::supportedMimeTypesChanged, this, &MprisController::supportedMimeTypesChanged);
    connect(&priv->m_mprisRootInterface, &MprisRootInterface::supportedUriSchemesChanged, this, &MprisController::supportedUriSchemesChanged);
    priv->m_mprisRootInterface.setUseCache(true);

    // Mpris Player Interface
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::asyncGetAllPropertiesFinished, priv, &MprisControllerPrivate::onAsyncGetAllPlayerPropertiesFinished);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::canControlChanged, priv, &MprisControllerPrivate::onCanControlChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::canGoNextChanged, this, &MprisController::canGoNextChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::canGoPreviousChanged, this, &MprisController::canGoPreviousChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::canPauseChanged, this, &MprisController::canPauseChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::canPlayChanged, this, &MprisController::canPlayChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::canSeekChanged, this, &MprisController::canSeekChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::canSeekChanged, this, &MprisController::canSeekChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::loopStatusChanged, this, &MprisController::loopStatusChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::maximumRateChanged, this, &MprisController::maximumRateChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::metadataChanged, priv, &MprisControllerPrivate::onMetadataChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::minimumRateChanged, this, &MprisController::minimumRateChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::playbackStatusChanged, priv, &MprisControllerPrivate::onPlaybackStatusChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::positionChanged, priv, &MprisControllerPrivate::onPositionChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::rateChanged, this, &MprisController::rateChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::shuffleChanged, this, &MprisController::shuffleChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::volumeChanged, this, &MprisController::volumeChanged);
    connect(&priv->m_mprisPlayerInterface, &MprisPlayerInterface::Seeked, priv, &MprisControllerPrivate::onSeeked);
    priv->m_mprisPlayerInterface.setUseCache(true);

    priv->m_mprisRootInterface.getAllProperties();
    priv->m_mprisPlayerInterface.getAllProperties();
}

MprisController::~MprisController()
{
}

bool MprisController::isValid() const
{
    return priv->m_initedRootInterface && priv->m_initedPlayerInterface;
}

int MprisController::positionInterval() const
{
    return priv->m_positionTimer.interval();
}

void MprisController::setPositionInterval(int interval)
{
    priv->m_positionTimer.setInterval(interval);
}

// Mpris2 Root Interface
bool MprisController::quit()
{
    if (!canQuit()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = priv->m_mprisRootInterface.Quit();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

    return true;
}

bool MprisController::raise()
{
    if (!canRaise()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = priv->m_mprisRootInterface.Raise();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

    return true;
}

// Mpris2 Player Interface
bool MprisController::next()
{
    if (!canGoNext()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = priv->m_mprisPlayerInterface.Next();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

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

    if (!priv->m_mprisRootInterface.supportedUriSchemes().contains(uri.scheme())) {
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
        if (priv->m_mprisRootInterface.supportedMimeTypes().contains(mimeNames[i])) {
            QDBusPendingReply<> reply = priv->m_mprisPlayerInterface.OpenUri(uri.toString());
            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
            QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                             priv, &MprisControllerPrivate::onFinishedPendingCall);
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

    QDBusPendingReply<> reply = priv->m_mprisPlayerInterface.Pause();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

    return true;
}

bool MprisController::play()
{
    if (!canPlay()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = priv->m_mprisPlayerInterface.Play();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

    return true;
}

bool MprisController::playPause()
{
    if (!canPlay() && !canPause()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = priv->m_mprisPlayerInterface.PlayPause();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

    return true;
}

bool MprisController::previous()
{
    if (!canGoPrevious()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = priv->m_mprisPlayerInterface.Previous();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

    return true;
}

bool MprisController::seek(qlonglong offset)
{
    if (!canSeek()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = priv->m_mprisPlayerInterface.Seek(offset);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

    return true;
}

bool MprisController::setPosition(qlonglong position)
{
    if (!canSeek()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QVariant trackId = metaData()->trackId();
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

    QVariant length = metaData()->duration();
    if (length.isValid()) {
        qlonglong reportedLength = length.toLongLong();

        if (position < 0 || position > reportedLength) {
            qDebug() << Q_FUNC_INFO << "Requested position out of range";
            return false;
        }
    }

    QDBusPendingReply<> reply = priv->m_mprisPlayerInterface.SetPosition(trackId, position * 1000);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

    return true;
}

bool MprisController::stop()
{
    if (!canControl()) {
        qDebug() << Q_FUNC_INFO << "The method is not allowed";
        return false;
    }

    QDBusPendingReply<> reply = priv->m_mprisPlayerInterface.Stop();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     priv, &MprisControllerPrivate::onFinishedPendingCall);

    return true;
}


// Slots

QString MprisController::service() const
{
    return priv->m_mprisRootInterface.service();
}

// Mpris2 Root Interface
bool MprisController::canQuit() const
{
    return priv->m_mprisRootInterface.canQuit();
}

bool MprisController::canRaise() const
{
    return priv->m_mprisRootInterface.canRaise();
}

bool MprisController::canSetFullscreen() const
{
    return priv->m_mprisRootInterface.canSetFullscreen();
}

QString MprisController::desktopEntry() const
{
    return priv->m_mprisRootInterface.desktopEntry();
}

bool MprisController::fullscreen() const
{
    return priv->m_mprisRootInterface.fullscreen();
}

void MprisController::setFullscreen(bool fullscreen)
{
    priv->m_mprisRootInterface.setFullscreen(fullscreen);
}

bool MprisController::hasTrackList() const
{
    return priv->m_mprisRootInterface.hasTrackList();
}

QString MprisController::identity() const
{
    return priv->m_mprisRootInterface.identity();
}

QStringList MprisController::supportedUriSchemes() const
{
    return priv->m_mprisRootInterface.supportedUriSchemes();
}

QStringList MprisController::supportedMimeTypes() const
{
    return priv->m_mprisRootInterface.supportedMimeTypes();
}

// Mpris2 Player Interface
bool MprisController::canControl() const
{
    return priv->m_mprisPlayerInterface.canControl();
}

bool MprisController::canGoNext() const
{
    if (canControl()) {
        return priv->m_mprisPlayerInterface.canGoNext();
    }

    return false;
}

bool MprisController::canGoPrevious() const
{
    if (canControl()) {
        return priv->m_mprisPlayerInterface.canGoPrevious();
    }

    return false;
}

bool MprisController::canPause() const
{
    if (canControl()) {
        return priv->m_mprisPlayerInterface.canPause();
    }

    return false;
}

bool MprisController::canPlay() const
{
    if (canControl()) {
        return priv->m_mprisPlayerInterface.canPlay();
    }

    return false;
}

bool MprisController::canSeek() const
{
    if (canControl()) {
        return priv->m_mprisPlayerInterface.canSeek();
    }

    return false;
}

Mpris::LoopStatus MprisController::loopStatus() const
{
    bool ok;
    int enumVal = QMetaEnum::fromType<Mpris::LoopStatus>().keyToValue(priv->m_mprisPlayerInterface.loopStatus().toUtf8(), &ok);

    if (ok) {
        return static_cast<Mpris::LoopStatus>(enumVal);
    }

    return Mpris::None;
}

void MprisController::setLoopStatus(Mpris::LoopStatus loopStatus)
{
    const char *strVal = QMetaEnum::fromType<Mpris::LoopStatus>().valueToKey(loopStatus);
    priv->m_mprisPlayerInterface.setLoopStatus(QString::fromLatin1(strVal));
}

double MprisController::maximumRate() const
{
    return priv->m_mprisPlayerInterface.maximumRate();
}

MprisMetaData *MprisController::metaData() const
{
    return &priv->m_metaData;
}

double MprisController::minimumRate() const
{
    return priv->m_mprisPlayerInterface.minimumRate();
}

Mpris::PlaybackStatus MprisController::playbackStatus() const
{
    bool ok;
    int enumVal = QMetaEnum::fromType<Mpris::PlaybackStatus>().keyToValue(priv->m_mprisPlayerInterface.playbackStatus().toUtf8(), &ok);

    if (ok) {
        return static_cast<Mpris::PlaybackStatus>(enumVal);
    }

    return Mpris::Stopped;
}

qlonglong MprisController::position() const
{
    if (playbackStatus() == Mpris::Playing) {
        return priv->m_lastPosition + (QDateTime::currentMSecsSinceEpoch() - priv->m_lastStamp) * priv->m_mprisPlayerInterface.rate();
    }
    return priv->m_lastPosition;
}

void MprisController::requestPosition() const
{
    if (priv->m_requestedPosition) {
        return;
    }

    priv->m_mprisPlayerInterface.setUseCache(false);
    priv->m_mprisPlayerInterface.position();
    priv->m_mprisPlayerInterface.setUseCache(true);
    if (priv->m_mprisPlayerInterface.lastExtendedError().isValid()) {
        qWarning() << Q_FUNC_INFO
                   << "Failed requesting the current position in the MPRIS2 Player Interface!!!";
        return;
    }
    priv->m_requestedPosition = true;
}

double MprisController::rate() const
{
    return priv->m_mprisPlayerInterface.rate();
}

void MprisController::setRate(double rate)
{
    priv->m_mprisPlayerInterface.setRate(rate);
}

bool MprisController::shuffle() const
{
    return priv->m_mprisPlayerInterface.shuffle();
}

void MprisController::setShuffle(bool shuffle)
{
    priv->m_mprisPlayerInterface.setShuffle(shuffle);
}

double MprisController::volume() const
{
    return priv->m_mprisPlayerInterface.volume();
}

void MprisController::setVolume(double volume)
{
    priv->m_mprisPlayerInterface.setVolume(volume);
}

void MprisController::connectNotify(const QMetaMethod &method)
{
    if (method == QMetaMethod::fromSignal(&MprisController::positionChanged)) {
        if (!priv->m_positionConnected++ && playbackStatus() == Mpris::Playing) {
            priv->m_positionTimer.start();
        }
    }

    QObject::connectNotify(method);
}

void MprisController::disconnectNotify(const QMetaMethod &method)
{
    if (method == QMetaMethod::fromSignal(&MprisController::positionChanged)) {
        if (!--priv->m_positionConnected) {
            priv->m_positionTimer.stop();
        }
    }

    QObject::disconnectNotify(method);
}

// Protected

void MprisControllerPrivate::onAsyncGetAllRootPropertiesFinished()
{
    if (m_mprisRootInterface.lastExtendedError().isValid()) {
        qWarning() << Q_FUNC_INFO
                   << "Error" << m_mprisRootInterface.lastExtendedError().name()
                   << "happened:" << m_mprisRootInterface.lastExtendedError().message();
        return;
    }

    m_initedRootInterface = true;

    if (parent()->isValid()) {
            Q_EMIT parent()->isValidChanged();
    }
}

void MprisControllerPrivate::onAsyncGetAllPlayerPropertiesFinished()
{
    if (m_mprisPlayerInterface.lastExtendedError().isValid()) {
        qWarning() << Q_FUNC_INFO
                   << "Error" << m_mprisPlayerInterface.lastExtendedError().name()
                   << "happened:" << m_mprisPlayerInterface.lastExtendedError().message();
        return;
    }

    m_initedPlayerInterface = true;

    if (parent()->isValid()) {
            Q_EMIT parent()->isValidChanged();
    }
}

void MprisControllerPrivate::onAsyncPropertyFinished(const QString &propertyName)
{
    if (propertyName == QLatin1String("Position")) {
        m_requestedPosition = false;
    }
}

void MprisControllerPrivate::onCanControlChanged()
{
    // On first reception, we are using a "GetAll" so we can skip this
    if (m_canControlReceived) {
        // I could disconnect and re-connect the signals so I avoid
        // double arriving signals but this really shouldn't happen
        // ever.
        Q_EMIT parent()->canGoNextChanged();
        Q_EMIT parent()->canGoPreviousChanged();
        Q_EMIT parent()->canPauseChanged();
        Q_EMIT parent()->canPlayChanged();
        Q_EMIT parent()->canSeekChanged();
        qWarning() << Q_FUNC_INFO
                   << "CanControl is not supposed to change its value!";
        return;
    }

    m_canControlReceived = true;
}

void MprisControllerPrivate::onMetadataChanged()
{
    QString oldTrackId = m_metaData.trackId().toString();
    m_metaData.priv->setMetaData(m_mprisPlayerInterface.metadata());

    if (oldTrackId != m_metaData.trackId()) {
        m_lastPosition = 0;
        m_lastStamp = QDateTime::currentMSecsSinceEpoch();
        Q_EMIT parent()->positionChanged(parent()->position());
    }
}

void MprisControllerPrivate::onPositionChanged(qlonglong aPosition)
{
    m_lastStamp = QDateTime::currentMSecsSinceEpoch();
    m_lastPosition = aPosition / 1000;
    Q_EMIT parent()->positionChanged(aPosition / 1000);
}

// Private

void MprisControllerPrivate::onPlaybackStatusChanged()
{
    switch (parent()->playbackStatus()) {
    case Mpris::Paused:
        m_lastPosition += QDateTime::currentMSecsSinceEpoch() - m_lastStamp;
        m_positionTimer.stop();
        break;
    case Mpris::Stopped:
        m_lastPosition = 0;
        m_positionTimer.stop();
        Q_EMIT parent()->positionChanged(0);
        break;

    case Mpris::Playing:
        m_lastStamp = QDateTime::currentMSecsSinceEpoch();
        if (m_positionConnected) {
            m_positionTimer.start();
        }
        break;
    }

    Q_EMIT parent()->playbackStatusChanged();
}

void MprisControllerPrivate::onSeeked(qlonglong aPosition)
{
    m_lastPosition = aPosition / 1000;
    m_lastStamp = QDateTime::currentMSecsSinceEpoch();
    Q_EMIT parent()->positionChanged(aPosition / 1000);
    Q_EMIT parent()->seeked(aPosition);
}

void MprisControllerPrivate::onFinishedPendingCall(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    if (reply.isError()) {
        qWarning() << Q_FUNC_INFO
                   << "Error" << reply.error().name()
                   << "happened:" << reply.error().message();
    }

    call->deleteLater();
}

#include "mpriscontroller.moc"
