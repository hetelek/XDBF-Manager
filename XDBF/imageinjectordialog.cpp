#include "imageinjectordialog.h"
#include "ui_imageinjectordialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QBuffer>

ImageInjectorDialog::ImageInjectorDialog(QWidget *parent, XDBF* xdbf) : QDialog(parent), ui(new Ui::ImageInjectorDialog), xdbf(xdbf), imageOpened(false)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    knownIDs = new unsigned long long[2];
    knownIDs[0] = 0x8007;
    knownIDs[1] = 0x8000;

    // add the setting id strings to the combo box
    for (int i = 0; i < 2; i++)
        if (xdbf->get_entry_by_id(knownIDs[i], ET_SETTING) == NULL)
        {
            ui->comboBox->addItem(QString::fromStdString(Entry_ID_to_string(knownIDs[i])));
            cmbxIDs.push_back(knownIDs[i]);
        }
}

ImageInjectorDialog::~ImageInjectorDialog()
{
    delete ui;
}

void ImageInjectorDialog::on_pushButton_clicked()
{
    close();
}

void ImageInjectorDialog::on_pushButton_2_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open PNG Image"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "*.png");

    FileIO file(filePath.toStdString());

    // read the file's magic
    unsigned int magic = file.readUInt32();

    // check for ‰PNG magic
    if (magic != 0x89504E47)
    {
        QMessageBox::warning(this, "Invalid Image", "The file selected was not a PNG file.");
        return;
    }

    file.close();

    image = new QImage(filePath);

    if (image->isNull())
    {
        QMessageBox::critical(this, "Invalid Image", "Error occured while opening the file!");
        return;
    }

    ui->image->setPixmap(QPixmap::fromImage(*image));
    ui->label_2->setText("(" + QString::number(image->width()) + ", " + QString::number(image->height()) + ")");

    imageOpened = true;
}

void ImageInjectorDialog::on_pushButton_3_clicked()
{
    // make sure that there is an image to inject
    if (!imageOpened)
    {
        QMessageBox::warning(this, "Whoops!", "You need to provide an image to inject.");
        return;
    }

    // convert the image to a byte array for writing
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    ui->image->pixmap()->save(&buffer, "PNG");

    // get the id for the entry
    unsigned long long id;
    if (ui->comboBox->currentIndex() == 0)
    {
        // ensure that there are only number characters in the line edit
        if (!isAllNumbers(ui->lineEdit->text()))
        {
            QMessageBox::warning(this, "Invalid Characters", "Invalid Characters in user defined entry ID!");
            return;
        }
        id = ui->lineEdit->text().toLongLong();
    }
    else
        id = cmbxIDs.at(ui->comboBox->currentIndex() - 1);

    // inject the image entry
    xdbf->injectImageEntry(ba.data(), ba.size(), id);

    QMessageBox::information(this, "Success", "Successfully injected image entry.");
    close();
}

bool ImageInjectorDialog::isAllNumbers(QString s)
{
    for (int i = 0; i < s.length(); i++)
        if (!s.at(0).isDigit())
            return false;

    return true;
}

void ImageInjectorDialog::on_comboBox_currentIndexChanged(int index)
{
    ui->lineEdit->setEnabled(index == 0);
}
