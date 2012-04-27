#include "imagedialog.h"
#include "ui_imagedialog.h"

ImageDialog::ImageDialog(QWidget *parent, QImage *img_) : QDialog(parent), ui(new Ui::ImageDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    img = img_;
    ui->label->setPixmap(QPixmap::fromImage(*img));
    ui->label->setMaximumSize(img->size());

    if(ui->label->width() < minimumWidth())
        ui->label->setGeometry((width() / 2) - (ui->label->width() / 2), ui->label->geometry().y(), img->width(), img->height());

    ui->label_2->setText("(" + QString::number(img->width()) + ", " + QString::number(img->height()) + ")");

    this->setFixedSize(this->sizeHint());
}

ImageDialog::~ImageDialog()
{
    delete ui;
}

void ImageDialog::on_pushButton_clicked()
{
    QString save_path = QFileDialog::getSaveFileName(this, QString(), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "PNG Image (*.png)");
    if(save_path == "")
        return;

    if(img->save(save_path))
        QMessageBox::information(this, "Saved Successfully", "The image has been saved successfully!", QMessageBox::Ok);
    else
        QMessageBox::warning(this, "Save Failed", "The image could not be saved (reason unknown).", QMessageBox::Ok);
}

void ImageDialog::on_pushButton_2_clicked()
{
    close();
}
