#include "viewintform.h"
#include "ui_viewintform.h"

viewintform::viewintform(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::viewintform)
{
    ui->setupUi(this);
}

viewintform::~viewintform()
{
    delete ui;
}
