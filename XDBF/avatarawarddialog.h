#ifndef AVATARAWARDDIALOG_H
#define AVATARAWARDDIALOG_H

#include <QDialog>
#include "xdbf.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QMessageBox>

namespace Ui {
class AvatarAwardDialog;
}

class AvatarAwardDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AvatarAwardDialog(QWidget *parent = 0, Avatar_Award_Entry *entry = NULL, XDBF *xdbf = NULL);
    ~AvatarAwardDialog();
    
private:
    Ui::AvatarAwardDialog *ui;
    Avatar_Award_Entry *award;
    XDBF *xdbf;

private slots:
    void replyFinished(QNetworkReply* aReply);
    void on_saveBtn_clicked();
    void on_cancelBtn_clicked();
};

#endif // AVATARAWARDDIALOG_H
