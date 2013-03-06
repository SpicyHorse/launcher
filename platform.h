#ifndef PLATFORM_H
#define PLATFORM_H

#include <QString>

class QSettings;

#define LAUNCHER_VERSION "SpicyLauncher/2"
#define LAUNCHER_VERSION_INT 2

// get common paths
QString getGameName();
QString getPlatformId();
QString getGameTorrentFile();
QString getGameTorrentStateFile();
QString getDefaultGameDataDirectory();

QSettings * getGameSettings();

// get path to asset
QString getAsset(QString);

// initialize common paths and platform specific staff
void platformInitialize();

#endif // PLATFORM_H
