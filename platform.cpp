#include "platform.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

#include <QDebug>

#if MAC_OS_X_VERSION_10_5
#include <sys/resource.h>
#define min(a,b) (((a) < (b)) ? (a) : (b))
#elif WINNT
#include <stdio.h>
#endif

Q_GLOBAL_STATIC(QString, game_config_dir)
Q_GLOBAL_STATIC(QString, game_config_file)
Q_GLOBAL_STATIC(QString, game_torrent_file)
Q_GLOBAL_STATIC(QString, game_torrent_state_file)
Q_GLOBAL_STATIC(QString, game_data_directory)
Q_GLOBAL_STATIC(QString, platform_id)

QString getGameConfigFile()
{
    return *game_config_file();
}

QString getGameTorrentFile()
{
    return *game_torrent_file();
}

QString getGameTorrentStateFile()
{
    return *game_torrent_state_file();
}

QString getGameDataDirectory()
{
    return *game_data_directory();
}

QString getPlatformId()
{
    return *platform_id();
}

QString getAsset(QString asset)
{
    return *game_config_dir() + asset;
}

void platformInitialize()
{
    QFileInfo executable_file_info(QCoreApplication::applicationFilePath());

#if MAC_OS_X_VERSION_10_5
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("plugins");
    QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));

    *game_config_dir()          = executable_file_info.path() + "/../game_config/";
    *game_config_file()         = executable_file_info.path() + "/../game_config/game.cfg";
    *game_torrent_file()        = executable_file_info.path() + "/../game_config/game.torrent";
    *game_torrent_state_file()  = executable_file_info.path() + "/../game_config/game.state";
    *game_data_directory()      = executable_file_info.path() + "/../game_data/";
    *platform_id()              = "mac";

    rlimit rlp;
    getrlimit(RLIMIT_NOFILE, &rlp);
    qDebug() << "Platform initialization: open files limit" << rlp.rlim_cur << "rising to max" << min(OPEN_MAX, rlp.rlim_max);
    rlp.rlim_cur = min(OPEN_MAX, rlp.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rlp);
#elif WINNT
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cd("plugins");
    QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));

    *game_config_dir()          = executable_file_info.path() + "/game_config/";
    *game_config_file()         = executable_file_info.path() + "/game_config/game.torrent";
    *game_torrent_file()        = executable_file_info.path() + "/game_config/game.cfg";
    *game_torrent_state_file()  = executable_file_info.path() + "/game_config/ltrr.state";
    *game_data_directory()      = executable_file_info.path() + "/game_data/";
    *platform_id()              = "win";

    qDebug() << "Platform initialization: open files limit" << _getmaxstdio() << "rising to max" << 2048;
    _setmaxstdio(2048);
#endif
}
