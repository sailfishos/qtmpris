#include <QVariant>
#include <QTimer>
#include <QMetaObject>
#include <QMetaProperty>
#include <QDateTime>
#include <QUrl>
#include <QDir>
#include <QDBusObjectPath>

#include <QDebug>

#include "mprismetadata.h"
#include "mprismetadata_p.h"

namespace {
    const QString MetaFieldTrackId = QStringLiteral("mpris:trackid");
    const QString MetaFieldLength = QStringLiteral("mpris:length");
    const QString MetaFieldArtUrl = QStringLiteral("mpris:artUrl");
    const QString MetaFieldAlbum = QStringLiteral("xesam:album");
    const QString MetaFieldAlbumArtist = QStringLiteral("xesam:albumArtist");
    const QString MetaFieldArtist = QStringLiteral("xesam:artist");
    const QString MetaFieldAsText = QStringLiteral("xesam:asText");
    const QString MetaFieldAudioBPM = QStringLiteral("xesam:audioBPM");
    const QString MetaFieldAutoRating = QStringLiteral("xesam:autoRating");
    const QString MetaFieldComment = QStringLiteral("xesam:comment");
    const QString MetaFieldComposer = QStringLiteral("xesam:composer");
    const QString MetaFieldContentCreated = QStringLiteral("xesam:contentCreated");
    const QString MetaFieldDiscNumber = QStringLiteral("xesam:discNumber");
    const QString MetaFieldFirstUsed = QStringLiteral("xesam:firstUsed");
    const QString MetaFieldGenre = QStringLiteral("xesam:genre");
    const QString MetaFieldLastUsed = QStringLiteral("xesam:lastUsed");
    const QString MetaFieldLyricist = QStringLiteral("xesam:lyricist");
    const QString MetaFieldTitle = QStringLiteral("xesam:title");
    const QString MetaFieldTrackNumber = QStringLiteral("xesam:trackNumber");
    const QString MetaFieldUrl = QStringLiteral("xesam:url");
    const QString MetaFieldUseCount = QStringLiteral("xesam:useCount");
    const QString MetaFieldUserRating = QStringLiteral("xesam:userRating");

    const QString MetaFieldInternalYear = QStringLiteral("year");
    const QString MetaFieldInternalArtUrlSmall = QStringLiteral("artUrlSmall");

    const QString TrackObjectPathPrefix = QStringLiteral("/org/mpris/MediaPlayer2/TrackList/");

    template<class T> QVariant ensureType(const QVariant &from)
    {
        if (from.isNull())
            return QVariant();
        return QVariant::fromValue(qvariant_cast<T>(from));
    }

    template<> QVariant ensureType<QDateTime>(const QVariant &from)
    {
        if (from.isNull())
            return QVariant();
        QDateTime d = from.toDateTime();
        return d.toString(Qt::ISODate);
    }
    template<> QVariant ensureType<QDBusObjectPath>(const QVariant &from)
    {
        QString path;

        if (from.isNull()) {
            path = TrackObjectPathPrefix + QLatin1String("NoTrack");
        } else {
            path = from.toString();
            if (!path.startsWith('/')) {
                path = TrackObjectPathPrefix + path;
            }
        }

        return QVariant::fromValue(QDBusObjectPath(path));
    }

    QVariant convertLength(const QVariant &from) {
        if (from.isNull())
            return QVariant();
        return QVariant::fromValue(qvariant_cast<qint64>(from) * 1000);
    }

    const QMap<QString, QVariant (*)(const QVariant &)> converters {
        { MetaFieldTrackId, ensureType<QDBusObjectPath> },
        { MetaFieldLength, convertLength },
        { MetaFieldArtUrl, ensureType<QString> },
        { MetaFieldAlbum, ensureType<QString> },
        { MetaFieldAlbumArtist, ensureType<QStringList> },
        { MetaFieldArtist, ensureType<QStringList> },
        { MetaFieldAsText, ensureType<QString> },
        { MetaFieldAudioBPM, ensureType<qint32> },
        { MetaFieldAutoRating, ensureType<double> },
        { MetaFieldComment, ensureType<QString> },
        { MetaFieldComposer, ensureType<QStringList> },
        { MetaFieldContentCreated, ensureType<QDateTime> },
        { MetaFieldDiscNumber, ensureType<qint32> },
        { MetaFieldFirstUsed, ensureType<QDateTime> },
        { MetaFieldGenre, ensureType<QStringList> },
        { MetaFieldLastUsed, ensureType<QDateTime> },
        { MetaFieldLyricist, ensureType<QStringList> },
        { MetaFieldTitle, ensureType<QString> },
        { MetaFieldTrackNumber, ensureType<qint32> },
        { MetaFieldUrl, ensureType<QString> },
        { MetaFieldUseCount, ensureType<qint32> },
        { MetaFieldUserRating, ensureType<double> },
    };
}

MprisMetaDataPrivate::MprisMetaDataPrivate(MprisMetaData *metaData)
    : QObject(metaData)
{
    m_changedDelay.setInterval(50);
    m_changedDelay.setSingleShot(true);
    m_fillFromDelay.setInterval(10);
    m_fillFromDelay.setSingleShot(true);
    connect(&m_changedDelay, &QTimer::timeout, this, [=] { Q_EMIT metaData->metaDataChanged(); });
    connect(&m_fillFromDelay, &QTimer::timeout, this, &MprisMetaDataPrivate::fillFrom);
}

MprisMetaDataPrivate::~MprisMetaDataPrivate() {}

MprisMetaData *MprisMetaDataPrivate::parent() const
{
    return static_cast<MprisMetaData *>(QObject::parent());
}

void MprisMetaDataPrivate::fillFromPropertyChange()
{
    for (auto p : m_signalPropertyMap[senderSignalIndex()]) {
        m_changedProperties << p;
    }

    m_fillFromDelay.start();
}

void MprisMetaDataPrivate::fillFrom()
{
    for (auto p : m_changedProperties) {
        parent()->setProperty(p, m_fillFromObject->property(p));
    }

    m_changedProperties.clear();
}

QVariantMap MprisMetaDataPrivate::typedMetaData() const
{
    QVariantMap rv;

    for (auto c = m_metaData.cbegin();
         c != m_metaData.cend();
         ++c) {
        if (converters.contains(c.key())) {
            QVariant v = converters[c.key()](c.value());
            if (!v.isNull()) {
                rv[c.key()] = v;
            }
        } else if (c.key() == MetaFieldInternalYear) {
            if (!m_metaData.contains(MetaFieldContentCreated) || m_metaData[MetaFieldContentCreated].isNull()) {
                QDateTime d = QDateTime::fromString(QStringLiteral("%1-01-02T00:00:00Z").arg(c.value().toString()), Qt::ISODate);
                rv[MetaFieldContentCreated] = d.toString(Qt::ISODate);
            }
        } else if (c.key() == MetaFieldInternalArtUrlSmall) {
            if (!m_metaData.contains(MetaFieldArtUrl)) {
                rv[MetaFieldArtUrl] = c.value();
            }
        } else if (c.key().count(':') == 1 && !c.key().startsWith(QLatin1String("mpris:")) && !c.key().startsWith(QLatin1String("xesam:"))) {
            rv[c.key()] = c.value();
        }
    }

    return rv;
}

void MprisMetaDataPrivate::setMetaData(const QString &key, const QVariant &value)
{
    if (m_metaData.value(key) != value) {
        if (!value.isValid() || value.isNull()) {
            m_metaData.remove(key);
        } else {
            m_metaData[key] = value;
        }
        m_changedDelay.start();
    }
}

void MprisMetaDataPrivate::setMetaData(const QVariantMap &metaData)
{
    if (metaData != m_metaData) {
        m_metaData = metaData;
        Q_EMIT parent()->metaDataChanged();
    }
}

MprisMetaData::MprisMetaData(QObject *parent)
    : QObject(parent)
    , priv(new MprisMetaDataPrivate(this))
{
}

QVariant MprisMetaData::trackId() const
{
    if (priv->m_metaData.contains(MetaFieldTrackId)) {
        QString trackId = priv->m_metaData[MetaFieldTrackId].toString();
        if (trackId.startsWith(TrackObjectPathPrefix)) {
            return trackId.mid(TrackObjectPathPrefix.size());
        }
        return priv->m_metaData[MetaFieldTrackId];
    }

    return QVariant();
}

void MprisMetaData::setTrackId(const QVariant &trackId)
{
    priv->setMetaData(MetaFieldTrackId, trackId);
}

QVariant MprisMetaData::duration() const
{
    if (priv->m_metaData.contains(MetaFieldLength)) {
        return qvariant_cast<qint64>(priv->m_metaData[MetaFieldLength]) / 1000;
    }

    return QVariant();
}

void MprisMetaData::setDuration(const QVariant &duration)
{
    if (duration.toLongLong() <= 0)
        priv->setMetaData(MetaFieldLength, QVariant());
    else
        priv->setMetaData(MetaFieldLength, duration);
}

QVariant MprisMetaData::coverArtUrlSmall() const
{
    return priv->m_metaData.value(MetaFieldArtUrl);
}

void MprisMetaData::setCoverArtUrlSmall(const QVariant &url)
{
    priv->setMetaData(MetaFieldArtUrl, url);
}

QVariant MprisMetaData::coverArtUrlLarge() const
{
    return priv->m_metaData.value(MetaFieldArtUrl);
}

void MprisMetaData::setCoverArtUrlLarge(const QVariant &url)
{
    priv->setMetaData(MetaFieldArtUrl, url);
}

QVariant MprisMetaData::contributingArtist() const
{
    return priv->m_metaData.value(MetaFieldArtist);
}

void MprisMetaData::setContributingArtist(const QVariant &artist)
{
    priv->setMetaData(MetaFieldArtist, artist);
}

QVariant MprisMetaData::albumTitle() const
{
    return priv->m_metaData.value(MetaFieldAlbum);
}

void MprisMetaData::setAlbumTitle(const QVariant &title)
{
    priv->setMetaData(MetaFieldAlbum, title);
}

QVariant MprisMetaData::albumArtist() const
{
    return priv->m_metaData.value(MetaFieldAlbumArtist);
}

void MprisMetaData::setAlbumArtist(const QVariant &artist)
{
    priv->setMetaData(MetaFieldAlbumArtist, artist);
}

QVariant MprisMetaData::lyrics() const
{
    return priv->m_metaData.value(MetaFieldAsText);
}

void MprisMetaData::setLyrics(const QVariant &lyrics)
{
    priv->setMetaData(MetaFieldAsText, lyrics);
}

QVariant MprisMetaData::comment() const
{
    return priv->m_metaData.value(MetaFieldComment);
}

void MprisMetaData::setComment(const QVariant &comment)
{
    priv->setMetaData(MetaFieldComment, comment);
}

QVariant MprisMetaData::composer() const
{
    return priv->m_metaData.value(MetaFieldComposer);
}

void MprisMetaData::setComposer(const QVariant &composer)
{
    priv->setMetaData(MetaFieldComposer, composer);
}

QVariant MprisMetaData::year() const
{
    if (priv->m_metaData.contains(MetaFieldInternalYear)) {
        return priv->m_metaData[MetaFieldInternalYear];
    }
    if (priv->m_metaData.contains(MetaFieldContentCreated) &&
        priv->m_metaData[MetaFieldContentCreated].isValid()) {
        QDateTime d = QDateTime::fromString(priv->m_metaData[MetaFieldContentCreated].toString(), Qt::ISODate);
        return d.date().year();
    }

    return QVariant();
}

void MprisMetaData::setYear(const QVariant &year)
{
    priv->setMetaData(MetaFieldInternalYear, year);
}

QVariant MprisMetaData::date() const
{
    if (priv->m_metaData.contains(MetaFieldContentCreated)) {
        return QDateTime::fromString(priv->m_metaData[MetaFieldContentCreated].toString(), Qt::ISODate);
    }
    return QVariant();
}

void MprisMetaData::setDate(const QVariant &date)
{
    priv->setMetaData(MetaFieldContentCreated, date);
}

QVariant MprisMetaData::chapterNumber() const
{
    return priv->m_metaData.value(MetaFieldDiscNumber);
}

void MprisMetaData::setChapterNumber(const QVariant &chapter)
{
    priv->setMetaData(MetaFieldDiscNumber, chapter);
}

QVariant MprisMetaData::genre() const
{
    return priv->m_metaData.value(MetaFieldGenre);
}

void MprisMetaData::setGenre(const QVariant &genre)
{
    priv->setMetaData(MetaFieldGenre, genre);
}

QVariant MprisMetaData::writer() const
{
    return priv->m_metaData.value(MetaFieldLyricist);
}

void MprisMetaData::setWriter(const QVariant &writer)
{
    priv->setMetaData(MetaFieldLyricist, writer);
}

QVariant MprisMetaData::title() const
{
    return priv->m_metaData.value(MetaFieldTitle);
}

void MprisMetaData::setTitle(const QVariant &title)
{
    priv->setMetaData(MetaFieldTitle, title);
}

QVariant MprisMetaData::trackNumber() const
{
    return priv->m_metaData.value(MetaFieldTrackNumber);
}

void MprisMetaData::setTrackNumber(const QVariant &track)
{
    priv->setMetaData(MetaFieldTrackNumber, track);
}

QVariant MprisMetaData::userRating() const
{
    return priv->m_metaData.value(MetaFieldUserRating);
}

void MprisMetaData::setUserRating(const QVariant &rating)
{
    priv->setMetaData(MetaFieldUserRating, rating);
}

QVariant MprisMetaData::audioBpm() const
{
    return priv->m_metaData.value(MetaFieldAudioBPM);
}

void MprisMetaData::setAudioBpm(const QVariant &bpm)
{
    priv->setMetaData(MetaFieldAudioBPM, bpm);
}

QVariant MprisMetaData::autoRating() const
{
    return priv->m_metaData.value(MetaFieldAutoRating);
}

void MprisMetaData::setAutoRating(const QVariant &rating)
{
    priv->setMetaData(MetaFieldAutoRating, rating);
}

QVariant MprisMetaData::firstUsed() const
{
    return priv->m_metaData.value(MetaFieldFirstUsed);
}

void MprisMetaData::setFirstUsed(const QVariant &used)
{
    priv->setMetaData(MetaFieldFirstUsed, used);
}

QVariant MprisMetaData::lastUsed() const
{
    return priv->m_metaData.value(MetaFieldLastUsed);
}

void MprisMetaData::setLastUsed(const QVariant &used)
{
    priv->setMetaData(MetaFieldLastUsed, used);
}

QVariant MprisMetaData::url() const
{
    return priv->m_metaData.value(MetaFieldUrl);
}

void MprisMetaData::setUrl(const QVariant &url)
{
    priv->setMetaData(MetaFieldUrl, url);
}

QVariant MprisMetaData::useCount() const
{
    return priv->m_metaData.value(MetaFieldUseCount);
}

void MprisMetaData::setUseCount(const QVariant &count)
{
    priv->setMetaData(MetaFieldUseCount, count);
}


/*
QVariant MprisMetaData::subTitle() const
{
}

void MprisMetaData::setSubTitle(const QVariant &title)
{
}

QVariant MprisMetaData::author() const
{
    return priv->m_metaData.value(MetaFieldArtist);
}

void MprisMetaData::setAuthor(const QVariant &author)
{
}

QVariant MprisMetaData::description() const
{
}
void MprisMetaData::setDescription(const QVariant &description)
{
}
QVariant MprisMetaData::category() const
{
}
void MprisMetaData::setCategory(const QVariant &category)
{
}
QVariant MprisMetaData::keywords() const
{
}
void MprisMetaData::setKeywords(const QVariant &keywords)
{
}
QVariant MprisMetaData::language() const
{
}
void MprisMetaData::setLanguage(const QVariant &language)
{
}
QVariant MprisMetaData::publisher() const
{
}
void MprisMetaData::setPublisher(const QVariant &publisher)
{
}
QVariant MprisMetaData::copyright() const
{
}
void MprisMetaData::setCopyright(const QVariant &copyright)
{
}
QVariant MprisMetaData::parentalRating() const
{
}
void MprisMetaData::setParentalRating(const QVariant &rating)
{
}
QVariant MprisMetaData::ratingOrganization() const
{
}
void MprisMetaData::setRatingOrganization(const QVariant &organization)
{
}
QVariant MprisMetaData::size() const
{
}
void MprisMetaData::setSize(const QVariant &size)
{
}
QVariant MprisMetaData::mediaType() const
{
}
void MprisMetaData::setMediaType(const QVariant &type)
{
}
QVariant MprisMetaData::audioBitRate() const
{
}
void MprisMetaData::setAudioBitRate(const QVariant &rate)
{
}
QVariant MprisMetaData::audioCodec() const
{
}
void MprisMetaData::setAudioCodec(const QVariant &codec)
{
}
QVariant MprisMetaData::averageLevel() const
{
}
void MprisMetaData::setAverageLevel(const QVariant &level)
{
}
QVariant MprisMetaData::channelCount() const
{
}
void MprisMetaData::setChannelCount(const QVariant &count)
{
}
QVariant MprisMetaData::peakValue() const
{
}
void MprisMetaData::setPeakValue(const QVariant &value)
{
}
QVariant MprisMetaData::sampleRate() const
{
}
void MprisMetaData::setSampleRate(const QVariant &rate)
{
}

QVariant MprisMetaData::conductor() const
{
}
void MprisMetaData::setConductor(const QVariant &conductor)
{
}
QVariant MprisMetaData::mood() const
{
}
void MprisMetaData::setMood(const QVariant &mood)
{
}

QVariant MprisMetaData::trackCount() const
{
}
void MprisMetaData::setTrackCount(const QVariant &count)
{
}
QVariant MprisMetaData::resolution() const
{
}
void MprisMetaData::setResolution(const QVariant &resolution)
{
}
QVariant MprisMetaData::pixelAspectRatio() const
{
}
void MprisMetaData::setPixelAspectRatio(const QVariant &ratio)
{
}
QVariant MprisMetaData::videoFrameRate() const
{
}
void MprisMetaData::setVideoFrameRate(const QVariant &rate)
{
}
QVariant MprisMetaData::videoBitRate() const
{
}
void MprisMetaData::setVideoBitRate(const QVariant &rate)
{
}
QVariant MprisMetaData::videoCodec() const
{
}
void MprisMetaData::setVideoCodec(const QVariant &codec)
{
}
QVariant MprisMetaData::director() const
{
}
void MprisMetaData::setDirector(const QVariant &director)
{
}
QVariant MprisMetaData::leadPerformer() const
{
}
void MprisMetaData::setLeadPerformer(const QVariant &performer)
{
}
*/

void MprisMetaData::setFillFrom(const QVariant &fillFrom)
{
    if (priv->m_fillFromObject) {
        priv->m_fillFromObject->disconnect(priv);
        priv->m_fillFromObject.clear();
        priv->m_signalPropertyMap.clear();
    }

    priv->m_fillFromObject = qvariant_cast<QObject *>(fillFrom);

    if (priv->m_fillFromObject) {
        const QMetaObject *thisMeta = this->metaObject();
        const QMetaObject *privMeta = priv->metaObject();
        const QMetaObject *thatMeta = priv->m_fillFromObject->metaObject();
        QMetaMethod fillFromChanged = privMeta->method(privMeta->indexOfSlot("fillFromPropertyChange()"));

        for (int i = thisMeta->propertyOffset(); i < thisMeta->propertyCount(); i++) {
            QMetaProperty thisProp = thisMeta->property(i);
            int j;

            if (QLatin1String("fillFrom") == thisProp.name())
                continue;

            j = thatMeta->indexOfProperty(thisProp.name());

            if (j >= 0) {
                QMetaProperty thatProp = thatMeta->property(j);
                if (thatProp.hasNotifySignal()) {
                    connect(&*priv->m_fillFromObject, thatProp.notifySignal(),
                            priv, fillFromChanged);
                    priv->m_signalPropertyMap[thatProp.notifySignalIndex()] << thatProp.name();
                }
                setProperty(thisProp.name(), priv->m_fillFromObject->property(thatProp.name()));
            }
        }
    } else {
        QMetaType tt(fillFrom.userType());
    }

    priv->m_fillFrom = fillFrom;
}

QVariant MprisMetaData::fillFrom() const
{
    return priv->m_fillFrom;
}

QVariantMap MprisMetaData::extraFields() const
{
    QVariantMap rv;

    for (auto c = priv->m_metaData.cbegin();
         c != priv->m_metaData.cend();
         ++c) {
        if (c.key().count(':') == 1 && !c.key().startsWith(QLatin1String("mpris:")) && !c.key().startsWith(QLatin1String("xesam:"))) {
            rv[c.key()] = c.value();
        }
    }

    return rv;
}

void MprisMetaData::setExtraFields(const QVariantMap &fields)
{
    for (auto c = fields.cbegin();
         c != fields.cend();
         ++c) {
        if (c.key().count(':') == 1 && !c.key().startsWith(QLatin1String("mpris:")) && !c.key().startsWith(QLatin1String("xesam:"))) {
            priv->m_metaData[c.key()] = c.value();
        }
    }
}
