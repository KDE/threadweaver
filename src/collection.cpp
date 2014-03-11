/* -*- C++ -*-

This file implements the Collection class.

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

#include "collection.h"

#include "queueapi.h"
#include "debuggingaids.h"
#include "managedjobpointer.h"
#include "queueing.h"
#include "collection_p.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>

#include "dependencypolicy.h"
#include "executewrapper_p.h"
#include "thread.h"

namespace ThreadWeaver
{

class CollectionExecuteWrapper : public ExecuteWrapper
{
public:
    CollectionExecuteWrapper()
        : collection(0)
    {}

    void setCollection(Collection *collection_)
    {
        collection = collection_;
    }

    void begin(JobPointer job, Thread *thread) Q_DECL_OVERRIDE {
        ExecuteWrapper::begin(job, thread);
        Q_ASSERT(collection);
        collection->d()->elementStarted(collection, job, thread);
    }

    void end(JobPointer job, Thread *thread) Q_DECL_OVERRIDE {
        Q_ASSERT(collection);
        collection->d()->elementFinished(collection, job, thread);
        ExecuteWrapper::end(job, thread);
    }

    void cleanup(JobPointer job, Thread *) Q_DECL_OVERRIDE {
        //Once job is unwrapped from us, this object is dangling. Job::executor points to the next higher up execute wrapper.
        //It is thus safe to "delete this". By no means add any later steps after delete!
        delete unwrap(job);
    }

private:
    ThreadWeaver::Collection *collection;
};

Collection::Collection()
    : Job(new Private::Collection_Private)
{
    d()->selfExecuteWrapper.wrap(setExecutor(&d()->selfExecuteWrapper));
    CollectionExecuteWrapper *wrapper = new CollectionExecuteWrapper();
    wrapper->setCollection(this);
    wrapper->wrap(setExecutor(wrapper));
}

Collection::Collection(Private::Collection_Private *d__)
    : Job(d__)
{
    d()->selfExecuteWrapper.wrap(setExecutor(&d()->selfExecuteWrapper));
    CollectionExecuteWrapper *wrapper = new CollectionExecuteWrapper();
    wrapper->setCollection(this);
    wrapper->wrap(setExecutor(wrapper));
}

Collection::~Collection()
{
    // dequeue all remaining jobs:
    QMutexLocker l(mutex()); Q_UNUSED(l);
    if (d()->api != 0) { // still queued
        d()->dequeueElements(this, false);
    }
}

void Collection::addJob(JobPointer job)
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    REQUIRE(d()->api == 0 || d()->selfIsExecuting == true); // not queued yet or still running
    REQUIRE(job != 0);

    CollectionExecuteWrapper *wrapper = new CollectionExecuteWrapper();
    wrapper->setCollection(this);
    wrapper->wrap(job->setExecutor(wrapper));
    d()->elements.append(job);
}

void Collection::stop(JobPointer job)
{
    Q_UNUSED(job);
    QMutexLocker l(mutex()); Q_UNUSED(l);
    if (d()->api != 0) {
        debug(4, "Collection::stop: dequeueing %p.\n", (void *)this);
        if (!d()->api->dequeue(ManagedJobPointer<Collection>(this))) {
            d()->dequeueElements(this, false);
        }
    }
}

void Collection::aboutToBeQueued_locked(QueueAPI *api)
{
    Q_ASSERT(!mutex()->tryLock());
    Q_ASSERT(d()->api == 0); // never queue twice
    d()->api = api;
    Job::aboutToBeQueued_locked(api);
}

void Collection::aboutToBeDequeued_locked(QueueAPI *api)
{
    Q_ASSERT(!mutex()->tryLock());
    Q_ASSERT(api && d()->api == api);
    d()->dequeueElements(this, true);
    d()->api = 0;
    Job::aboutToBeDequeued_locked(api);
}

void Collection::execute(JobPointer job, Thread *thread)
{
    {
        QMutexLocker l(mutex()); Q_UNUSED(l);
        Q_ASSERT(d()->self.isNull());
        Q_ASSERT(d()->api != 0);
        d()->self = job;
        d()->selfIsExecuting = true; // reset in elementFinished
    }
    Job::execute(job, thread);
}

void Collection::run(JobPointer, Thread *)
{
    //empty
}

Private::Collection_Private *Collection::d()
{
    return reinterpret_cast<Private::Collection_Private*>(Job::d());
}

const Private::Collection_Private *Collection::d() const
{
    return reinterpret_cast<const Private::Collection_Private*>(Job::d());
}

JobPointer Collection::jobAt(int i)
{
    Q_ASSERT(!mutex()->tryLock());
    Q_ASSERT(i >= 0 && i < d()->elements.size());
    return d()->elements.at(i);
}

int Collection::elementCount() const
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    return jobListLength_locked();
}


int Collection::jobListLength() const
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    return jobListLength_locked();
}

int Collection::jobListLength_locked() const
{
    return d()->elements.size();
}

Collection &Collection::operator<<(JobInterface *job)
{
    addJob(make_job(job));
    return *this;
}

Collection &Collection::operator<<(const JobPointer &job)
{
    addJob(job);
    return *this;
}

Collection &Collection::operator<<(JobInterface &job)
{
    addJob(make_job_raw(&job));
    return *this;
}

}

