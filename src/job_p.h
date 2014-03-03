#ifndef JOB_P_H
#define JOB_P_H

#include <QMutex>
#include "executewrapper_p.h"

namespace ThreadWeaver {

namespace Private {

class DefaultExecutor : public ThreadWeaver::Executor
{
public:
    void begin(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *thread);
    void execute(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *thread) Q_DECL_OVERRIDE;
    void end(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *thread);
};

static DefaultExecutor defaultExecutor;

class DebugExecuteWrapper : public ThreadWeaver::ExecuteWrapper
{
public:
    void execute(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *th) Q_DECL_OVERRIDE;
};

class Job_Private
{
public:
    Job_Private();

    ~Job_Private();

    /* The list of QueuePolicies assigned to this Job. */
    QList<QueuePolicy *> queuePolicies;

    mutable QMutex mutex;
    /* @brief The status of the Job. */
    QAtomicInt status;

    /** The Executor that will execute this Job. */
    QAtomicPointer<Executor> executor;

    //FIXME What is the correct KDE frameworks no debug switch?
#if !defined(NDEBUG)
    /** DebugExecuteWrapper for logging of Job execution. */
    DebugExecuteWrapper debugExecuteWrapper;
#endif
};

}

}

#endif // JOB_P_H
