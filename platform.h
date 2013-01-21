#ifndef PLATFORM_H
#define PLATFORM_H

#include <QString>

// get common paths
QString getGameConfigFile();
QString getGameTorrentFile();
QString getGameTorrentStateFile();
QString getGameDataDirectory();
QString getPlatformId();

// get path to asset
QString getAsset(QString);

// initialize common paths and platform specific staff
void platformInitialize();

#endif // PLATFORM_H
