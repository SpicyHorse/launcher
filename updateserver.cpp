#include "updateserver.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QCryptographicHash>
#include <QFile>

#include <QSettings>
#include <QDebug>

#include "platform.h"

UpdateServer::UpdateServer(QObject *parent, QSettings *cfg) :
    QObject(parent), game_cfg(cfg), net_manager(new QNetworkAccessManager(this))
{
    connect(net_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

QString UpdateServer::calculateMD5(QString file)
{
    QFile f(file);
    QCryptographicHash h(QCryptographicHash::Md5);

    f.open(QIODevice::ReadOnly);
    while (!f.atEnd()) {
        h.addData(f.read(4096));
    }

    return h.result().toHex();
}

void UpdateServer::checkUpdates()
{
    emit message(tr("Connecting to update server"));

    QString url(game_cfg->value("global/channel").toString() + getPlatformId() + "/latest/" + calculateMD5(getGameTorrentFile()));
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "SpicyLauncher");

    net_manager->get(request);
    qDebug() << "UpdateServer::checkUpdates() request started" << url;
}

void UpdateServer::requestFinished(QNetworkReply *reply)
{
    int http_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray http_data = reply->readAll();

    if (http_code != 200) {
        qCritical() << "UpdateServer::requestFinished(): http_code != 200" << http_code << http_data;
        emit message(tr("Error connectin to update server"));
        emit error();
        return;
    }

    if (http_data.startsWith("FRESH")) {
        emit message(tr("No updates avaliable"));
        emit success(false);
    } else if (http_data.startsWith("UPDATE:")) {
        emit message(tr("Applying update to game BT-info"));

        QFile t(getGameTorrentFile());
        if (t.open(QIODevice::WriteOnly)) {
            t.write(http_data.mid(7));
            t.close();
            emit message(tr("BT-info updated"));
            emit success(true);
        } else {
            qCritical() << "UpdateServer::requestFinished(): unable to write BT-info" << t.errorString();
            emit message(tr("Unable to write BT-info"));
            emit error();
        }
    } else {
        qCritical() << "UpdateServer::requestFinished(): unknown command in response" << http_data;
        emit message(tr("Update server is insane."));
        emit error();
    }
}
