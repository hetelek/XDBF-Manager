#include "titleentrydialog.h"
#include "ui_titleentrydialog.h"

TitleEntryDialog::TitleEntryDialog(QWidget *parent, Title_Entry *te) : QDialog(parent), ui(new Ui::TitleEntryDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->nameLbl->setText("<b>Name: </b>" + QString::fromWCharArray(te->gameName));
    ui->titleIDLbl->setText("<b>Title ID: </b>" + QString::number(te->titleID, 16).toUpper());
    ui->gamerscoreLbl->setText("<b>Gamerscore: </b>" + QString::number(te->gamerscoreUnlocked) + " unlocked out of " + QString::number(te->totalGamerscore));
    ui->achievementsLbl->setText("<b>Achievements: </b>" + QString::number(te->achievementCount) + " unlocked out of " + QString::number(te->achievementCount));
    ui->avatarAwardsLbl->setText("<b>Avatar Awards: </b>" + QString::number(te->avatarAwardsEarned) + " unlocked out of " + QString::number(te->avatarAwardCount));

    QStringList flags;
    if(te->flags & ACHIEVMENT_NOT_SYNCED)
        flags.append(QString::fromStdString(te_flag_to_string(ACHIEVMENT_NOT_SYNCED)));
    else if(te->flags & DOWNLOAD_ACHIEVEMENT_IMAGE)
        flags.append(QString::fromStdString(te_flag_to_string(DOWNLOAD_ACHIEVEMENT_IMAGE)));
    else if(te->flags & DOWNLOAD_AVATAR_AWARD)
        flags.append(QString::fromStdString(te_flag_to_string(DOWNLOAD_AVATAR_AWARD)));

    QString flags_str = flags.join(", ");
    if(flags_str == "")
        flags_str = "<i>None</i>";

    ui->flagsLbl->setText("<b>Flags: </b>" + flags_str);
    ui->lastPlayedLbl->setText("<b>Last Played: </b>" + QString::fromStdString(XDBF::FILETIME_to_string(&te->lastPlayed)));

    QString url = "http://download.xbox.com/content/images/66acd000-77fe-1000-9115-d802" + QString::number(te->titleID, 16) + "/1033/boxartsm.jpg";
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(url)));
}

void TitleEntryDialog::replyFinished(QNetworkReply* aReply)
{
    QByteArray boxArt = aReply->readAll();
    if(boxArt.size() != 0)
        ui->boxArtImg->setPixmap(QPixmap::fromImage(QImage::fromData(boxArt)));
    else
    {
        ui->boxArtImg->setText("<i>Unable to download image.</i>");
    }
}

TitleEntryDialog::~TitleEntryDialog()
{
    delete ui;
}
