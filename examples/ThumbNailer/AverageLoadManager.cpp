#ifdef Q_OS_UNIX
#include <stdlib.h>
#endif

#include <QTimer>
#include <QThread>
#include <QDebug>

#include "AverageLoadManager.h"

AverageLoadManager::AverageLoadManager(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    m_timer->setSingleShot(false);
    m_timer->setInterval(2000);
    connect(m_timer, SIGNAL(timeout()), SLOT(update()));
}

void AverageLoadManager::activate(bool enabled)
{
    if (available()) {
        if (enabled) {
            m_timer->start();
        } else {
            m_timer->stop();
        }
    }
}

bool AverageLoadManager::available() const
{
#ifdef Q_OS_UNIX
    return true;
#else
    return false;
#endif
}

void AverageLoadManager::update()
{
#ifdef Q_OS_UNIX
    double averages[3];
    if (getloadavg(averages, 3) == -1) {
        return;
    }

    const float processors = QThread::idealThreadCount();
    const float relativeLoadPerProcessor = averages[0] / processors; // relative system load
    const float targetedBaseLoad = 0.7f;

    const float x = relativeLoadPerProcessor / targetedBaseLoad;
    auto const linearLoadFunction = [](float x) { return -x+2.0f; };
    //auto const reciprocalLoadFunction = [](float x) { return 1.0f / (0.5*x+0.5); };

    const float y = linearLoadFunction(x);
    const int threads = qRound(processors * y);
    qDebug() << threads << y << x << relativeLoadPerProcessor << averages[0] << processors;
    //<< qRound(qBound(1.0f, c, b)) << c << e(relativeLoadPerProcessor) << relativeLoadPerProcessor << deviation
    emit recommendedWorkerCount(threads);
#endif
}

