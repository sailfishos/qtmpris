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


#include "mpriscontroller_p.h"

#include <QtCore/QtDebug>


/*
 * Implementation of interface class MprisRootInterface
 */

MprisRootInterface::MprisRootInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : DBusExtendedAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
    connect(this, SIGNAL(propertyChanged(QString, QVariant)), this, SLOT(onPropertyChanged(QString, QVariant)));
}

MprisRootInterface::~MprisRootInterface()
{
}

void MprisRootInterface::onPropertyChanged(const QString &propertyName, const QVariant &value)
{
    if (propertyName == QStringLiteral("CanQuit")) {
        emit canQuitChanged(value.toBool());
    } else if (propertyName == QStringLiteral("CanRaise")) {
        emit canRaiseChanged(value.toBool());
    } else if (propertyName == QStringLiteral("CanSetFullscreen")) {
        emit canSetFullscreenChanged(value.toBool());
    } else if (propertyName == QStringLiteral("DesktopEntry")) {
        emit desktopEntryChanged(value.toString());
    } else if (propertyName == QStringLiteral("Fullscreen")) {
        emit fullscreenChanged(value.toBool());
    } else if (propertyName == QStringLiteral("HasTrackList")) {
        emit hasTrackListChanged(value.toBool());
    } else if (propertyName == QStringLiteral("Identity")) {
        emit identityChanged(value.toString());
    } else if (propertyName == QStringLiteral("SupportedMimeTypes")) {
        emit supportedMimeTypesChanged(value.toStringList());
    } else if (propertyName == QStringLiteral("SupportedUriSchemes")) {
        emit supportedUriSchemesChanged(value.toStringList());
    } else {
        qWarning() << Q_FUNC_INFO << "Received PropertyChanged signal from unknown property.";
    }
}

