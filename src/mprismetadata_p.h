#ifndef MPRISMETADATA_P_H
#define MPRISMETADATA_P_H

#include "mprismetadata.h"
#include <QObject>
#include <QVariantMap>
#include <QMap>
#include <QTimer>
#include <QPointer>
#include <QSet>

class MprisMetaDataPrivate : public QObject
{
    Q_OBJECT

public:
    MprisMetaDataPrivate(MprisMetaData *metaData);
    ~MprisMetaDataPrivate();

    QVariantMap typedMetaData() const;
    void setMetaData(const QString &key, const QVariant &value);
    void setMetaData(const QVariantMap &metaData);
    MprisMetaData *parent() const;

public Q_SLOTS:
    void fillFromPropertyChange();
    void fillFrom();

public:
    QVariantMap m_metaData;
    QTimer m_changedDelay;
    QTimer m_fillFromDelay;
    QVariant m_fillFrom;
    QPointer<QObject> m_fillFromObject;
    QMap<int, QList<const char *>> m_signalPropertyMap;
    QSet<const char *> m_changedProperties;
};

#endif
