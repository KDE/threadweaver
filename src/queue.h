/* -*- C++ -*-

   The Queue class in ThreadWeaver.

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

#ifndef THREADWEAVER_QUEUE_H
#define THREADWEAVER_QUEUE_H

#include <QObject>

#include "queuesignals.h"
#include "queuestream.h"

namespace ThreadWeaver
{

class Job;
class State;

/** @brief Queue implements a ThreadWeaver job queue.
 *
 * Queues process jobs enqueued in them by automatically assigning them to worker threads they manage.
 * Applications using ThreadWeaver can make use of a global Queue which is instantiated on demand, or
 * create multiple queues as needed. A job assigned to a queue will be processed by that specific queue.
 *
 * Worker threads are created by the queues as needed. To create a customized global queue,
 * see GlobalQueueFactory.
 *
 * @see GlobalQueueFactory
 * @see Queue::enqueue()
 * @see Queue::instance()
 */
class THREADWEAVER_EXPORT Queue : public QueueSignals
{
    Q_OBJECT
public:
    explicit Queue(QObject *parent = nullptr);
    /** @brief Construct a queue with a customized implementation
     * The queue takes ownership and will delete the implementation upon destruction. */
    explicit Queue(QueueSignals *implementation, QObject *parent = nullptr);
    ~Queue() override;

    QueueStream stream();

    const State *state() const override;

    void setMaximumNumberOfThreads(int cap) override;
    int maximumNumberOfThreads() const override;
    int currentNumberOfThreads() const override;

    static ThreadWeaver::Queue *instance();
    void enqueue(const QVector<JobPointer> &jobs) override;
    void enqueue(const JobPointer &job);
    bool dequeue(const JobPointer &) override;
    void dequeue() override;
    void finish() override;
    void suspend() override;
    void resume() override;
    bool isEmpty() const override;
    bool isIdle() const override;
    int queueLength() const override;
    void requestAbort() override;
    void reschedule() override;
    void shutDown() override;

    /** @brief Interface for the global queue factory. */
    struct GlobalQueueFactory {
        virtual ~GlobalQueueFactory() {}
        virtual Queue *create(QObject *parent) = 0;

    };
    static void setGlobalQueueFactory(GlobalQueueFactory *factory);
private:
    class Private;
    Private *const d;
};

}

#endif // THREADWEAVER_QUEUE_H
