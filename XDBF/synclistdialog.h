#ifndef SYNCLISTDIALOG_H
#define SYNCLISTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <xdbf.h>
#include <QTableWidget>

namespace Ui {
class SyncListDialog;
}

class SyncListDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SyncListDialog(QWidget *parent = 0, Sync_List *_list = NULL, XDBF *_xdbf = NULL);
    Sync_List *list;
    ~SyncListDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::SyncListDialog *ui;
    void moveItem(QTableWidget *from, QTableWidget *to);
    int ObjectRole;
    XDBF *xdbf;
};

#endif // SYNCLISTDIALOG_H
