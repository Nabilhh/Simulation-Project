#include "mainwindow.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    //testMain();   // this runs any tests specified in testmain.h
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
