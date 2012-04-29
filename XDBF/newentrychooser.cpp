#include "newentrychooser.h"
#include "ui_newentrychooser.h"
#include "achievementinjectordialog.h"

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
        case 0:
            this->setVisible(false);
            AchievementInjectorDialog dialog(this, xdbf);
            dialog.exec();
            this->close();
            break;
    }
}
