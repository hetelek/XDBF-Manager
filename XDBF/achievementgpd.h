#ifndef ACHIEVEMENTGPD_H
#define ACHIEVEMENTGPD_H

#include <QDialog>
#include "xdbf.h"

namespace Ui {
class AchievementGPD;
}

class AchievementGPD : public QDialog
{
    Q_OBJECT
    
public:
    explicit AchievementGPD(QWidget *parent = 0, XDBF **xdbf = NULL);
    ~AchievementGPD();
    
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::AchievementGPD *ui;
    bool imageOpened;
    XDBF **xdbf;
};

#endif // ACHIEVEMENTGPD_H
