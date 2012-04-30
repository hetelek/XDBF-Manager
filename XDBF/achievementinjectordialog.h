#ifndef ACHIEVEMENTINJECTORDIALOG_H
#define ACHIEVEMENTINJECTORDIALOG_H

#include <QDialog>
#include "xdbf.h"

using std::wstring;

namespace Ui {
class AchievementInjectorDialog;
}

class AchievementInjectorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AchievementInjectorDialog(QWidget *parent = 0, XDBF *xdbf = NULL);
    ~AchievementInjectorDialog();
    
private slots:
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_clicked();
    void on_imageChbx_stateChanged(int arg1);
    void on_openImageBtn_clicked();

private:
    Ui::AchievementInjectorDialog *ui;
    XDBF *xdbf;
    QImage image;
};

#endif // ACHIEVEMENTINJECTORDIALOG_H
