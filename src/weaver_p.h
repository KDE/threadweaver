#ifndef WEAVER_P_H
#define WEAVER_P_H

#include <QWaitCondition>
#include <QSemaphore>
#include <QSharedPointer>

#include "state.h"
#include "thread.h"
#include "jobpointer.h"

#include "queuesignals_p.h"

namespace ThreadWeaver {

namespace Private {

class Weaver_Private : public QueueSignals_Private
{
public:
    Weaver_Private();
    ~Weaver_Private();

    void dumpJobs();

    bool canBeExecuted(JobPointer);
    /** The thread inventory. */
    QList<Thread *> inventory;
    /** The job queue. */
    QList<JobPointer> assignments;
    /** The number of jobs that are assigned to the worker threads, but not finished. */
    int active;
    /** The maximum number of worker threads. */
    int inventoryMax;
    /** Wait condition all idle or done threads wait for. */
    QWaitCondition jobAvailable;
    /** Wait for a job to finish. */
    QWaitCondition jobFinished;
    /** Mutex to serialize operations. */
    QMutex *mutex;
    /** Semaphore to ensure thread startup is in sequence. */
    QSemaphore semaphore;
    /** Before shutdown can proceed to close the running threads, it needs to ensure that all of them
     *  entered the run method. */
    QAtomicInt createdThreads;
    /** The state of the art.
    * @see StateId
    */
    QAtomicPointer<State> state;
    /** The state objects. */
    QSharedPointer<State> states[NoOfStates];
};

}

}

#endif // WEAVER_P_H
