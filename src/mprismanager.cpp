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


#include "mprismanager.h"

#include "mpriscontroller.h"
#include "mprisqt_p.h"

#include <QMetaMethod>
#include <QTimer>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

#include <QDebug>

namespace {
    const QString mprisNameSpace = QStringLiteral("org.mpris.MediaPlayer2.");
    const QString dBusService = QStringLiteral("org.freedesktop.DBus");
    const QString dBusObjectPath = QStringLiteral("/org/freedesktop/DBus");
    const QString dBusInterface = QStringLiteral("org.freedesktop.DBus");
    const QString dBusNameOwnerChangedSignal = QStringLiteral("NameOwnerChanged");
}

class MprisManagerPrivate : public QObject
{
    Q_OBJECT

public:
    MprisManagerPrivate(MprisManager *parent);
    ~MprisManagerPrivate();

    MprisManager *parent() const;


public Q_SLOTS:
    void onNameOwnerChanged(const QString &service, const QString &oldOwner, const QString& newOwner);
    void onServiceAppeared(const QString &service);
    void onServiceVanished(const QString &service);
    void onAvailableControllerPlaybackStatusChanged(QWeakPointer<MprisController> wController);

public:
    QSharedPointer<MprisController> availableController(const QString &service);
    void setCurrentController(QSharedPointer<MprisController> controller);
    bool checkController(const char *callerName) const;

    bool m_singleService;
    QSharedPointer<MprisController> m_currentController;
    QDBusConnection m_connection;
    MprisMetaData m_dummyMetaData;
    QList< QSharedPointer<MprisController> > m_availableControllers;
    QList< QSharedPointer<MprisController> > m_otherPlayingControllers;
    unsigned m_positionConnected;
};

MprisManagerPrivate::MprisManagerPrivate(MprisManager *parent)
    : QObject(parent)
    , m_singleService(false)
    , m_connection(getDBusConnection())
    , m_dummyMetaData(this)
    , m_positionConnected(0)
{
    if (!m_connection.isConnected()) {
        qWarning() << "Mpris: Failed attempting to connect to DBus";
        return;
    }

    m_connection.connect(dBusService, dBusObjectPath, dBusInterface, dBusNameOwnerChangedSignal,
                         QStringList(), QString(),
                         this, SLOT(onNameOwnerChanged(QString, QString, QString)));

    QTimer::singleShot(0, [this]() {
        QStringList serviceNames = m_connection.interface()->registeredServiceNames();
        for (auto i = serviceNames.constBegin();
             i != serviceNames.constEnd();
             ++i) {
            if (i->startsWith(mprisNameSpace)) {
                onServiceAppeared(*i);
            }
        }
    });
}

MprisManagerPrivate::~MprisManagerPrivate()
{
}

MprisManager *MprisManagerPrivate::parent() const
{
    return static_cast<MprisManager *>(QObject::parent());
}

MprisManager::MprisManager(QObject *parent)
    : QObject(parent)
    , priv(new MprisManagerPrivate(this))
{
}

MprisManager::~MprisManager()
{
}

// Mpris2 Root Interface
bool MprisManager::quit() const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->quit();
}

bool MprisManager::raise() const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->raise();
}

// Mpris2 Player Interface
bool MprisManager::next() const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->next();
}

bool MprisManager::openUri(const QUrl &uri) const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->openUri(uri);
}

bool MprisManager::pause() const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->pause();
}

bool MprisManager::play() const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->play();
}

bool MprisManager::playPause() const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->playPause();
}

bool MprisManager::previous() const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->previous();
}

bool MprisManager::seek(qlonglong offset) const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->seek(offset);
}

bool MprisManager::setPosition(qlonglong position) const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->setPosition(position);
}

bool MprisManager::setPosition(const QString &trackId, qlonglong position) const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->setPosition(trackId, position);
}

bool MprisManager::stop() const
{
    return priv->checkController(Q_FUNC_INFO) && priv->m_currentController->stop();
}


// Slots

bool MprisManager::singleService() const
{
    return priv->m_singleService;
}

void MprisManager::setSingleService(bool single)
{
    if (priv->m_singleService == single) {
        return;
    }

    priv->m_singleService = single;
    Q_EMIT singleServiceChanged();
}

QString MprisManager::currentService() const
{
    return priv->m_currentController.isNull() ? QString() : priv->m_currentController->service();
}

void MprisManager::setCurrentService(const QString &service)
{
    if (!priv->m_currentController.isNull() && priv->m_currentController->service() == service) {
        return;
    }

    if (!service.startsWith(mprisNameSpace)) {
        qWarning() << "Mpris:" << service << "is not a proper Mpris2 service";
        return;
    }

    QSharedPointer<MprisController> controller = priv->availableController(service);
    if (controller.isNull()) {
        controller = QSharedPointer<MprisController>(new MprisController(service, getDBusConnection(), this));
    } else {
        priv->m_availableControllers.move(priv->m_availableControllers.indexOf(controller), 0);
    }

    priv->setCurrentController(controller);
}

QStringList MprisManager::availableServices() const
{
    QStringList result;

    for (auto i = priv->m_availableControllers.constBegin();
         i != priv->m_availableControllers.constEnd();
         ++i) {
        const QSharedPointer<MprisController> controller = *i;
        result.append(controller->service());
    }

    return result;
}

// Mpris2 Root Interface
bool MprisManager::canQuit() const
{
    return priv->m_currentController && priv->m_currentController->canQuit();
}

bool MprisManager::canRaise() const
{
    return priv->m_currentController && priv->m_currentController->canRaise();
}

bool MprisManager::canSetFullscreen() const
{
    return priv->m_currentController && priv->m_currentController->canSetFullscreen();
}

QString MprisManager::desktopEntry() const
{
    return priv->m_currentController ? priv->m_currentController->desktopEntry() : QString();
}

bool MprisManager::fullscreen() const
{
    return priv->m_currentController && priv->m_currentController->fullscreen();
}

void MprisManager::setFullscreen(bool fullscreen)
{
    if (priv->checkController(Q_FUNC_INFO)) {
        priv->m_currentController->setFullscreen(fullscreen);
    }
}

bool MprisManager::hasTrackList() const
{
    return priv->m_currentController && priv->m_currentController->hasTrackList();
}

QString MprisManager::identity() const
{
    return priv->m_currentController ? priv->m_currentController->identity() : QString();
}

QStringList MprisManager::supportedUriSchemes() const
{
    return priv->m_currentController ? priv->m_currentController->supportedUriSchemes() : QStringList();
}

QStringList MprisManager::supportedMimeTypes() const
{
    return priv->m_currentController ? priv->m_currentController->supportedMimeTypes() : QStringList();
}

// Mpris2 Player Interface
bool MprisManager::canControl() const
{
    return priv->m_currentController && priv->m_currentController->canControl();
}

bool MprisManager::canGoNext() const
{
    return priv->m_currentController && priv->m_currentController->canGoNext();
}

bool MprisManager::canGoPrevious() const
{
    return priv->m_currentController && priv->m_currentController->canGoPrevious();
}

bool MprisManager::canPause() const
{
    return priv->m_currentController && priv->m_currentController->canPause();
}

bool MprisManager::canPlay() const
{
    return priv->m_currentController && priv->m_currentController->canPlay();
}

bool MprisManager::canSeek() const
{
    return priv->m_currentController && priv->m_currentController->canSeek();
}

Mpris::LoopStatus MprisManager::loopStatus() const
{
    return priv->m_currentController ? priv->m_currentController->loopStatus() : Mpris::None;
}

void MprisManager::setLoopStatus(Mpris::LoopStatus loopStatus)
{
    if (priv->checkController(Q_FUNC_INFO)) {
        priv->m_currentController->setLoopStatus(loopStatus);
    }
}

double MprisManager::maximumRate() const
{
    return priv->m_currentController ? priv->m_currentController->maximumRate() : 1;
}

MprisMetaData *MprisManager::metaData() const
{
    return priv->m_currentController ? priv->m_currentController->metaData() : &priv->m_dummyMetaData;
}

double MprisManager::minimumRate() const
{
    return priv->m_currentController ? priv->m_currentController->minimumRate() : 1;
}

Mpris::PlaybackStatus MprisManager::playbackStatus() const
{
    return priv->m_currentController ? priv->m_currentController->playbackStatus() : Mpris::Stopped;
}

qlonglong MprisManager::position() const
{
    return priv->m_currentController ? priv->m_currentController->position() : 0;
}

void MprisManager::requestPosition() const
{
    if (priv->checkController(Q_FUNC_INFO)) {
        priv->m_currentController->requestPosition();
    }
}

double MprisManager::rate() const
{
    return priv->m_currentController ? priv->m_currentController->rate() : 1;
}

void MprisManager::setRate(double rate)
{
    if (priv->checkController(Q_FUNC_INFO)) {
        priv->m_currentController->setRate(rate);
    }
}

bool MprisManager::shuffle() const
{
    return priv->m_currentController && priv->m_currentController->shuffle();
}

void MprisManager::setShuffle(bool shuffle)
{
    if (priv->checkController(Q_FUNC_INFO)) {
        priv->m_currentController->setShuffle(shuffle);
    }
}

double MprisManager::volume() const
{
    return priv->m_currentController ? priv->m_currentController->volume() : 0;
}

void MprisManager::setVolume(double volume)
{
    if (priv->checkController(Q_FUNC_INFO)) {
        priv->m_currentController->setVolume(volume);
    }
}

void MprisManager::connectNotify(const QMetaMethod &method)
{
    if (method == QMetaMethod::fromSignal(&MprisManager::positionChanged)) {
        if (!priv->m_positionConnected++ && priv->m_currentController) {
            connect(priv->m_currentController.data(), &MprisController::positionChanged, this, &MprisManager::positionChanged);
        }
    }
}

void MprisManager::disconnectNotify(const QMetaMethod &method)
{
    if (method == QMetaMethod::fromSignal(&MprisManager::positionChanged)) {
        if (!--priv->m_positionConnected && priv->m_currentController) {
            disconnect(priv->m_currentController.data(), &MprisController::positionChanged, this, &MprisManager::positionChanged);
        }
    }
}

// Private

void MprisManagerPrivate::onNameOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner)
{
    // Unfortunately, QDBus doesn't allow flexible signal watchers.
    // Would it allow them, we could pass the filter "arg0namespace='org.mpris.MediaPlayer2'"
    // Therefore, we will receive here all the name changes in the
    // bus, not just the ones for our name space of interest, and we
    // will have to filter on our own :(

    if (!service.startsWith(mprisNameSpace)) {
        return;
    }

    if (oldOwner.isEmpty()) {
        onServiceAppeared(service);
        return;
    }

    if (newOwner.isEmpty()) {
        onServiceVanished(service);
        return;
    }

    // Service changed owner. Nothing to do ...
}

void MprisManagerPrivate::onServiceAppeared(const QString &service)
{
    QSharedPointer<MprisController> controller = availableController(service);
    if (!controller.isNull()) {
        Q_ASSERT(m_availableControllers.contains(controller));
        m_availableControllers.removeOne(controller);
        m_otherPlayingControllers.removeOne(controller);
    } else {
        if (!m_currentController.isNull() && service == m_currentController->service()) {
            controller = m_currentController;
        } else {
            controller = QSharedPointer<MprisController>(new MprisController(service, getDBusConnection(), this));
        }

        QWeakPointer<MprisController> weakController(controller);

        connect(controller.data(), &MprisController::playbackStatusChanged, this, [this, weakController] { onAvailableControllerPlaybackStatusChanged(weakController); });
    }

    if (m_currentController.isNull()) {
        setCurrentController(controller);
    }

    if (controller == m_currentController) {
        m_availableControllers.prepend(controller);
        Q_EMIT parent()->availableServicesChanged();
        return;
    }

    if (!m_singleService
        && m_currentController->playbackStatus() != Mpris::Playing) {
        m_availableControllers.prepend(controller);
        setCurrentController(controller);
    } else {
        m_availableControllers.insert(1, controller);
        if (controller->playbackStatus() == Mpris::Playing) {
            m_otherPlayingControllers.prepend(controller);
        }
    }

    Q_EMIT parent()->availableServicesChanged();
}

void MprisManagerPrivate::onServiceVanished(const QString &service)
{
    QSharedPointer<MprisController> controller = availableController(service);
    if (!controller.isNull()) {
        Q_ASSERT(m_availableControllers.contains(controller));
        m_availableControllers.removeOne(controller);
        m_otherPlayingControllers.removeOne(controller);
    }

    if (!m_currentController.isNull() && service == m_currentController->service()) {
        if (m_singleService) {
            Q_EMIT parent()->availableServicesChanged();
            return;
        }

        if (!m_availableControllers.isEmpty()) {
            setCurrentController(m_availableControllers[0]);
        } else {
            setCurrentController(QSharedPointer<MprisController>());
        }
    }

    Q_EMIT parent()->availableServicesChanged();
}

void MprisManagerPrivate::onAvailableControllerPlaybackStatusChanged(QWeakPointer<MprisController> wController)
{
    QSharedPointer<MprisController> controller = wController.lock();
    Q_ASSERT(!controller.isNull());

    if (m_currentController == controller) {
        if (m_currentController->playbackStatus() == Mpris::Playing) {
            return;
        }

        if (!m_otherPlayingControllers.isEmpty()) {
            QSharedPointer<MprisController> currentController = m_otherPlayingControllers.takeFirst();
            m_availableControllers.move(m_availableControllers.indexOf(currentController), 0);
            setCurrentController(currentController);
        }
    } else {
        if (controller->playbackStatus() != Mpris::Playing) {
            m_otherPlayingControllers.removeOne(controller);
            return;
        }

        if (!m_singleService
            && m_currentController->playbackStatus() != Mpris::Playing) {
            setCurrentController(controller);
        } else {
            m_availableControllers.move(m_availableControllers.indexOf(controller), 1);
            m_otherPlayingControllers.removeOne(controller); // Just in case, shouldn't be needed
            m_otherPlayingControllers.prepend(controller);
        }
    }
}

QSharedPointer<MprisController> MprisManagerPrivate::availableController(const QString &service)
{
    QList< QSharedPointer<MprisController> >::iterator i = m_availableControllers.begin();
    while (i != m_availableControllers.end()) {
        QSharedPointer<MprisController> controller = *i;
        if (!controller.isNull() && controller->service() == service) {
            return controller;
        }

        ++i;
    }

    return QSharedPointer<MprisController>();
}

void MprisManagerPrivate::setCurrentController(QSharedPointer<MprisController> controller)
{
    if (controller == m_currentController) {
        return;
    }

    if (!m_currentController.isNull()) {
        // Mpris Root Interface
        disconnect(m_currentController.data(), &MprisController::canQuitChanged, parent(), &MprisManager::canQuitChanged);
        disconnect(m_currentController.data(), &MprisController::canRaiseChanged, parent(), &MprisManager::canRaiseChanged);
        disconnect(m_currentController.data(), &MprisController::canSetFullscreenChanged, parent(), &MprisManager::canSetFullscreenChanged);
        disconnect(m_currentController.data(), &MprisController::desktopEntryChanged, parent(), &MprisManager::desktopEntryChanged);
        disconnect(m_currentController.data(), &MprisController::fullscreenChanged, parent(), &MprisManager::fullscreenChanged);
        disconnect(m_currentController.data(), &MprisController::hasTrackListChanged, parent(), &MprisManager::hasTrackListChanged);
        disconnect(m_currentController.data(), &MprisController::identityChanged, parent(), &MprisManager::identityChanged);
        disconnect(m_currentController.data(), &MprisController::supportedUriSchemesChanged, parent(), &MprisManager::supportedUriSchemesChanged);
        disconnect(m_currentController.data(), &MprisController::supportedMimeTypesChanged, parent(), &MprisManager::supportedMimeTypesChanged);

        // Mpris Player Interface
        disconnect(m_currentController.data(), &MprisController::canControlChanged, parent(), &MprisManager::canControlChanged);
        disconnect(m_currentController.data(), &MprisController::canGoNextChanged, parent(), &MprisManager::canGoNextChanged);
        disconnect(m_currentController.data(), &MprisController::canGoPreviousChanged, parent(), &MprisManager::canGoPreviousChanged);
        disconnect(m_currentController.data(), &MprisController::canPauseChanged, parent(), &MprisManager::canPauseChanged);
        disconnect(m_currentController.data(), &MprisController::canPlayChanged, parent(), &MprisManager::canPlayChanged);
        disconnect(m_currentController.data(), &MprisController::canSeekChanged, parent(), &MprisManager::canSeekChanged);
        disconnect(m_currentController.data(), &MprisController::loopStatusChanged, parent(), &MprisManager::loopStatusChanged);
        disconnect(m_currentController.data(), &MprisController::maximumRateChanged, parent(), &MprisManager::maximumRateChanged);
        disconnect(m_currentController.data(), &MprisController::minimumRateChanged, parent(), &MprisManager::minimumRateChanged);
        disconnect(m_currentController.data(), &MprisController::playbackStatusChanged, parent(), &MprisManager::playbackStatusChanged);
        disconnect(m_currentController.data(), &MprisController::positionChanged, parent(), &MprisManager::positionChanged);
        disconnect(m_currentController.data(), &MprisController::rateChanged, parent(), &MprisManager::rateChanged);
        disconnect(m_currentController.data(), &MprisController::shuffleChanged, parent(), &MprisManager::shuffleChanged);
        disconnect(m_currentController.data(), &MprisController::volumeChanged, parent(), &MprisManager::volumeChanged);
        disconnect(m_currentController.data(), &MprisController::seeked, parent(), &MprisManager::seeked);

        if (m_currentController->playbackStatus() == Mpris::Playing) {
            m_otherPlayingControllers.prepend(m_currentController);
        }
    }

    bool oldCanQuit = parent()->canQuit();
    bool oldCanRaise = parent()->canRaise();
    bool oldCanSetFullscreen = parent()->canSetFullscreen();
    QString oldDesktopEntry = parent()->desktopEntry();
    bool oldFullscreen = parent()->fullscreen();
    bool oldHasTrackList = parent()->hasTrackList();
    QString oldIdentity = parent()->identity();
    QStringList oldSupportedUriSchemes = parent()->supportedUriSchemes();
    QStringList oldSupportedMimeTypes = parent()->supportedMimeTypes();
    bool oldCanControl = parent()->canControl();
    bool oldCanGoNext = parent()->canGoNext();
    bool oldCanGoPrevious = parent()->canGoPrevious();
    bool oldCanPause = parent()->canPause();
    bool oldCanPlay = parent()->canPlay();
    bool oldCanSeek = parent()->canSeek();
    Mpris::LoopStatus oldLoopStatus = parent()->loopStatus();
    double oldMaximumRate = parent()->maximumRate();
    const MprisMetaData *oldMetaData = parent()->metaData();
    double oldMinimumRate = parent()->minimumRate();
    Mpris::PlaybackStatus oldPlaybackStatus = parent()->playbackStatus();
    double oldRate = parent()->rate();
    bool oldShuffle = parent()->shuffle();
    double oldVolume = parent()->volume();

    m_currentController = controller;

    if (oldCanQuit != parent()->canQuit()) {
        Q_EMIT parent()->canQuitChanged();
    }
    if (oldCanRaise != parent()->canRaise()) {
        Q_EMIT parent()->canRaiseChanged();
    }
    if (oldCanSetFullscreen != parent()->canSetFullscreen()) {
        Q_EMIT parent()->canSetFullscreenChanged();
    }
    if (oldDesktopEntry != parent()->desktopEntry()) {
        Q_EMIT parent()->desktopEntryChanged();
    }
    if (oldFullscreen != parent()->fullscreen()) {
        Q_EMIT parent()->fullscreenChanged();
    }
    if (oldHasTrackList != parent()->hasTrackList()) {
        Q_EMIT parent()->hasTrackListChanged();
    }
    if (oldIdentity != parent()->identity()) {
        Q_EMIT parent()->identityChanged();
    }
    if (oldSupportedUriSchemes != parent()->supportedUriSchemes()) {
        Q_EMIT parent()->supportedUriSchemesChanged();
    }
    if (oldSupportedMimeTypes != parent()->supportedMimeTypes()) {
        Q_EMIT parent()->supportedMimeTypesChanged();
    }

    if (oldCanControl != parent()->canControl()) {
        Q_EMIT parent()->canControlChanged();
    }
    if (oldCanGoNext != parent()->canGoNext()) {
        Q_EMIT parent()->canGoNextChanged();
    }
    if (oldCanGoPrevious != parent()->canGoPrevious()) {
        Q_EMIT parent()->canGoPreviousChanged();
    }
    if (oldCanPause != parent()->canPause()) {
        Q_EMIT parent()->canPauseChanged();
    }
    if (oldCanPlay != parent()->canPlay()) {
        Q_EMIT parent()->canPlayChanged();
    }
    if (oldCanSeek != parent()->canSeek()) {
        Q_EMIT parent()->canSeekChanged();
    }
    if (oldLoopStatus != parent()->loopStatus()) {
        Q_EMIT parent()->loopStatusChanged();
    }
    if (oldMaximumRate != parent()->maximumRate()) {
        Q_EMIT parent()->maximumRateChanged();
    }
    if (oldMetaData != parent()->metaData()) {
        Q_EMIT parent()->metaDataChanged();
    }
    if (oldMinimumRate != parent()->minimumRate()) {
        Q_EMIT parent()->minimumRateChanged();
    }
    if (oldPlaybackStatus != parent()->playbackStatus()) {
        Q_EMIT parent()->playbackStatusChanged();
    }
    if (oldRate != parent()->rate()) {
        Q_EMIT parent()->rateChanged();
    }
    if (oldShuffle != parent()->shuffle()) {
        Q_EMIT parent()->shuffleChanged();
    }
    if (oldVolume != parent()->volume()) {
        Q_EMIT parent()->volumeChanged();
    }

    if (!m_currentController.isNull()) {
        // Mpris Root Interface
        connect(m_currentController.data(), &MprisController::canQuitChanged, parent(), &MprisManager::canQuitChanged);
        connect(m_currentController.data(), &MprisController::canRaiseChanged, parent(), &MprisManager::canRaiseChanged);
        connect(m_currentController.data(), &MprisController::canSetFullscreenChanged, parent(), &MprisManager::canSetFullscreenChanged);
        connect(m_currentController.data(), &MprisController::desktopEntryChanged, parent(), &MprisManager::desktopEntryChanged);
        connect(m_currentController.data(), &MprisController::fullscreenChanged, parent(), &MprisManager::fullscreenChanged);
        connect(m_currentController.data(), &MprisController::hasTrackListChanged, parent(), &MprisManager::hasTrackListChanged);
        connect(m_currentController.data(), &MprisController::identityChanged, parent(), &MprisManager::identityChanged);
        connect(m_currentController.data(), &MprisController::supportedUriSchemesChanged, parent(), &MprisManager::supportedUriSchemesChanged);
        connect(m_currentController.data(), &MprisController::supportedMimeTypesChanged, parent(), &MprisManager::supportedMimeTypesChanged);
        connect(m_currentController.data(), &MprisController::canControlChanged, parent(), &MprisManager::canControlChanged);

        // Mpris Player Interface
        connect(m_currentController.data(), &MprisController::canGoNextChanged, parent(), &MprisManager::canGoNextChanged);
        connect(m_currentController.data(), &MprisController::canGoPreviousChanged, parent(), &MprisManager::canGoPreviousChanged);
        connect(m_currentController.data(), &MprisController::canPauseChanged, parent(), &MprisManager::canPauseChanged);
        connect(m_currentController.data(), &MprisController::canPlayChanged, parent(), &MprisManager::canPlayChanged);
        connect(m_currentController.data(), &MprisController::canSeekChanged, parent(), &MprisManager::canSeekChanged);
        connect(m_currentController.data(), &MprisController::loopStatusChanged, parent(), &MprisManager::loopStatusChanged);
        connect(m_currentController.data(), &MprisController::maximumRateChanged, parent(), &MprisManager::maximumRateChanged);
        connect(m_currentController.data(), &MprisController::minimumRateChanged, parent(), &MprisManager::minimumRateChanged);
        connect(m_currentController.data(), &MprisController::playbackStatusChanged, parent(), &MprisManager::playbackStatusChanged);
        if (m_positionConnected) {
            connect(m_currentController.data(), &MprisController::positionChanged, parent(), &MprisManager::positionChanged);
        }
        connect(m_currentController.data(), &MprisController::rateChanged, parent(), &MprisManager::rateChanged);
        connect(m_currentController.data(), &MprisController::shuffleChanged, parent(), &MprisManager::shuffleChanged);
        connect(m_currentController.data(), &MprisController::volumeChanged, parent(), &MprisManager::volumeChanged);
        connect(m_currentController.data(), &MprisController::seeked, parent(), &MprisManager::seeked);

        if (m_currentController->playbackStatus() == Mpris::Playing) {
            m_otherPlayingControllers.removeOne(m_currentController);
        }
    }

    Q_EMIT parent()->currentServiceChanged();
    Q_EMIT parent()->metaDataChanged();
    Q_EMIT parent()->positionChanged(parent()->position());
}

bool MprisManagerPrivate::checkController(const char *callerName) const
{
    if (m_currentController.isNull()) {
        qWarning() << callerName << "None service available/selected";
        return false;
    }

    return true;
}

#include "mprismanager.moc"
