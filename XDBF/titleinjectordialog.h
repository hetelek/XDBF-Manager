#ifndef TITLEINJECTORDIALOG_H
#define TITLEINJECTORDIALOG_H

#include <QDialog>
#include "xdbf.h"

namespace Ui {
class TitleInjectorDialog;
}

class TitleInjectorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TitleInjectorDialog(QWidget *parent = 0, XDBF *xdbf = NULL);
    ~TitleInjectorDialog();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::TitleInjectorDialog *ui;
    XDBF *xdbf;
};

#endif // TITLEINJECTORDIALOG_H
