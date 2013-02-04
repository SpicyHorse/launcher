#include "platform.h"

#include <QDesktopServices>
#include <QCoreApplication>
#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

#include <QDebug>

#ifdef Q_WS_MAC
#include <sys/resource.h>
#define min(a,b) (((a) < (b)) ? (a) : (b))
#elif defined Q_WS_WIN
#include <stdio.h>
#endif

Q_GLOBAL_STATIC(QString, platform_id)
Q_GLOBAL_STATIC(QString, game_config_dir)
Q_GLOBAL_STATIC(QString, game_config_file)
Q_GLOBAL_STATIC(QString, launcher_data_path)

static QSettings * game_settings;

QString getGameName()
{
    return game_settings->value("global/name").toString();
}

QString getPlatformId()
{
    return *platform_id();
}

QString getGameTorrentFile()
{
    return *launcher_data_path() + "/game.torrent";
}

QString getGameTorrentStateFile()
{
    return *launcher_data_path() + "/game.state";
}

QString getDefaultGameDataDirectory()
{
    return *launcher_data_path() + "/game_data/";
}

QString getAsset(QString asset)
{
    return *game_config_dir() + asset;
}

QSettings * getGameSettings()
{
    return game_settings;
}

void platformInitialize()
{
    QFileInfo executable_file_info(QCoreApplication::applicationFilePath());

#ifdef Q_WS_MAC
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("plugins");
    QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));

    *game_config_dir()          = executable_file_info.path() + "/../game_config/";
    *game_config_file()         = executable_file_info.path() + "/../game_config/game.cfg";
    *platform_id()              = "mac";

    rlimit rlp;
    getrlimit(RLIMIT_NOFILE, &rlp);
    qDebug() << "Platform initialization: open files limit" << rlp.rlim_cur << "rising to max" << min(OPEN_MAX, rlp.rlim_max);
    rlp.rlim_cur = min(OPEN_MAX, rlp.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rlp);
#elif defined Q_WS_WIN
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cd("plugins");
    QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));

    *game_config_dir()          = executable_file_info.path() + "/game_config/";
    *game_config_file()         = executable_file_info.path() + "/game_config/game.cfg";
    *platform_id()              = "win";

    qDebug() << "Platform initialization: open files limit" << _getmaxstdio() << "rising to max" << 2048;
    _setmaxstdio(2048);
#endif

    if (!QFileInfo(*game_config_file()).isReadable()) {
        qCritical() << "MainWindow::MainWindow() unable to open configuration" << *game_config_file();
        QMessageBox::critical(0, "Unable to open game configuration", "Unable to open game configuration.");
        exit(EXIT_FAILURE);
    }
    game_settings = new QSettings(*game_config_file(), QSettings::IniFormat);

    QCoreApplication::setOrganizationName("SpicyHorse");
    QCoreApplication::setOrganizationDomain("spicyhorse.com");
    QCoreApplication::setApplicationName(getGameName());

    *launcher_data_path() = QDir::fromNativeSeparators(QDesktopServices::storageLocation(QDesktopServices::DataLocation));

    QSettings app_settings;
    // Migration one
    if (!app_settings.contains("version")) {
        QDir m1_dir(*launcher_data_path());
        m1_dir.cdUp();

        if (m1_dir.cd("Launcher")) {
            QDir().rename(m1_dir.absolutePath(), *launcher_data_path());
            QDir().rename(
                        *launcher_data_path() + "/akaneiro/",
                        *launcher_data_path() + "/game_data/"
                        );
            QFile().rename(
                        *launcher_data_path() + "/akaneiro.torrent",
                        *launcher_data_path() + "/game.torrent"
                        );
        }

        if (app_settings.contains("bt/datapath")) {
            app_settings.setValue("bt/datapath", QDir::fromNativeSeparators(app_settings.value("bt/datapath").toString()));
        }
    }

    // done with migrations, tag current version
    app_settings.setValue("version", LAUNCHER_VERSION_INT);
    app_settings.sync();

    if (!QFileInfo(*launcher_data_path()).exists()) {
        QDir().mkpath(*launcher_data_path());
    }
}
