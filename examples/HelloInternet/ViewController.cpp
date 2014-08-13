#include <QThread>
#include <QFile>

#include <ThreadWeaver/ThreadWeaver>
//#include <threadweaver/DebuggingAids.h>

#include "ViewController.h"
#include "MainWidget.h"

ViewController::ViewController(MainWidget *mainwidget)
    : QObject() // no parent
{
    connect(this, SIGNAL(setImage(QImage)), mainwidget, SLOT(setImage(QImage)));
    connect(this, SIGNAL(setCaption(QString)), mainwidget, SLOT(setCaption(QString)));
    connect(this, SIGNAL(setStatus(QString)), mainwidget, SLOT(setStatus(QString)));

    using namespace ThreadWeaver;
    //ThreadWeaver::setDebugLevel(true, 3);
    auto sequence = new Sequence;
    *sequence << make_job( [=]() { loadPlaceholderFromResource(); } );
    stream() << sequence;
}

ViewController::~ViewController()
{
    ThreadWeaver::Queue::instance()->finish();
}

void ViewController::loadPlaceholderFromResource()
{
    QThread::msleep(1000);
    auto const path = QStringLiteral("://resources/IMG_20140813_004131.png");
    Q_ASSERT(QFile::exists(path));
    const QImage i(path);
    Q_ASSERT(!i.isNull());
    emit setImage(i);
}
