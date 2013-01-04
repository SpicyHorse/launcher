#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class GameUpdate;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *e);

public slots:
    void startGame();

    void gameProcessStarted();
    void gameProcessError(QProcess::ProcessError);
    void gameProcessExited(int);

private:
    Ui::MainWindow *ui;
    GameUpdate *gu;
    QProcess *gp;
};

#endif // MAINWINDOW_H
