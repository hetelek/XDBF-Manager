#include "titleinjectordialog.h"
#include "ui_titleinjectordialog.h"

TitleInjectorDialog::TitleInjectorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TitleInjectorDialog)
{
    ui->setupUi(this);
}

TitleInjectorDialog::~TitleInjectorDialog()
{
    delete ui;
}
