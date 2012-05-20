#include "about.h"
#include "ui_about.h"
#include <QDesktopServices>
#include <QUrl>

About::About(QWidget *parent) : QDialog(parent), ui(new Ui::About)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    setFixedSize(sizeHint());
}

About::~About()
{
    delete ui;
}

void About::on_pushButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.free60.org"));
}

void About::on_pushButton_3_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/hetelek/XDBF-Manager"));
}

void About::on_pushButton_2_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.xboxmb.com/"));
}
