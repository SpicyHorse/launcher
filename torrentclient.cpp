#include "torrentclient.h"

#include <QFileInfo>
#include <QDebug>

#include "laexception.h"

#include <libtorrent/session.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>

#include <unistd.h>


TorrentClient::TorrentClient(QObject *parent) :
    QObject(parent), a(false), s(0)
{
}

TorrentClient::~TorrentClient()
{
    if (s)
        closeSession();
}

void TorrentClient::initSession()
{
    libtorrent::error_code ec;
    s = new libtorrent::session();
    s->set_alert_mask(libtorrent::alert::all_categories);
    s->listen_on(std::make_pair(56881, 56889), ec);

    if (ec) {
        qCritical() << this << ec.message().c_str();
        throw LaException("Unable to start listening");
    }

    s->start_dht();
    s->start_lsd();
    s->start_upnp();
    s->start_natpmp();
}

void TorrentClient::closeSession()
{
    s->stop_natpmp();
    s->stop_upnp();
    s->stop_lsd();
    s->stop_dht();

    delete s; s=0;
}

bool yes(libtorrent::torrent_status const&) { return true; }

void TorrentClient::sync(QString torrent, QString destination_dir) {

    if (!QFileInfo(torrent).isReadable()) {
        throw LaException("Game torrent is not readable");
    }

    libtorrent::error_code ec;
    libtorrent::add_torrent_params p;

    libtorrent::torrent_info *ti;
    ti = new libtorrent::torrent_info(torrent.toLocal8Bit().data(), ec);
    if (ec) {
        qCritical() << "Unable to get torrent info. TRR error:" << ec.message().c_str();
        throw LaException("Unable to get torrent info");
    }

    p.save_path = destination_dir.toLocal8Bit().data();
    p.ti = ti;
    s->add_torrent(p, ec);
    if (ec) {
        qCritical() << "Unable to add torrent to queue. TRR error:" << ec.message().c_str();
        throw LaException("Unable to add torrent to queue");
    }

    bool done = false;
    while (true) {
        std::vector<libtorrent::torrent_status> tss;
        std::vector<libtorrent::torrent_status>::iterator tss_i;

        s->get_torrent_status(&tss, &yes);
        for (tss_i = tss.begin(); tss_i != tss.end(); tss_i ++) {
            libtorrent::torrent_status ts = *tss_i;
            qDebug() << a << s->is_paused() << s->status().download_rate << ts.error.c_str() << ts.progress;

            emit progress(100 * ts.progress);
            if (ts.progress == 1)
                done = true;

            if (!ts.error.empty()) {
                // todo inform about error;
                break;
            }
        }

        std::deque<libtorrent::alert*> alerts;
        s->pop_alerts(&alerts);
        for (std::deque<libtorrent::alert*>::iterator i = alerts.begin(), end(alerts.end()); i != end; ++i) {
            libtorrent::alert *al = (*i);
            qDebug() << al->message().c_str();
            delete al;
        }
        alerts.clear();

        if (a || done)
            break;

        usleep(250000);
    }
}

void TorrentClient::abort()
{
    a = true;
}
