#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

namespace Ui {
class ImageDialog;
}

class ImageDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ImageDialog(QWidget *parent = 0, QImage *img_ = 0);
    ~ImageDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ImageDialog *ui;
    QImage *img;
};

#endif // IMAGEDIALOG_H
