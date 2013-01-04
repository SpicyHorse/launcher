#ifndef GAMEUPDATE_H
#define GAMEUPDATE_H

#include <QThread>
#include <QStringList>
#include <QByteArray>
#include <QString>
#include <QPair>
#include <QUrl>

class QSettings;
class TorrentClient;

class GameUpdate : public QThread
{
    Q_OBJECT
public:
    explicit GameUpdate(QObject *parent = 0);
    ~GameUpdate();
    QString getGameExecutable();
    QStringList getGameArgs();

protected:
    void run();


private:
    bool have_update;
    QString file_torrent;
    QString file_config;
    QString file_state;
    QString path_gamedir;
    QSettings * settings;
    TorrentClient *tc;

    void checkVersion();
    void downloadUpdate();

    QPair<int, QByteArray> fetchURL(QUrl url);
    QString calculateMD5(QString file);

signals:
    void message(QString m);
    void showProgress(bool);
    void progress(int p);
    void showPlay(bool);

public slots:
    void abort();
};

#endif // GAMEUPDATE_H
