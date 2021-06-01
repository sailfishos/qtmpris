#include "declarativemprisplayer_p.h"

qlonglong DeclarativeMprisPlayer::position() const
{
    QVariant rv;
    if (QMetaObject::invokeMethod(const_cast<DeclarativeMprisPlayer *>(this), "getPosition", Q_RETURN_ARG(QVariant, rv))) {
        return rv.toLongLong();
    }
    return -1;
}

QVariant DeclarativeMprisPlayer::getPosition() const
{
    return MprisPlayer::position();
}
