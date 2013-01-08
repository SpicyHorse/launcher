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
    explicit MainWindow(QApplication *app, QWidget *parent = 0);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *e);

public slots:
    void startGame();

    void gameProcessStarted();
    void gameProcessError(QProcess::ProcessError);
    void gameProcessExited(int);

private:
    Ui::MainWindow *ui;
    int diffX;
    int diffY;
    bool diffA;
    GameUpdate *gu;
    QProcess *gp;
};

#endif // MAINWINDOW_H
