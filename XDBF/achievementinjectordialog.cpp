#include "achievementinjectordialog.h"
#include "ui_achievementinjectordialog.h"
#include "xdbf.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QBuffer>

AchievementInjectorDialog::AchievementInjectorDialog(QWidget *parent, XDBF *xdbf) : QDialog(parent), ui(new Ui::AchievementInjectorDialog), xdbf(xdbf)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    setFixedSize(sizeHint());
}

AchievementInjectorDialog::~AchievementInjectorDialog()
{
    delete ui;
}

void AchievementInjectorDialog::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
             ui->typeDescLbl->setText("The player has completed a game");
             break;
        case 1:
            ui->typeDescLbl->setText("The player has increased their level");
            break;
        case 2:
            ui->typeDescLbl->setText("The player has unlocked a new game feature");
            break;
        case 3:
            ui->typeDescLbl->setText("The player has completed a special goal in the game");
            break;
        case 4:
            ui->typeDescLbl->setText("The player has received an award in a tournament-level event");
            break;
        case 5:
            ui->typeDescLbl->setText("The player has reached a certain point or completed a specific portion of the game");
            break;
        case 6:
            ui->typeDescLbl->setText("Other achievement type");
            break;
    }
}

void AchievementInjectorDialog::on_pushButton_clicked()
{
    Achievement_Entry entry = {0};

    // set the entry's name, i have to make a copy of the text so that the internal function can
    // can reverse the endian, assuming the user's machine is little endian architecture
    wchar_t *nameTemp = new wchar_t[ui->nameTxt->text().length() + 1];
    entry.name = new std::wstring(nameTemp);
    ui->nameTxt->text().toWCharArray(nameTemp);
    nameTemp[ui->nameTxt->text().length()] = 0;
    entry.name = new std::wstring(nameTemp);

    // set the entry's locked description
    wchar_t *lockedDescTemp = new wchar_t[ui->lockedDescTxt->toPlainText().length() + 1];
    lockedDescTemp[ui->lockedDescTxt->toPlainText().length()] = 0;
    ui->lockedDescTxt->toPlainText().toWCharArray(lockedDescTemp);
    entry.lockedDescription = new std::wstring(lockedDescTemp);

    // set the entry's unlocked description
    wchar_t *unlockedDescTemp = new wchar_t[ui->unlockedDescTxt->toPlainText().length() + 1];
    unlockedDescTemp[ui->unlockedDescTxt->toPlainText().length()] = 0;
    ui->unlockedDescTxt->toPlainText().toWCharArray(unlockedDescTemp);
    entry.unlockedDescription = new std::wstring(unlockedDescTemp);

    // set the flags, inlcudes the type and whether or not the achievement is secret
    entry.gamerscore = ui->gamerscoreSpin->value();
    entry.flags |= (ui->comboBox->currentIndex() + 1);
    entry.flags |= (ui->secretChbx->checkState() == 2) ? 8 : 0;

    unsigned long long imageID = 0;

    // get the achievement image, if there is one
    if (ui->imageChbx->checkState() == 2)
    {
        // convert the image to a byte array for writing
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        ui->chievImg->pixmap()->save(&buffer, "PNG");

        // set the imageID for the achievement to be the same as the id of the image we inject
        imageID = xdbf->getNextId(ET_IMAGE);
        entry.imageID = imageID;

        // inject the new image entry
        xdbf->injectImageEntry(ba.data(), ba.length(), imageID);
    }

    entry.unlockedTime.dwHighDateTime = 0x0E434800;
    entry.unlockedTime.dwLowDateTime = 0x01BF5415;

    // inject the achievement entry
    xdbf->injectAchievementEntry(&entry);

    QMessageBox::information(this, "Success", "Successfully added achievement entry.");
    close();
}

void AchievementInjectorDialog::on_imageChbx_stateChanged(int arg1)
{
    ui->openImageBtn->setEnabled(arg1 >> 1);
    if (arg1 == 0)
        ui->chievImg->setText("<center style=\"font-size:7pt\">Image<br />(64x64)</center>");
    else if(image != QImage())
        ui->chievImg->setPixmap(QPixmap::fromImage(image));
}

void AchievementInjectorDialog::on_openImageBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "*.png");
    image.load(fileName);

    // make sure the user selected a file
    if (fileName == "")
        return;
    // ensure that the image selected is 64x64
    else if (image.height() != 64 && image.width() != 64)
    {
        QMessageBox::warning(this, "Wrong Dimensions", "The image must be 64x64!");
        return;
    }
    ui->chievImg->setPixmap(QPixmap::fromImage(image));
}
