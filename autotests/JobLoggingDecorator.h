/* -*- C++ -*-

This file is part of ThreadWeaver.

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

*/

#ifndef JOBLOGGINGDECORATOR_H
#define JOBLOGGINGDECORATOR_H

#include <QElapsedTimer>
#include <QVector>
#include <QDateTime>
#include <QMutex>

#include "src/iddecorator.h"

class JobLoggingDecoratorCollector;

class JobLoggingDecorator : public ThreadWeaver::IdDecorator
{
public:
    struct JobData {
        int threadId;
        QString description;
        qint64 start;
        qint64 end;
        JobData(): threadId(0) {}
    };

    explicit JobLoggingDecorator(const ThreadWeaver::JobPointer &job, JobLoggingDecoratorCollector* collector);
    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;
private:
    JobData data_;
    JobLoggingDecoratorCollector* collector_;
};

class JobLoggingDecoratorCollector {
public:
    JobLoggingDecoratorCollector();
    void storeJobData(const JobLoggingDecorator::JobData& data);
    qint64 time();

private:
    QVector<JobLoggingDecorator::JobData> jobData_;
    QElapsedTimer elapsed_;
    QDateTime start_;
    QMutex mutex_;
};

#endif // JOBLOGGINGDECORATOR_H
