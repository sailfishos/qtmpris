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

#include <Mpris>

class LegacyMpris : public QObject {
    Q_OBJECT

public:
    explicit LegacyMpris(QObject *parent = nullptr);

    // These need to be repeated, as we cannot inherit from Mpris in a way
    // that Qt would understand.
    enum PlaybackStatus {
        Stopped,
        Playing,
        Paused
    };
    Q_ENUM(PlaybackStatus);

    enum LoopStatus {
        None,
        Track,
        Playlist
    };
    Q_ENUM(LoopStatus);

    enum Metadata {
        InvalidMetadata = -1,
        TrackId,
        Length,
        ArtUrl,
        Album,
        AlbumArtist,
        Artist,
        AsText,
        AudioBPM,
        AutoRating,
        Comment,
        Composer,
        ContentCreated,
        DiscNumber,
        FirstUsed,
        Genre,
        LastUsed,
        Lyricist,
        Title,
        TrackNumber,
        Url,
        UseCount,
        UserRating
    };
    Q_ENUM(Metadata);

    Q_INVOKABLE static QString metadataToString(Metadata metadata);
};
