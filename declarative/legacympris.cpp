#include "legacympris_p.h"
#include <QMetaEnum>

LegacyMpris::LegacyMpris(QObject *parent)
    : QObject(parent)
{
}

QString LegacyMpris::metadataToString(LegacyMpris::Metadata metadata)
{
    const char *strVal = QMetaEnum::fromType<LegacyMpris::Metadata>().valueToKey(metadata);
    return QString::fromLatin1(strVal);
}
