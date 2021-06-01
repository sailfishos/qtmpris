#ifndef LEGACYMPRISPLAYER_P_H
#define LEGACYMPRISPLAYER_P_H

#include <MprisPlayer>
#include <QObject>

class LegacyMprisPlayer : public MprisPlayer
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap metadata READ metadata WRITE setMetadata NOTIFY metadataChanged)

public:
    LegacyMprisPlayer(QObject *parent = nullptr);
    ~LegacyMprisPlayer();

    QVariantMap metadata() const;
    void setMetadata(const QVariantMap &metadata);

Q_SIGNALS:
    void metadataChanged();
};

#endif
