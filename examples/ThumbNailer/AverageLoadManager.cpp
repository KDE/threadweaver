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

    qDebug() << averages[0] << averages[1] << averages[2];
    const float targetedBaseLoad = 0.7f;
    const float processors = QThread::idealThreadCount();
    const float b = 1.5f * processors; // base thread count
    const float r = averages[0] / processors; // relative system load
    const float deviation = targetedBaseLoad - r;

    auto const e = [deviation, processors](float r) { return processors * 1.2 * deviation; };
    const float c = b + e(r);
    qDebug() << qRound(qBound(1.0f, c, b)) << c << e(r) << r << deviation << b << processors;
#endif
}

