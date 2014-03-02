#include "JobLoggingWeaver.h"
#include "JobLoggingDecorator.h"

using namespace ThreadWeaver;

JobLoggingWeaver::JobLoggingWeaver(QObject *parent)
    : Weaver(parent)
{
}

void JobLoggingWeaver::enqueue(const QVector<JobPointer> &jobs)
{
    QVector<JobPointer> decorated;
    std::transform(jobs.begin(), jobs.end(), std::back_inserter(decorated),
                   [this](const JobPointer & job) {
        return JobPointer(new JobLoggingDecorator(job, &collector_));
    } );
    Weaver::enqueue(decorated);

}
