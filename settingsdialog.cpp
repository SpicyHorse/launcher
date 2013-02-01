#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>

#include "platform.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->debugLinkButton, SIGNAL(clicked()), this, SIGNAL(debugRequested()) );
    connect(ui->dataPathButton, SIGNAL(clicked()), this, SLOT(selectPath()) );
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::show()
{
    QSettings s;

    ui->seedCheckBox->setChecked(s.value("bt/seed_enabled", true).toBool());
    ui->dataPathEdit->setText(s.value("bt/datapath", getDefaultGameDataDirectory()).toString());

    ui->downloadCheckBox->setChecked(s.value("bt/download_limit_enabled", false).toBool());
    ui->uploadCheckBox->setChecked(s.value("bt/upload_limit_enabled", false).toBool());

    ui->downloadSpin->setEnabled(s.value("bt/download_limit_enabled", false).toBool());
    ui->uploadSpin->setEnabled(s.value("bt/upload_limit_enabled", false).toBool());
    ui->downloadSpin->setValue(s.value("bt/download_limit_value", 128).toInt());
    ui->uploadSpin->setValue(s.value("bt/upload_limit_value", 128).toInt());
    ui->connectionsSpin->setValue(s.value("bt/connections_limit_value", 32).toInt());

    ui->utpCheckBox->setChecked(s.value("bt/utp_enabled", true).toBool());

    QDialog::show();
}

void SettingsDialog::accept()
{
    QSettings s;

    s.setValue("bt/seed_enabled", ui->seedCheckBox->isChecked());

    QDir src_dir(s.value("bt/datapath", getDefaultGameDataDirectory()).toString());
    QDir dst_dir(ui->dataPathEdit->text());
    if (src_dir != dst_dir) {
        s.setValue("bt/datapath", ui->dataPathEdit->text());
        emit moveGameDataRequested(src_dir.absolutePath(), dst_dir.absolutePath());
    }
    s.setValue("bt/download_limit_enabled", ui->downloadCheckBox->isChecked());
    s.setValue("bt/upload_limit_enabled", ui->uploadCheckBox->isChecked());
    s.setValue("bt/download_limit_value", ui->downloadSpin->value());
    s.setValue("bt/upload_limit_value", ui->uploadSpin->value());
    s.setValue("bt/connections_limit_value", ui->connectionsSpin->value());
    s.setValue("bt/utp_enabled", ui->utpCheckBox->isChecked());

    QDialog::accept();
}

void SettingsDialog::selectPath()
{
    QDir cur_dir(ui->dataPathEdit->text());
    if (!cur_dir.exists())
        cur_dir.mkdir(cur_dir.absolutePath());

    QString path = QFileDialog::getExistingDirectory(
                this,
                tr("Choose path to store game"),
                cur_dir.absolutePath()
    );

    QDir new_dir(path);
    if (path.isEmpty() || cur_dir == new_dir)
        return;

    qDebug() << "SettingsDialog::selectPath() validating path" << path;
    QFileInfo fi(path);
    if (fi.isWritable()) {
        if (!QDir().rmdir(path)) {
            QMessageBox::critical(
                        this,
                        tr("Unable to use selected directory"),
                        tr("Unable to use selected directory\n\nDirectory is not empty, we need empty directory to relocate game data.")
            );
            return;
        }
        QDir().mkdir(path);
        ui->dataPathEdit->setText(path);
    } else {
        QMessageBox::critical(
                    this,
                    tr("Unable to use selected directory"),
                    tr("Unable to use selected directory\n\nDirectory is not writable, make sure that you are not using system directory.")
        );
    }
}
