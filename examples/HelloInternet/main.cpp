#include <QApplication>

#include <ThreadWeaver/ThreadWeaver>
//#include <threadweaver/DebuggingAids.h>

#include "MainWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //ThreadWeaver::setDebugLevel(true, 3);
    MainWidget w;
    w.show();
    a.exec();
    ThreadWeaver::Queue::instance()->finish();
}
