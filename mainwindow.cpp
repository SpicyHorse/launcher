#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gameupdate.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QFrame>

MainWindow::MainWindow(QApplication *app, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), gu(0), gp(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("MainWindow {background:transparent;}");
    setAttribute(Qt::WA_TranslucentBackground);

    gu = new GameUpdate(this);
    gp = new QProcess(this);

    ui->buttonPlay->hide();
    ui->progressReport->hide();
    ui->labelReport->setText("Loading Game Information");
    ui->centralWidget->setImage(new QImage(":/images/spicy.png"));

    connect(gu, SIGNAL(message(QString)),   ui->labelReport,    SLOT(setText(QString))  );
    connect(gu, SIGNAL(showProgress(bool)), ui->progressReport, SLOT(setVisible(bool))  );
    connect(gu, SIGNAL(progress(int)),      ui->progressReport, SLOT(setValue(int))     );
    connect(gu, SIGNAL(showPlay(bool)),     ui->buttonPlay,     SLOT(setVisible(bool))  );

    connect(ui->buttonPlay, SIGNAL(clicked()), this,            SLOT(startGame())       );
    connect(ui->buttonClose, SIGNAL(clicked()), app,            SLOT(quit())            );

    connect(gp, SIGNAL(started()),          this,               SLOT(gameProcessStarted())   );
    connect(gp, SIGNAL(error(QProcess::ProcessError)), this,    SLOT(gameProcessError(QProcess::ProcessError)) );
    connect(gp, SIGNAL(finished(int)),      this,               SLOT(gameProcessExited(int))   );

    gu->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (gu->isRunning()) {
        gu->abort();
    }

    e->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
        QPoint p;
        p = e->pos();
        diffX = p.x();
        diffY = p.y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    QPoint p = e->globalPos();
    move(p.x() - diffX, p.y() - diffY);
}

void MainWindow::startGame()
{
    QString command = gu->getGameExecutable();
    QStringList args = gu->getGameArgs();

    QFile f(command);
    f.setPermissions(QFile::ExeGroup | QFile::ExeOther | QFile::ExeUser
                     | QFile::ReadGroup | QFile::ReadUser | QFile::ReadOther
                     | QFile::WriteUser );

    ui->buttonPlay->setDisabled(true);
    gp->start(command, args);
}

void MainWindow::gameProcessStarted()
{
    ui->labelReport->setText("Game started. I'll wait while you finish, ok?");
    hide();
}

void MainWindow::gameProcessError(QProcess::ProcessError)
{
    show();
    ui->labelReport->setText("FFFFFFailed to start game,\nPlease, report this problem to support");
    ui->buttonPlay->setDisabled(false);
    QMessageBox::critical(this,
                          "Error starting process",
                          QString("Unable to start game process.\n\nCommand:%1\nArgs:%2\n\nBecause: %3").arg(
                              gu->getGameExecutable(), gu->getGameArgs().join(" "), gp->errorString()));
}

void MainWindow::gameProcessExited(int r)
{
    show();
    ui->buttonPlay->setDisabled(false);
    if (r != 0)
        QMessageBox::critical(this, "Game exit code doesn't looks good", "Game exit code doesn't looks good\nYou can report bug, if you'd like to.");

    ui->labelReport->setText("One more time?");
}
