#include "job_p.h"
#include "executor_p.h"
#include "debuggingaids.h"
#include "thread.h"

ThreadWeaver::Private::Job_Private::Job_Private()
    : mutex(QMutex::NonRecursive)
    , status(Job::Status_NoStatus)
    , executor(&defaultExecutor)
{
}

ThreadWeaver::Private::Job_Private::~Job_Private()
{}


void ThreadWeaver::Private::DebugExecuteWrapper::execute(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *th)
{
    Q_ASSERT_X(job, Q_FUNC_INFO, "job may not be zero!");
    ThreadWeaver::debug(3, "DefaultExecuteWrapper::execute: executing job %p in thread %i.\n", job.data(), th ? th->id() : 0);
    executeWrapped(job, th);
    ThreadWeaver::debug(3, "Job::execute: finished execution of job in thread %i.\n", th ? th->id() : 0);
}


void ThreadWeaver::Private::DefaultExecutor::begin(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *thread)
{
    defaultBegin(job, thread);
}

void ThreadWeaver::Private::DefaultExecutor::execute(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *thread)
{
    run(job, thread);
}

void ThreadWeaver::Private::DefaultExecutor::end(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *thread)
{
    defaultEnd(job, thread);
}

