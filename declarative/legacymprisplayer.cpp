#include "legacymprisplayer_p.h"
#include <MprisMetaData>
#include <QHash>
#include <QSet>

#include <QDebug>

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
    QVariantMap returnValue;
    QList<QPair<QString, QString>> properties;

    for (auto i = PropertyMapping.cbegin();
         i != PropertyMapping.cend();
         ++i) {
        properties << qMakePair(i.value(), i.key());
    }

    for (auto i : properties) {
        QVariant value = metaData()->property(i.first.toLatin1());

        if (value.isValid() && !value.isNull()) {
            returnValue[i.second] = value;
        }
    }

    return returnValue;
}

void LegacyMprisPlayer::setMetadata(const QVariantMap &metadata)
{
    QList<QString> props = PropertyMapping.values();
    QSet<QString> unused;

    for (auto i : props)
        unused << i;

    for (auto i = metadata.cbegin();
         i != metadata.cend();
         ++i) {
        if (PropertyMapping.contains(i.key())) {
            metaData()->setProperty(PropertyMapping[i.key()].toLatin1(), i.value());
            unused.remove(PropertyMapping[i.key()]);
        }
    }

    for (const QString &key : unused) {
        metaData()->setProperty(key.toLatin1(), QVariant());
    }
}
