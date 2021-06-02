#ifndef LEGACYMPRISMANAGER_P_H
#define LEGACYMPRISMANAGER_P_H

#include <MprisManager>
#include <QObject>

class LegacyMprisManager : public MprisManager
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap metadata READ metadata NOTIFY metadataChanged)

public:
    LegacyMprisManager(QObject *parent = nullptr);
    ~LegacyMprisManager();

    QVariantMap metadata() const;

    qlonglong position() const;

Q_SIGNALS:
    void metadataChanged();
};

#endif
