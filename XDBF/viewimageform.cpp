#include "viewimageform.h"
#include "ui_viewimageform.h"

viewimageform::viewimageform(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::viewimageform)
{
    ui->setupUi(this);
}

viewimageform::~viewimageform()
{
    delete ui;
}
