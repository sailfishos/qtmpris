// -*- c++ -*-

/*!
 *
 * Copyright (C) 2015-2021 Jolla Ltd.
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

#ifndef MPRIS_PLAYER_H
#define MPRIS_PLAYER_H

#include <QObject>
#include <mprisqt.h>
#include <mprisservice.h>
#include <mpris.h>

class MprisPlayerPrivate;
class MprisMetaData;

class MPRIS_QT_EXPORT MprisPlayer : public MprisService
{
    Q_OBJECT

    Q_PROPERTY(bool canControl READ canControl WRITE setCanControl NOTIFY canControlChanged)
    Q_PROPERTY(bool canGoNext READ canGoNext WRITE setCanGoNext NOTIFY canGoNextChanged)
    Q_PROPERTY(bool canGoPrevious READ canGoPrevious WRITE setCanGoPrevious NOTIFY canGoPreviousChanged)
    Q_PROPERTY(bool canPause READ canPause WRITE setCanPause NOTIFY canPauseChanged)
    Q_PROPERTY(bool canPlay READ canPlay WRITE setCanPlay NOTIFY canPlayChanged)
    Q_PROPERTY(bool canSeek READ canSeek WRITE setCanSeek NOTIFY canSeekChanged)
    Q_PROPERTY(Mpris::LoopStatus loopStatus READ loopStatus WRITE setLoopStatus NOTIFY loopStatusChanged)
    Q_PROPERTY(double maximumRate READ maximumRate WRITE setMaximumRate NOTIFY maximumRateChanged)
    Q_PROPERTY(MprisMetaData *metaData READ metaData NOTIFY metaDataChanged)
    Q_PROPERTY(double minimumRate READ minimumRate WRITE setMinimumRate NOTIFY minimumRateChanged)
    Q_PROPERTY(Mpris::PlaybackStatus playbackStatus READ playbackStatus WRITE setPlaybackStatus NOTIFY playbackStatusChanged)
    Q_PROPERTY(qlonglong position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(double rate READ rate WRITE setRate NOTIFY rateChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged)
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
    MprisPlayer(QObject *parent = 0);
    virtual ~MprisPlayer();

    bool canControl() const;
    bool canGoNext() const;
    bool canGoPrevious() const;
    bool canPause() const;
    bool canPlay() const;
    bool canSeek() const;
    Mpris::LoopStatus loopStatus() const;
    double maximumRate() const;
    MprisMetaData *metaData() const;
    double minimumRate() const;
    Mpris::PlaybackStatus playbackStatus() const;
    virtual qlonglong position() const;
    double rate() const;
    bool shuffle() const;
    double volume() const;

public Q_SLOTS:
    void setCanControl(bool canControl);
    void setCanGoNext(bool canGoNext);
    void setCanGoPrevious(bool canGoPrevious);
    void setCanPause(bool canPause);
    void setCanPlay(bool canPlay);
    void setCanSeek(bool canSeek);
    void setLoopStatus(Mpris::LoopStatus loopStatus);
    void setMaximumRate(double maximumRate);
    void setMinimumRate(double minimumRate);
    void setPlaybackStatus(Mpris::PlaybackStatus playbackStatus);
    void setPosition(qlonglong position);
    void setRate(double rate);
    void setShuffle(bool shuffle);
    void setVolume(double volume);

Q_SIGNALS:
    void canControlChanged();
    void canGoNextChanged();
    void canGoPreviousChanged();
    void canPauseChanged();
    void canPlayChanged();
    void canSeekChanged();
    void loopStatusChanged();
    void maximumRateChanged();
    void metaDataChanged();
    void minimumRateChanged();
    void playbackStatusChanged();
    void positionChanged();
    void rateChanged();
    void shuffleChanged();
    void volumeChanged();

    void loopStatusRequested(Mpris::LoopStatus loopStatus);
    void rateRequested(double rate);
    void shuffleRequested(bool shuffle);
    void volumeRequested(double volume);
    void nextRequested();
    void openUriRequested(const QUrl &url);
    void pauseRequested();
    void playRequested();
    void playPauseRequested();
    void previousRequested();
    void seekRequested(qlonglong offset);
    void seeked(qlonglong position);
    void setPositionRequested(const QString &trackId, qlonglong position);
    void stopRequested();
};

#endif // MPRIS_PLAYER_H
