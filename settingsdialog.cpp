#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::show()
{
    QSettings s;

    ui->seedCheckBox->setChecked(s.value("bt/seed_enabled", true).toBool());
    ui->downloadCheckBox->setChecked(s.value("bt/download_limit_enabled", false).toBool());
    ui->uploadCheckBox->setChecked(s.value("bt/upload_limit_enabled", false).toBool());
    ui->downloadSpin->setValue(s.value("bt/download_limit_value", 128).toInt());
    ui->uploadSpin->setValue(s.value("bt/upload_limit_value", 128).toInt());

    QDialog::show();
}

void SettingsDialog::accept()
{
    QSettings s;

    s.setValue("bt/seed_enabled", ui->seedCheckBox->isChecked());
    s.setValue("bt/download_limit_enabled", ui->downloadCheckBox->isChecked());
    s.setValue("bt/upload_limit_enabled", ui->uploadCheckBox->isChecked());
    s.setValue("bt/download_limit_value", ui->downloadSpin->value());
    s.setValue("bt/upload_limit_value", ui->uploadSpin->value());

    QDialog::accept();
}
