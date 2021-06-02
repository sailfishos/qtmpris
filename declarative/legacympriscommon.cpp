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
