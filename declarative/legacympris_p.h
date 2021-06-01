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
