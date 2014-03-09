/* -*- C++ -*-

   This file implements the Sequence class.

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

   $Id: DebuggingAids.h 30 2005-08-16 16:16:04Z mirko $
*/

#include "sequence.h"
#include "sequence_p.h"
#include "managedjobpointer.h"
#include "queueapi.h"
#include "debuggingaids.h"
#include "dependencypolicy.h"

namespace ThreadWeaver
{

Sequence::Sequence()
    : Collection(new Private::Sequence_Private())
{
}

//FIXME move to d
//FIXME add blocker when elements are added?
void Sequence::enqueueElements()
{
    Q_ASSERT(!mutex()->tryLock());
    const int jobs = jobListLength_locked();
    // probably incorrect:
    d()->completed_.storeRelease(0);
    // block the execution of the later jobs:
    for (int i = 0; i < jobs; ++i) {
        jobAt(i)->assignQueuePolicy(d()->blocker());
    }
    Collection::enqueueElements();
}

Private::Sequence_Private *Sequence::d()
{
    return reinterpret_cast<Private::Sequence_Private*>(Collection::d());
}

const Private::Sequence_Private *Sequence::d() const
{
    return reinterpret_cast<const Private::Sequence_Private*>(Collection::d());
}

void Sequence::elementFinished(JobPointer job, Thread *thread)
{
    REQUIRE(job != 0);

    JobPointer s(self());
    Q_ASSERT(!s.isNull());
    Collection::elementFinished(job, thread);
    if (!job->success()) {
        stop(job);
    }
    QMutexLocker l(mutex()); Q_UNUSED(l);
    const int next = d()->completed_.fetchAndAddAcquire(1);
    const int count = elementCount();
    if (count > 0) {
        if (next < count) {
            jobAt(next)->removeQueuePolicy(d()->blocker());
        }
    }
}

}

