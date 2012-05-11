#ifndef IMAGEINJECTORDIALOG_H
#define IMAGEINJECTORDIALOG_H

#include <QDialog>
#include "xdbf.h"

namespace Ui {
class ImageInjectorDialog;
}

class ImageInjectorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ImageInjectorDialog(QWidget *parent = 0, XDBF *xdbf = NULL);
    ~ImageInjectorDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    bool isAllNumbers(QString s);

    bool imageOpened;
    Ui::ImageInjectorDialog *ui;
    unsigned long long *knownIDs;
    QImage *image;
    XDBF *xdbf;
    vector<unsigned long long> cmbxIDs;
};

#endif // IMAGEINJECTORDIALOG_H
