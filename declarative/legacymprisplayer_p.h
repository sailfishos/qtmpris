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

#ifndef LEGACYMPRISPLAYER_P_H
#define LEGACYMPRISPLAYER_P_H

#include <MprisPlayer>
#include <QObject>
#include <QVariant>

class LegacyMprisPlayer : public MprisPlayer
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap metadata READ metadata WRITE setMetadata NOTIFY metadataChanged)

public:
    LegacyMprisPlayer(QObject *parent = nullptr);
    ~LegacyMprisPlayer();

    QVariantMap metadata() const;
    void setMetadata(const QVariantMap &metadata);

    void setPosition(qlonglong position);
    qlonglong position() const;

Q_SIGNALS:
    void metadataChanged();
};

#endif
