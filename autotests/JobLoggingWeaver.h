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

#ifndef JOBLOGGINGWEAVER_H
#define JOBLOGGINGWEAVER_H

#include "src/weaver.h"
#include "src/jobpointer.h"
#include "JobLoggingDecorator.h"

class JobLoggingWeaver : public ThreadWeaver::Weaver
{
    Q_OBJECT
public:
    explicit JobLoggingWeaver(QObject* parent = nullptr);
    void enqueue(const QVector<ThreadWeaver::JobPointer> &jobs) override;

private:
    JobLoggingDecoratorCollector collector_;
};

#endif // JOBLOGGINGWEAVER_H
