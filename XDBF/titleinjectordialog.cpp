#include "titleinjectordialog.h"
#include "ui_titleinjectordialog.h"

TitleInjectorDialog::TitleInjectorDialog(QWidget *parent, XDBF *xdbf) : QDialog(parent), ui(new Ui::TitleInjectorDialog), xdbf(xdbf)
{
    ui->setupUi(this);
}

TitleInjectorDialog::~TitleInjectorDialog()
{
    delete ui;
}

void TitleInjectorDialog::on_pushButton_clicked()
{
    ui->listWidget->addItem(ui->comboBox->currentText());
    ui->comboBox->removeItem(ui->comboBox->currentIndex());
}
