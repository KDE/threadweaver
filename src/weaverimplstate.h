/* -*- C++ -*-

   This file declares the StateIMplementation class.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2005-2013 Mirko Boehm $
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

   $Id$
*/

#ifndef WEAVERIMPLSTATE_H
#define WEAVERIMPLSTATE_H

#include "state.h"
#include "weaver.h"

namespace ThreadWeaver
{

class QueueSignals;

/** @brief Base class for all WeaverImpl states. */
class WeaverImplState : public State
{
public:
    explicit WeaverImplState(QueueSignals *weaver);

    const State *state() const override;

    /** Shut down the queue. */
    void shutDown() override;
    /** Set the maximum number of threads this Weaver object may start. */
    void setMaximumNumberOfThreads(int cap) override;
    /** Get the maximum number of threads this Weaver may start. */
    int maximumNumberOfThreads() const override;
    /** Returns the current number of threads in the inventory. */
    int currentNumberOfThreads() const override;
    /** Enqueue a job. */
    void enqueue(const QVector<JobPointer> &jobs) override;
    /** Dequeue a job. */
    bool dequeue(const JobPointer &job) override;
    /** Dequeue all jobs. */
    void dequeue() override;
    /** Finish all queued jobs. */
    void finish() override;
    /** Are no more jobs queued? */
    bool isEmpty() const override;
    /** Are all threads waiting? */
    bool isIdle() const override;
    /** How many jobs are currently queued? */
    int queueLength() const override;
    /** Request abort for all queued and currently executed jobs. */
    void requestAbort() override;
    /** Reschedule jobs to threads. */
    void reschedule() override;
    /** Wait (by suspending the calling thread) until a job becomes available. */
    void waitForAvailableJob(Thread *th) override;

protected:
    /** Provide correct return type for WeaverImpl states. */
    Weaver *weaver() override;
    const Weaver *weaver() const override;
};

}

#endif
