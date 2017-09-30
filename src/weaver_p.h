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
    ~Weaver_Private() override;

    void dumpJobs();

    bool canBeExecuted(JobPointer);
    void deleteExpiredThreads();

    /** The thread inventory. */
    QList<Thread *> inventory;
    /** Threads that have exited and can be deleted. */
    QList<Thread *> expiredThreads;
    /** The job queue. */
    QList<JobPointer> assignments;
    /** The number of jobs that are assigned to the worker threads, but not finished. */
    int active;
    /** The maximum number of worker threads. */
    int inventoryMax;
    /** Wait condition all idle or done threads wait for. */
    QWaitCondition jobAvailable;
    /** Wait for a job to finish. */
    QWaitCondition jobFinished;
    /** Mutex to serialize operations. */
    QMutex *mutex;
    /** Semaphore to ensure thread startup is in sequence. */
    QSemaphore semaphore;
    /** Before shutdown can proceed to close the running threads, it needs to ensure that all of them
     *  entered the run method. */
    QAtomicInt createdThreads;
    /** The state of the art.
    * @see StateId
    */
    QAtomicPointer<State> state;
    /** The state objects. */
    QSharedPointer<State> states[NoOfStates];
};

}

}

#endif // WEAVER_P_H
