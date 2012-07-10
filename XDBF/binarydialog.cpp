#include "binarydialog.h"
#include "ui_binarydialog.h"

#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

BinaryDialog::BinaryDialog(QWidget *parent, Setting_Entry *entry) : QDialog(parent), ui(new Ui::BinaryDialog)
{
    se = entry;

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

    QStringList columnHeaders;
    for(int i = 0; i < 0x10; i++)
        columnHeaders.push_back(QString::number(i, 16).toUpper());

    ui->tableWidget->setColumnCount(columnHeaders.size());
    ui->tableWidget->setHorizontalHeaderLabels(columnHeaders);

    int l1 = (entry->binary.size < 0x10) ? 1 : entry->binary.size / 0x10;
    for(int i = 0; i < l1; i++)
    {
        for(int x = 0; x < 0x10; x++)
        {
            if(x == 0)
            {
                ui->tableWidget->insertRow(i);

                QString hexValueStr = QString::number(i, 16);
                if(hexValueStr.length() < 2)
                    hexValueStr = hexValueStr.insert(0, "0");
                ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(hexValueStr.toUpper()));
            }

            int index = i + (i * 0x10) + x;

            if(index + 1 > entry->binary.size)
            {
                i = l1;
                break;
            }

            QString hexValueStr = QString::number((unsigned char)entry->binary.data[index], 16).toUpper();
            if(hexValueStr.length() < 2)
                hexValueStr = hexValueStr.insert(0, "0");
            ui->tableWidget->setItem(i, x, new QTableWidgetItem(hexValueStr));
        }
    }
}

BinaryDialog::~BinaryDialog()
{
    delete ui;
}

void BinaryDialog::on_pushButton_clicked()
{
    QString desktopLocation = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    QString saveLoc = QFileDialog::getSaveFileName(this, "Select a Save Path", desktopLocation);

    if(saveLoc == "")
        return;

    QByteArray ba = saveLoc.toAscii();
    char *path_c = ba.data();

    FILE *f;
    try
    {
        f = fopen(path_c, "wb");
        fwrite(se->binary.data, se->binary.size, sizeof(char), f);
        fclose(f);
    }
    catch(...)
    {
        QMessageBox::warning(this, "Error Occured", "The data could not be written to the disk.");
    }
}
