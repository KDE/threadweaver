#include <QApplication>

#include "MainWidget.h"
#include "ViewController.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;
    w.show();
    ViewController v(&w);
    a.exec();
}
