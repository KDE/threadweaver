#include <ThreadWeaver/ThreadWeaver>
//#include <threadweaver/DebuggingAids.h>

#include "ViewController.h"
#include "MainWidget.h"

ViewController::ViewController(MainWidget *mainwidget)
    : QObject() // no parent
{
    //ThreadWeaver::setDebugLevel(true, 3);
    connect(this, SIGNAL(setImage(QImage)), mainwidget, SLOT(setImage(QImage)));
    connect(this, SIGNAL(setCaption(QString)), mainwidget, SLOT(setCaption(QString)));
    connect(this, SIGNAL(setStatus(QString)), mainwidget, SLOT(setStatus(QString)));

    //temp:
    const QImage i(QStringLiteral("://resources/IMG_20140813_004131.png"));
    Q_ASSERT(!i.isNull());
    emit setImage(i);
}

ViewController::~ViewController()
{
    ThreadWeaver::Queue::instance()->finish();
}
