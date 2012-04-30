#include "titleinjectordialog.h"
#include "ui_titleinjectordialog.h"
#include <QMenu>
#include <QMessageBox>
#include <ctype.h>
#include <QDateTime>

TitleInjectorDialog::TitleInjectorDialog(QWidget *parent, XDBF *xdbf, Title_Entry *tentry) : QDialog(parent), ui(new Ui::TitleInjectorDialog), xdbf(xdbf), tentry(tentry)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    // if the caller passes in a title entry, then we'll display the entry's data
    if (tentry != NULL)
    {
        setWindowTitle("Title Entry Editor");
        ui->gameNameTxt->setText(QString::fromStdWString(*tentry->gameName));
        ui->chievCount->setValue(tentry->achievementCount);
        ui->chievsUnlocked->setValue(tentry->achievementUnlockedCount);
        ui->chievsUnlockedOnline->setValue(tentry->achievementsUnlockedOnlineCount);
        ui->gamerscore->setValue(tentry->totalGamerscore);
        ui->gamerscoreUnlocked->setValue(tentry->gamerscoreUnlocked);
        ui->aaUnlocked->setValue(tentry->avatarAwardsEarned);
        ui->aaCount->setValue(tentry->avatarAwardCount);
        ui->aaMaleUnlocked->setValue(tentry->maleAvatarAwardsEarned);
        ui->aaMaleCount->setValue(tentry->maleAvatarAwardCount);
        ui->aaFemaleUnlocked->setValue(tentry->femaleAvatarAwardsEarned);
        ui->aaFemaleCount->setValue(tentry->femaleAvaterAwardCount);
        ui->dateTimeEdit->setDateTime(QDateTime::fromTime_t(FILETIME_to_time_t(&tentry->lastPlayed)));
        ui->titleID->setText(QString::number(tentry->titleID, 16).toUpper());

        // add the flags that are set to the list widget
        if (tentry->flags & ACHIEVMENT_NOT_SYNCED)
        {
            ui->comboBox->removeItem(0);
            ui->listWidget->addItem("Achievement Not Synced");
        }
        if (tentry->flags & DOWNLOAD_ACHIEVEMENT_IMAGE)
        {
            ui->comboBox->removeItem(1);
            ui->listWidget->addItem("Download Achievment Image");
        }
        if (tentry->flags & DOWNLOAD_AVATAR_AWARD)
        {
            ui->comboBox->removeItem(2);
            ui->listWidget->addItem("Download Avatar Award");
        }
        if (tentry->flags & AVATAR_AWARD_NOT_SYNCED)
        {
            ui->comboBox->removeItem(3);
            ui->listWidget->addItem("Avatar Award Not Synced");
        }

        // change the button's text to save, because if they provided an entry, then we're
        // going to save it, not inject the changes as a new entry
        ui->pushButton_2->setText("Save");
        QIcon icon;
        icon.addPixmap(QPixmap::fromImage(QImage(":/images/save.png")));
        ui->pushButton_2->setIcon(icon);
    }

    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu(QPoint)));
}

TitleInjectorDialog::~TitleInjectorDialog()
{
    delete ui;
}

void TitleInjectorDialog::on_pushButton_clicked()
{
    // make sure that there are still flags to add
    if (ui->comboBox->currentText() == "")
        return;

    ui->listWidget->addItem(ui->comboBox->currentText());
    ui->comboBox->removeItem(ui->comboBox->currentIndex());
}

void TitleInjectorDialog::showRemoveContextMenu(const QPoint& pos)
{
    if (ui->listWidget->selectedItems().length() == 0)
        return;

    QPoint globalPos = ui->listWidget->mapToGlobal(pos);

    QMenu contextMenu;
    QIcon icon;
    icon.addPixmap(QPixmap::fromImage(QImage(":/images/cancel.png")));
    contextMenu.addAction(icon, "Remove Flag");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if (selectedItem)
    {
        QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->currentRow());
        ui->comboBox->addItem(item->text());
        delete item;
    }
}

void TitleInjectorDialog::on_pushButton_2_clicked()
{
    // enusre that all the information supplied is valid
    if (ui->gameNameTxt->text() == "")
    {
        QMessageBox::warning(this, "Missing Information", "You must supply a game name!");
        return;
    }
    else if (ui->titleID->text().length() != 8)
    {
        QMessageBox::warning(this, "Missing Information", "The Title ID must be exactly 8 hexadecimal digits in length!");
        return;
    }
    else if (!hexNumbersOnly(ui->titleID))
    {
        QMessageBox::warning(this, "Invalid Characters", "Invalid characters in TitleID.");
        return;
    }

    // set most of the fields in the new title entry
    Title_Entry entry =
    {
        NULL,
        ui->titleID->text().toInt(0, 16),
        ui->chievCount->value(),
        ui->chievsUnlocked->value(),
        ui->gamerscore->value(),
        ui->gamerscoreUnlocked->value(),
        0,
        ui->chievsUnlockedOnline->value(),
        ui->aaUnlocked->value(),
        ui->aaCount->value(),
        ui->aaMaleUnlocked->value(),
        ui->aaMaleCount->value(),
        ui->aaFemaleUnlocked->value(),
        ui->aaFemaleCount->value(),
    };

    // set the last played field
    entry.lastPlayed = time_t_to_FILETIME(ui->dateTimeEdit->dateTime().toTime_t());

    // set the game name field
    wstring *name = new wstring(ui->gameNameTxt->text().toStdWString());
    entry.gameName = name;

    // set all the flags in the list widget
    entry.flags = 0;
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        if (ui->listWidget->item(i)->text() == "Achievement Not Synced")
            entry.flags |= ACHIEVMENT_NOT_SYNCED;
        else if (ui->listWidget->item(i)->text() == "Download Achievment Image")
            entry.flags |= DOWNLOAD_ACHIEVEMENT_IMAGE;
        else if (ui->listWidget->item(i)->text() == "Download Avatar Award")
            entry.flags |= DOWNLOAD_AVATAR_AWARD;
        else if (ui->listWidget->item(i)->text() == "Avatar Award Not Synced")
            entry.flags |= AVATAR_AWARD_NOT_SYNCED;
    }

    if (tentry == NULL)
    {
        // inject the new title entry
        xdbf->injectTitleEntry(&entry, entry.titleID);

        QMessageBox::information(this, "Success", "Successfully added the new title entry!");
    }
    else
    {
         entry.entry = tentry->entry;
         xdbf->writeEntry(&entry);
         QMessageBox::information(this, "Success", "Successfully saved the title entry!");
    }

    delete name;
    close();
}

bool TitleInjectorDialog::hexNumbersOnly(QLineEdit *lineEdit)
{
    for (int i = 0; i < lineEdit->text().length(); i++)
        if (isxdigit(lineEdit->text().at(i).toAscii()) == 0)
            return false;
    return true;
}

















