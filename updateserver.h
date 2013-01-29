#ifndef UPDATESERVER_H
#define UPDATESERVER_H

#include <QObject>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;
class QSettings;

class UpdateServer : public QObject
{
    Q_OBJECT
public:
    explicit UpdateServer(QObject *parent);

private:
    QSettings * game_cfg;
    QNetworkAccessManager *net_manager;

    QString calculateMD5(QString file);

signals:
    void message(QString);
    void success(bool);
    void error();

public slots:
    void checkUpdates();

private slots:
    void requestFinished(QNetworkReply *);
};

#endif // UPDATESERVER_H
