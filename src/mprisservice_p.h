/*!
 *
 * Copyright (C) 2021 Jolla Ltd.
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
