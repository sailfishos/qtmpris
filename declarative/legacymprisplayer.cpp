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
