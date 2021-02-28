/* -*- C++ -*-
    This file declares the Collection class.

    SPDX-FileCopyrightText: 2004-2013 Mirko Boehm <mirko@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef JOBCOLLECTION_H
#define JOBCOLLECTION_H

#include "job.h"
#include "jobpointer.h"

namespace ThreadWeaver
{
class Thread;
class CollectionExecuteWrapper;

namespace Private
{
class Collection_Private;
}

/** A Collection is a vector of Jobs that will be queued together.
 * In a Collection, the order of execution of the elements is not specified.
 *
 * It is intended that the collection is set up first and then
 * queued. After queuing, no further jobs should be added to the collection.
 */
class THREADWEAVER_EXPORT Collection : public Job
{
public:
    Collection();
    Collection(ThreadWeaver::Private::Collection_Private *d);
    ~Collection() override;

    /** Append a job to the collection.
     *
     * To use Collection, create the Job objects first, add them to the collection, and then queue it. After
     * the collection has been queued, no further Jobs are supposed to be added.
     *
     * @note Once the job has been added, execute wrappers can no more be set on it */
    virtual void addJob(JobPointer);

    /** Stop processing, dequeue all remaining Jobs.
     * job is supposed to be an element of the collection.
     */
    // FIXME remove job argument?
    void stop(ThreadWeaver::JobPointer job);

    /** Return the number of elements in the collection. */
    int elementCount() const;

#if THREADWEAVER_ENABLE_DEPRECATED_SINCE(5, 0)
    /** @deprecated Since 5.0, use elementCount(). */
    THREADWEAVER_DEPRECATED_VERSION(5, 0, "Use Collection::elementCount()")
    int jobListLength() const;
#endif

    /** @brief Add the job to this collection by pointer. */
    Collection &operator<<(ThreadWeaver::JobInterface *job);

    /** @brief Add the job to this collection. */
    Collection &operator<<(const ThreadWeaver::JobPointer &job);
    Collection &operator<<(JobInterface &job);

protected:
    /** Overload to queue the collection. */
    void aboutToBeQueued_locked(QueueAPI *api) override;

    /** Overload to dequeue the collection. */
    void aboutToBeDequeued_locked(QueueAPI *api) override;

    /** Return a ref-erence to the job in the job list at position i. */
    JobPointer jobAt(int i);

    // FIXME remove
    /** Return the number of jobs in the joblist.
     *  Assumes that the mutex is being held.
     */
    virtual int jobListLength_locked() const;

protected:
    /** Overload the execute method. */
    void execute(const JobPointer &job, Thread *) override;

    /** Overload run().
     * We have to. */
    void run(JobPointer self, Thread *thread) override;

protected:
    friend class CollectionExecuteWrapper; // needs to access d()
    friend class Collection_Private;
    ThreadWeaver::Private::Collection_Private *d();
    const ThreadWeaver::Private::Collection_Private *d() const;
};

}

#endif
