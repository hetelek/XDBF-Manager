#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <time.h>
#include "avatarawarddialog.h"
#include "newentrychooser.h"
#include "titleinjectordialog.h"
#include "settinginjectorint.h"
#include "addressconverter.h"
#include "newgpddialog.h"
#include "about.h"

#include <QLabel>

Q_DECLARE_METATYPE(Entry*)

MainWindow::MainWindow(QWidget *parent, string filePath) : QMainWindow(parent), ui(new Ui::MainWindow)
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

    if (filePath != "")
    {
        try
        {
            xdbf = new XDBF(filePath);
            loadEntries();
        }
        catch (...)
        {
            QMessageBox::critical(this, "Error", "An unknown error occured while opening the GPD!");
            return;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// from: http://stackoverflow.com/questions/5298614/change-the-size-of-qt-dialogs-depending-on-the-platform
void MainWindow::adjustAppearanceToOS(QWidget *rootWidget)
{
    int fontSize = -1;
    #ifdef Q_OS_WIN | Q_OS_WIN32
            fontSize = 8;
    #elif Q_OS_MAC
            fontSize = 12;
    #endif

    if (rootWidget == NULL)
        return;

    QObject *child = NULL;
    QObjectList Containers;
    QObject *container  = NULL;
    QStringList DoNotAffect;

    // Make an exception list (Objects not to be affected)
    // DoNotAffect.append("widgetName");

    // Append root to containers
    Containers.append(rootWidget);

    while (!Containers.isEmpty())
    {
        container = Containers.takeFirst();
        if (container != NULL)
            for (int i = 0; i < container->children().size(); i++)
            {
                child = container->children()[i];
                if (!child->isWidgetType() || DoNotAffect.contains(child->objectName()))
                    continue;

                if (child->children().size() > 0)
                    Containers.append(child);
                else
                {
                    // (if the object is not of the correct type, it will be NULL)
                    QLabel *label = qobject_cast<QLabel *>(child);

                    if (label != NULL)
                    {
                        QFont font = label->font();
                        font.setPointSize(fontSize);
                        label->setFont(font);
                    }
                }
            }
    }
}
//

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

    try
    {
        xdbf = new XDBF(name);
        loadEntries();
    }
    catch (...)
    {
        QMessageBox::critical(this, "Error", "An unknown error occured while opening the GPD!");
        return;
    }

    ui->actionAdd_New_Entry->setEnabled(true);
    ui->actionExtract_All->setEnabled(true);
    ui->actionClean_GPD->setEnabled(true);
    ui->actionAddress_Converter->setEnabled(true);
    ui->actionClose->setEnabled(true);

    return;
}

void MainWindow::clear_items()
{
    ui->tableWidget->setSortingEnabled(false);

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
    ui->actionClose->setEnabled(false);

    xdbf->close();
    xdbf = NULL;
    clear_items();

    setWindowTitle("XDBF Manager - ALPHA BUID");
}

void MainWindow::loadEntries()
{
    try
    {
        clear_items();

        Entry *entries = xdbf->getEntries();
        if (entries == NULL)
            return;
        Header *header = xdbf->getHeader();

        clear_items();

        for(unsigned int i = 0; i < header->entryCount; i++)
        {
            QString name = QString::fromStdString(EntryIDToString(entries[i].identifier));
            QTableWidgetItem *item = new QTableWidgetItem((name == "") ? "0x" + QString::number(entries[i].identifier, 16).toUpper() : name);
            item->setData(ObjectRole, QVariant::fromValue(&entries[i]));

            ui->tableWidget->insertRow(i);
            ui->tableWidget->setItem(i, 0, item);
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem("0x" + QString::number(entries[i].length, 16).toUpper()));
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem("0x" + QString::number(entries[i].address, 16).toUpper()));

            QString setting_entry_name = "";
            if(entries[i].type == ET_SETTING && entries[i].identifier != SYNC_LIST && entries[i].identifier != SYNC_DATA)
            {
                Setting_Entry *entry = xdbf->getSettingEntry(&entries[i]);
                setting_entry_name = " - " + QString::fromStdString(xdbf->getSettingEntryName(entry));
            }
            else if (entries[i].identifier == 0x8000 && entries[i].type == ET_STRING)
            {
                std::wstring titleName = xdbf->getStringEntry(&entries[i]);
                setWindowTitle("XDBF Manager - ALPHA BUILD - " + QString::fromWCharArray(titleName.c_str()));
            }
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(friendlyNames[entries[i].type - 1] + setting_entry_name));
        }
        ui->tableWidget->setSortingEnabled(true);
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
            Sync_List sl = xdbf->getSyncList(e->type, e->identifier);
            SyncListDialog dialog(this, &sl, xdbf);
            dialog.exec();
        }
        else
        {
            Sync_Data data = xdbf->getSyncData(e->type, e->identifier);
            QMessageBox::about(this, "Sync Data", "<html><center><h3>Sync Data</h3></center><hr /><br /><b>Next Sync Value: </b>" + QString::number(data.nextSyncId) +
                               "<br /><b>Last Sync Value: </b>" + QString::number(data.lastSyncId) + "<br /><b>Last Synced Time: </b>" +
                               QString::fromStdString(XDBF::FILETIME_to_string(&data.lastSyncedTime)) + "</html>");
        }
        return;
    }
    else if (e->type == ET_SETTING)
    {
        Setting_Entry *se = xdbf->getSettingEntry(e);
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
        std::wstring str = xdbf->getStringEntry(e);
        showStringMessageBox(str.c_str(), "<html><center><h3>String Entry</h3></center><hr /><br />", "String Entry");
    }
    else if (e->type == ET_IMAGE)
    {
        unsigned char *img_data = (unsigned char*)xdbf->extractEntry(e);
        QImage img = QImage::fromData(img_data, e->length);
        ImageDialog im(this, &img);
        im.exec();
    }
    else if (e->type == ET_TITLE)
    {
        Title_Entry *tent = xdbf->getTitleEntry(e);
        if (tent == NULL)
            return;
        TitleInjectorDialog dialog(this, xdbf, tent);

        dialog.exec();
    }
    else if (e->type == ET_ACHIEVEMENT)
    {
        Achievement_Entry *chiev = xdbf->getAchievementEntry(e);
        Entry *entries = xdbf->getEntries();
        Entry *imageEntry = NULL;

        for (int i = 0; i < xdbf->getHeader()->entryCount; i++)
            if(entries[i].type == ET_IMAGE && entries[i].identifier == chiev->imageID )
                imageEntry = &entries[i];

        AchievementViewer *dialog;
        if (imageEntry != NULL)
            dialog = new AchievementViewer(this, chiev, xdbf->getFile(), QImage::fromData((unsigned char*)xdbf->extractEntry(imageEntry), imageEntry->length), e->address);
        else
            dialog = new AchievementViewer(this, chiev, xdbf->getFile(), QImage(":/images/HiddenAchievement.png"), e->address);

        dialog->exec();
    }
    else if (e->type == ET_AVATAR_AWARD)
    {
        Avatar_Award_Entry *award = xdbf->getAvatarAwardEntry(e);
        AvatarAwardDialog dialog(this, award, xdbf);
        dialog.exec();
    }
}

void MainWindow::showStringMessageBox(const wchar_t *wStr, QString message_header, QString title)
{
    int strLen = QString::fromWCharArray(wStr).length();
    QString uni_str = "<b>Unicode String:</b> " + ((strLen == 0) ? "<i>Null</i>" : QString::fromWCharArray(wStr));
    QString uni_len = "<b>String Length:</b> " + QString::number(strLen) + "</html>";

    QMessageBox::about(this, title, message_header + uni_str + "<br />" + uni_len);
}

void MainWindow::on_actionAdd_New_Entry_triggered()
{
    NewEntryChooser entryChooser(this, xdbf);
    entryChooser.exec();

    loadEntries();
}

void MainWindow::on_actionExtract_All_triggered()
{
    QList<QTableWidgetItem*> items;
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
        items.push_back(ui->tableWidget->item(i, 0));
    for (int x = 0; x < ui->tableWidget->rowCount() * 3; x++)
        items.push_back(NULL);

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
            char *data = xdbf->extractEntry(e);
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
    QIcon icon, icon2;
    icon.addPixmap(QPixmap::fromImage(QImage(":/images/extract.png")));
    contextMenu.addAction(icon, "Extract Selected");

    icon2.addPixmap(QPixmap::fromImage(QImage(":/images/cancel.png")));
    contextMenu.addAction(icon2, "Remove Selected");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    try
    {
        if(selectedItem->text() == "Remove Selected")
        {
            QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();

            for(int i = 0; i < items.count() / 4; i++)
            {
                Entry *e = items[i]->data(ObjectRole).value<Entry*>();
                xdbf->removeEntry(e);
                ui->tableWidget->removeRow(items[i]->row());
            }
        }
        else if(selectedItem->text() == "Extract Selected")
        {
            extractFiles(ui->tableWidget->selectedItems());
        }
    }
    catch(char *str)
    {
        QMessageBox::warning(this, "Error Thrown", QString::fromLocal8Bit(str));
    }
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
    bool a = false;
    NewGpdDialog dialog(this, &xdbf, &a);
    dialog.exec();

    if (!a)
        return;

    loadEntries();
    ui->actionAdd_New_Entry->setEnabled(true);
    ui->actionExtract_All->setEnabled(true);
    ui->actionClean_GPD->setEnabled(true);
    ui->actionAddress_Converter->setEnabled(true);
    ui->actionClose->setEnabled(true);
}

void MainWindow::on_actionAbout_triggered()
{
    About dialog(this);
    dialog.exec();
}
