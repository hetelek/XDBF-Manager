#include "stringinjector.h"
#include "ui_stringinjector.h"
#include <QMessageBox>

StringInjector::StringInjector(QWidget *parent, XDBF *xdbf) : QDialog(parent), ui(new Ui::StringInjector), xdbf(xdbf)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->setupUi(this);
}

StringInjector::~StringInjector()
{
    delete ui;
}

void StringInjector::on_comboBox_currentIndexChanged(int index)
{
    ui->lineEdit_2->setEnabled(index == 0);
}

void StringInjector::on_pushButton_clicked()
{
    if (!isAllNumbers(ui->lineEdit_2->text()))
    {
        QMessageBox::warning(this, "Invalid Characters", "Invalid characters in entry id!");
        return;
    }

    xdbf->injectStringEntry(ui->lineEdit->text().toStdWString(), ui->lineEdit_2->text().toULongLong());

    QMessageBox::information(this, "Success", "Successfully injected unicode string entry.");
    close();
}

bool StringInjector::isAllNumbers(QString s)
{
    for (int i = 0; i < s.length(); i++)
        if (!s.at(0).isDigit())
            return false;

    return true;
}

void StringInjector::on_pushButton_2_clicked()
{
    close();
}
