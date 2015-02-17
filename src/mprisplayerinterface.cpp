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
 * Implementation of interface class MprisPlayerInterface
 */

MprisPlayerInterface::MprisPlayerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : DBusExtendedAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
    connect(this, SIGNAL(propertyChanged(QString, QVariant)), this, SLOT(onPropertyChanged(QString, QVariant)));
}

MprisPlayerInterface::~MprisPlayerInterface()
{
}

void MprisPlayerInterface::onPropertyChanged(const QString &propertyName, const QVariant &value)
{
    if (propertyName == QStringLiteral("CanGoNext")) {
        emit canGoNextChanged(value.toBool());
    } else if (propertyName == QStringLiteral("CanGoPrevious")) {
        emit canGoPreviousChanged(value.toBool());
    } else if (propertyName == QStringLiteral("CanPause")) {
        emit canPauseChanged(value.toBool());
    } else if (propertyName == QStringLiteral("CanPlay")) {
        emit canPlayChanged(value.toBool());
    } else if (propertyName == QStringLiteral("CanSeek")) {
        emit canSeekChanged(value.toBool());
    } else if (propertyName == QStringLiteral("LoopStatus")) {
        emit loopStatusChanged(value.toString());
    } else if (propertyName == QStringLiteral("MaximumRate")) {
        emit maximumRateChanged(value.toDouble());
    } else if (propertyName == QStringLiteral("Metadata")) {
        emit metadataChanged(value.toMap());
    } else if (propertyName == QStringLiteral("MinimumRate")) {
        emit minimumRateChanged(value.toDouble());
    } else if (propertyName == QStringLiteral("PlaybackStatus")) {
        emit playbackStatusChanged(value.toString());
    } else if (propertyName == QStringLiteral("Rate")) {
        emit rateChanged(value.toDouble());
    } else if (propertyName == QStringLiteral("Shuffle")) {
        emit shuffleChanged(value.toBool());
    } else if (propertyName == QStringLiteral("Volume")) {
        emit volumeChanged(value.toDouble());
    } else {
        qWarning() << Q_FUNC_INFO << "Received PropertyChanged signal from unknown property.";
    }
}

