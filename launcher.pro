#-------------------------------------------------
#
# Project created by QtCreator 2012-12-27T21:26:59
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = launcher
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    adwidget.cpp \
    torrentclient.cpp \
    platform.cpp \
    updateserver.cpp \
    settingsdialog.cpp \
    utils.cpp \
    progresswidget.cpp

HEADERS  += mainwindow.h \
    adwidget.h \
    torrentclient.h \
    platform.h \
    updateserver.h \
    settingsdialog.h \
    utils.h \
    progresswidget.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    assets.qrc

OTHER_FILES += launcher.rc manifest.xml

CONFIG(debug, debug|release) {
    DEFINES += DEBUG_BUILD
}

macx {
    QMAKE_CXXFLAGS += -DTORRENT_USE_OPENSSL -DWITH_SHIPPED_GEOIP_H -DBOOST_ASIO_HASH_MAP_BUCKETS=1021 -DBOOST_EXCEPTION_DISABLE -DBOOST_ASIO_ENABLE_CANCELIO -DBOOST_ASIO_DYN_LINK -DTORRENT_LINKING_SHARED
    LIBS += -ltorrent-rasterbar -lboost_system-mt
    ICON = launcher.icns
}

windows {
    RC_FILE = launcher.rc

    QMAKE_CXXFLAGS += -ftemplate-depth-128 -O3 -finline-functions -Wno-inline -Wall -mthreads -Wno-missing-braces -fno-strict-aliasing
    QMAKE_CXXFLAGS += -DBOOST_ALL_NO_LIB -DBOOST_ASIO_ENABLE_CANCELIO -DBOOST_ASIO_HASH_MAP_BUCKETS=1021 -DBOOST_ASIO_SEPARATE_COMPILATION
    QMAKE_CXXFLAGS += -DBOOST_EXCEPTION_DISABLE -DBOOST_SYSTEM_STATIC_LINK=1 -DNDEBUG -DTORRENT_DISABLE_GEO_IP -DTORRENT_NO_BOOST_DATE_TIME
    QMAKE_CXXFLAGS += -DTORRENT_USE_I2P=1 -DTORRENT_USE_TOMMATH -DUNICODE -DWIN32 -DWIN32_LEAN_AND_MEAN -D_FILE_OFFSET_BITS=64 -D_UNICODE
    QMAKE_CXXFLAGS += -D_WIN32 -D_WIN32_WINNT=0x0500 -D__USE_W32_SOCKETS
    QMAKE_CXXFLAGS += -I"C:\install\include" -I"C:\Boost\include\boost-1_52"
    LIBS += -LC:\install\lib -ltorrent -lboost_system-mgw46-mt-1_52
    LIBS += -lws2_32 -lwsock32 -lpthread
}
