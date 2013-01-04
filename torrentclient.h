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
    explicit TorrentClient(QObject *parent = 0);
    ~TorrentClient();

    void initSession();
    void closeSession();
    void sync(QString torrent, QString destination_dir);

private:
    volatile bool a;
    libtorrent::session *s;

signals:
    void progress(int);

public slots:
    void abort();
};

#endif // TORRENTCLIENT_H
