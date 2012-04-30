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
    switch (ui->comboBox->currentIndex())
    {
        case 0:
        {
            this->setVisible(false);
            AchievementInjectorDialog dialog(this, xdbf);
            dialog.exec();
            this->close();
            break;
        }
        case 2:
        {
            this->setVisible(false);
            SettingInjectorInt dialog(this, xdbf);
            dialog.exec();
            this->close();
            break;
        }
        case 9:
        {
            this->setVisible(false);
            TitleInjectorDialog dialog(this, xdbf);
            dialog.exec();
            this->close();
            break;
        }
    }
}
