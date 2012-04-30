#include "titleinjectordialog.h"
#include "ui_titleinjectordialog.h"
#include <QMenu>
#include <QMessageBox>
#include <ctype.h>

TitleInjectorDialog::TitleInjectorDialog(QWidget *parent, XDBF *xdbf) : QDialog(parent), ui(new Ui::TitleInjectorDialog), xdbf(xdbf)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

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

    // inject the new title entry
    xdbf->injectTitleEntry(&entry, entry.titleID);

    delete name;

    QMessageBox::information(this, "Success", "Successfully added the new title entry!");
    close();
}

bool TitleInjectorDialog::hexNumbersOnly(QLineEdit *lineEdit)
{
    for (int i = 0; i < lineEdit->text().length(); i++)
        if (isxdigit(lineEdit->text().at(i).toAscii()) == 0)
            return false;
    return true;
}

















