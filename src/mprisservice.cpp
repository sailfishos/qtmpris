#include <QObject>
#include "mprisservice.h"
#include "mprisservice_p.h"
#include "mprisserviceadaptor_p.h"

namespace {
    const QString ServiceInterface = MprisServiceAdaptor::staticMetaObject.classInfo(MprisServiceAdaptor::staticMetaObject.indexOfClassInfo("D-Bus Interface")).value();
}

MprisServicePrivate::MprisServicePrivate(/* MprisService *service */)
    : QObject(/* service */)
    , m_dbusAdaptor(this)
    , m_connection(getDBusConnection())
    , m_canQuit(false)
    , m_canRaise(false)
    , m_canSetFullscreen(false)
    , m_fullscreen(false)
    , m_hasTrackList(false)
    , m_changedDelay(this)
{
    qDBusRegisterMetaType<QStringList>();
    connect(&m_changedDelay, &QTimer::timeout, this, &MprisServicePrivate::emitPropertiesChanged);
}

MprisServicePrivate::~MprisServicePrivate()
{
}

MprisService *MprisServicePrivate::parent() const
{
    return static_cast<MprisService *>(QObject::parent());
}

MprisService::MprisService(QObject *parent)
    : QObject(parent)
    , priv(new MprisServicePrivate)
{
    priv->setParent(this);
}

MprisService::MprisService(MprisServicePrivate *priv_, QObject *parent)
    : QObject(parent)
    , priv(priv_)
{
    priv->setParent(this);
}

MprisService::~MprisService()
{
    delete priv;
}

void MprisServicePrivate::quit()
{
    Q_EMIT parent()->quitRequested();
}

void MprisServicePrivate::raise()
{
    Q_EMIT parent()->raiseRequested();
}

QString MprisService::serviceName() const
{
    return priv->m_serviceName;
}

bool MprisService::canQuit() const
{
    return priv->m_canQuit;
}

bool MprisService::canRaise() const
{
    return priv->m_canRaise;
}

bool MprisService::canSetFullscreen() const
{
    return priv->m_canSetFullscreen;
}

QString MprisService::desktopEntry() const
{
    return priv->m_desktopEntry;
}

bool MprisService::fullscreen() const
{
    return priv->m_fullscreen;
}

bool MprisService::hasTrackList() const
{
    return priv->m_hasTrackList;
}

QString MprisService::identity() const
{
    return priv->m_identity;
}

QStringList MprisService::supportedUriSchemes() const
{
    return priv->m_supportedUriSchemes;
}

QStringList MprisService::supportedMimeTypes() const
{
    return priv->m_supportedMimeTypes;
}

void MprisService::setServiceName(const QString &serviceName)
{
    if (!priv->m_serviceName.isEmpty()) {
        priv->m_connection.unregisterObject(QStringLiteral("/org/mpris/MediaPlayer2"));
        priv->m_connection.unregisterService(priv->m_serviceName);
    }

    if (!serviceName.isEmpty()) {
        if (!serviceName.startsWith(QLatin1String("org.mpris.MediaPlayer2."))) {
            priv->m_serviceName = QStringLiteral("%1.%2.instance%3").arg(QLatin1String("org.mpris.MediaPlayer2")).arg(serviceName).arg(QCoreApplication::applicationPid());
        } else {
            priv->m_serviceName = QStringLiteral("%1.instance%2").arg(serviceName).arg(QCoreApplication::applicationPid());
        }

        priv->m_connection.registerService(priv->m_serviceName);
        priv->m_connection.registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), priv);
    } else {
        priv->m_serviceName = serviceName;
    }

    Q_EMIT serviceNameChanged();
}

void MprisService::setCanQuit(bool canQuit)
{
    if (priv->m_canQuit != canQuit) {
        priv->m_canQuit = canQuit;
        priv->propertyChanged(ServiceInterface, QStringLiteral("CanQuit"), canQuit);
        Q_EMIT canQuitChanged();
    }
}

void MprisService::setCanRaise(bool canRaise)
{
    if (priv->m_canRaise != canRaise) {
        priv->m_canRaise = canRaise;
        priv->propertyChanged(ServiceInterface, QStringLiteral("CanRaise"), canRaise);
        Q_EMIT canRaiseChanged();
    }
}

void MprisService::setCanSetFullscreen(bool canSetFullscreen)
{
    if (priv->m_canSetFullscreen != canSetFullscreen) {
        priv->m_canSetFullscreen = canSetFullscreen;
        priv->propertyChanged(ServiceInterface, QStringLiteral("CanSetFullscreen"), canSetFullscreen);
        Q_EMIT canSetFullscreenChanged();
    }
}

void MprisService::setDesktopEntry(const QString &desktopEntry)
{
    if (priv->m_desktopEntry != desktopEntry) {
        priv->m_desktopEntry = desktopEntry;
        priv->propertyChanged(ServiceInterface, QStringLiteral("DesktopEntry"), desktopEntry);
        Q_EMIT desktopEntryChanged();
    }
}

void MprisService::setFullscreen(bool fullscreen)
{
    if (priv->m_fullscreen != fullscreen) {
        priv->m_fullscreen = fullscreen;
        priv->propertyChanged(ServiceInterface, QStringLiteral("Fullscreen"), fullscreen);
        Q_EMIT fullscreenChanged();
    }
}

void MprisService::setHasTrackList(bool hasTrackList)
{
    if (priv->m_hasTrackList != hasTrackList) {
        priv->m_hasTrackList = hasTrackList;
        priv->propertyChanged(ServiceInterface, QStringLiteral("HasTrackList"), hasTrackList);
        Q_EMIT hasTrackListChanged();
    }
}

void MprisService::setIdentity(const QString &identity)
{
    if (priv->m_identity != identity) {
        priv->m_identity = identity;
        priv->propertyChanged(ServiceInterface, QStringLiteral("Identity"), identity);
        Q_EMIT identityChanged();
    }
}

void MprisService::setSupportedUriSchemes(const QStringList &supportedUriSchemes)
{
    if (priv->m_supportedUriSchemes != supportedUriSchemes) {
        priv->m_supportedUriSchemes = supportedUriSchemes;
        priv->propertyChanged(ServiceInterface, QStringLiteral("SupportedUriSchemes"), supportedUriSchemes);
        Q_EMIT supportedUriSchemesChanged();
    }
}

void MprisService::setSupportedMimeTypes(const QStringList &supportedMimeTypes)
{
    if (priv->m_supportedMimeTypes != supportedMimeTypes) {
        priv->m_supportedMimeTypes = supportedMimeTypes;
        priv->propertyChanged(ServiceInterface, QStringLiteral("SupportedMimeTypes"), supportedMimeTypes);
        Q_EMIT supportedMimeTypesChanged();
    }
}

void MprisServicePrivate::propertyChanged(const QString &iface, const QString &name, const QVariant &value)
{
    if (!value.isValid()) {
        m_changedProperties[iface].first.remove(name);
        m_changedProperties[iface].second << name;
    } else {
        m_changedProperties[iface].first[name] = value;
        m_changedProperties[iface].second.remove(name);
    }

    m_changedDelay.start(100);
}

void MprisServicePrivate::emitPropertiesChanged()
{
    for (auto i = m_changedProperties.cbegin();
         i != m_changedProperties.cend();
         ++i) {
        QDBusMessage msg = QDBusMessage::createSignal(QStringLiteral("/org/mpris/MediaPlayer2"),
                                                      QStringLiteral("org.freedesktop.DBus.Properties"),
                                                      QStringLiteral("PropertiesChanged"));

        msg << i.key();
        msg << QVariant::fromValue(i.value().first);
        msg << QStringList(i.value().second.values());

        m_connection.send(msg);
    }

    m_changedProperties.clear();
}
