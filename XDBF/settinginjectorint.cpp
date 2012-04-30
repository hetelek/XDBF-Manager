#include "settinginjectorint.h"
#include "ui_settinginjectorint.h"
#include "xdbf.h"
#include <QMessageBox>

SettingInjectorInt::SettingInjectorInt(QWidget *parent, XDBF *xdbf) : xdbf(xdbf), QDialog(parent), ui(new Ui::SettingInjectorInt)
{
    ui->setupUi(this);

    setFixedSize(sizeHint());
}

SettingInjectorInt::~SettingInjectorInt()
{
    delete ui;
}

void SettingInjectorInt::on_pushButton_clicked()
{
    // create a new setting entry of type int32, and set the value
    // to the one provided by the user
    Setting_Entry entry;
    entry.type = SET_INT32;
    entry.i32_data = ui->spinBox->value();

    // add the entry
    xdbf->injectSettingEntry(&entry, 0);

    QMessageBox::information(this, "Success", "Successfully added Int32 setting entry.");

    close();
}
