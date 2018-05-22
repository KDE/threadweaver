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

#ifndef SEQUENCE_P_H
#define SEQUENCE_P_H

#include <QAtomicInt>

#include "sequence.h"
#include "queuepolicy.h"
#include "collection_p.h"

namespace ThreadWeaver {

namespace Private {

class BlockerPolicy : public QueuePolicy {
public:
    bool canRun(JobPointer) override;
    void free(JobPointer) override;
    void release(JobPointer) override;
    void destructed(JobInterface *job) override;
};

class Sequence_Private : public Collection_Private
{
public:
    Sequence_Private();
    BlockerPolicy* blocker();
    void prepareToEnqueueElements() override;
    void processCompletedElement(Collection* collection, JobPointer job, Thread *thread) override;
    void elementDequeued(const JobPointer& job) override;
    BlockerPolicy blocker_;
    QAtomicInt completed_;
};

}

}

#endif // SEQUENCE_P_H
