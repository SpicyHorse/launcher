#ifndef TORRENTCLIENT_H
#define TORRENTCLIENT_H

#include <QObject>
#include <QString>

namespace libtorrent {
    class session;
}

class TorrentClient : public QObject
{
    Q_OBJECT
public:
    explicit TorrentClient(QObject *parent);
    virtual ~TorrentClient();

public slots:
    bool openSession();
    void closeSession();
    void applySettings();
    bool openTorrent(QString torrent, QString destination_dir);

protected:
    void timerEvent(QTimerEvent *);
    void timerLog();
    void timerSync();

private:
    libtorrent::session *session;
    int log_timer_id;
    int sync_timer_id;

signals:
    void message(QString);
    void progress(int);
    void success(bool);
    void error();
};

#endif // TORRENTCLIENT_H
