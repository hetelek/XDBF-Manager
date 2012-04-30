#ifndef SETTINGINJECTORINT_H
#define SETTINGINJECTORINT_H

#include <QDialog>
#include "xdbf.h"

namespace Ui {
class SettingInjectorInt;
}

class SettingInjectorInt : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingInjectorInt(QWidget *parent = 0, XDBF *xdbf = NULL);
    ~SettingInjectorInt();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::SettingInjectorInt *ui;
    XDBF *xdbf;
};

#endif // SETTINGINJECTORINT_H
