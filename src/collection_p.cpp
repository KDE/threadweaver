/* -*- C++ -*-

This file is part of ThreadWeaver.

$ Author: Mirko Boehm $
$ Copyright: (C) 2004-2013 Mirko Boehm $
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
*/

#include "managedjobpointer.h"
#include "debuggingaids.h"
#include "queueapi.h"
#include "collection_p.h"
#include "collection.h"

namespace ThreadWeaver {

namespace Private {

Collection_Private::Collection_Private()
    : api(0)
    , jobCounter(0)
    , selfIsExecuting(false)
{
}

Collection_Private::~Collection_Private()
{
}

void Collection_Private::finalCleanup(Collection *collection)
{
    Q_ASSERT(!self.isNull());
    Q_ASSERT(!mutex.tryLock());
    freeQueuePolicyResources(self);
    if (collection->status() < Job::Status_Success) {
        collection->setStatus(Job::Status_Success);
    }
    api = 0;
}

void Collection_Private::enqueueElements()
{
    Q_ASSERT(!mutex.tryLock());
    prepareToEnqueueElements();
    jobCounter.fetchAndStoreOrdered(elements.count() + 1); //including self
    api->enqueue(elements);
}

void Collection_Private::elementStarted(Collection*, JobPointer job, Thread*)
{
    QMutexLocker l(&mutex); Q_UNUSED(l);
    Q_UNUSED(job) // except in Q_ASSERT
    Q_ASSERT(!self.isNull());
    if (jobsStarted.fetchAndAddOrdered(1) == 0) {
        //emit started() signal on beginning of first job execution
        selfExecuteWrapper.callBegin();
    }
}

namespace {
struct MutexUnlocker {
    QMutexLocker* locker;
    MutexUnlocker(QMutexLocker* l) : locker(l) { locker->unlock(); }
    ~MutexUnlocker() { locker->relock(); }
};
}

void Collection_Private::elementFinished(Collection *collection, JobPointer job, Thread *thread)
{
    JobPointer saveYourSelf = self; Q_UNUSED(saveYourSelf);
    QMutexLocker l(&mutex); Q_UNUSED(l);
    Q_ASSERT(!self.isNull());
    Q_UNUSED(job) // except in Q_ASSERT
    if (selfIsExecuting) {
        // the element that is finished is the collection itself
        // the collection is always executed first
        // queue the collection elements:
        enqueueElements();
        selfIsExecuting = false;
    }
    const int started = jobsStarted.loadAcquire();
    Q_ASSERT(started >= 0); Q_UNUSED(started);
    processCompletedElement(collection, job, thread);
    const int remainingJobs = jobCounter.fetchAndAddOrdered(-1) - 1;
    TWDEBUG(4, "Collection_Private::elementFinished: %i\n", remainingJobs);
    if (remainingJobs <= -1) {
        //its no use to count, the elements have been dequeued, now the threads call back that have been processing jobs in the meantime
    } else {
        collection->progressInterface()->setProgress(saveYourSelf, 1.0-((float)remainingJobs / (float)elements.size()));
        Q_ASSERT(remainingJobs >= 0);
        if (remainingJobs == 0) {
            // all elements can only be done if self has been executed:
            // there is a small chance that (this) has been dequeued in the
            // meantime, in this case, there is nothing left to clean up
            finalCleanup(collection);
            {
                MutexUnlocker u(&l); Q_UNUSED(u);
                selfExecuteWrapper.callEnd();
            }
            self.clear();
        }
    }
}

void Collection_Private::prepareToEnqueueElements()
{
    //empty in Collection
}

void Collection_Private::processCompletedElement(Collection*, JobPointer, Thread*)
{
    //empty in Collection
}

void Collection_Private::stop_locked(Collection *collection)
{
    Q_ASSERT(!mutex.tryLock());
    if (api != 0) {
        TWDEBUG(4, "Collection::stop: dequeueing %p.\n", collection);
        if (!api->dequeue(ManagedJobPointer<Collection>(collection))) {
            dequeueElements(collection, false);
        }
    }
}

void Collection_Private::dequeueElements(Collection* collection, bool queueApiIsLocked)
{
    // dequeue everything:
    Q_ASSERT(!mutex.tryLock());
    if (api == 0) {
        return;    //not queued
    }

    for (int index = 0; index < elements.size(); ++index) {
        bool result;
        if (queueApiIsLocked) {
            result = api->dequeue_p(elements.at(index));
        } else {
            result = api->dequeue(elements.at(index));
        }
        if (result) {
            jobCounter.fetchAndAddOrdered(-1);
        }
        TWDEBUG(3, "Collection::Private::dequeueElements: dequeueing %p (%s, %i jobs left).\n",
                (void *)elements.at(index).data(),
                result ? "found" : "not found",
                jobCounter.loadAcquire());
        elementDequeued(elements.at(index));
    }

    if (jobCounter.loadAcquire() == 1) {
        finalCleanup(collection);
    }
}

void CollectionSelfExecuteWrapper::begin(const JobPointer &job, Thread *thread)
{
    job_ = job;
    thread_ = thread;

}

void CollectionSelfExecuteWrapper::end(const JobPointer &job, Thread *thread)
{
    Q_ASSERT(job_ == job && thread_ == thread);
    Q_UNUSED(job); Q_UNUSED(thread); //except in assert
}

void CollectionSelfExecuteWrapper::callBegin()
{
    ExecuteWrapper::begin(job_, thread_);
}

void CollectionSelfExecuteWrapper::callEnd()
{
    ExecuteWrapper::end(job_, thread_);
    job_.clear();
}

}

}
