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

#include "src/thread.h"

#include "JobLoggingDecorator.h"

using namespace ThreadWeaver;

JobLoggingDecorator::JobLoggingDecorator(const JobPointer &job, JobLoggingDecoratorCollector* collector)
    : IdDecorator(job.data(), false)
    , collector_(collector)
{
    Q_ASSERT(collector);
}

void JobLoggingDecorator::run(JobPointer self, Thread *thread)
{
    data_.start = collector_->time();
    if (thread) {
        data_.threadId = thread->id();
    } else {
        data_.threadId = -1;
    }
    IdDecorator::run(self, thread);
    data_.end = collector_->time();
    collector_->storeJobData(data_);
}


JobLoggingDecoratorCollector::JobLoggingDecoratorCollector()
{
    elapsed_.start();
    start_ = QDateTime::currentDateTime();
}

void JobLoggingDecoratorCollector::storeJobData(const JobLoggingDecorator::JobData &data)
{
    QMutexLocker m(&mutex_);
    jobData_.append(data);
}

qint64 JobLoggingDecoratorCollector::time()
{
    return elapsed_.nsecsElapsed();
}

