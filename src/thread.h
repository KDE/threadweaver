/* -*- C++ -*-

   This file is part of ThreadWeaver. It declares the Thread class.

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

#ifndef THREADWEAVER_THREAD_H
#define THREADWEAVER_THREAD_H

#include <QtCore/QMutex>
#include <QtCore/QThread>

#include "threadweaver_export.h"
#include "jobpointer.h"

namespace ThreadWeaver
{

class Job;
class Weaver;

/** @brief Thread represents a worker thread in a Queue's inventory.
 *
 * Threads are created and managed by queues on demand. A Thread will try to retrieve and process
 * jobs from the queue until it is told to exit. */
class THREADWEAVER_EXPORT Thread : public QThread
{
    Q_OBJECT
public:
    /** @brief Create a thread.
     *
     *  @param parent the parent Weaver
     */
    explicit Thread(Weaver *parent = nullptr);

    /** The destructor. */
    ~Thread();

    /** @brief The run method is reimplemented to execute jobs from the queue.
     *
     * Whenever the thread is idle, it will ask its Weaver parent for a Job to do. The Weaver will either return a Job or a null
     * pointer. When a null pointer is returned, it tells the thread to exit.
     */
    void run() Q_DECL_OVERRIDE;

    /** @brief Returns the thread id.
     *
     * This id marks the respective Thread object, and must therefore not be confused with, e.g., the pthread thread ID.
     * The way threads are implemented and identified is platform specific. id() is the only way to uniquely identify a thread
     * within ThreadWeaver.
     */
    unsigned int id() const;

    /** @brief Request the abortion of the job that is processed currently.
     *
     * If there is no current job, this method will do nothing, but can safely be called. It forwards the request to the
     * current Job.
     */
    void requestAbort();

Q_SIGNALS:
    //FIXME needed?
    /** The thread has been started. */
    void started(ThreadWeaver::Thread *);

private:
    class Private;
    Private *const d;
};

}

#endif

