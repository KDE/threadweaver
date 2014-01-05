#include <QVector>

#include "QueueStream.h"
#include "Queue.h"
#include "Queueing.h"

namespace ThreadWeaver
{

class QueueStream::Private
{
public:
    Private(Queue *w) : weaver(w) {}
    Queue *weaver;
    QVector<JobPointer> jobs;
};

QueueStream::QueueStream(Queue *queue)
    : d(new Private(queue))
{
    Q_ASSERT(queue);
}

QueueStream::~QueueStream()
{
    flush();
    delete d;
}

void ThreadWeaver::QueueStream::add(const ThreadWeaver::JobPointer &job)
{
    d->jobs.append(job);
}

void QueueStream::flush()
{
    if (d->jobs.isEmpty()) {
        return;
    }
    Q_ASSERT(d->weaver);
    d->weaver->enqueue(d->jobs);
    d->jobs.clear();
}

QueueStream &QueueStream::operator<<(const JobPointer &job)
{
    add(job);
    return *this;
}

QueueStream &QueueStream::operator<<(JobInterface *job)
{
    add(make_job(job));
    return *this;
}

QueueStream &QueueStream::operator<<(Job &job)
{
    add(make_job_raw(&job));
    return *this;
}

/** @brief Return a stream the enqueues jobs in the ThreadWeaver global queue.
 *  Using this is synonymous to Weaver::instance()::stream(). */
QueueStream stream()
{
    return QueueStream(Queue::instance());
}

}

