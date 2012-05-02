#ifndef TITLEINJECTORDIALOG_H
#define TITLEINJECTORDIALOG_H

#include <QDialog>
#include "xdbf.h"
#include <QtGui/QLineEdit>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

namespace Ui {
class TitleInjectorDialog;
}

class TitleInjectorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TitleInjectorDialog(QWidget *parent = 0, XDBF *xdbf = NULL, Title_Entry *tentry = NULL);
    ~TitleInjectorDialog();
    
private slots:
    void on_pushButton_clicked();
    void showRemoveContextMenu(const QPoint& pos);
    void replyFinished(QNetworkReply* aReply);
    void on_pushButton_2_clicked();

private:
    Ui::TitleInjectorDialog *ui;
    bool hexNumbersOnly(QLineEdit *lineEdit);
    XDBF *xdbf;
    Title_Entry *tentry;
};

#endif // TITLEINJECTORDIALOG_H
