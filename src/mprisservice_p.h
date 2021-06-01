#ifndef MPRISSERVICE_H
#define MPRISSERVICE_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QString>
#include <QVariant>
#include <QTimer>
#include <QPair>

#include "mprisserviceadaptor_p.h"

class MprisService;

class MprisServicePrivate : public QObject, public QDBusContext
{
    Q_OBJECT

public:
    MprisServicePrivate(/* MprisService *service */);
    ~MprisServicePrivate();

    MprisService *parent() const;

    MprisServiceAdaptor m_dbusAdaptor;

    QDBusConnection m_connection;

    QString m_serviceName;
    bool m_canQuit;
    bool m_canRaise;
    bool m_canSetFullscreen;
    QString m_desktopEntry;
    bool m_fullscreen;
    bool m_hasTrackList;
    QString m_identity;
    QStringList m_supportedUriSchemes;
    QStringList m_supportedMimeTypes;
    QMap<QString, QPair<QVariantMap, QSet<QString>>> m_changedProperties;
    QTimer m_changedDelay;

    void quit();
    void raise();

public Q_SLOTS:
    void propertyChanged(const QString &iface, const QString &name, const QVariant &value);

private Q_SLOTS:
    void emitPropertiesChanged();
};

#endif
