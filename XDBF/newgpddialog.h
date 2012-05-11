#ifndef NEWGPDDIALOG_H
#define NEWGPDDIALOG_H

#include <QDialog>
#include "xdbf.h"

namespace Ui {
class NewGpdDialog;
}

class NewGpdDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewGpdDialog(QWidget *parent = 0, XDBF **xdbf = NULL);
    ~NewGpdDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::NewGpdDialog *ui;
    XDBF **xdbf;
};

#endif // NEWGPDDIALOG_H
