#ifndef VIEWIMAGEFORM_H
#define VIEWIMAGEFORM_H

#include <QDialog>

namespace Ui {
class viewimageform;
}

class viewimageform : public QDialog
{
    Q_OBJECT
    
public:
    explicit viewimageform(QWidget *parent = 0);
    ~viewimageform();
    
private:
    Ui::viewimageform *ui;
};

#endif // VIEWIMAGEFORM_H
