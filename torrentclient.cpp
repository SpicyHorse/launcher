#include "torrentclient.h"

#include <QTextStream>
#include <QFileInfo>
#include <QDebug>

#include "laexception.h"

#include <libtorrent/session.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>

#include <unistd.h>


TorrentClient::TorrentClient(QObject *parent) :
    QObject(parent), a(false), s(0), timer_id(-1)
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
#if DEBUG_BUILD
    s->set_alert_mask(libtorrent::alert::all_categories);
#endif
    s->listen_on(std::make_pair(56881, 56889), ec);

    if (ec) {
        qCritical() << this << ec.message().c_str();
        throw LaException("Unable to start listening");
    }

    s->start_dht();
    s->start_lsd();
    s->start_upnp();
    s->start_natpmp();

    timer_id = startTimer(500);
}

void TorrentClient::closeSession()
{
    killTimer(timer_id);
    s->stop_natpmp();
    s->stop_upnp();
    s->stop_lsd();
    s->stop_dht();

    delete s; s=0;
}

void TorrentClient::timerEvent(QTimerEvent *)
{
    std::deque<libtorrent::alert*> alerts;
    s->pop_alerts(&alerts);
    for (std::deque<libtorrent::alert*>::iterator i = alerts.begin(), end(alerts.end()); i != end; ++i) {
        libtorrent::alert *al = (*i);
        qDebug() << "TorrentClient::timerEvent() libtorrent allerts" << al->message().c_str();
        delete al;
    }
    alerts.clear();
}

bool yes(libtorrent::torrent_status const&) { return true; }

QString formatSize(int s) {
    const char *arr[] = { "B", "KB", "MB", "GB", "TB"};

    int t = 0;
    while (s > 1024 && t < 5) {
        s = s >> 10;
        t++;
    }
    QString str;
    QTextStream stream(&str);
    stream << s << arr[t];
    return str;
}

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
            libtorrent::torrent_status torrent_status = *tss_i;
            libtorrent::session_status sessions_status = s->status();

            qDebug() << "TorrentClient::sync()" << a << s->is_paused()
                     << sessions_status.download_rate << torrent_status.progress;

            if (torrent_status.progress == 1)
                done = true;

            if (!torrent_status.error.empty()) {
                qCritical() << "TorrentClient::sync()" << torrent_status.error.c_str();
            }

            QString str;
            QTextStream stream(&str);
            stream << "Syncing game: ";

            switch (torrent_status.state) {
            case libtorrent::torrent_status::queued_for_checking:
                stream << "queued";
                break;
            case libtorrent::torrent_status::checking_files:
                stream << "checking local files";
                break;
            case libtorrent::torrent_status::downloading_metadata:
                stream << "fetching meta";
                break;
            case libtorrent::torrent_status::downloading:
                stream << "downloading";
                break;
            case libtorrent::torrent_status::finished:
                stream << "finished";
                break;
            case libtorrent::torrent_status::seeding:
                stream << "seeding";
                break;
            case libtorrent::torrent_status::allocating:
                stream << "allocating";
                break;
            case libtorrent::torrent_status::checking_resume_data:
                stream << "checking resume";
                break;
            default:
                stream << "ololo";
                break;
            }
            stream << "\n";

            stream << "got: "<< formatSize(torrent_status.total_wanted_done) <<" from: " << formatSize(torrent_status.total_wanted)
                   << " speed: "<< formatSize(sessions_status.download_rate);

            emit progress(100 * torrent_status.progress);
            emit info(str);

            if (s->is_paused())
                s->resume();
        }

        if (a || done)
            break;

        usleep(250000);
    }
}

void TorrentClient::abort()
{
    a = true;
}
