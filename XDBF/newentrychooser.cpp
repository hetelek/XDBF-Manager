#include "newentrychooser.h"
#include "ui_newentrychooser.h"
#include "achievementinjectordialog.h"
#include "titleinjectordialog.h"
#include "settinginjectorint.h"

NewEntryChooser::NewEntryChooser(QWidget *parent, XDBF *xdbf) : QDialog(parent), ui(new Ui::NewEntryChooser), xdbf(xdbf)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    setFixedSize(sizeHint());
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

    switch (ui->comboBox->currentIndex())
    {
        case 0:
        {
            AchievementInjectorDialog dialog(this, xdbf);
            dialog.exec();
            break;
        }
        case 2:
        {
            SettingInjectorInt dialog(this, xdbf, SET_INT32);
            dialog.exec();
            break;
        }
        case 3:
        {
            SettingInjectorInt dialog(this, xdbf, SET_INT64);
            dialog.exec();
            break;
        }
        case 4:
        {
            SettingInjectorInt dialog(this, xdbf, SET_DOUBLE);
            dialog.exec();
            break;
        }
        case 5:
        {
            SettingInjectorInt dialog(this, xdbf, SET_UNICODE);
            dialog.exec();
            break;
        }
        case 6:
        {
            SettingInjectorInt dialog(this, xdbf, SET_FLOAT);
            dialog.exec();
            break;
        }
        case 9:
        {
            TitleInjectorDialog dialog(this, xdbf);
            dialog.exec();
            break;
        }
    }

    this->close();
}
