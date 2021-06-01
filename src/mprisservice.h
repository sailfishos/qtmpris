#ifndef MPRIS_SERVICE_H
#define MPRIS_SERVICE_H

#include <QObject>
#include <QDBusContext>
#include "mprisqt.h"

class MprisServicePrivate;

class MPRIS_QT_EXPORT MprisService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY serviceNameChanged)

    Q_PROPERTY(bool canQuit READ canQuit WRITE setCanQuit NOTIFY canQuitChanged)
    Q_PROPERTY(bool canRaise READ canRaise WRITE setCanRaise NOTIFY canRaiseChanged)
    Q_PROPERTY(bool canSetFullscreen READ canSetFullscreen WRITE setCanSetFullscreen NOTIFY canSetFullscreenChanged)
    Q_PROPERTY(QString desktopEntry READ desktopEntry WRITE setDesktopEntry NOTIFY desktopEntryChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(bool hasTrackList READ hasTrackList WRITE setHasTrackList NOTIFY hasTrackListChanged)
    Q_PROPERTY(QString identity READ identity WRITE setIdentity NOTIFY identityChanged)
    Q_PROPERTY(QStringList supportedUriSchemes READ supportedUriSchemes WRITE setSupportedUriSchemes NOTIFY supportedUriSchemesChanged)
    Q_PROPERTY(QStringList supportedMimeTypes READ supportedMimeTypes WRITE setSupportedMimeTypes NOTIFY supportedMimeTypesChanged)

public:
    MprisService(QObject *parent = 0);
    virtual ~MprisService();

    QString serviceName() const;
    bool canQuit() const;
    bool canRaise() const;
    bool canSetFullscreen() const;
    QString desktopEntry() const;
    bool fullscreen() const;
    bool hasTrackList() const;
    QString identity() const;
    QStringList supportedUriSchemes() const;
    QStringList supportedMimeTypes() const;

public Q_SLOTS:
    void setServiceName(const QString &serviceName);
    void setCanQuit(bool canQuit);
    void setCanRaise(bool canRaise);
    void setCanSetFullscreen(bool canSetFullscreen);
    void setDesktopEntry(const QString &desktopEntry);
    void setFullscreen(bool fullscreen);
    void setHasTrackList(bool hasTrackList);
    void setIdentity(const QString &identity);
    void setSupportedUriSchemes(const QStringList &supportedUriSchemes);
    void setSupportedMimeTypes(const QStringList &supportedMimeTypes);

Q_SIGNALS:
    void serviceNameChanged();
    void canQuitChanged();
    void canRaiseChanged();
    void canSetFullscreenChanged();
    void desktopEntryChanged();
    void fullscreenChanged();
    void hasTrackListChanged();
    void identityChanged();
    void supportedUriSchemesChanged();
    void supportedMimeTypesChanged();
    void fullscreenRequested(bool fullscreen);

    void quitRequested();
    void raiseRequested();

protected:
    MprisService(MprisServicePrivate *priv, QObject *parent);
    MprisServicePrivate *priv;
};

#endif // MPRIS_SERVICE_H
