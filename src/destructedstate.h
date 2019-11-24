/* -*- C++ -*-

   This file declares the DestructedState class.

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

   $Id: DestructedState.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef DestructedState_H
#define DestructedState_H

#include "weaverimplstate.h"

namespace ThreadWeaver
{

class QueueSignals;

/** DestructedState is only active after the thread have been destroyed by
 *  the destructor, but before superclass destructors have finished.
 */
class DestructedState : public WeaverImplState
{
public:
    explicit DestructedState(QueueSignals *weaver);
    void shutDown() override;
    Weaver *weaver() override;
    const Weaver *weaver() const override;
    void setMaximumNumberOfThreads(int cap) override;
    int maximumNumberOfThreads() const override;
    int currentNumberOfThreads() const override;
    void enqueue(const QVector<JobPointer> &job) override;
    bool dequeue(const JobPointer &job) override;
    void dequeue() override;
    void finish() override;
    bool isEmpty() const override;
    bool isIdle() const override;
    int queueLength() const override;
    void requestAbort() override;
    void suspend() override;
    void resume() override;
    JobPointer applyForWork(Thread *th, bool wasBusy) override;
    void waitForAvailableJob(Thread *th) override;
    StateId stateId() const override;
};

}

#endif // DestructedState_H
