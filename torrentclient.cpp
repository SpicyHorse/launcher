#include "torrentclient.h"

#include <QTimerEvent>
#include <QTextStream>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>

#include <libtorrent/session.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>

#include <unistd.h>

#include "utils.h"

TorrentClient::TorrentClient(QObject *parent) :
    QObject(parent), session(0), log_timer_id(-1), sync_timer_id(-1)
{
}

TorrentClient::~TorrentClient()
{
    closeSession();
}

bool TorrentClient::openSession()
{
    libtorrent::error_code ec;
    session = new libtorrent::session();

    applySettings();

#if DEBUG_BUILD
    session->set_alert_mask(libtorrent::alert::all_categories);
#endif
    session->listen_on(std::make_pair(56881, 56889), ec);

    if (ec) {
        qCritical() << this << ec.message().c_str();
        emit message(tr("Unable to start listening"));
        return false;
    }

    session->start_dht();
    session->start_lsd();
    session->start_upnp();
    session->start_natpmp();

    log_timer_id = startTimer(500);

    return true;
}

void TorrentClient::closeSession()
{
    if (!session)
        return;

    session->stop_natpmp();
    session->stop_upnp();
    session->stop_lsd();
    session->stop_dht();

    delete session; session=0;
}

void TorrentClient::applySettings()
{
    QSettings s;

    libtorrent::session_settings settings = session->settings();

    settings.user_agent = "SpicyhorseLauncher/0010";
    settings.stop_tracker_timeout = 1;
    settings.connections_limit = 128;
    settings.file_pool_size = 32;

    if (s.value("bt/upload_limit_enabled", false).toBool()) {
        settings.upload_rate_limit = s.value("bt/upload_limit_value", 128).toInt() * 1024;
    } else {
        settings.upload_rate_limit = 0;
    }

    if (s.value("bt/download_limit_enabled", false).toBool()) {
        settings.download_rate_limit = s.value("bt/download_limit_value", 128).toInt() * 1024;
    } else {
        settings.download_rate_limit = 0;
    }

    session->set_settings(settings);
}

bool TorrentClient::openTorrent(QString torrent, QString destination_dir) {
    emit message(tr("Syncing game files"));

    if (!session) {
        qCritical() << "Programming error, session not started";
        emit message(tr("Programming error, bt-session is not started"));
        return false;
    }

    if (!QFileInfo(torrent).isReadable()) {
        emit message(tr("Game torrent is not readable"));
        return false;
    }

    libtorrent::error_code ec;
    libtorrent::add_torrent_params p;

    libtorrent::torrent_info *ti;
    ti = new libtorrent::torrent_info(torrent.toLocal8Bit().data(), ec);
    if (ec) {
        qCritical() << "Unable to get torrent info. TRR error:" << ec.message().c_str();
        emit message(tr("Unable to get torrent info"));
        return false;
    }

    p.save_path = destination_dir.toLocal8Bit().data();
    p.ti = ti;
    session->add_torrent(p, ec);
    if (ec) {
        qCritical() << "Unable to add torrent to queue. TRR error:" << ec.message().c_str();
        emit message(tr("Unable to add torrent to queue"));
        return false;
    }

    sync_timer_id = startTimer(500);

    return true;
}

bool yes(libtorrent::torrent_status const&) { return true; }

void TorrentClient::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == log_timer_id)
        timerLog();
    else if (event->timerId() == sync_timer_id)
        timerSync();
    else
        qCritical() << "TorrentClient::timerEvent(): programming error";
}

void TorrentClient::timerLog()
{
    std::deque<libtorrent::alert*> alerts;
    session->pop_alerts(&alerts);
    for (std::deque<libtorrent::alert*>::iterator i = alerts.begin(), end(alerts.end()); i != end; ++i) {
        libtorrent::alert *al = (*i);
        qDebug() << "TorrentClient::timerLog():" << al->message().c_str();
        delete al;
    }
    alerts.clear();
}

void TorrentClient::timerSync()
{
    std::vector<libtorrent::torrent_status> tss;
    std::vector<libtorrent::torrent_status>::iterator tss_i;

    session->get_torrent_status(&tss, &yes);
    for (tss_i = tss.begin(); tss_i != tss.end(); tss_i ++) {
        libtorrent::torrent_status torrent_status = *tss_i;
        libtorrent::session_status sessions_status = session->status();

        qDebug() << "TorrentClient::timerSync()" << session->is_paused()
                 << sessions_status.download_rate << torrent_status.progress;

        QString str;
        QTextStream stream(&str);
        stream << "Syncing game: ";

        switch (torrent_status.state) {
        case libtorrent::torrent_status::queued_for_checking:
            stream << tr("queued");
            break;
        case libtorrent::torrent_status::checking_files:
            stream << tr("checking local files");
            break;
        case libtorrent::torrent_status::downloading_metadata:
            stream << tr("fetching meta");
            break;
        case libtorrent::torrent_status::downloading:
            stream << tr("downloading");
            break;
        case libtorrent::torrent_status::finished:
            stream << tr("finished");
            break;
        case libtorrent::torrent_status::seeding:
            stream << tr("seeding");
            break;
        case libtorrent::torrent_status::allocating:
            stream << tr("allocating");
            break;
        case libtorrent::torrent_status::checking_resume_data:
            stream << tr("checking resume");
            break;
        default:
            stream << "ololo";
            break;
        }
        stream << "\n";

        stream << formatSize(torrent_status.total_wanted_done) <<" of " << formatSize(torrent_status.total_wanted)
               << " speed: "<< formatSize(sessions_status.download_rate) << "/s";

        emit progress(10000 * torrent_status.progress);
        emit message(str);

        if (session->is_paused())
            session->resume();

        if (torrent_status.progress == 1) {

            killTimer(sync_timer_id); sync_timer_id = -1;
            emit message(tr("Game Syncronized"));
            emit success(true);
        }

        if (!torrent_status.error.empty()) {
            qCritical() << "TorrentClient::timerSync() error" << torrent_status.error.c_str();
        }
    }
}
