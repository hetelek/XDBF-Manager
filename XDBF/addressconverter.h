#ifndef ADDRESSCONVERTER_H
#define ADDRESSCONVERTER_H

#include <QDialog>
#include "xdbf.h"

namespace Ui {
class AddressConverter;
}

class AddressConverter : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddressConverter(QWidget *parent = 0, XDBF *xdbf = NULL);
    ~AddressConverter();
    
private slots:
    void on_pushButton_clicked();

    void on_realSpBx_valueChanged(int arg1);

    void on_specifierSpbx_valueChanged(int arg1);

private:
    Ui::AddressConverter *ui;
    bool realLastChanged;
    XDBF *xdbf;
};

#endif // ADDRESSCONVERTER_H
