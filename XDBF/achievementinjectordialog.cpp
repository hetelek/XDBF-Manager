#include "achievementinjectordialog.h"
#include "ui_achievementinjectordialog.h"
#include "xdbf.h"

AchievementInjectorDialog::AchievementInjectorDialog(QWidget *parent, XDBF *xdbf) : QDialog(parent), ui(new Ui::AchievementInjectorDialog), xdbf(xdbf)
{
    ui->setupUi(this);
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
    if (!numbersOnly(ui->gamerscoreTxt->text()))
        return;

    Achievement_Entry entry = {0};

    wchar_t *nameTemp = new wchar_t[ui->nameTxt->text().length() + 1];
    int length = ui->nameTxt->text().toWCharArray(nameTemp);
    entry.name = new std::wstring(length, nameTemp);

    wchar_t *lockedDescTemp = new wchar_t[ui->lockedDescTxt->toPlainText().length() + 1];
    ui->lockedDescTxt->toPlainText().toWCharArray(lockedDescTemp);
    entry.lockedDescription = lockedDescTemp;

    wchar_t *unlockedDescTemp = new wchar_t[ui->unlockedDescTxt->toPlainText().length() + 1];
    ui->unlockedDescTxt->toPlainText().toWCharArray(unlockedDescTemp);
    entry.unlockedDescription = unlockedDescTemp;

    entry.gamerscore = ui->gamerscoreTxt->text().toUInt();
    entry.flags |= (ui->comboBox->currentIndex() + 1);
    entry.flags != (ui->secretChbx->checkState() == 2) ? 8 : 0;

    xdbf->injectAchievementEntry(&entry);
}

bool AchievementInjectorDialog::numbersOnly(QString s)
{
    for (int i = 0; i < s.length(); i++)
        if (!s.at(i).isDigit())
            return false;
    return true;
}
