#include "addressconverter.h"
#include "ui_addressconverter.h"

AddressConverter::AddressConverter(QWidget *parent, XDBF *xdbf) : QDialog(parent), ui(new Ui::AddressConverter), xdbf(xdbf)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->realSpBx->setValue(xdbf->get_offset(0));
    ui->realSpBx->setMinimum(ui->realSpBx->value());

    setFixedSize(sizeHint());
}

AddressConverter::~AddressConverter()
{
    delete ui;
}

void AddressConverter::on_pushButton_clicked()
{
    if (realLastChanged)
        ui->specifierSpbx->setValue(xdbf->getFakeOffset(ui->realSpBx->value()));
    else
        ui->realSpBx->setValue(xdbf->get_offset(ui->specifierSpbx->value()));
}

void AddressConverter::on_realSpBx_valueChanged(int arg1)
{
    realLastChanged = true;
}

void AddressConverter::on_specifierSpbx_valueChanged(int arg1)
{
    realLastChanged = false;
}
