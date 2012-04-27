#ifndef TITLEENTRYDIALOG_H
#define TITLEENTRYDIALOG_H

#include <QDialog>
#include <xdbf.h>
#include <time.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

namespace Ui {
class TitleEntryDialog;
}

class TitleEntryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TitleEntryDialog(QWidget *parent = 0, Title_Entry *te = NULL);
    ~TitleEntryDialog();
    
private:
    Ui::TitleEntryDialog *ui;

private slots:
    void replyFinished(QNetworkReply* aReply);

};

#endif // TITLEENTRYDIALOG_H
