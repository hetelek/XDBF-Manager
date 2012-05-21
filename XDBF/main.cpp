#include <QtGui/QApplication>
#include "mainwindow.h"
#include <iostream>

using std::string;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (argc == 1)
    {
        MainWindow w(NULL);
        w.show();
        return a.exec();
    }

    else
    {
        string s(argv[1]);
        MainWindow w(NULL, s);
        w.show();
        return a.exec();
    }
}
