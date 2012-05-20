#include "newentrychooser.h"
#include "ui_newentrychooser.h"
#include "achievementinjectordialog.h"
#include "titleinjectordialog.h"
#include "settinginjectorint.h"
#include "imageinjectordialog.h"
#include "stringinjector.h"
#include "binaryinjector.h"

NewEntryChooser::NewEntryChooser(QWidget *parent, XDBF *xdbf) : QDialog(parent), ui(new Ui::NewEntryChooser), xdbf(xdbf)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    setFixedSize(sizeHint());

    vector<unsigned short> ids = xdbf->getEntrySyncTypes();
    if (std::find(ids.begin(), ids.end(), 1) != ids.end())
        ui->comboBox->addItem("Achievement");
    ui->comboBox->addItem("Image");
    if (std::find(ids.begin(), ids.end(), 3) != ids.end())
    {
        ui->comboBox->addItem("Setting - Int32");
        ui->comboBox->addItem("Setting - Int64");
        ui->comboBox->addItem("Setting - Double");
        ui->comboBox->addItem("Setting - String");
        ui->comboBox->addItem("Setting - Float");
        ui->comboBox->addItem("Setting - Binary");
        ui->comboBox->addItem("Setting - DateTime");
    }
    if (std::find(ids.begin(), ids.end(), 4) != ids.end())
        ui->comboBox->addItem("Title");
    ui->comboBox->addItem("String");
}

NewEntryChooser::~NewEntryChooser()
{
    delete ui;
}

void NewEntryChooser::on_pushButton_2_clicked()
{
    this->close();
}

void NewEntryChooser::on_pushButton_clicked()
{
    this->setVisible(false);

    if (ui->comboBox->currentText() == "Achievement")
    {
        AchievementInjectorDialog dialog(this, xdbf);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "Image")
    {
        ImageInjectorDialog dialog(this, xdbf);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "Setting - Int32")
    {
        SettingInjectorInt dialog(this, xdbf, SET_INT32);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "Setting - Int64")
    {
        SettingInjectorInt dialog(this, xdbf, SET_INT64);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "Setting - Double")
    {
        SettingInjectorInt dialog(this, xdbf, SET_DOUBLE);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "Setting - Unicode")
    {
        SettingInjectorInt dialog(this, xdbf, SET_UNICODE);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "Setting - Float")
    {
        SettingInjectorInt dialog(this, xdbf, SET_FLOAT);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "Setting - Binary")
    {
        BinaryInjector dialog(xdbf, this);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "Setting - DateTime")
    {
        SettingInjectorInt dialog(this, xdbf, SET_DATETIME);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "Title")
    {
        TitleInjectorDialog dialog(this, xdbf);
        dialog.exec();
    }
    else if (ui->comboBox->currentText() == "String")
    {
        StringInjector dialog(this, xdbf);
        dialog.exec();
    }

    this->close();
}
