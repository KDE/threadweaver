/* -*- C++ -*-

This file implements the Collection class.

$ Author: Mirko Boehm $
$ Copyright: (C) 2004-2013 Mirko Boehm $
$ Contact: mirko@kde.org
https://www.kde.org
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
#include "queueing.h"
#include "collection_p.h"

#include <QList>
#include <QObject>
#include <QPointer>

#include "dependencypolicy.h"
#include "executewrapper_p.h"
#include "thread.h"

namespace ThreadWeaver
{

class CollectionExecuteWrapper : public ExecuteWrapper
{
public:
    CollectionExecuteWrapper()
        : collection(nullptr)
    {}

    void setCollection(Collection *collection_)
    {
        collection = collection_;
    }

    void begin(const JobPointer& job, Thread *thread) override {
        TWDEBUG(4, "CollectionExecuteWrapper::begin: collection %p\n", collection);
        ExecuteWrapper::begin(job, thread);
        Q_ASSERT(collection);
        collection->d()->elementStarted(collection, job, thread);
        ExecuteWrapper::begin(job, thread);
    }

    void end(const JobPointer& job, Thread *thread) override {
        TWDEBUG(4, "CollectionExecuteWrapper::end: collection %p\n", collection);
        Q_ASSERT(collection);
        ExecuteWrapper::end(job, thread);
        collection->d()->elementFinished(collection, job, thread);
    }

    void cleanup(const JobPointer& job, Thread *) override {
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
}

Collection::Collection(Private::Collection_Private *d__)
    : Job(d__)
{
}

Collection::~Collection()
{
    MUTEX_ASSERT_UNLOCKED(mutex());
    // dequeue all remaining jobs:
    QMutexLocker l(mutex()); Q_UNUSED(l);
    if (d()->api != nullptr) { // still queued
        d()->dequeueElements(this, false);
    }
}

void Collection::addJob(JobPointer job)
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    REQUIRE(d()->api == nullptr || d()->selfIsExecuting == true); // not queued yet or still running
    REQUIRE(job != nullptr);

    CollectionExecuteWrapper *wrapper = new CollectionExecuteWrapper();
    wrapper->setCollection(this);
    wrapper->wrap(job->setExecutor(wrapper));
    d()->elements.append(job);
}

void Collection::stop(JobPointer job)
{
    Q_UNUSED(job);
    QMutexLocker l(mutex()); Q_UNUSED(l);
    d()->stop_locked(this);
}

void Collection::aboutToBeQueued_locked(QueueAPI *api)
{
    Q_ASSERT(!mutex()->tryLock());
    Q_ASSERT(d()->api == nullptr); // never queue twice
    d()->api = api;
    d()->selfExecuteWrapper.wrap(setExecutor(&d()->selfExecuteWrapper));
    CollectionExecuteWrapper *wrapper = new CollectionExecuteWrapper();
    wrapper->setCollection(this);
    wrapper->wrap(setExecutor(wrapper));
    Job::aboutToBeQueued_locked(api);
}

void Collection::aboutToBeDequeued_locked(QueueAPI *api)
{
    Q_ASSERT(!mutex()->tryLock());
    Q_ASSERT(api && d()->api == api);
    d()->dequeueElements(this, true);
    d()->api = nullptr;
    Job::aboutToBeDequeued_locked(api);
}

void Collection::execute(const JobPointer& job, Thread *thread)
{
    {
        QMutexLocker l(mutex()); Q_UNUSED(l);
        Q_ASSERT(d()->self.isNull());
        Q_ASSERT(d()->api != nullptr);
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


#if THREADWEAVER_BUILD_DEPRECATED_SINCE(5, 0)
int Collection::jobListLength() const
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    return jobListLength_locked();
}
#endif

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

