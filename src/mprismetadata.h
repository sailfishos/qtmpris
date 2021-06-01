#ifndef MPRISMETADATA_H
#define MPRISMETADATA_H

#include <QObject>
#include <QVariant>
#include <mpris.h>

class MprisMetaDataPrivate;

class MPRIS_QT_EXPORT MprisMetaData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant trackId READ trackId WRITE setTrackId NOTIFY metaDataChanged)

    Q_PROPERTY(QVariant duration READ duration WRITE setDuration NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant coverArtUrlSmall READ coverArtUrlSmall WRITE setCoverArtUrlSmall NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant coverArtUrlLarge READ coverArtUrlLarge WRITE setCoverArtUrlLarge NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant albumTitle READ albumTitle WRITE setAlbumTitle NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant albumArtist READ albumArtist WRITE setAlbumArtist NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant contributingArtist READ contributingArtist WRITE setContributingArtist NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant lyrics READ lyrics WRITE setLyrics NOTIFY metaDataChanged)

    // Not in QDeclarativeMediaMetaData
    Q_PROPERTY(QVariant audioBpm READ audioBpm WRITE setAudioBpm NOTIFY metaDataChanged)
    // Not in QDeclarativeMediaMetaData
    Q_PROPERTY(QVariant autoRating READ autoRating WRITE setAutoRating NOTIFY metaDataChanged)

    Q_PROPERTY(QVariant comment READ comment WRITE setComment NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant composer READ composer WRITE setComposer NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant year READ year WRITE setYear NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant date READ date WRITE setDate NOTIFY metaDataChanged)

    // same as discnumber?
    Q_PROPERTY(QVariant chapterNumber READ chapterNumber WRITE setChapterNumber NOTIFY metaDataChanged)

    // Not in QDeclarativeMediaMetaData
    Q_PROPERTY(QVariant firstUsed READ firstUsed WRITE setFirstUsed NOTIFY metaDataChanged)

    Q_PROPERTY(QVariant genre READ genre WRITE setGenre NOTIFY metaDataChanged)

    // Not in QDeclarativeMediaMetaData
    Q_PROPERTY(QVariant lastUsed READ lastUsed WRITE setLastUsed NOTIFY metaDataChanged)

    Q_PROPERTY(QVariant writer READ writer WRITE setWriter NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant title READ title WRITE setTitle NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant trackNumber READ trackNumber WRITE setTrackNumber NOTIFY metaDataChanged)

    // Not in QDeclarativeMediaMetaData
    Q_PROPERTY(QVariant url READ url WRITE setUrl NOTIFY metaDataChanged)
    // Not in QDeclarativeMediaMetaData
    Q_PROPERTY(QVariant useCount READ useCount WRITE setUseCount NOTIFY metaDataChanged)

    Q_PROPERTY(QVariant userRating READ userRating WRITE setUserRating NOTIFY metaDataChanged)

    /* QDeclarativeMediaMetaData fields not in MPRIS spec
    Q_PROPERTY(QVariant audioBitRate READ audioBitRate WRITE setAudioBitRate NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant audioCodec READ audioCodec WRITE setAudioCodec NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant author READ author WRITE setAuthor NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant averageLevel READ averageLevel WRITE setAverageLevel NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant category READ category WRITE setCategory NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant channelCount READ channelCount WRITE setChannelCount NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant conductor READ conductor WRITE setConductor NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant copyright READ copyright WRITE setCopyright NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant description READ description WRITE setDescription NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant director READ director WRITE setDirector NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant keywords READ keywords WRITE setKeywords NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant language READ language WRITE setLanguage NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant leadPerformer READ leadPerformer WRITE setLeadPerformer NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant mediaType READ mediaType WRITE setMediaType NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant mood READ mood WRITE setMood NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant parentalRating READ parentalRating WRITE setParentalRating NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant peakValue READ peakValue WRITE setPeakValue NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant pixelAspectRatio READ pixelAspectRatio WRITE setPixelAspectRatio NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant posterUrl READ posterUrl WRITE setPosterUrl NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant publisher READ publisher WRITE setPublisher NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant ratingOrganization READ ratingOrganization WRITE setRatingOrganization NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant resolution READ resolution WRITE setResolution NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant sampleRate READ sampleRate WRITE setSampleRate NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant size READ size WRITE setSize NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant trackCount READ trackCount WRITE setTrackCount NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant videoBitRate READ videoBitRate WRITE setVideoBitRate NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant videoCodec READ videoCodec WRITE setVideoCodec NOTIFY metaDataChanged)
    Q_PROPERTY(QVariant videoFrameRate READ videoFrameRate WRITE setVideoFrameRate NOTIFY metaDataChanged)
    */

    // Not in QDeclarativeMediaMetaData
    Q_PROPERTY(QVariantMap extraFields READ extraFields WRITE setExtraFields NOTIFY metaDataChanged)

    Q_PROPERTY(QVariant fillFrom READ fillFrom WRITE setFillFrom NOTIFY fillFromChanged)

public:
    MprisMetaData(QObject *parent = 0);
    MprisMetaData();

    QVariant trackId() const;
    void setTrackId(const QVariant &trackId);

    QVariant duration() const;
    void setDuration(const QVariant &duration);
    QVariant coverArtUrlSmall() const;
    void setCoverArtUrlSmall(const QVariant &url);
    QVariant coverArtUrlLarge() const;
    void setCoverArtUrlLarge(const QVariant &url);
    QVariant albumTitle() const;
    void setAlbumTitle(const QVariant &title);
    QVariant albumArtist() const;
    void setAlbumArtist(const QVariant &artist);
    QVariant contributingArtist() const;
    void setContributingArtist(const QVariant &artist);
    QVariant lyrics() const;
    void setLyrics(const QVariant &lyrics);

    QVariant comment() const;
    void setComment(const QVariant &comment);
    QVariant composer() const;
    void setComposer(const QVariant &composer);
    QVariant year() const;
    void setYear(const QVariant &year);
    QVariant date() const;
    void setDate(const QVariant &date);
    QVariant chapterNumber() const;
    void setChapterNumber(const QVariant &chapter);

    QVariant genre() const;
    void setGenre(const QVariant &genre);

    QVariant writer() const;
    void setWriter(const QVariant &writer);

    QVariant title() const;
    void setTitle(const QVariant &title);
    QVariant trackNumber() const;
    void setTrackNumber(const QVariant &track);

    QVariant userRating() const;
    void setUserRating(const QVariant &rating);

    QVariant audioBpm() const;
    void setAudioBpm(const QVariant &bpm);
    QVariant autoRating() const;
    void setAutoRating(const QVariant &rating);
    QVariant firstUsed() const;
    void setFirstUsed(const QVariant &used);
    QVariant lastUsed() const;
    void setLastUsed(const QVariant &used);
    QVariant url() const;
    void setUrl(const QVariant &url);
    QVariant useCount() const;
    void setUseCount(const QVariant &count);

    QVariantMap extraFields() const;
    void setExtraFields(const QVariantMap &fields);

    /*
    QVariant subTitle() const;
    void setSubTitle(const QVariant &title);
    QVariant author() const;
    void setAuthor(const QVariant &author);
    QVariant description() const;
    void setDescription(const QVariant &description);
    QVariant category() const;
    void setCategory(const QVariant &category);
    QVariant keywords() const;
    void setKeywords(const QVariant &keywords);
    QVariant language() const;
    void setLanguage(const QVariant &language);
    QVariant publisher() const;
    void setPublisher(const QVariant &publisher);
    QVariant copyright() const;
    void setCopyright(const QVariant &copyright);
    QVariant parentalRating() const;
    void setParentalRating(const QVariant &rating);
    QVariant ratingOrganization() const;
    void setRatingOrganization(const QVariant &organization);
    QVariant size() const;
    void setSize(const QVariant &size);
    QVariant mediaType() const;
    void setMediaType(const QVariant &type);
    QVariant audioBitRate() const;
    void setAudioBitRate(const QVariant &rate);
    QVariant audioCodec() const;
    void setAudioCodec(const QVariant &codec);
    QVariant averageLevel() const;
    void setAverageLevel(const QVariant &level);
    QVariant channelCount() const;
    void setChannelCount(const QVariant &count);
    QVariant peakValue() const;
    void setPeakValue(const QVariant &value);
    QVariant sampleRate() const;
    void setSampleRate(const QVariant &rate);
    QVariant conductor() const;
    void setConductor(const QVariant &conductor);
    QVariant mood() const;
    void setMood(const QVariant &mood);
    QVariant trackCount() const;
    void setTrackCount(const QVariant &count);
    QVariant resolution() const;
    void setResolution(const QVariant &resolution);
    QVariant pixelAspectRatio() const;
    void setPixelAspectRatio(const QVariant &ratio);
    QVariant videoFrameRate() const;
    void setVideoFrameRate(const QVariant &rate);
    QVariant videoBitRate() const;
    void setVideoBitRate(const QVariant &rate);
    QVariant videoCodec() const;
    void setVideoCodec(const QVariant &codec);
    QVariant posterUrl() const;
    void setPosterUrl(const QVariant &url);
    QVariant director() const;
    void setDirector(const QVariant &director);
    QVariant leadPerformer() const;
    void setLeadPerformer(const QVariant &performer);
    */

    QVariant fillFrom() const;
    void setFillFrom(const QVariant &fillFrom);

Q_SIGNALS:
    void metaDataChanged();
    void fillFromChanged();

private:
    MprisMetaDataPrivate *priv;
    friend class MprisPlayerPrivate;
    friend class MprisControllerPrivate;
};
#endif
