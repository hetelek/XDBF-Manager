#ifndef VIEWINTFORM_H
#define VIEWINTFORM_H

#include <QMainWindow>

namespace Ui {
class viewintform;
}

class viewintform : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit viewintform(QWidget *parent = 0);
    ~viewintform();
    
private:
    Ui::viewintform *ui;
};

#endif // VIEWINTFORM_H
