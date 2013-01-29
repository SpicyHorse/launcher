#ifndef PLATFORM_H
#define PLATFORM_H

#include <QString>

class QSettings;

// get common paths
QString getPlatformId();
QString getGameConfigFile();
QString getGameTorrentFile();
QString getGameTorrentStateFile();
QString getGameDataDirectory();

QSettings * getGameSettings();

// get path to asset
QString getAsset(QString);

// initialize common paths and platform specific staff
void platformInitialize();

#endif // PLATFORM_H
