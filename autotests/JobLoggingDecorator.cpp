#include "src/thread.h"

#include "JobLoggingDecorator.h"

using namespace ThreadWeaver;

JobLoggingDecorator::JobLoggingDecorator(const JobPointer &job, JobLoggingDecoratorCollector* collector)
    : IdDecorator(job.data(), false)
    , collector_(collector)
{
    Q_ASSERT(collector);
}

void JobLoggingDecorator::run(JobPointer self, Thread *thread)
{
    data_.start = QDateTime::currentDateTime();
    if (thread) {
        data_.threadId = thread->id();
    } else {
        data_.threadId = -1;
    }
    IdDecorator::run(self, thread);
    data_.end = QDateTime::currentDateTime();
    collector_->storeJobData(data_);
}


void JobLoggingDecoratorCollector::storeJobData(const JobLoggingDecorator::JobData &data)
{
    QMutexLocker m(&mutex_);
    jobData_.append(data);
}
