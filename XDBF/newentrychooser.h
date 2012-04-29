#ifndef NEWENTRYCHOOSER_H
#define NEWENTRYCHOOSER_H

#include <QDialog>
#include "xdbf.h"

namespace Ui {
class NewEntryChooser;
}

class NewEntryChooser : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewEntryChooser(QWidget *parent = 0, XDBF *xdbf = NULL);
    ~NewEntryChooser();
    
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::NewEntryChooser *ui;
    XDBF *xdbf;
};

#endif // NEWENTRYCHOOSER_H
