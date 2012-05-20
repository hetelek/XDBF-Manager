#ifndef STRINGINJECTOR_H
#define STRINGINJECTOR_H

#include <QDialog>
#include "xdbf.h"

namespace Ui {
class StringInjector;
}

class StringInjector : public QDialog
{
    Q_OBJECT
    
public:
    explicit StringInjector(QWidget *parent = 0, XDBF *xdbf = NULL);
    ~StringInjector();
    
private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::StringInjector *ui;
    XDBF *xdbf;

    bool isAllNumbers(QString s);
};

#endif // STRINGINJECTOR_H
