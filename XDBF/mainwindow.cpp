#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <time.h>
#include "avatarawarddialog.h"
#include "newentrychooser.h"
#include "titleinjectordialog.h"
#include "settinginjectorint.h"
#include "addressconverter.h"
#include "newgpddialog.h"

Q_DECLARE_METATYPE(Entry*)

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    desktop_location = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    ObjectRole = Qt::UserRole + 1;

    ui->setupUi(this);

    friendlyNames[0] = "Achievement";
    friendlyNames[1] = "Image";
    friendlyNames[2] = "Setting";
    friendlyNames[3] = "Title";
    friendlyNames[4] = "String";
    friendlyNames[5] = "Avatar Award";

    QStringList columnHeaders;
    columnHeaders.push_back("ID");
    columnHeaders.push_back("Size");
    columnHeaders.push_back("Offset");
    columnHeaders.push_back("Type");

    ui->tableWidget->setColumnCount(columnHeaders.length());
    ui->tableWidget->setHorizontalHeaderLabels(columnHeaders);

    xdbf = NULL;

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu(QPoint)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), desktop_location, "*.gpd");

    if(file_name == NULL)
        return;

    if(xdbf != NULL)
    {
        xdbf->close();
        xdbf = NULL;
    }

    QByteArray ba = file_name.toAscii();
    name = ba.data();

    xdbf = new XDBF(name);
    loadEntries();

    ui->actionAdd_New_Entry->setEnabled(true);
    ui->actionExtract_All->setEnabled(true);
    ui->actionClean_GPD->setEnabled(true);
    ui->actionAddress_Converter->setEnabled(true);

    return;
}

void MainWindow::clear_items()
{
    for (int i = ui->tableWidget->rowCount() - 1; i >= 0; --i)
        ui->tableWidget->removeRow(i);
}

void MainWindow::on_actionClose_triggered()
{
    if(xdbf == NULL)
        return;

    ui->actionAdd_New_Entry->setEnabled(false);
    ui->actionExtract_All->setEnabled(false);
    ui->actionClean_GPD->setEnabled(false);
    ui->actionAddress_Converter->setEnabled(false);

    xdbf->close();
    xdbf = NULL;
    clear_items();

    setWindowTitle("XDBF Manager");
}

void MainWindow::loadEntries()
{
    try
    {
        clear_items();
        Entry *entries = xdbf->get_entries();
        if (entries == NULL)
            return;
        Header *header = xdbf->get_header();

        clear_items();

        for(unsigned int i = 0; i < header->entry_count; i++)
        {
            QString name = QString::fromStdString(Entry_ID_to_string(entries[i].identifier));
            QTableWidgetItem *item = new QTableWidgetItem((name == "") ? "0x" + QString::number(entries[i].identifier, 16).toUpper() : name);
            item->setData(ObjectRole, QVariant::fromValue(&entries[i]));

            ui->tableWidget->insertRow(i);
            ui->tableWidget->setItem(i, 0, item);
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem("0x" + QString::number(entries[i].length, 16).toUpper()));
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem("0x" + QString::number(entries[i].address, 16).toUpper()));

            QString setting_entry_name = "";
            if(entries[i].type == ET_SETTING && entries[i].identifier != SYNC_LIST && entries[i].identifier != SYNC_DATA)
            {
                Setting_Entry *entry = xdbf->get_setting_entry(&entries[i]);
                setting_entry_name = " - " + QString::fromStdString(xdbf->get_setting_entry_name(entry));
            }
            else if (entries[i].identifier == 0x8000 && entries[i].type == ET_STRING)
            {
                wchar_t *titleName = (wchar_t*)xdbf->extract_entry(&entries[i]);
                SwapEndianUnicode(titleName, entries[i].length);
                setWindowTitle("XDBF Manager - " + QString::fromWCharArray(titleName));
            }
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(friendlyNames[entries[i].type - 1] + setting_entry_name));
        }
    }
    catch(char *exception)
    {
        QMessageBox::warning(this, "Error", exception, QMessageBox::Ok);
    }
}

void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    QTableWidgetItem* item = ui->tableWidget->selectedItems()[0];
    Entry *e = item->data(ObjectRole).value<Entry*>();

    if(e->identifier == SYNC_LIST || e->identifier == SYNC_DATA || ((e->identifier == 1 || e->identifier == 2) && e->type == ET_AVATAR_AWARD))
    {
        if(e->identifier == SYNC_LIST || e->identifier == 1)
        {
            Sync_List sl = xdbf->get_sync_list(e->type, e->identifier);
            SyncListDialog dialog(this, &sl, xdbf);
            dialog.exec();
        }
        else
        {
            Sync_Data data = xdbf->get_sync_data(e->type, e->identifier);
            QMessageBox::about(this, "Sync Data", "<html><center><h3>Sync Data</h3></center><hr /><br /><b>Next Sync Value: </b>" + QString::number(data.next_sync_id) +
                               "<br /><b>Last Sync Value: </b>" + QString::number(data.last_sync_id) + "<br /><b>Last Synced Time: </b>" +
                               QString::fromStdString(XDBF::FILETIME_to_string(&data.last_synced_time)) + "</html>");
        }
        return;
    }
    else if (e->type == ET_SETTING)
    {
        Setting_Entry *se = xdbf->get_setting_entry(e);
        switch (se->type)
        {
            case SET_INT32:
            case SET_INT64:
            case SET_FLOAT:
            case SET_DOUBLE:
            case SET_DATETIME:
            case SET_UNICODE:
            {
                SettingInjectorInt dialog(this, xdbf, se->type, se);
                dialog.exec();
                break;
            }
            case SET_BINARY:
                BinaryDialog bd(this, se);
                bd.exec();
                break;
        }
    }
    else if (e->type == ET_STRING)
    {
        wchar_t *str = (wchar_t*)xdbf->extract_entry(e);
        SwapEndianUnicode(str, e->length);
        showStringMessageBox(str, "<html><center><h3>String Entry</h3></center><hr /><br />", "String Entry");
    }
    else if (e->type == ET_IMAGE)
    {
        unsigned char *img_data = (unsigned char*)xdbf->extract_entry(e);
        QImage img = QImage::fromData(img_data, e->length);
        ImageDialog im(this, &img);
        im.exec();
    }
    else if (e->type == ET_TITLE)
    {
        Title_Entry *tent = xdbf->get_title_entry(e);
        if (tent == NULL)
            return;
        TitleInjectorDialog dialog(this, xdbf, tent);

        dialog.exec();
    }
    else if (e->type == ET_ACHIEVEMENT)
    {
        Achievement_Entry *chiev = xdbf->get_achievement_entry(e);
        Entry *entries = xdbf->get_entries();
        Entry *imageEntry = NULL;

        for (int i = 0; i < xdbf->get_header()->entry_count; i++)
            if(entries[i].type == ET_IMAGE && entries[i].identifier == chiev->imageID )
                imageEntry = &entries[i];

        AchievementViewer *dialog;
        if (imageEntry != NULL)
            dialog = new AchievementViewer(this, chiev, xdbf->get_file(), QImage::fromData((unsigned char*)xdbf->extract_entry(imageEntry), imageEntry->length), e->address);
        else
            dialog = new AchievementViewer(this, chiev, xdbf->get_file(), QImage(":/images/HiddenAchievement.png"), e->address);

        dialog->exec();
    }
    else if (e->type == ET_AVATAR_AWARD)
    {
        Avatar_Award_Entry *award = xdbf->get_avatar_award_entry(e);
        AvatarAwardDialog dialog(this, award, xdbf);
        dialog.exec();
    }
}

void MainWindow::showIntMessageBox(unsigned long long num, QString message_header, QString title)
{
    QString decimal = "<b>Decimal:</b> " + QString::number(num);
    QString hex = "<b>Hexadecimal:</b> 0x" + QString::number(num, 16);
    QString octal = "<b>Octal:</b> 0" + QString::number(num, 8) + "</html>";

    QMessageBox::about(this, title, message_header + decimal + "<br />" + hex + "<br />" + octal);
}

void MainWindow::showFloatMessageBox(double num, QString message_header, QString title)
{
    QString standard = "<b>Standard:</b> " + QString::number(num);
    char nipples[25];
    sprintf(nipples, "%E\0", num);
    QString scientificNotation = "<b>Scientific Notation: </b>" + QString::fromAscii(nipples);
    QMessageBox::about(this, title, message_header + standard + "<br />" + scientificNotation + "</html>");
}

void MainWindow::showStringMessageBox(const wchar_t *wStr, QString message_header, QString title)
{
    int strLen = QString::fromWCharArray(wStr).length();
    QString uni_str = "<b>Unicode String:</b> " + ((strLen == 0) ? "<i>Null</i>" : QString::fromWCharArray(wStr));
    QString uni_len = "<b>String Length:</b> " + QString::number(strLen) + "</html>";

    QMessageBox::about(this, title, message_header + uni_str + "<br />" + uni_len);
}

void MainWindow::showDatetimeMessageBox(FILETIME time, QString message_header, QString title)
{
    QString time_str = "<b>Datetime:</b> " + QString::fromStdString(XDBF::FILETIME_to_string(&time)) + "</html>";
    QMessageBox::about(this, title, message_header + time_str);
}

void MainWindow::on_pushButton_3_clicked()
{
    if(ui->tableWidget->selectedItems().count() < 1)
        return;

    for(int i = 0; i < ui->tableWidget->selectedItems().count(); i++)
    {
        QTableWidgetItem *item = ui->tableWidget->selectedItems()[i];
        Entry *e = item->data(ObjectRole).value<Entry*>();
        xdbf->removeEntry(e);

        ui->tableWidget->removeRow(item->row());
    }
}

void MainWindow::on_actionAdd_New_Entry_triggered()
{
    NewEntryChooser entryChooser(this, xdbf);
    entryChooser.exec();

    loadEntries();
}

void MainWindow::on_actionExtract_All_triggered()
{
    QString s = ui->tableWidget->itemAt(0, 4)->text();

    QList<QTableWidgetItem*> items;
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
        for (int x = 0; x < ui->tableWidget->columnCount(); x++)
            items.push_back(ui->tableWidget->itemAt(x, i));
    extractFiles(items);
}

void MainWindow::extractFiles(QList<QTableWidgetItem*> items)
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select a Directory"), desktop_location) + "\\";

    if(dir == "\\")
        return;

    for(int i = 0; i < items.size() / 4; i++)
    {
        QTableWidgetItem* item = items[i];
        Entry *e = item->data(ObjectRole).value<Entry*>();

        FILE *f;

        QString path = dir + "0x" + QString::number(e->identifier, 16).toUpper() + " - " + friendlyNames[e->type - 1];

        if(QFileInfo(path).exists())
        {
            int adder = 2;
            while(QFileInfo(path + "(" + QString::number(adder) + ")").exists())
                adder++;

            path += "(" + QString::number(adder) + ")";
        }

        QByteArray ba = path.toAscii();
        char *path_c = ba.data();
        f = fopen(path_c, "wb");

        try
        {
            char *data = xdbf->extract_entry(e);
            fwrite(data, e->length, sizeof(char), f);
            fclose(f);
        }
        catch(char *exce)
        {
            QMessageBox::information(this, "Error Occurred", exce, QMessageBox::Ok);
        }
    }

    QMessageBox::information(this, "Extraction Successful", "All selected files have been extracted successfully!", QMessageBox::Ok);
}

void MainWindow::showRemoveContextMenu(const QPoint &pos)
{
    if (ui->tableWidget->selectedItems().length() == 0)
        return;

    QPoint globalPos = ui->tableWidget->mapToGlobal(pos);

    QMenu contextMenu;
    QIcon icon;
    icon.addPixmap(QPixmap::fromImage(QImage(":/images/extract.png")));
    contextMenu.addAction(icon, "Extract Selected");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if (selectedItem)
        extractFiles(ui->tableWidget->selectedItems());
}

void MainWindow::on_actionAddress_Converter_triggered()
{
    AddressConverter converter(this, xdbf);
    converter.exec();
}

void MainWindow::on_actionClean_GPD_triggered()
{
    if (QMessageBox::question(this, "Continue?", "Cleaning the GPD will remove all of the unused memory from the file, making it smaller. Would you like to continue?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        xdbf->cleanGPD();
        QMessageBox::information(this, "Success", "Successfully cleaned the GPD.");
    }
}

void MainWindow::on_actionNew_triggered()
{
    NewGpdDialog dialog(this, &xdbf);
    dialog.exec();

    loadEntries();
}
