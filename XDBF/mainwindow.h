#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <sstream>
#include <iostream>

//forms / xdbf class
#include <binarydialog.h>
#include <imagedialog.h>
#include <achievementviewer.h>
#include <xdbf.h>
#include <synclistdialog.h>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionOpen_triggered();

    void on_pushButton_clicked();

    void on_actionClose_triggered();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    char *friendlyNames[6];
    XDBF *xdbf;
    void clear_items();
    template<typename T2, typename T1> inline T2 parse_decimal(const T1 &in);
    template<typename T2, typename T1> inline T2 parse_hex(const T1 &in);
    AchievementViewer *achViewer;
    void showIntMessageBox(unsigned long long num, QString message_header, QString title);
    void showFloatMessageBox(double num, QString message_header, QString title);
    void showStringMessageBox(const wchar_t *wStr, QString message_header, QString title);
    void showDatetimeMessageBox(FILETIME time, QString message_header, QString title);
    int ObjectRole;
    void UnixTimeToFileTime(time_t* pt, FILETIME *pft);
    string FILETIME_to_string(FILETIME *pft);
    QString desktop_location;
};

#endif // MAINWINDOW_H
