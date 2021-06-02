#include <QObject>
#include "mprisplayeradaptor_p.h"
#include "mprisplayer.h"
#include "mprisplayer_p.h"
#include "mprisservice_p.h"
#include "mprismetadata_p.h"
#include "mprismetadata.h"

namespace {
    const QString PlayerInterface = MprisPlayerAdaptor::staticMetaObject.classInfo(MprisPlayerAdaptor::staticMetaObject.indexOfClassInfo("D-Bus Interface")).value();
    const QString TrackPrefix = QStringLiteral("/org/mpris/MediaPlayer2/TrackList/");
}

MprisPlayerPrivate::MprisPlayerPrivate(/* MprisPlayer *player */)
    : MprisServicePrivate(/* player */)
    , m_connection(getDBusConnection())
    , m_dbusAdaptor(this)
    , m_metaData(this)
    , m_canControl(false)
    , m_canGoNext(false)
    , m_canGoPrevious(false)
    , m_canPause(false)
    , m_canPlay(false)
    , m_canSeek(false)
    , m_loopStatus(Mpris::None)
    , m_maximumRate(1)
    , m_minimumRate(1)
    , m_playbackStatus(Mpris::Stopped)
    , m_position(0)
    , m_rate(1.0)
    , m_shuffle(false)
    , m_volume(0.0)
{
    connect(&m_metaData, &MprisMetaData::metaDataChanged, this, [this] { propertyChanged(PlayerInterface, QStringLiteral("Metadata"), metaData()); });
}

MprisPlayerPrivate::~MprisPlayerPrivate()
{
}

MprisPlayer *MprisPlayerPrivate::parent() const
{
    return static_cast<MprisPlayer *>(QObject::parent());
}

void MprisPlayerPrivate::setLoopStatus(const QString &value)
{
    bool ok;
    int enumVal = QMetaEnum::fromType<Mpris::LoopStatus>().keyToValue(value.toUtf8(), &ok);

    if (!ok) {
        sendErrorReply(QDBusError::InvalidArgs, QStringLiteral("Invalid loop status"));
        return;
    }

    Q_EMIT parent()->loopStatusRequested(static_cast<Mpris::LoopStatus>(enumVal));
}

QString MprisPlayerPrivate::loopStatus() const
{
    const char *strVal = QMetaEnum::fromType<Mpris::LoopStatus>().valueToKey(static_cast<int>(parent()->loopStatus()));
    return QString::fromLatin1(strVal);
}

/*
void MprisPlayerPrivate::setPlaybackStatus(const QString &value)
{
    bool ok;
    int enumVal = QMetaEnum::fromType<Mpris::LoopStatus>().keyToValue(value.toUtf8(), &ok);

    if (!ok) {
        sendErrorReply(QDBusError::InvalidArgs, QStringLiteral("Invalid loop status"));
        return;
    }

    parent()->setPlaybackStatus(static_cast<Mpris::PlaybackStatus>(enumVal));
}
*/

QString MprisPlayerPrivate::playbackStatus() const
{
    const char *strVal = QMetaEnum::fromType<Mpris::PlaybackStatus>().valueToKey(static_cast<int>(parent()->playbackStatus()));
    return QString::fromLatin1(strVal);
}

void MprisPlayerPrivate::setRate(double rate)
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else if (rate < parent()->minimumRate() || rate > parent()->maximumRate()) {
        sendErrorReply(QDBusError::InvalidArgs, QStringLiteral("Rate not in the allowed range"));
    } else {
        Q_EMIT parent()->rateRequested(rate);
    }
}

void MprisPlayerPrivate::setShuffle(bool shuffle)
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else {
        Q_EMIT parent()->shuffleRequested(shuffle);
    }
}

void MprisPlayerPrivate::setVolume(double volume)
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else {
        Q_EMIT parent()->volumeRequested(volume);
    }
}

void MprisPlayerPrivate::Next()
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else if (!parent()->canGoNext()) {
        sendErrorReply(QDBusError::Failed, QStringLiteral("The operation can not be performed"));
    } else {
        Q_EMIT parent()->nextRequested();
    }
}

void MprisPlayerPrivate::OpenUri(const QString &Uri)
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else {
        Q_EMIT parent()->openUriRequested(QUrl::fromUserInput(Uri));
    }
}

void MprisPlayerPrivate::Pause()
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else if (!parent()->canPause()) {
        sendErrorReply(QDBusError::Failed, QStringLiteral("The operation can not be performed"));
    } else {
        Q_EMIT parent()->pauseRequested();
    }
}

void MprisPlayerPrivate::Play()
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else if (!parent()->canPlay()) {
        sendErrorReply(QDBusError::Failed, QStringLiteral("The operation can not be performed"));
    } else {
        Q_EMIT parent()->playRequested();
    }
}

void MprisPlayerPrivate::PlayPause()
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else if (!parent()->canPlay() && !parent()->canPause()) {
        sendErrorReply(QDBusError::Failed, QStringLiteral("The operation can not be performed"));
    } else {
        Q_EMIT parent()->playPauseRequested();
    }
}

void MprisPlayerPrivate::Previous()
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else if (!parent()->canGoPrevious()) {
        sendErrorReply(QDBusError::Failed, QStringLiteral("The operation can not be performed"));
    } else {
        Q_EMIT parent()->previousRequested();
    }
}

void MprisPlayerPrivate::Seek(qlonglong Offset)
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else if (!parent()->canSeek()) {
        sendErrorReply(QDBusError::Failed, QStringLiteral("The operation can not be performed"));
    } else {
        Q_EMIT parent()->seekRequested(Offset / 1000);
    }
}

void MprisPlayerPrivate::SetPosition(const QDBusObjectPath &TrackId, qlonglong position)
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else if (!parent()->canSeek()) {
        sendErrorReply(QDBusError::Failed, QStringLiteral("The operation can not be performed"));
    } else {
        QString tid = TrackId.path();
        if (tid.startsWith(TrackPrefix))
            tid = tid.mid(TrackPrefix.size());
        Q_EMIT parent()->setPositionRequested(tid, position / 1000);
    }
}

void MprisPlayerPrivate::Stop()
{
    if (!parent()->canControl()) {
        sendErrorReply(QDBusError::NotSupported, QStringLiteral("The operation is not supported"));
    } else {
        Q_EMIT parent()->stopRequested();
    }
}

QVariantMap MprisPlayerPrivate::metaData() const
{
    return m_metaData.priv->typedMetaData();
}

MprisPlayer::MprisPlayer(QObject *parent) :
    MprisService(new MprisPlayerPrivate, parent)
{
    connect(this, &MprisPlayer::seeked, priv, [this](qlonglong position) { Q_EMIT static_cast<MprisPlayerPrivate *>(priv)->m_dbusAdaptor.Seeked(position * 1000); });
}

MprisPlayer::~MprisPlayer()
{
}

bool MprisPlayer::canControl() const
{
    return static_cast<MprisPlayerPrivate *>(priv)->m_canControl;
}

bool MprisPlayer::canGoNext() const
{
    if (!canControl())
        return false;
    return static_cast<MprisPlayerPrivate *>(priv)->m_canGoNext;
}

bool MprisPlayer::canGoPrevious() const
{
    if (!canControl())
        return false;
    return static_cast<MprisPlayerPrivate *>(priv)->m_canGoPrevious;
}

bool MprisPlayer::canPause() const
{
    if (!canControl())
        return false;
    return static_cast<MprisPlayerPrivate *>(priv)->m_canPause;
}

bool MprisPlayer::canPlay() const
{
    if (!canControl())
        return false;
    return static_cast<MprisPlayerPrivate *>(priv)->m_canPlay;
}

bool MprisPlayer::canSeek() const
{
    if (!canControl())
        return false;
    return static_cast<MprisPlayerPrivate *>(priv)->m_canSeek;
}

Mpris::LoopStatus MprisPlayer::loopStatus() const
{
    return static_cast<MprisPlayerPrivate *>(priv)->m_loopStatus;
}

double MprisPlayer::maximumRate() const
{
    return static_cast<MprisPlayerPrivate *>(priv)->m_maximumRate;
}

MprisMetaData *MprisPlayer::metaData() const
{
    return &static_cast<MprisPlayerPrivate *>(priv)->m_metaData;
}

double MprisPlayer::minimumRate() const
{
    return static_cast<MprisPlayerPrivate *>(priv)->m_minimumRate;
}
Mpris::PlaybackStatus MprisPlayer::playbackStatus() const
{
    return static_cast<MprisPlayerPrivate *>(priv)->m_playbackStatus;
}
qlonglong MprisPlayer::position() const
{
    return static_cast<MprisPlayerPrivate *>(priv)->m_position;
}
double MprisPlayer::rate() const
{
    return static_cast<MprisPlayerPrivate *>(priv)->m_rate;
}
bool MprisPlayer::shuffle() const
{
    return static_cast<MprisPlayerPrivate *>(priv)->m_shuffle;
}
double MprisPlayer::volume() const
{
    return static_cast<MprisPlayerPrivate *>(priv)->m_volume;
}

void MprisPlayer::setCanControl(bool canControl)
{
    if (canControl != static_cast<MprisPlayerPrivate *>(priv)->m_canControl) {
        static_cast<MprisPlayerPrivate *>(priv)->m_canControl = canControl;
        Q_EMIT canControlChanged();
    }
}
void MprisPlayer::setCanGoNext(bool canGoNext)
{
    if (canGoNext != static_cast<MprisPlayerPrivate *>(priv)->m_canGoNext) {
        static_cast<MprisPlayerPrivate *>(priv)->m_canGoNext = canGoNext;
        if (canControl()) {
            Q_EMIT canGoNextChanged();
            static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("CanGoNext"), canGoNext);
        }
    }
}
void MprisPlayer::setCanGoPrevious(bool canGoPrevious)
{
    if (canGoPrevious != static_cast<MprisPlayerPrivate *>(priv)->m_canGoPrevious) {
        static_cast<MprisPlayerPrivate *>(priv)->m_canGoPrevious = canGoPrevious;
        if (canControl()) {
            Q_EMIT canGoPreviousChanged();
            static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("CanGoPrevious"), canGoPrevious);
        }
    }
}
void MprisPlayer::setCanPause(bool canPause)
{
    if (canPause != static_cast<MprisPlayerPrivate *>(priv)->m_canPause) {
        static_cast<MprisPlayerPrivate *>(priv)->m_canPause = canPause;
        if (canControl()) {
            Q_EMIT canPauseChanged();
            static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("CanPause"), canPause);
        }
    }
}
void MprisPlayer::setCanPlay(bool canPlay)
{
    if (canPlay != static_cast<MprisPlayerPrivate *>(priv)->m_canPlay) {
        static_cast<MprisPlayerPrivate *>(priv)->m_canPlay = canPlay;
        if (canControl()) {
            Q_EMIT canPlayChanged();
            static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("CanPlay"), canPlay);
        }
    }
}
void MprisPlayer::setCanSeek(bool canSeek)
{
    if (canSeek != static_cast<MprisPlayerPrivate *>(priv)->m_canSeek) {
        static_cast<MprisPlayerPrivate *>(priv)->m_canSeek = canSeek;
        if (canControl()) {
            Q_EMIT canSeekChanged();
            static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("CanSeek"), canSeek);
        }
    }
}

void MprisPlayer::setLoopStatus(Mpris::LoopStatus loopStatus)
{
    if (loopStatus != static_cast<MprisPlayerPrivate *>(priv)->m_loopStatus) {
        static_cast<MprisPlayerPrivate *>(priv)->m_loopStatus = loopStatus;
        Q_EMIT loopStatusChanged();
        static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("LoopStatus"), static_cast<MprisPlayerPrivate *>(priv)->m_dbusAdaptor.loopStatus());
    }
}

void MprisPlayer::setMaximumRate(double maximumRate)
{
    if (maximumRate != static_cast<MprisPlayerPrivate *>(priv)->m_maximumRate) {
        static_cast<MprisPlayerPrivate *>(priv)->m_maximumRate = maximumRate;
        Q_EMIT maximumRateChanged();
        static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("MaximumRate"), maximumRate);
    }
}
void MprisPlayer::setMinimumRate(double minimumRate)
{
    if (minimumRate != static_cast<MprisPlayerPrivate *>(priv)->m_minimumRate) {
        static_cast<MprisPlayerPrivate *>(priv)->m_minimumRate = minimumRate;
        Q_EMIT minimumRateChanged();
        static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("MinimumRate"), minimumRate);
    }
}
void MprisPlayer::setPlaybackStatus(Mpris::PlaybackStatus playbackStatus)
{
    if (playbackStatus != static_cast<MprisPlayerPrivate *>(priv)->m_playbackStatus) {
        static_cast<MprisPlayerPrivate *>(priv)->m_playbackStatus = playbackStatus;
        Q_EMIT playbackStatusChanged();
        static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("PlaybackStatus"), static_cast<MprisPlayerPrivate *>(priv)->m_dbusAdaptor.playbackStatus());
    }
}
void MprisPlayer::setPosition(qlonglong position)
{
    if (position != static_cast<MprisPlayerPrivate *>(priv)->m_position) {
        static_cast<MprisPlayerPrivate *>(priv)->m_position = position;
        Q_EMIT positionChanged();
    }
}
void MprisPlayer::setRate(double rate)
{
    if (rate != static_cast<MprisPlayerPrivate *>(priv)->m_rate) {
        static_cast<MprisPlayerPrivate *>(priv)->m_rate = rate;
        Q_EMIT rateChanged();
        static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("Rate"), rate);
    }
}
void MprisPlayer::setShuffle(bool shuffle)
{
    if (shuffle != static_cast<MprisPlayerPrivate *>(priv)->m_shuffle) {
        static_cast<MprisPlayerPrivate *>(priv)->m_shuffle = shuffle;
        Q_EMIT shuffleChanged();
        static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("Shuffle"), shuffle);
    }
}
void MprisPlayer::setVolume(double volume)
{
    if (volume != static_cast<MprisPlayerPrivate *>(priv)->m_volume) {
        static_cast<MprisPlayerPrivate *>(priv)->m_volume = volume;
        Q_EMIT volumeChanged();
        static_cast<MprisPlayerPrivate *>(priv)->propertyChanged(PlayerInterface, QStringLiteral("Volume"), volume);
    }
}
