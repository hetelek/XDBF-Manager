#ifndef TITLEINJECTORDIALOG_H
#define TITLEINJECTORDIALOG_H

#include <QDialog>

namespace Ui {
class TitleInjectorDialog;
}

class TitleInjectorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TitleInjectorDialog(QWidget *parent = 0);
    ~TitleInjectorDialog();
    
private:
    Ui::TitleInjectorDialog *ui;
};

#endif // TITLEINJECTORDIALOG_H
