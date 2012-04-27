#ifndef ACHIEVEMENTVIEWER_H
#define ACHIEVEMENTVIEWER_H

#include <QDialog>
#include <xdbf.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>

namespace Ui {
class AchievementViewer;
}

class AchievementViewer : public QDialog
{
    Q_OBJECT
    
public:
    explicit AchievementViewer(QWidget *parent = 0, Achievement_Entry *chiev = 0, FILE *f = 0, QImage image = QImage(), long address = 0);
    ~AchievementViewer();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::AchievementViewer *ui;
    Achievement_Entry *entry;
    long entryAddr;
    FILE *opened_file;
    QImage img;
};

#endif // ACHIEVEMENTVIEWER_H
