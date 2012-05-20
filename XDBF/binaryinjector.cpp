#include "binaryinjector.h"
#include "ui_binaryinjector.h"

#include <QFileDialog>
#include <QDesktopServices>
#include <QFile>
#include <QMessageBox>

BinaryInjector::BinaryInjector(XDBF *xdbf_pv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BinaryInjector), xdbf(xdbf_pv)
{
    ui->setupUi(this);

    knownIDs = new unsigned long long[29];
    knownIDs[0] = 0x10040003;
    knownIDs[1] = 0x63E83FFF;
    knownIDs[2] = 0x63E83FFE;
    knownIDs[3] = 0x63E83FFD;
    knownIDs[4] = 0x10040002;
    knownIDs[5] = 0x10040004;
    knownIDs[6] = 0x10040005;
    knownIDs[7] = 0x10040006;
    knownIDs[8] = 0x50040011;
    knownIDs[9] = 0x10040012;
    knownIDs[10] = 0x10040013;
    knownIDs[11] = 0x10040063;
    knownIDs[12] = 0x10040014;
    knownIDs[13] = 0x10040018;
    knownIDs[14] = 0x10040019;
    knownIDs[15] = 0x10040021;
    knownIDs[16] = 0x10040024;
    knownIDs[17] = 0x10040029;
    knownIDs[18] = 0x10040030;
    knownIDs[19] = 0x10040034;
    knownIDs[20] = 0x10040035;
    knownIDs[21] = 0x10040036;
    knownIDs[22] = 0x10040038;
    knownIDs[23] = 0x10040039;
    knownIDs[24] = 0x10040040;
    knownIDs[25] = 0x10040041;
    knownIDs[26] = 0x10040056;
    knownIDs[27] = 0x10040057;
    knownIDs[28] = 0x4064000F;

    // add the setting id strings to the combo box
    for (int i = 0; i < 29; i++)
        if (xdbf->get_entry_by_id(knownIDs[i], ET_SETTING) == NULL)
        {
            ui->comboBox->addItem(QString::fromStdString(Entry_ID_to_string(knownIDs[i])));
        }
}

BinaryInjector::~BinaryInjector()
{
    delete ui;
}

void BinaryInjector::on_pushButton_clicked()
{
    if (!isAllNumbers(ui->lineEdit->text()))
    {
        QMessageBox::warning(this, "Invalid Characters", "Invalid characters in entry id!");
        return;
    }

    if(path == "")
    {
        QMessageBox::warning(this, "No File Selected", "You must select a file to inject.");
        return;
    }
    QFile file;
    file.setFileName(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "File Could Not Open", "The selected file could not be opened. Please close any programs that are using it, check your permissions, or choose a new file.");
        return;
    }

    QByteArray data = file.readAll();

    file.close();

    if(data.size() > 0x3E8)
    {
        QMessageBox::warning(this, "File to Large", "The size of the selected file is to large. Select a file less than or equal to 1000 bytes.");
        return;
    }
    char* tmp = new char [data.size()];
    strcpy( tmp, data.constData() );

    Setting_Entry se;
    se.type = SET_BINARY;
    se.binary.size = data.size();
    se.binary.data = tmp;
    xdbf->injectSettingEntry(&se, ui->lineEdit->text().toULongLong());

    QMessageBox::information(this, "Success", "Successfully injected binary entry.");
    close();
}

bool BinaryInjector::isAllNumbers(QString s)
{
    for (int i = 0; i < s.length(); i++)
        if (!s.at(0).isDigit())
            return false;

    return true;
}

void BinaryInjector::on_pushButton_3_clicked()
{
    QString path1 = QFileDialog::getOpenFileName(this, "Select File", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
    if(path1 == "")
        return;

    path = path1;

    QFileInfo info(path);
    ui->label->setText(info.fileName());
}
void BinaryInjector::on_comboBox_currentIndexChanged(int index)
{
    ui->lineEdit->setEnabled(index == 0);
}
