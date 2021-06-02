#include "legacymprismanager_p.h"
#include "legacympriscommon_p.h"
#include <MprisManager>
#include <QObject>

LegacyMprisManager::LegacyMprisManager(QObject *parent)
    : MprisManager(parent)
{
    connect(metaData(), &MprisMetaData::metaDataChanged, this, &LegacyMprisManager::metadataChanged);
}

LegacyMprisManager::~LegacyMprisManager()
{
}

QVariantMap LegacyMprisManager::metadata() const
{
    QVariantMap rv;
    return rv << metaData();
}

qlonglong LegacyMprisManager::position() const
{
    return MprisManager::position() * 1000;
}
