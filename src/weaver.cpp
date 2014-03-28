/* -*- C++ -*-

This file implements the WeaverImpl class.

$ Author: Mirko Boehm $
$ Copyright: (C) 2005-2013 Mirko Boehm $
$ Contact: mirko@kde.org
http://www.kde.org
http://creative-destruction.me $

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

$Id: WeaverImpl.cpp 30 2005-08-16 16:16:04Z mirko $

*/

#include "weaver.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QDebug>

#include "job.h"
#include "managedjobpointer.h"
#include "state.h"
#include "thread.h"
#include "threadweaver.h"
#include "debuggingaids.h"
#include "queuepolicy.h"
#include "weaver_p.h"
#include "suspendedstate.h"
#include "suspendingstate.h"
#include "destructedstate.h"
#include "workinghardstate.h"
#include "shuttingdownstate.h"
#include "inconstructionstate.h"

using namespace ThreadWeaver;

/** @brief Constructs a Weaver object. */
Weaver::Weaver(QObject *parent)
    : QueueAPI(new Private::Weaver_Private(), parent)
{
    qRegisterMetaType<ThreadWeaver::JobPointer>("ThreadWeaver::JobPointer");
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    // initialize state objects:
    d()->states[InConstruction] = QSharedPointer<State>(new InConstructionState(this));
    setState_p(InConstruction);
    d()->states[WorkingHard] = QSharedPointer<State>(new WorkingHardState(this));
    d()->states[Suspending] = QSharedPointer<State>(new SuspendingState(this));
    d()->states[Suspended] = QSharedPointer<State>(new SuspendedState(this));
    d()->states[ShuttingDown] = QSharedPointer<State>(new ShuttingDownState(this));
    d()->states[Destructed] = QSharedPointer<State>(new DestructedState(this));

    setState_p(WorkingHard);
}

/** @brief Destructs a Weaver object. */
Weaver::~Weaver()
{
    Q_ASSERT_X(state()->stateId() == Destructed, Q_FUNC_INFO, "shutDown() method was not called before Weaver destructor!");
}

/** @brief Enter Destructed state.
  *
  * Once this method returns, it is save to delete this object.
  */
void Weaver::shutDown()
{
    state()->shutDown();
}

void Weaver::shutDown_p()
{
    // the constructor may only be called from the thread that owns this
    // object (everything else would be what we professionals call "insane")

    REQUIRE(QThread::currentThread() == thread());
    TWDEBUG(3, "WeaverImpl::shutDown: destroying inventory.\n");
    d()->semaphore.acquire(d()->createdThreads.loadAcquire());
    finish();
    suspend();
    setState(ShuttingDown);
    reschedule();
    d()->jobFinished.wakeAll();

    // problem: Some threads might not be asleep yet, just finding
    // out if a job is available. Those threads will suspend
    // waiting for their next job (a rare case, but not impossible).
    // Therefore, if we encounter a thread that has not exited, we
    // have to wake it again (which we do in the following for
    // loop).

    for (;;) {
        Thread *th = 0;
        {
            QMutexLocker l(d()->mutex); Q_UNUSED(l);
            if (d()->inventory.isEmpty()) {
                break;
            }
            th = d()->inventory.takeFirst();
        }
        if (!th->isFinished()) {
            for (;;) {
                Q_ASSERT(state()->stateId() == ShuttingDown);
                reschedule();
                if (th->wait(100)) {
                    break;
                }
                TWDEBUG(1,  "WeaverImpl::shutDown: thread %i did not exit as expected, "
                      "retrying.\n", th->id());
            }
        }
        emit(threadExited(th));
        delete th;
    }
    Q_ASSERT(d()->inventory.isEmpty());
    TWDEBUG(3, "WeaverImpl::shutDown: done\n");
    setState(Destructed);    // Destructed ignores all calls into the queue API
}

/** @brief Set the Weaver state.
 * @see StateId
 * @see WeaverImplState
 * @see State
 */
void Weaver::setState(StateId id)
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    setState_p(id);
}

void Weaver::setState_p(StateId id)
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    State *newState = d()->states[id].data();
    State *previous = d()->state.fetchAndStoreOrdered(newState);
    if (previous == 0 || previous->stateId() != id) {
        newState->activated();
        TWDEBUG(2, "WeaverImpl::setState: state changed to \"%s\".\n", newState->stateName().toLatin1().constData());
        if (id == Suspended) {
            emit(suspended());
        }
        emit(stateChanged(newState));
    }
}

const State *Weaver::state() const
{
    return d()->state.loadAcquire();
}

State *Weaver::state()
{
    return d()->state.loadAcquire();
}

void Weaver::setMaximumNumberOfThreads(int cap)
{
    //FIXME really? Why not 0?
    Q_ASSERT_X(cap > 0, "Weaver Impl", "Thread inventory size has to be larger than zero.");
    QMutexLocker l(d()->mutex);  Q_UNUSED(l);
    state()->setMaximumNumberOfThreads(cap);
}

void Weaver::setMaximumNumberOfThreads_p(int cap)
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    d()->inventoryMax = cap;
}

int Weaver::maximumNumberOfThreads() const
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    return state()->maximumNumberOfThreads();
}

int Weaver::maximumNumberOfThreads_p() const
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    return d()->inventoryMax;
}

int Weaver::currentNumberOfThreads() const
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    return state()->currentNumberOfThreads();
}

int Weaver::currentNumberOfThreads_p() const
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    return d()->inventory.count();
}

void Weaver::enqueue(const QVector<JobPointer> &jobs)
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    state()->enqueue(jobs);
}

void Weaver::enqueue_p(const QVector<JobPointer> &jobs)
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    if (jobs.isEmpty()) {
        return;
    }
    Q_FOREACH (const JobPointer &job, jobs) {
        if (job) {
            Q_ASSERT(job->status() == Job::Status_New);
            adjustInventory(1);
            TWDEBUG(3, "WeaverImpl::enqueue: queueing job %p.\n", (void *)job.data());
            job->aboutToBeQueued(this);
            // find position for insertion:
            int i = d()->assignments.size();
            if (i > 0) {
                while (i > 0 && d()->assignments.at(i - 1)->priority() < job->priority()) {
                    --i;
                }
                d()->assignments.insert(i, job);
            } else {
                d()->assignments.append(job);
            }
            job->setStatus(Job::Status_Queued);
            reschedule();
        }
    }
}

bool Weaver::dequeue(const JobPointer &job)
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    return state()->dequeue(job);
}

bool Weaver::dequeue_p(JobPointer job)
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    int position = d()->assignments.indexOf(job);
    if (position != -1) {
        job->aboutToBeDequeued(this);
        int newPosition = d()->assignments.indexOf(job);
        JobPointer job = d()->assignments.takeAt(newPosition);
        job->setStatus(Job::Status_New);
        Q_ASSERT(!d()->assignments.contains(job));
        TWDEBUG(3, "WeaverImpl::dequeue: job %p dequeued, %i jobs left.\n", (void *)job.data(), queueLength_p());
        // from the queues point of view, a job is just as finished if it gets dequeued:
        d()->jobFinished.wakeAll();
        Q_ASSERT(!d()->assignments.contains(job));
        return true;
    } else {
        TWDEBUG(3, "WeaverImpl::dequeue: job %p not found in queue.\n", (void *)job.data());
        return false;
    }
}

void Weaver::dequeue()
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    state()->dequeue();
}

void Weaver::dequeue_p()
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    TWDEBUG(3, "WeaverImpl::dequeue: dequeueing all jobs.\n");
    for (int index = 0; index < d()->assignments.size(); ++index) {
        d()->assignments.at(index)->aboutToBeDequeued(this);
    }
    d()->assignments.clear();
    ENSURE(d()->assignments.isEmpty());
}

void Weaver::finish()
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    state()->finish();
}

void Weaver::finish_p()
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
#ifdef QT_NO_DEBUG
    const int MaxWaitMilliSeconds = 50;
#else
    const int MaxWaitMilliSeconds = 500;
#endif
    while (!isIdle_p()) {
        Q_ASSERT_X(state()->stateId() == WorkingHard, Q_FUNC_INFO, qPrintable(state()->stateName()));
        TWDEBUG(2, "WeaverImpl::finish: not done, waiting.\n");
        if (d()->jobFinished.wait(d()->mutex, MaxWaitMilliSeconds) == false) {
            TWDEBUG(2, "WeaverImpl::finish: wait timed out, %i jobs left, waking threads.\n", queueLength_p());
            reschedule();
        }
    }
    TWDEBUG(2, "WeaverImpl::finish: done.\n\n\n");
}

void Weaver::suspend()
{
    //FIXME?
    //QMutexLocker l(m_mutex); Q_UNUSED(l);
    state()->suspend();
}

void Weaver::suspend_p()
{
    //FIXME ?
}

void Weaver::resume()
{
    //FIXME?
    //QMutexLocker l(m_mutex); Q_UNUSED(l);
    state()->resume();
}

void Weaver::resume_p()
{
    //FIXME ?
}

bool Weaver::isEmpty() const
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    return state()->isEmpty();
}

bool Weaver::isEmpty_p() const
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    return  d()->assignments.isEmpty();
}

bool Weaver::isIdle() const
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    return state()->isIdle();
}

bool Weaver::isIdle_p() const
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    return isEmpty_p() && d()->active == 0;
}

int Weaver::queueLength() const
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    return state()->queueLength();
}

int Weaver::queueLength_p() const
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    return d()->assignments.count();
}

void Weaver::requestAbort()
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    return state()->requestAbort();
}

void Weaver::reschedule()
{
    d()->jobAvailable.wakeAll();
}

void Weaver::requestAbort_p()
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    for (int i = 0; i <d()-> inventory.size(); ++i) {
        d()->inventory[i]->requestAbort();
    }
}

/** @brief Adjust the inventory size.
 *
 * Requires that the mutex is being held when called.
 *
 * This method creates threads on demand. Threads in the inventory
 * are not created upon construction of the WeaverImpl object, but
 * when jobs are queued. This avoids costly delays on the application
 * startup time. Threads are created when the inventory size is under
 * inventoryMin and new jobs are queued.
 */
//TODO add code to raise inventory size over inventoryMin
//TODO add code to quit unnecessary threads
void Weaver::adjustInventory(int numberOfNewJobs)
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    //number of threads that can be created:
    const int reserve = d()->inventoryMax - d()->inventory.count();

    if (reserve > 0) {
        for (int i = 0; i < qMin(reserve, numberOfNewJobs); ++i) {
            Thread *th = createThread();
            th->moveToThread(th);   // be sane from the start
            d()->inventory.append(th);
            th->start();
            d()->createdThreads.ref();
            TWDEBUG(2, "WeaverImpl::adjustInventory: thread created, "
                  "%i threads in inventory.\n", currentNumberOfThreads_p());
        }
    }
}

Private::Weaver_Private *Weaver::d()
{
    return reinterpret_cast<Private::Weaver_Private*>(QueueSignals::d());
}

const Private::Weaver_Private *Weaver::d() const
{
    return reinterpret_cast<const Private::Weaver_Private*>(QueueSignals::d());
}

/** @brief Factory method to create the threads.
 *
 * Overload in adapted Weaver implementations.
 */
Thread *Weaver::createThread()
{
    return new Thread(this);
}

/** @brief Increment the count of active threads. */
void Weaver::incActiveThreadCount()
{
    adjustActiveThreadCount(1);
}

/** brief Decrement the count of active threads. */
void Weaver::decActiveThreadCount()
{
    adjustActiveThreadCount(-1);
    // the done job could have freed another set of jobs, and we do not know how
    // many - therefore we need to wake all threads:
    d()->jobFinished.wakeAll();
}

/** @brief Adjust active thread count.
 *
 * This is a helper function for incActiveThreadCount and decActiveThreadCount.
 */
void Weaver::adjustActiveThreadCount(int diff)
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    d()->active += diff;
    TWDEBUG(4, "WeaverImpl::adjustActiveThreadCount: %i active threads (%i jobs"
          " in queue).\n", d()->active,  queueLength_p());

    if (d()->assignments.isEmpty() && d()->active == 0) {
        P_ASSERT(diff < 0);    // cannot reach zero otherwise
        emit(finished());
    }
}

/** @brief Returns the number of active threads.
 *
 * Threads are active if they process a job. Requires that the mutex is being held when called.
 */
int Weaver::activeThreadCount()
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    return d()->active;
}

/** @brief Called from a new thread when entering the run method. */
void Weaver::threadEnteredRun(Thread *thread)
{
    d()->semaphore.release(1);
    emit threadStarted(thread);
}

/** @brief Take the first available job out of the queue and return it.
 *
 * The job will be removed from the queue (therefore, take). Only jobs that have no unresolved dependencies
 * are considered available. If only jobs that depened on other unfinished jobs are in the queue, this method
 * blocks on m_jobAvailable.
 *
 * This method will enter suspended state if the active thread count is now zero and
 * suspendIfAllThreadsInactive is true.
 * If justReturning is true, do not assign a new job, just process the completed previous one.
 */
JobPointer Weaver::takeFirstAvailableJobOrSuspendOrWait(Thread *th, bool threadWasBusy,
        bool suspendIfInactive, bool justReturning)
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    Q_ASSERT(threadWasBusy == false || (threadWasBusy == true && d()->active > 0));
    TWDEBUG(3, "WeaverImpl::takeFirstAvailableJobOrWait: trying to assign new job to thread %i (%s state).\n",
          th->id(), qPrintable(state()->stateName()));
    TWDEBUG(5, "WeaverImpl::takeFirstAvailableJobOrWait: %i active threads, was busy: %s, suspend: %s, assign new job: %s.\n",
          activeThreadCount(), threadWasBusy ? "yes" : "no", suspendIfInactive ? "yes" : "no", !justReturning ? "yes" : "no");
    if (threadWasBusy) {
        // cleanup and send events:
        decActiveThreadCount();
    }
    Q_ASSERT(d()->active >= 0);

    if (suspendIfInactive && d()->active == 0 && state()->stateId() == Suspending) {
        setState_p(Suspended);
        return JobPointer();
    }

    if (state()->stateId() != WorkingHard || justReturning) {
        return JobPointer();
    }

    JobPointer next;
    for (int index = 0; index < d()->assignments.size(); ++index) {
        const JobPointer &candidate = d()->assignments.at(index);
        if (d()->canBeExecuted(candidate)) {
            next = candidate;
            d()->assignments.removeAt(index);
            break;
        }
    }
    if (next) {
        incActiveThreadCount();
        TWDEBUG(3, "WeaverImpl::takeFirstAvailableJobOrWait: job %p assigned to thread %i (%s state).\n",
              next.data(), th->id(), qPrintable(state()->stateName()));
        return next;
    }

    blockThreadUntilJobsAreBeingAssigned_locked(th);
    return JobPointer();
}

/** @brief Assign a job to the calling thread.
 *
 * This is supposed to be called from the Thread objects in the inventory. Do not call this method from
 * your code.
 * Returns 0 if the weaver is shutting down, telling the calling thread to finish and exit. If no jobs are
 * available and shut down is not in progress, the calling thread is suspended until either condition is met.
 * @param wasBusy True if the thread is returning from processing a job
 */
JobPointer Weaver::applyForWork(Thread *th, bool wasBusy)
{
    return state()->applyForWork(th, wasBusy);
}

/** @brief Wait for a job to become available. */
void Weaver::waitForAvailableJob(Thread *th)
{
    state()->waitForAvailableJob(th);
}

/** @brief Blocks the calling thread until jobs can be assigned. */
void Weaver::blockThreadUntilJobsAreBeingAssigned(Thread *th)
{
    QMutexLocker l(d()->mutex); Q_UNUSED(l);
    blockThreadUntilJobsAreBeingAssigned_locked(th);
}

/** @brief Blocks the calling thread until jobs can be assigned.
 *
 * The mutex must be held when calling this method.
 */
void Weaver::blockThreadUntilJobsAreBeingAssigned_locked(Thread *th)
{
    Q_ASSERT(!d()->mutex->tryLock()); //mutex has to be held when this method is called
    TWDEBUG(4, "WeaverImpl::blockThreadUntilJobsAreBeingAssigned_locked: thread %i blocked (%s state).\n",
          th->id(), qPrintable(state()->stateName()));
    emit threadSuspended(th);
    d()->jobAvailable.wait(d()->mutex);
    TWDEBUG(4, "WeaverImpl::blockThreadUntilJobsAreBeingAssigned_locked: thread %i resumed  (%s state).\n",
          th->id(), qPrintable(state()->stateName()));
}
