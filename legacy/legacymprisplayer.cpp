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

#include "legacymprisplayer_p.h"
#include "legacympriscommon_p.h"
#include <MprisMetaData>

#include <QDebug>

LegacyMprisPlayer::LegacyMprisPlayer(QObject *parent)
    : MprisPlayer(parent)
{
    connect(this, &MprisPlayer::metaDataChanged, this, &LegacyMprisPlayer::metadataChanged);
}

LegacyMprisPlayer::~LegacyMprisPlayer()
{
}

QVariantMap LegacyMprisPlayer::metadata() const
{
    QVariantMap rv;
    return rv << metaData();
}

void LegacyMprisPlayer::setMetadata(const QVariantMap &metadata)
{
    metaData() << metadata;
}

void LegacyMprisPlayer::setPosition(qlonglong position)
{
    MprisPlayer::setPosition(position / 1000);
}

qlonglong LegacyMprisPlayer::position() const
{
    return MprisPlayer::position() * 1000;
}
