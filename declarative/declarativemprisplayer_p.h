#ifndef DECLARATIVEMPRISPLAYER_P_H
#define DECLARATIVEMPRISPLAYER_P_H

#include "mprisplayer.h"

class MPRIS_QT_EXPORT DeclarativeMprisPlayer : public MprisPlayer
{
    Q_OBJECT

    virtual qlonglong position() const;
protected:
    Q_INVOKABLE QVariant getPosition() const;
};

#endif
