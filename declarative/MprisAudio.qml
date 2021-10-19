import QtQuick 2.0
import QtMultimedia 5.6
import Amber.Mpris 2.0

MprisPlayer {
    property Audio player: parent
    property Playlist playlist: player ? player.playlist : null
    property bool seekDetection: true
    property int _oldLoopStatus: Playlist.Sequential
    property var _oldPosition: 0
    property var _oldStamp

    playbackStatus: {
        switch (player.playbackState) {
        case Audio.PlayingState:
            return Mpris.Playing
        case Audio.PausedState:
            return Mpris.Paused
        default:
            return Mpris.Stopped
        }
    }

    canControl: true

    canGoNext: playlist && playlist.currentIndex < playlist.itemCount - 1 || loopStatus != Mpris.None
    canGoPrevious: playlist && playlist.currentIndex > 0 || loopStatus != Mpris.None

    canPlay: player.playbackState != Mpris.Playing && (player.source || playlist.itemCount)
    canPause: player.playbackState == Mpris.Playing

    canSeek: player.seekable
    metaData.fillFrom: player.metaData
    metaData.url: player.source
    metaData.trackId: playlist && playlist.currentIndex >= 0 ? playlist.currentIndex : null

    loopStatus: {
        if (!playlist) {
            return Mpris.None
        } else if (playlist.playbackMode == Playlist.Random) {
            return Mpris.Playlist
        }

        _oldLoopStatus = playlist.playbackMode

        if (playlist.playbackMode == Playlist.CurrentItemInLoop) {
            return Mpris.Track
        } else if (playlist.playbackMode == Playlist.Loop) {
            return Mpris.Playlist
        } else {
            return Mpris.None
        }
    }

    shuffle: playlist ? playlist.playbackMode == Playlist.Random : false
    rate: player.playbackRate
    volume: player.muted ? 0 : player.volume
    onPlayRequested: player.play()
    onStopRequested: player.stop()
    onPauseRequested: player.pause()
    onPlayPauseRequested: if (player.playbackState == Audio.PlayingState) { player.pause() } else { player.play() }
    onNextRequested: if (playlist) { playlist.next() }
    onPreviousRequested: if (playlist) { playlist.previous() }
    onShuffleRequested: if (playlist) { playlist.playbackMode = (shuffle ? Playlist.Random : _oldLoopStatus) }
    onLoopStatusRequested: {
        if (!playlist) {
            return
        }
        if (loopStatus == Mpris.Track) {
            playlist.playbackMode = Playlist.CurrentItemInLoop
        } else if (loopStatus == Mpris.Playlist) {
            playlist.playbackMode = Playlist.Loop
        } else {
            playlist.playbackMode = Playlist.Sequential
        }
    }

    onSetPositionRequested: {
        if (!playlist || playlist.currentIndex == trackId) {
            player.seek(position)
            if (!seekDetection) {
                seeked(player.position)
            }
        }
    }

    onSeekRequested: {
        if (playlist && player.metaData.duration && player.position + offset >= player.metaData.duration) {
            playlist.next()
        } else {
            player.seek(player.position + offset)
            if (!seekDetection) {
                seeked(player.position)
            }
        }
    }

    onPlaybackStatusChanged: {
        if (!seekDetection) {
            return;
        }

        _oldPosition = player.position
        _oldStamp = Date.now()
    }

    property Connections _connections: Connections {
        target: player
        onPositionChanged: {
            if (!seekDetection) {
                return;
            }
            var stamp = Date.now();
            if ((playbackStatus != Mpris.Playing && position != _oldPosition)
                || (_oldStamp
                    && Math.abs((player.position - _oldPosition) - (stamp - _oldStamp) * rate) > 100)) {
                seeked(player.position)
            }

            _oldStamp = stamp
            _oldPosition = player.position
        }

        onSourceChanged: {
            if (!seekDetection) {
                return;
            }
            _oldPosition = player.position
            _oldStamp = Date.now()
        }
    }

    function getPosition() {
        return player.position
    }
}
