#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

public slots:
    void show();
    void accept();
    void selectPath();

signals:
    void debugRequested();
    void moveGameDataRequested(QString, QString);

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
