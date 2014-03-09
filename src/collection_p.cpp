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
    collection->setStatus(Job::Status_Success);
    api = 0;
}

void Collection_Private::enqueueElements()
{
    Q_ASSERT(!mutex.tryLock());
    prepareToEnqueueElements();
    jobCounter.fetchAndStoreOrdered(elements.count() + 1); //including self
    api->enqueue(elements);
}

void Collection_Private::elementStarted(Collection *collection, JobPointer job, Thread *thread)
{
    Q_UNUSED(job) // except in Q_ASSERT
    Q_ASSERT(!self.isNull());
    if (jobsStarted.fetchAndAddOrdered(1) == 0) {
        //emit started() signal on beginning of first job execution
        collection->executor()->defaultBegin(self, thread);
    }
}

void Collection_Private::prepareToEnqueueElements()
{
    //empty in Collection
}

void Collection_Private::dequeueElements(Collection* collection, bool queueApiIsLocked)
{
    // dequeue everything:
    Q_ASSERT(!mutex.tryLock());
    if (api == 0) {
        return;    //not queued
    }

    for (int index = 0; index < elements.size(); ++index) {
        debug(4, "Collection::Private::dequeueElements: dequeueing %p.\n", (void *)elements.at(index).data());
        if (queueApiIsLocked) {
            api->dequeue_p(elements.at(index));
        } else {
            api->dequeue(elements.at(index));
        }
    }

    const int jobCount = jobCounter.fetchAndStoreAcquire(0);
    if (jobCount != 0) {
        // if jobCounter is not zero, then we where waiting for the
        // last job to finish before we would have freed our queue
        // policies. In this case we have to do it here:
        finalCleanup(collection);
    }
}

void CollectionSelfExecuteWrapper::begin(JobPointer, Thread *) {
}

void CollectionSelfExecuteWrapper::end(JobPointer, Thread *) {
}

}

}
