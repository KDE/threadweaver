#ifndef QUEUESTREAM_H
#define QUEUESTREAM_H

#include "JobInterface.h"
#include "threadweaver_export.h"

namespace ThreadWeaver
{

class Queue;
class Job;

/** @brief QueueStream implements a stream based API to access ThreadWeaver queues. */
class THREADWEAVER_EXPORT QueueStream
{
public:
    explicit QueueStream(Queue *queue);
    ~QueueStream();
    void add(const JobPointer &job);
    void flush();

    QueueStream &operator<<(const JobPointer &job);
    QueueStream &operator<<(JobInterface *job);
    //FIXME try with QObjectDecorator (JobInterface&)
    QueueStream &operator<<(Job &job);

private:
    class Private;
    Private *const d;
};

QueueStream THREADWEAVER_EXPORT stream();

}

#endif // QUEUESTREAM_H
