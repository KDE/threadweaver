#include <QThread>

#include "PriorityDecorator.h"

using namespace ThreadWeaver;

PriorityDecorator::PriorityDecorator(int priority, ThreadWeaver::JobInterface *job, bool autoDelete)
    : IdDecorator(job, autoDelete)
    , m_priority(priority)
{
}

//FIXME remove
void PriorityDecorator::run(JobPointer self, Thread *thread) {
//    QThread::msleep(500);
    IdDecorator::run(self, thread);
}

int PriorityDecorator::priority() const {
    return m_priority;
}
