#include "achievementviewer.h"
#include "ui_achievementviewer.h"

AchievementViewer::AchievementViewer(QWidget *parent, Achievement_Entry *chiev, FileIO *f, QImage image, long address)
    : QDialog(parent), ui(new Ui::AchievementViewer), img(image), entry(chiev), entryAddr(address)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    opened_file = f;

    ui->gamerscoreLbl->setText("<b>Gamerscore: </b>" + QString::number(chiev->gamerscore));
    ui->nameLbl->setText("<b>Name: </b>" + QString::fromWCharArray(chiev->name->c_str()));
    if (chiev->flags & Completion)
        ui->typeLbl->setText("<b>Type: </b>Completion");
    else if (chiev->flags & Leveling)
        ui->typeLbl->setText("<b>Type: </b>Leveling");
    else if (chiev->flags & Unlock)
        ui->typeLbl->setText("<b>Type: </b>Unlock");
    else if (chiev->flags & Event)
        ui->typeLbl->setText("<b>Type: </b>Event");
    else if (chiev->flags & Tournament)
        ui->typeLbl->setText("<b>Type: </b>Tournament");
    else if (chiev->flags & Checkpoint)
        ui->typeLbl->setText("<b>Type: </b>Checkpoint");
    else if (chiev->flags & Other)
        ui->typeLbl->setText("<b>Type: </b>Other");

    QString lockedQ = QString::fromWCharArray(chiev->lockedDescription->c_str());
    QString unlockedQ = QString::fromWCharArray(chiev->unlockedDescription->c_str());

    ui->lockedDescLbl->setText("<b>Locked Description: </b>" + lockedQ);
    ui->unlockedDescLbl->setText("<b>Unlocked Description: </b>" + unlockedQ);

    QDateTime unlockedTime = QDateTime::fromTime_t(FILETIME_to_time_t(&chiev->unlockedTime));
    ui->unlockedTimeDte->setDateTime(unlockedTime);

    int state = (chiev->flags >> 16) & 3;
    ui->stateCmbx->setCurrentIndex((state == 0) ? 0 : state - 1);
    ui->label_2->setPixmap(QPixmap::fromImage(image));
}

AchievementViewer::~AchievementViewer()
{
    delete ui;
}

void AchievementViewer::on_pushButton_clicked()
{
    QString save_path = QFileDialog::getSaveFileName(this, "", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "PNG Image (*.png)");
    if(save_path == "")
        return;

    if(img.save(save_path))
        QMessageBox::information(this, "Saved Successfully", "The image has been saved successfully!", QMessageBox::Ok);
    else
        QMessageBox::warning(this, "Save Failed", "The image could not be saved (reason unknown).", QMessageBox::Ok);
}
void AchievementViewer::on_pushButton_2_clicked()
{
    unsigned int toWrite = entry->flags & 0xFFFCFFFF;
    toWrite |= (ui->stateCmbx->currentIndex() == 0) ? 0 : ((ui->stateCmbx->currentIndex() + 1) << 16);

    opened_file->setPosition(entryAddr + 0x10);
    opened_file->writeUInt32(toWrite);

    FILETIME time = time_t_to_FILETIME(ui->unlockedTimeDte->dateTime().toTime_t());

    opened_file->setPosition(entryAddr + 0x14);
    opened_file->writeUInt32(time.dwHighDateTime);
    opened_file->writeUInt32(time.dwLowDateTime);

    QMessageBox::information(this, "Success", "Successfully saved changes.");
}
