#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QSharedMemory>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QTimer>
#include <QFileInfo>
#include <QFile>

#include <QDebug>

#include "platform.h"
#include "updateserver.h"
#include "torrentclient.h"
#include "settingsdialog.h"

MainWindow::MainWindow(QApplication *app, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), sd(new SettingsDialog(this)), shm(new QSharedMemory("SpicyhorseLauncher", this)),
    diffX(0), diffY(0), diffA(false),
    app_settings(0), game_settings(0),
    gp(0), us(0), tc(0)
{
    if (!shm->create(1)) {
#ifdef MAC_OS_X_VERSION_10_5
        QMessageBox::critical(this, "Launcher is already running", "Launcher is already running. If it is not please restart your mac.");
#else
        QMessageBox::critical(this, "Launcher is already running", "Launcher is already running.");
#endif
        exit(EXIT_FAILURE);
    }

    setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("MainWindow {background:transparent;}");
    setAttribute(Qt::WA_TranslucentBackground);

    ui->setupUi(this);
    ui->playButton->setDisabled(true);
    ui->reportLabel->setText(tr("Loading Game Information"));

    connect( ui->settingsButton, SIGNAL(clicked()), this->sd, SLOT(show()) );
    connect( ui->closeButton, SIGNAL(clicked()), app, SLOT(quit()) );
    connect( ui->playButton, SIGNAL(clicked()), this, SLOT(gameProcessStart()) );

    gp = new QProcess(this);
    connect( gp, SIGNAL(started()), this, SLOT(gameProcessStarted()) );
    connect( gp, SIGNAL(error(QProcess::ProcessError)), this, SLOT(gameProcessError(QProcess::ProcessError)) );
    connect( gp, SIGNAL(finished(int)), this, SLOT(gameProcessExited(int)) );

    if (!QFileInfo(getGameConfigFile()).isReadable()) {
        qCritical() << "MainWindow::MainWindow() unable to open configuration" << getGameConfigFile();
        ui->reportLabel->setText(tr("Unable to open game configuration"));
        return;
    }

    app_settings = new QSettings();
    game_settings = new QSettings(getGameConfigFile(), QSettings::IniFormat);
    if (game_settings->childGroups().contains("gui")) {
        initUI();
    }

    us = new UpdateServer(this, game_settings);
    connect( us, SIGNAL(message(QString)), ui->reportLabel, SLOT(setText(QString)) );
    connect( us, SIGNAL(success(bool)), this, SLOT(updateServerSuccess(bool)) );
    connect( us, SIGNAL(error()), this, SLOT(updateServerError()) );

    tc = new TorrentClient(this);
    if (!tc->openSession()) {
        qCritical() << "MainWindow::MainWindow() tc->openSession failed";
        ui->reportLabel->setText(tr("BT session not started. Report bug to develeper."));
        return;
    }
    connect( tc, SIGNAL(message(QString)), ui->reportLabel, SLOT(setText(QString)) );
    connect( tc, SIGNAL(progress(int)), ui->reportProgress, SLOT(setValue(int)) );
    connect( tc, SIGNAL(success(bool)), this, SLOT(torrentClientSuccess(bool)) );
    connect( tc, SIGNAL(error()), this, SLOT(torrentClientError()) );
    connect( this->sd, SIGNAL(accepted()), this->tc, SLOT(applySettings()) );
    connect( this->sd, SIGNAL(debugRequested()), this, SLOT(showDebugInfo()) );

    QTimer::singleShot(0, this, SLOT(startUpdate()));
}

MainWindow::~MainWindow()
{
    if (shm->isAttached()) {
        shm->detach();
    }

    delete ui;
}

/*
 * Window event section
 */

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    QPoint p = e->pos();
    diffX = p.x();
    diffY = p.y();
    diffA = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    diffA = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (diffA) {
        QPoint p = e->globalPos();
        move(p.x() - diffX, p.y() - diffY);
    }
}

/*
 * Game Process section
 */

void MainWindow::startUpdate()
{
    us->checkUpdates();
}

/*
 * Update server section
 */

void MainWindow::updateServerSuccess(bool)
{
    tc->openTorrent(getGameTorrentFile(), getGameDataDirectory());
}

void MainWindow::updateServerError()
{

    if (game_settings->value("global/play_on_error").toBool() && QFileInfo(getGameTorrentFile()).size() > 0) {
        QMessageBox::warning(this,
                              tr("Unable to check update server"),
                              tr("Unable to check update server.\n\nLet's imagine that our current version is known as latest one.")
                              );
        updateServerSuccess(false);
    } else {
        QMessageBox::critical(this,
                              tr("Unable to check update server"),
                              tr("Unable to check update server.\n\nCheck network and try again.")
                              );
    }
}

/*
 * Torrent Client section
 */

void MainWindow::torrentClientSuccess(bool)
{
    ui->reportLabel->setText("I'm ready, Press play!");
    ui->playButton->setDisabled(false);
}

void MainWindow::torrentClientError()
{
    QMessageBox::critical(this,
                          tr("Unable to syncronize game data"),
                          tr("Unable to syncronize game data.\n\nCheck disk space, network connection and try again.")
                          );
}

/*
 * Game Process section
 */

void MainWindow::gameProcessStart()
{
    QString command = getGameDataDirectory() + game_settings->value("global/executable").toString();
    QStringList args = game_settings->value("global/args").toStringList();

    QFile f(command);
    f.setPermissions(QFile::ExeGroup | QFile::ExeOther | QFile::ExeUser
                     | QFile::ReadGroup | QFile::ReadUser | QFile::ReadOther
                     | QFile::WriteUser );

    ui->playButton->setDisabled(true);
    gp->start(command, args);
}

void MainWindow::gameProcessStarted()
{
    ui->reportLabel->setText(tr("Game started. I'll wait while you finish, ok?"));
    hide();
}

void MainWindow::gameProcessError(QProcess::ProcessError)
{
    show();
    ui->reportLabel->setText("Failed to start game,\nPlease, report this problem to support");
    ui->playButton->setDisabled(false);
    QMessageBox::critical(this,
                          "Error starting process",
                          QString("Unable to start game process.\n\nCommand:%1\nArgs:%2\n\nBecause: %3").arg(
                              getGameDataDirectory() + game_settings->value("global/executable").toString(),
                              game_settings->value("global/args").toStringList().join(" "),
                              gp->errorString())
                          );
}

void MainWindow::gameProcessExited(int r)
{
    show();
    ui->playButton->setDisabled(false);
    if (r != 0)
        qWarning() << "MainWindow::gameProcessExited(): Game exit code doesn't looks good. You can report bug, if you'd like to.";

    ui->reportLabel->setText("The Demons are Growing in Strength!");
    activateWindow();
}

void MainWindow::initUI()
{
    qDebug() << "MainWindow::initUI() initializing window styling";

    // Window and central widget
    QSize mws(game_settings->value("gui/main_window_w").toInt(), game_settings->value("gui/main_window_h").toInt());
    QRect desk_geom = QApplication::desktop()->geometry();
    resize(mws);
    setGeometry(
                desk_geom.width()/2-width()/2,
                desk_geom.height()/2-height()/2,
                game_settings->value("gui/main_window_w").toInt(),
                game_settings->value("gui/main_window_h").toInt()
                );
    setMinimumSize(mws);
    setMaximumSize(mws);

    ui->centralWidget->setFrame(game_settings->value("gui/main_window_frame").toString());
    ui->centralWidget->setAdvertisment(game_settings->value("gui/main_window_ads").toStringList());
    ui->centralWidget->setAdvertismentOffset(game_settings->value("gui/main_window_ads_fx").toInt(), game_settings->value("gui/main_window_ads_fy").toInt());

    // Close button
    ui->closeButton->setGeometry(
                game_settings->value("gui/main_window_close_x").toInt(),
                game_settings->value("gui/main_window_close_y").toInt(),
                game_settings->value("gui/main_window_close_w").toInt(),
                game_settings->value("gui/main_window_close_h").toInt()
                );
    ui->closeButton->setIcon(QIcon(getAsset(game_settings->value("gui/main_window_close_on").toString())));
    ui->closeButton->setIconSize(QSize(game_settings->value("gui/main_window_close_w").toInt(), game_settings->value("gui/main_window_close_h").toInt()));

    // Settings button
    ui->settingsButton->setGeometry(
                game_settings->value("gui/main_window_settings_x").toInt(),
                game_settings->value("gui/main_window_settings_y").toInt(),
                game_settings->value("gui/main_window_settings_w").toInt(),
                game_settings->value("gui/main_window_settings_h").toInt()
                );
    ui->settingsButton->setIcon(QIcon(getAsset(game_settings->value("gui/main_window_settings_on").toString())));
    ui->settingsButton->setIconSize(QSize(game_settings->value("gui/main_window_settings_w").toInt(), game_settings->value("gui/main_window_settings_h").toInt()));

    // play button
    ui->playButton->setGeometry(
                game_settings->value("gui/main_window_play_x").toInt(),
                game_settings->value("gui/main_window_play_y").toInt(),
                game_settings->value("gui/main_window_play_h").toInt(),
                game_settings->value("gui/main_window_play_w").toInt()
                );
    ui->playButton->setIcon(QIcon(getAsset(game_settings->value("gui/main_window_play_on").toString())));
    ui->playButton->setIconSize(QSize(game_settings->value("gui/main_window_play_w").toInt(), game_settings->value("gui/main_window_play_h").toInt()));

    // Progress Bar
    ui->reportProgress->move(game_settings->value("gui/main_window_progress_x").toInt(), game_settings->value("gui/main_window_progress_y").toInt());
    ui->reportProgress->resize(game_settings->value("gui/main_window_progress_w").toInt(), game_settings->value("gui/main_window_progress_h").toInt());
    ui->reportProgress->setBarSize(
                game_settings->value("gui/main_window_progress_dx").toInt(),
                game_settings->value("gui/main_window_progress_dy").toInt(),
                game_settings->value("gui/main_window_progress_dw").toInt(),
                game_settings->value("gui/main_window_progress_dh").toInt()
                );
    ui->reportProgress->setBackground(game_settings->value("gui/main_window_progress_back").toString());
    ui->reportProgress->setForeground(game_settings->value("gui/main_window_progress_front").toString());
    ui->reportProgress->setFrame(game_settings->value("gui/main_window_progress_frame").toString());

    // Progress Text
    ui->reportLabel->move(game_settings->value("gui/main_window_progress_text_x").toInt(), game_settings->value("gui/main_window_progress_text_y").toInt());
    ui->reportLabel->resize(game_settings->value("gui/main_window_progress_text_w").toInt(), game_settings->value("gui/main_window_progress_text_h").toInt());
    ui->reportLabel->setStyleSheet(game_settings->value("gui/main_window_progress_text_style").toString());
}

void MainWindow::showDebugInfo()
{
    QMessageBox::information(this, "Debug Info", tc->getDebug());
}
