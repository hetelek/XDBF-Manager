#include "settinginjectorint.h"
#include "ui_settinginjectorint.h"
#include "xdbf.h"
#include <QMessageBox>
#include <iostream>
#include <float.h>

using std::string;

SettingInjectorInt::SettingInjectorInt(QWidget *parent, XDBF *xdbf, char type) : xdbf(xdbf), QDialog(parent), ui(new Ui::SettingInjectorInt), spinBox(NULL), dSpinBox(NULL), lineEditP(NULL), type(type)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

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
            cmbxIDs.push_back(knownIDs[i]);
        }

    switch (type)
    {
        case SET_INT32:
            ui->label->setText("<b>Int32 Value: </b>");
            spinBox = new QSpinBox(this);
            spinBox->setMaximum(0x7FFFFFFF);
            spinBox->setMinimum(0xFFFFFFFE);
            spinBox->setValue(0);
            spinBox->setEnabled(true);
            ui->gridLayout->addWidget(spinBox, 0, 1);
            this->setWindowTitle("Int32 Setting Entry Adder");
            break;
        case SET_INT64:
            ui->label->setText("<b>Int64 Value:</b>");
            lineEditP = new QLineEdit(this);
            ui->gridLayout->addWidget(lineEditP, 0, 1);
            this->setWindowTitle("Int64 Setting Entry Adder");
            break;
        case SET_DOUBLE:
            ui->label->setText("<b>Double Value: </b>");
            dSpinBox = new QDoubleSpinBox(this);
            dSpinBox->setValue(0);
            dSpinBox->setMaximum(DBL_MAX);
            dSpinBox->setMinimum(DBL_MIN);
            ui->gridLayout->addWidget(dSpinBox, 0, 1);
            this->setWindowTitle("Double Setting Entry Adder");
            break;
        case SET_FLOAT:
            ui->label->setText("<b>Float Value: </b>");
            dSpinBox = new QDoubleSpinBox(this);
            dSpinBox->setValue(0);
            dSpinBox->setMaximum(FLT_MAX);
            dSpinBox->setMinimum(FLT_MIN);
            ui->gridLayout->addWidget(dSpinBox, 0, 1);
            this->setWindowTitle("Float Setting Entry Adder");
            break;
        case SET_UNICODE:
            ui->label->setText("<b>Unicode String: </b>");
            lineEditP = new QLineEdit(this);
            ui->gridLayout->addWidget(lineEditP, 0, 1);
            this->setWindowTitle("String Setting Entry Adder");
            break;
        case SET_DATETIME:
            ui->label->setText("<b>Date Time: </b>");
            dte = new QDateTimeEdit(this);
            ui->gridLayout->addWidget(dte, 0, 1);
            this->setWindowTitle("Date Time Setting Entry Adder");
            break;
    }


    setFixedSize(sizeHint());
}

SettingInjectorInt::~SettingInjectorInt()
{
    if (spinBox != NULL)
        delete spinBox;
    if (dSpinBox != NULL)
        delete dSpinBox;
    if (lineEditP != NULL)
        delete lineEditP;
    delete ui;
}

void SettingInjectorInt::on_pushButton_clicked()
{
    // create a new setting entry of type int32, and set the value
    // to the one provided by the user
    Setting_Entry entry;
    QString msgBoxText = "";

    entry.type = type;

    switch (type)
    {
        case SET_INT32:
            entry.i32_data = spinBox->value();
            msgBoxText = "int32";
            break;
        case SET_INT64:
            if (!isAllNumbers(lineEditP->text()))
            {
                QMessageBox::warning(this, "Invalid Characters", "Invalid characters in the value text box!");
                return;
            }
            entry.i64_data = lineEditP->text().toLongLong();
            msgBoxText = "int64";
            break;
        case SET_DOUBLE:
            entry.double_data = dSpinBox->value();
            msgBoxText = "double";
            break;
        case SET_FLOAT:
            entry.float_data = (float)dSpinBox->value();
            msgBoxText = "float";
            break;
        case SET_UNICODE:
        {
            wstring *entryString = new wstring(lineEditP->text().toStdWString());
            entry.unicode_string.str = entryString;
            entry.unicode_string.str_len_in_bytes = (entry.unicode_string.str->length() + 1) * 2;
            msgBoxText = "unicode string";
            break;
        }
        case SET_DATETIME:
            entry.time_stamp = time_t_to_FILETIME(dte->dateTime().toTime_t());
            msgBoxText = "date time";
            break;
    }

    unsigned long long id;
    if (ui->comboBox->currentIndex() == 0)
    {
        // ensure that there are only number characters in the line edit
        if (!isAllNumbers(ui->lineEdit->text()))
        {
            QMessageBox::warning(this, "Invalid Characters", "Invalid Characters in user defined entry ID!");
            return;
        }
        id = ui->lineEdit->text().toLongLong();
    }
    else
        id = cmbxIDs.at(ui->comboBox->currentIndex() - 1);


    // add the entry
    xdbf->injectSettingEntry(&entry, id);

    QMessageBox::information(this, "Success", "Successfully added " + msgBoxText + " setting entry.");

    close();
}

bool SettingInjectorInt::isAllNumbers(QString s)
{
    for (int i = 0; i < s.length(); i++)
        if (!s.at(0).isDigit())
            return false;

    return true;
}

void SettingInjectorInt::on_comboBox_currentIndexChanged(int index)
{
     ui->lineEdit->setEnabled(index == 0);

}

void SettingInjectorInt::on_pushButton_2_clicked()
{
    close();
}
