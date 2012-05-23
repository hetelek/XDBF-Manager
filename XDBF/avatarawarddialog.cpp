#include "avatarawarddialog.h"
#include "ui_avatarawarddialog.h"
#include "mainwindow.h"

AvatarAwardDialog::AvatarAwardDialog(QWidget *parent, Avatar_Award_Entry *entry, XDBF *xdbf) : xdbf(xdbf), award(entry), QDialog(parent), ui(new Ui::AvatarAwardDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    MainWindow::adjustAppearanceToOS(this);

    ui->nameLbl->setText("<b>Name: </b>" + QString::fromWCharArray(award->name->c_str()));
    ui->clothingTypeLbl->setText("<b>Clothing Type: </b>" + QString::fromStdString(getClothingType(award)));
    ui->titleIdLbl->setText("<b>Title ID: </b>" + QString::number(award->titleID, 16).toUpper());
    ui->lockedDescLbl->setText("<b>Locked Description:</b> " + QString::fromWCharArray(award->lockedDescription->c_str()));
    ui->unlockedDescLbl->setText("<b>Unlocked Description:</b> " + QString::fromWCharArray(award->unlockedDescription->c_str()));

    if (GENDER_FROM_FLAGS(award->flags64) == male)
        ui->genderLbl->setText("<b>Gender: </b>Male");
    else if (GENDER_FROM_FLAGS(award->flags64) == female)
        ui->genderLbl->setText("<b>Gender: </b>Female");
    else if (GENDER_FROM_FLAGS(award->flags64) == both)
        ui->genderLbl->setText("<b>Gender: </b>Both");

    ui->secretLbl->setText((award->flags32 & Secret) ? "<b>Secret: </b>True" : "<b>Secret: </b>False");

    int state = (award->flags32 >> 16) & 3;
    ui->stateCbx->setCurrentIndex((state == 0) ? 0 : state - 1);

    QDateTime unlockedTime = QDateTime::fromTime_t(FILETIMEToTime_t(&award->unlockTime));
    ui->unlockedTimeDte->setDateTime(unlockedTime);

    QString url = "http://avatar.xboxlive.com/global/t." + QString::number(award->titleID, 16) + "/avataritem/" + QString::fromStdString(guid(award)) + "/128";
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(url)));

    setFixedSize(sizeHint());
}

AvatarAwardDialog::~AvatarAwardDialog()
{
    delete ui;
}

void AvatarAwardDialog::replyFinished(QNetworkReply *aReply)
{
    QByteArray awardImage = aReply->readAll();
    if(awardImage.size() != 0)
    {
        ui->awardImg->setPixmap(QPixmap::fromImage(QImage::fromData(awardImage)));
    }
    else
    {
        ui->awardImg->setText("<i>Unable to download image.</i>");
    }
}

void AvatarAwardDialog::on_saveBtn_clicked()
{
    award->flags32 &= 0xFFFCFFFF;
    int index = ui->stateCbx->currentIndex();
    award->flags32 |= (index == 0) ? 0 : ((index + 1) << 16);

    award->unlockTime = time_tToFILETIME(ui->unlockedTimeDte->dateTime().toTime_t());

    xdbf->writeEntry(award);
    QMessageBox::information(this, "Success", "Successfully saved changes.");
}

void AvatarAwardDialog::on_cancelBtn_clicked()
{
    close();
}
