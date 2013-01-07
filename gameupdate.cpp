#include "gameupdate.h"

#include <QCryptographicHash>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QEventLoop>
#include <QSettings>
#include <QFileInfo>
#include <QFile>

#include <QDebug>

#include "laexception.h"
#include "torrentclient.h"

GameUpdate::GameUpdate(QObject *parent) :
    QThread(parent), have_update(false),
    file_torrent(), file_config(), file_state(), path_gamedir(),
    settings(0), tc(0)
{
    QFileInfo bi(QCoreApplication::applicationFilePath());
#if MAC_OS_X_VERSION_10_5
    file_torrent = bi.path() + "/../game_config/game.torrent";
    file_config = bi.path() + "/../game_config/game.cfg";
    file_state = bi.path() + "/../game_config/ltrr.state";
    path_gamedir = bi.path() + "/../game_data/";
#elif WIN32 or WINNT
    file_torrent = bi.path() + "/game_config/game.torrent";
    file_config = bi.path() + "/game_config/game.cfg";
    file_state = bi.path() + "/game_config/ltrr.state";
    path_gamedir = bi.path() + "/game_data/";
#endif

    if (QFileInfo(file_config).isReadable())
        settings = new QSettings(file_config, QSettings::IniFormat);

    tc = new TorrentClient();
    tc->initSession();

    connect(tc, SIGNAL(progress(int)), this, SIGNAL(progress(int)) );
}

GameUpdate::~GameUpdate()
{
    if (isRunning()) {
        wait();
    }
}

QString GameUpdate::getGameExecutable()
{
    if (!settings)
        throw LaException("Game configaration not found, can't construct game executable path");
    return path_gamedir + settings->value("global/executable").toString();
}

QStringList GameUpdate::getGameArgs()
{
    if (!settings)
        throw LaException("Game configaration not found, can't construct game args");
    return settings->value("global/args").toStringList();
}

void GameUpdate::run()
{
    if (!settings) {
        emit message("Game Configuration is not found\nNOWAI u can has cheezburger");
        return;
    }

    try {
        emit message("Checking game version");
        checkVersion();
    } catch (LaException &e) {
        if (settings->value("global/play_on_error").toBool()) {
            emit message("Can't connect to update server\nAssuming that you have latest");
            emit showPlay(true);
        } else {
            emit message("Can't connect to update server\nPlease check your internet connection");
        }
        return;
    }

    try {
        emit showProgress(true);
        downloadUpdate();
        emit showProgress(false);

        emit message("I'm ready. Press play");
        emit showPlay(true);
    } catch (LaException &e) {
        emit message("Error while checking game updates");
        emit showProgress(false);
    }
}

void GameUpdate::checkVersion()
{
    QPair<int, QByteArray> response = fetchURL(QUrl(settings->value("global/channel").toString() + "/check/" + calculateMD5(file_torrent)));

    if (response.first != 200) {
        qDebug() << "GameUpdate::checkVersion(): !=200" << response.first << response.second;
        throw LaException("GameUpdate::checkVersion(), wrong response");
    }

    if (response.second.startsWith("FRESH")) {
        have_update = false;
    } else if (response.second.startsWith("UPDATE:")) {
        QByteArray s = response.second;
        s.remove(0, 7);

        QFile t(file_torrent);
        if (t.open(QIODevice::WriteOnly)) {
            t.write(s);
            t.close();
            have_update = true;
        } else {
            throw LaException("Can't open torrent file for writing");
        }
    } else {
        qDebug() << "GameUpdate::checkVersion(): Unknown command in response" << response.first << response.second;
        throw LaException("GameUpdate::checkVersion(), wrong response");
    }
}

void GameUpdate::downloadUpdate()
{
    tc->sync(file_torrent, path_gamedir);
}

QPair<int, QByteArray> GameUpdate::fetchURL(QUrl url)
{
    QEventLoop event_loop;
    QNetworkAccessManager manager;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &event_loop, SLOT(quit()));

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "SpicyLauncher");

    QNetworkReply *reply = manager.get(request);

    event_loop.exec();

    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    return QPair<int, QByteArray>(status_code, reply->readAll());
}

QString GameUpdate::calculateMD5(QString file)
{
    QFile f(file);
    QCryptographicHash h(QCryptographicHash::Md5);

    f.open(QIODevice::ReadOnly);
    h.addData(f.readAll()); // TODO: i can eat memory too

    return h.result().toHex();
}

void GameUpdate::abort()
{
    tc->abort();
}
