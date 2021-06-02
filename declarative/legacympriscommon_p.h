#ifndef LEGACYMPRISCOMMON_P_H
#define LEGACYMPRISCOMMON_P_H

#include <QVariantMap>
#include <MprisMetaData>

QVariantMap &operator<<(QVariantMap &a, const MprisMetaData *b);
MprisMetaData *operator<<(MprisMetaData *a, const QVariantMap &b);

#endif
