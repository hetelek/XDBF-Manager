#ifndef SETTINGINJECTORINT_H
#define SETTINGINJECTORINT_H

#include <QDialog>
#include "xdbf.h"
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLineEdit>
#include <QtGui/QDateTimeEdit>

namespace Ui {
class SettingInjectorInt;
}

class SettingInjectorInt : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingInjectorInt(QWidget *parent = 0, XDBF *xdbf = NULL, char type = 0, Setting_Entry *entry = NULL);
    ~SettingInjectorInt();
    
private slots:
    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_2_clicked();

private:
    Ui::SettingInjectorInt *ui;
    QSpinBox *spinBox;
    QDoubleSpinBox *dSpinBox;
    QLineEdit *lineEditP;
    QDateTimeEdit *dte;
    Setting_Entry *entry;
    unsigned long long *knownIDs;
    vector<unsigned long long> cmbxIDs;
    bool isAllNumbers(QString s);
    char type;
    XDBF *xdbf;
};

#endif // SETTINGINJECTORINT_H
