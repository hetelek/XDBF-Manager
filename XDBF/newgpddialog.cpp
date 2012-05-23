#include "newgpddialog.h"
#include "ui_newgpddialog.h"
#include "xdbf.h"
#include <QDesktopServices>
#include <QBuffer>
#include <QFileDialog>
#include "achievementgpd.h"
#include <QMessageBox>
#include "mainwindow.h"

NewGpdDialog::NewGpdDialog(QWidget *parent, XDBF **xdbf, bool *b) : QDialog(parent), ui(new Ui::NewGpdDialog), xdbf(xdbf), b(b)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    MainWindow::adjustAppearanceToOS(this);
}

NewGpdDialog::~NewGpdDialog()
{
    delete ui;
}

void NewGpdDialog::on_pushButton_clicked()
{
    switch (ui->comboBox->currentIndex())
    {
        // achievment gpd
        case 0:
        {
            AchievementGPD dialog(this, xdbf, b);
            this->setVisible(false);
            dialog.exec();
            break;
        }

        // dashboard gpd
        case 1:
        {
            QString filePath = QFileDialog::getSaveFileName(this, tr("Where should the new GPD be created?"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "*.gpd");
            if (filePath == "")
                return;

            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);

            QPixmap::fromImage(QImage(":/images/default image.png")).save(&buffer, "PNG");

            *xdbf = XDBFcreate(filePath.toStdString(), Dashboard, ba.data(), ba.size());

            QMessageBox::information(this, "Success", "Successfully created the new dashboard GPD.");
            *b = true;
            break;
        }

        // avatar award gpd
        case 2:
        {
            QString filePath = QFileDialog::getSaveFileName(this, tr("Where should the new GPD be created?"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "*.gpd");
            if (filePath == "")
                return;

            *xdbf = XDBFcreate(filePath.toStdString(), AvatarAward);

            QMessageBox::information(this, "Success", "Successfully created the new avatar award GPD.");
            *b = true;
            break;
        }
    }

    close();
}

void NewGpdDialog::on_pushButton_2_clicked()
{
    close();
}
