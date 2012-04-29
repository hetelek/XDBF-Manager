#include "newentrychooser.h"
#include "ui_newentrychooser.h"
#include "achievementinjectordialog.h"
#include "titleinjectordialog.h"

NewEntryChooser::NewEntryChooser(QWidget *parent, XDBF *xdbf) : QDialog(parent), ui(new Ui::NewEntryChooser), xdbf(xdbf)
{
    ui->setupUi(this);
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
       /* case 0:
            this->setVisible(false);
            AchievementInjectorDialog dialog1(this, xdbf);
            dialog1.exec();
            this->close();
            break; */

         case 3:
            this->setVisible(false);
            TitleInjectorDialog dialog2(this, xdbf);
            dialog2.exec();
            this->close();
            break;
    }
}
