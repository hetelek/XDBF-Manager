#include "synclistdialog.h"
#include "ui_synclistdialog.h"
#include "mainwindow.h"

Q_DECLARE_METATYPE(Sync_Entry*)

SyncListDialog::SyncListDialog(QWidget *parent, Sync_List *_list, XDBF *_xdbf) : xdbf(_xdbf), QDialog(parent), ui(new Ui::SyncListDialog), list(_list)
{
    ObjectRole = Qt::UserRole + 1;
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    MainWindow::adjustAppearanceToOS(this);

    for(int i = 0; i < list->entryCount; i++)
    {
        QString name = QString::fromStdString(EntryIDToString(list->entries->at(i).identifier));
        QTableWidgetItem *lwi = new QTableWidgetItem((name == "") ?  "0x" + QString::number(list->entries->at(i).identifier, 16).toUpper() : name);

        lwi->setData(ObjectRole, QVariant::fromValue(&list->entries->at(i)));
        if(list->entries->at(i).syncId != 0)
        {
            int row = ui->tableWidget_queueList->rowCount();
            ui->tableWidget_queueList->insertRow(row);
            ui->tableWidget_queueList->setItem(row, 0, lwi);
        }
        else
        {
            int row = ui->tableWidget_syncList->rowCount();
            ui->tableWidget_syncList->insertRow(row);
            ui->tableWidget_syncList->setItem(row, 0, lwi);
        }
    }
}

SyncListDialog::~SyncListDialog()
{
    delete ui;
}

void SyncListDialog::on_pushButton_clicked()
{
    moveItem(ui->tableWidget_syncList, ui->tableWidget_queueList);
}

void SyncListDialog::on_pushButton_2_clicked()
{
    moveItem(ui->tableWidget_queueList, ui->tableWidget_syncList);
}

void SyncListDialog::moveItem(QTableWidget *from, QTableWidget *to)
{
    QList<QTableWidgetItem*> selectedItems = from->selectedItems();
    if(selectedItems.count() < 1)
        return;

    for(int i = 0; i < selectedItems.count(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(*selectedItems[i]);

        int row = to->rowCount();
        to->insertRow(row);
        to->setItem(row, 0, item);

        from->removeRow(selectedItems[i]->row());
    }
}

void SyncListDialog::on_pushButton_3_clicked()
{
    int qlRowCount = ui->tableWidget_queueList->rowCount();
    for(int i = 0; i < qlRowCount; i++)
    {
        QTableWidgetItem *item = ui->tableWidget_queueList->item(i, 0);
        Sync_Entry *e = item->data(ObjectRole).value<Sync_Entry*>();
        e->syncId = qlRowCount - i;
    }

    for(int i = 0; i < ui->tableWidget_syncList->rowCount(); i++)
    {
        QTableWidgetItem *item = ui->tableWidget_syncList->item(i, 0);
        Sync_Entry *e = item->data(ObjectRole).value<Sync_Entry*>();
        e->syncId = 0;
    }

    xdbf->writeSyncList(list);

    QMessageBox::information(this, "Updated Successfully", "The sync list has been updated successfully!", QMessageBox::Ok);
    close();
}

void SyncListDialog::on_pushButton_4_clicked()
{
    close();
}
