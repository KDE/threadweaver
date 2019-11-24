/* -*- C++ -*-

   This file declares the ShuttingDownState class.

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

   $Id: ShuttingDownState.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef ShuttingDownState_H
#define ShuttingDownState_H

#include "weaverimplstate.h"

namespace ThreadWeaver
{

class Queue;

/** ShuttingDownState is enabled when the Weaver destructor is entered. It
 *  prevents threads from still accessing queue management methods, and new jobs being queued.
 */
class ShuttingDownState : public WeaverImplState
{
public:
    explicit ShuttingDownState(QueueSignals *weaver);

    /** Shut down the queue. */
    void shutDown() override;
    /** Suspend job processing. */
    void suspend() override;
    /** Resume job processing. */
    void resume() override;
    /** Assign a job to an idle thread. */
    JobPointer applyForWork(Thread *th, bool wasBusy) override;
    /** Wait (by suspending the calling thread) until a job becomes available. */
    void waitForAvailableJob(Thread *th) override;
    /** reimpl */
    StateId stateId() const override;
};

}

#endif // ShuttingDownState_H
