/*!
 *
 * Copyright (C) 2021 Jolla Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MPRISPLAYER_P_H
#define MPRISPLAYER_P_H

#include <QObject>
#include <QVariantMap>
#include "mprismetadata.h"
#include "mprisplayeradaptor_p.h"
#include "mprisplayer.h"
#include "mprisservice_p.h"

class MprisPlayerPrivate : public MprisServicePrivate
{
    Q_OBJECT

public:
    MprisPlayerPrivate(/* MprisPlayer *player */);
    ~MprisPlayerPrivate();

    MprisPlayer *parent() const;

    void setLoopStatus(const QString &value);
    QString loopStatus() const;

    /*
    void setPlaybackStatus(const QString &value);
    */
    QString playbackStatus() const;

    void setVolume(double volume);
    void setRate(double rate);
    void setShuffle(bool shuffle);

    void Next();
    void OpenUri(const QString &Uri);
    void Pause();
    void Play();
    void PlayPause();
    void Previous();
    void Seek(qlonglong Offset);
    void SetPosition(const QDBusObjectPath &TrackId, qlonglong position);
    void Stop();

    QVariantMap metaData() const;

    QDBusConnection m_connection;
    MprisPlayerAdaptor m_dbusAdaptor;

    MprisMetaData m_metaData;
    bool m_canControl;
    bool m_canGoNext;
    bool m_canGoPrevious;
    bool m_canPause;
    bool m_canPlay;
    bool m_canSeek;
    Mpris::LoopStatus m_loopStatus;
    double m_maximumRate;
    double m_minimumRate;
    Mpris::PlaybackStatus m_playbackStatus;
    qlonglong m_position;
    double m_rate;
    bool m_shuffle;
    double m_volume;
};

#endif
