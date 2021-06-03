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

#include "legacympriscommon_p.h"
#include <QSet>
#include <QVariantMap>

namespace {
#define PAIR(x, y) { QLatin1String(#x), QLatin1String(#y) }
    const QHash<QString, QString> PropertyMapping {
        PAIR(TrackId, trackId),
        PAIR(Length, duration),
        PAIR(ArtUrl, coverArtUrlLarge),
        PAIR(Album, albumTitle),
        PAIR(AlbumArtist, albumArtist),
        PAIR(Artist, contributingArtist),
        PAIR(AsText, lyrics),
        PAIR(AudioBPM, audioBpm),
        PAIR(AutoRating, autoRating),
        PAIR(Comment, comment),
        PAIR(Composer, composer),
        PAIR(ContentCreated, date),
        PAIR(DiscNumber, chapterNumber),
        PAIR(FirstUsed, firstUsed),
        PAIR(Genre, genre),
        PAIR(LastUsed, lastUsed),
        PAIR(Lyricist, writer),
        PAIR(Title, title),
        PAIR(TrackNumber, trackNumber),
        PAIR(Url, url),
        PAIR(UseCount, useCount),
        PAIR(UserRating, userRating)
    };
#undef PAIR
}

QVariantMap &operator<<(QVariantMap &a, const MprisMetaData *b)
{
    QList<QPair<QString, QString>> properties;

    for (auto i = PropertyMapping.cbegin();
         i != PropertyMapping.cend();
         ++i) {
        properties << qMakePair(i.value(), i.key());
    }

    for (auto i : properties) {
        QVariant value = b->property(i.first.toLatin1());

        if (value.isValid() && !value.isNull()) {
            a[i.second] = value;
        }
    }

    return a;
}

MprisMetaData *operator<<(MprisMetaData *a, const QVariantMap &b)
{
    QList<QString> props = PropertyMapping.values();
    QSet<QString> unused;

    for (auto i : props)
        unused << i;

    for (auto i = b.cbegin();
         i != b.cend();
         ++i) {
        if (PropertyMapping.contains(i.key())) {
            a->setProperty(PropertyMapping[i.key()].toLatin1(), i.value());
            unused.remove(PropertyMapping[i.key()]);
        }
    }

    for (const QString &key : unused) {
        a->setProperty(key.toLatin1(), QVariant());
    }

    return a;
}
