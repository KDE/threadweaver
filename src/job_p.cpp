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

#include "job_p.h"
#include "executor_p.h"
#include "debuggingaids.h"
#include "queuepolicy.h"
#include "thread.h"

ThreadWeaver::Private::Job_Private::Job_Private()
    : mutex(QMutex::NonRecursive)
    , status(Job::Status_NoStatus)
    , executor(&defaultExecutor)
{
}

ThreadWeaver::Private::Job_Private::~Job_Private()
{}

void ThreadWeaver::Private::Job_Private::freeQueuePolicyResources(JobPointer job)
{
    for (int index = 0; index < queuePolicies.size(); ++index) {
        queuePolicies.at(index)->free(job);
    }
}


void ThreadWeaver::Private::DebugExecuteWrapper::execute(const JobPointer& job, ThreadWeaver::Thread *th)
{
    Q_ASSERT_X(job, Q_FUNC_INFO, "job may not be zero!");
    TWDEBUG(3, "DefaultExecuteWrapper::execute: executing job %p in thread %i.\n", job.data(), th ? th->id() : 0);
    executeWrapped(job, th);
    TWDEBUG(3, "Job::execute: finished execution of job in thread %i.\n", th ? th->id() : 0);
}


void ThreadWeaver::Private::DefaultExecutor::begin(const JobPointer &job, Thread *thread)
{
    defaultBegin(job, thread);
}

void ThreadWeaver::Private::DefaultExecutor::execute(const JobPointer& job, Thread *thread)
{
    run(job, thread);
}

void ThreadWeaver::Private::DefaultExecutor::end(const JobPointer& job, Thread *thread)
{
    defaultEnd(job, thread);
}

