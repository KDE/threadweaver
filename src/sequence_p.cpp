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

#include "sequence_p.h"
#include "debuggingaids.h"

namespace ThreadWeaver {

namespace Private {

Sequence_Private::Sequence_Private()
{
}

BlockerPolicy *Sequence_Private::blocker()
{
    return &blocker_;
}

void Sequence_Private::prepareToEnqueueElements()
{
    Q_ASSERT(!mutex.tryLock());
    const int jobs = elements.count();
    // probably incorrect:
    completed_.storeRelease(0);
    // block the execution of the later jobs:
    for (int i = 0; i < jobs; ++i) {
        TWDEBUG(4, "Sequence_Private::processCompletedElement: blocking %p\n", elements.at(i).data());
        JobPointer nextJob = elements.at(i);
        QMutexLocker l(nextJob->mutex());
        nextJob->assignQueuePolicy(blocker());
    }
}

void Sequence_Private::processCompletedElement(Collection* collection, JobPointer job, Thread*)
{
    Q_ASSERT(!mutex.tryLock());
    Q_ASSERT(job != 0);
    Q_ASSERT(!self.isNull());
    const JobInterface::Status status = job->status();
    if (status != JobInterface::Status_Success) {
        stop_locked(collection);
        collection->setStatus(status);
    }
    const int next = completed_.fetchAndAddAcquire(1);
    const int count = elements.count();
    if (count > 0) {
        if (next < count) {
            TWDEBUG(4, "Sequence_Private::processCompletedElement: unblocking %p\n", elements.at(next).data());
            JobPointer nextJob = elements.at(next);
            QMutexLocker l(nextJob->mutex());
            nextJob->removeQueuePolicy(blocker());
        }
    }

}

void Sequence_Private::elementDequeued(const JobPointer &job)
{
    Q_ASSERT(!mutex.tryLock());
    QMutexLocker l(job->mutex());
    job->removeQueuePolicy(blocker());
}

void BlockerPolicy::destructed(JobInterface*)
{
}

bool BlockerPolicy::canRun(JobPointer)
{
    return false;
}

void BlockerPolicy::free(JobPointer)
{
}

void BlockerPolicy::release(JobPointer)
{
}

}

}
