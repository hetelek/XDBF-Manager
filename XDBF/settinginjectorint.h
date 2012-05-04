#ifndef SETTINGINJECTORINT_H
#define SETTINGINJECTORINT_H

#include <QDialog>
#include "xdbf.h"
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLineEdit>

namespace Ui {
class SettingInjectorInt;
}

class SettingInjectorInt : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingInjectorInt(QWidget *parent = 0, XDBF *xdbf = NULL, char type = 0);
    ~SettingInjectorInt();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::SettingInjectorInt *ui;
    QSpinBox *spinBox;
    QDoubleSpinBox *dSpinBox;
    QLineEdit *lineEdit;
    unsigned long long *knownIDs;
    bool isAllNumbers(QString s);
    char type;
    XDBF *xdbf;
};

#endif // SETTINGINJECTORINT_H
