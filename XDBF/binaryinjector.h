#ifndef BINARYINJECTOR_H
#define BINARYINJECTOR_H

#include <QDialog>
#include <xdbf.h>

namespace Ui {
class BinaryInjector;
}

class BinaryInjector : public QDialog
{
    Q_OBJECT
    
public:
    explicit BinaryInjector(XDBF *xdbf_pv, QWidget *parent = 0);
    ~BinaryInjector();
    
private slots:

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::BinaryInjector *ui;
    bool isAllNumbers(QString s);
    XDBF *xdbf;
    QString path;
    unsigned long long *knownIDs;

};

#endif // BINARYINJECTOR_H
