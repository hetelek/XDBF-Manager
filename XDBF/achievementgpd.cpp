#include "achievementgpd.h"
#include "ui_achievementgpd.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QBuffer>
#include "mainwindow.h"

AchievementGPD::AchievementGPD(QWidget *parent, XDBF **xdbf, bool *b) : QDialog(parent), ui(new Ui::AchievementGPD), xdbf(xdbf), imageOpened(false), b(b)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    MainWindow::adjustAppearanceToOS(this);
}

AchievementGPD::~AchievementGPD()
{
    delete ui;
}

void AchievementGPD::on_pushButton_2_clicked()
{
    close();
}

void AchievementGPD::on_pushButton_clicked()
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

    QImage *image = new QImage(filePath);

    if (image->isNull())
    {
        QMessageBox::critical(this, "Invalid Image", "Error occured while opening the file!");
        return;
    }
    else if (image->height() != 64 && image->width() != 64)
    {
        QMessageBox::warning(this, "Wrong Dimensions", "The image must be 64x64!");
        return;
    }

    ui->image->setPixmap(QPixmap::fromImage(*image));

    imageOpened = true;
}

void AchievementGPD::on_pushButton_3_clicked()
{
    if (!imageOpened)
    {
        QMessageBox::warning(this, "Whoops!", "You forgot to select a game image.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, tr("Where should the new GPD be created?"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "*.gpd");
    if (filePath == "")
        return;

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);

    ui->image->pixmap()->save(&buffer, "PNG");

    wstring str = ui->lineEdit->text().toStdWString();
    *xdbf = XDBFcreate(filePath.toStdString(), Achievement, ba.data(), ba.size(), &str);

    QMessageBox::information(this, "Success", "Successfully created a new achievement GPD.");
    *b = true;

    close();
}
