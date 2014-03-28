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

    bool canBeExecuted(JobPointer);
    /** The thread inventory. */
    QList<Thread *> m_inventory;
    /** The job queue. */
    QList<JobPointer> m_assignments;
    /** The number of jobs that are assigned to the worker threads, but not finished. */
    int m_active;
    /** The maximum number of worker threads. */
    int m_inventoryMax;
    /** Wait condition all idle or done threads wait for. */
    QWaitCondition m_jobAvailable;
    /** Wait for a job to finish. */
    QWaitCondition m_jobFinished;
    /** Mutex to serialize operations. */
    QMutex *m_mutex;
    /** Semaphore to ensure thread startup is in sequence. */
    QSemaphore m_semaphore;
    /** Before shutdown can proceed to close the running threads, it needs to ensure that all of them
     *  entered the run method. */
    QAtomicInt m_createdThreads;
    /** The state of the art.
    * @see StateId
    */
    QAtomicPointer<State> m_state;
    /** The state objects. */
    QSharedPointer<State> m_states[NoOfStates];
};

}

}

#endif // WEAVER_P_H
