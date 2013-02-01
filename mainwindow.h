#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
    class MainWindow;
}

class QSharedMemory;
class QSettings;
class UpdateServer;
class TorrentClient;
class SettingsDialog;
class QProgressDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QApplication *app, QWidget *parent = 0);
    virtual ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    void closeEvent(QCloseEvent *);

public slots:
    void startUpdate();

    void updateServerSuccess(bool);
    void updateServerError();

    void torrentClientSuccess(bool);
    void torrentClientError();

    void gameProcessStart();
    void gameProcessStarted();
    void gameProcessError(QProcess::ProcessError);
    void gameProcessExited(int);

    void showDebugInfo();
    void moveGameData(QString, QString);

private slots:
    void initUI();
    QString getGameFolder();

private:
    Ui::MainWindow *ui;
    SettingsDialog *sd;
    QSharedMemory *shm;
    int diffX;
    int diffY;
    bool diffA;

    QSettings *app_settings;
    QSettings *game_settings;

    QProcess *gp;
    UpdateServer *us;
    TorrentClient *tc;
};

#endif // MAINWINDOW_H
