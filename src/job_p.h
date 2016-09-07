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

#ifndef JOB_P_H
#define JOB_P_H

#include <QMutex>
#include "executewrapper_p.h"
#include "progressinterface.h"

namespace ThreadWeaver {

namespace Private {

class THREADWEAVER_EXPORT DefaultExecutor : public ThreadWeaver::Executor
{
public:
    void begin(const JobPointer& job, Thread *thread) Q_DECL_OVERRIDE;
    void execute(const JobPointer& job, Thread *thread) Q_DECL_OVERRIDE;
    void end(const JobPointer& job, Thread *thread) Q_DECL_OVERRIDE;
};

class THREADWEAVER_EXPORT DefaultProgressInterface : public ThreadWeaver::ProgressInterface
{
public:
    virtual ~DefaultProgressInterface() {}
    void setProgress(const JobPointer& /*self*/, float /*progress*/) {}
    void setProgressMessage(const JobPointer& /*self*/, const QString& /*message*/) {}
};

extern DefaultExecutor defaultExecutor;
extern DefaultProgressInterface defaultProgressInterface;

class DebugExecuteWrapper : public ThreadWeaver::ExecuteWrapper
{
public:
    void execute(const JobPointer &job, ThreadWeaver::Thread *th) Q_DECL_OVERRIDE;
};

class Job_Private
{
public:
    Job_Private();
    virtual ~Job_Private();

    /** Free the queue policies acquired before this job has been executed. */
    virtual void freeQueuePolicyResources(JobPointer);

    /* The list of QueuePolicies assigned to this Job. */
    QList<QueuePolicy *> queuePolicies;

    mutable QMutex mutex;
    /* @brief The status of the Job. */
    QAtomicInt status;

    /** The Executor that will execute this Job. */
    QAtomicPointer<Executor> executor;

    /** The progress Interface of this Job. */
    QAtomicPointer<ProgressInterface> progressInterface;

    //FIXME What is the correct KDE frameworks no debug switch?
#if !defined(NDEBUG)
    /** DebugExecuteWrapper for logging of Job execution. */
    DebugExecuteWrapper debugExecuteWrapper;
#endif
};

}

}

#endif // JOB_P_H
