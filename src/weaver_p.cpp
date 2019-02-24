/* -*- C++ -*-

This file is part of ThreadWeaver.

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

*/

#include <QMutexLocker>

#include "debuggingaids.h"
#include "weaver_p.h"
#include "suspendedstate.h"
#include "suspendingstate.h"
#include "destructedstate.h"
#include "workinghardstate.h"
#include "shuttingdownstate.h"
#include "inconstructionstate.h"
#include "queuepolicy.h"

namespace ThreadWeaver {

namespace Private {

Weaver_Private::Weaver_Private()
    : QueueSignals_Private()
    , active(0)
    , inventoryMax(qMax(4, 2 * QThread::idealThreadCount()))
    , mutex(new QMutex(QMutex::NonRecursive))

{
}

Weaver_Private::~Weaver_Private()
{
    //FIXME no need for dynamic allocation
    delete mutex;
}

/** @brief Dump the current jobs to the console.
 *
 * Use at your own risk.
 */
void Weaver_Private::dumpJobs()
{
    QMutexLocker l(mutex); Q_UNUSED(l);
    TWDEBUG(0, "WeaverImpl::dumpJobs: current jobs:\n");
    for (int index = 0; index < assignments.size(); ++index) {
        TWDEBUG(0, "--> %4i: %p (priority %i, can be executed: %s)\n", index, (void *)assignments.at(index).data(),
                assignments.at(index)->priority(),
                canBeExecuted(assignments.at(index)) ? "yes" : "no");
    }
}

/** @brief Check with the assigned queue policies if the job can be executed.
 *
 * If it returns true, it expects that the job is executed right after that. The done() methods of the
 * queue policies will be automatically called when the job is finished.
 *
 * If it returns false, all queue policy resources have been freed, and the method can be called again
 * at a later time.
 */
bool Weaver_Private::canBeExecuted(JobPointer job)
{
    Q_ASSERT(!mutex->tryLock()); //mutex has to be held when this method is called

    QList<QueuePolicy *> acquired;

    bool success = true;

    QMutexLocker l(job->mutex());
    QList<QueuePolicy *> policies = job->queuePolicies();
    if (!policies.isEmpty()) {
        TWDEBUG(4, "WeaverImpl::canBeExecuted: acquiring permission from %i queue %s.\n",
              policies.size(), policies.size() == 1 ? "policy" : "policies");
        for (int index = 0; index < policies.size(); ++index) {
            if (policies.at(index)->canRun(job)) {
                acquired.append(policies.at(index));
            } else {
                success = false;
                break;
            }
        }

        TWDEBUG(4, "WeaverImpl::canBeExecuted: queue policies returned %s.\n", success ? "true" : "false");

        if (!success) {
            for (int index = 0; index < acquired.size(); ++index) {
                acquired.at(index)->release(job);
            }
        }
    } else {
        TWDEBUG(4, "WeaverImpl::canBeExecuted: no queue policies, this job can be executed.\n");
    }
    return success;
}

void Weaver_Private::deleteExpiredThreads()
{
    Q_ASSERT(!mutex->tryLock()); //mutex has to be held when this method is called
    for (Thread* thread : qAsConst(expiredThreads)) {
        thread->wait();
        delete thread;
    }
    expiredThreads.clear();
}

}

}
