/* -*- C++ -*-

   This file contains a testsuite for the memory management in ThreadWeaver.

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

#include <QEventLoop>

#include "DeleteTest.h"

#include <ThreadWeaver/JobPointer>
#include <ThreadWeaver/QObjectDecorator>
#include <ThreadWeaver/Sequence>
#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/DebuggingAids>

#include "AppendCharacterJob.h"

class InstanceCountingCollection : public Collection
{
public:
    explicit InstanceCountingCollection()
        : Collection()
    {
        instances_.fetchAndAddAcquire(1);
    }

    ~InstanceCountingCollection()
    {
        instances_.fetchAndAddAcquire(-1);
    }

    static int instances()
    {
        return instances_.loadAcquire();
    }

private:
    static QAtomicInt instances_;
};

QAtomicInt InstanceCountingCollection::instances_;

class InstanceCountingSequence : public Sequence
{
public:
    explicit InstanceCountingSequence()
        : Sequence()
    {
        instances_.fetchAndAddAcquire(1);
    }

    ~InstanceCountingSequence()
    {
        instances_.fetchAndAddAcquire(-1);
    }

    static int instances()
    {
        return instances_.loadAcquire();
    }

private:
    static QAtomicInt instances_;
};

QAtomicInt InstanceCountingSequence::instances_;

DeleteTest::DeleteTest()
{
    ThreadWeaver::setDebugLevel(true, 1);
}

void DeleteTest::DeleteCollectionTest()
{
    const int NumberOfCollections = 100;
    m_finishCount.storeRelease(0);
    ThreadWeaver::Queue queue;
    queue.setMaximumNumberOfThreads(1);
    queue.suspend();
    for (int i = 0; i < NumberOfCollections; ++i) {
        QJobPointer col(new QObjectDecorator(new InstanceCountingCollection));
        col->collection()->addJob(JobPointer(new BusyJob));
        col->collection()->addJob(JobPointer(new BusyJob));
        QVERIFY(connect(col.data(), SIGNAL(done(ThreadWeaver::JobPointer)), SLOT(deleteCollection(ThreadWeaver::JobPointer))));

        queue.enqueue(col);
        m_finishCount.fetchAndAddRelease(1);
    }
    QCOMPARE(m_finishCount.loadAcquire(), NumberOfCollections);
    QEventLoop loop;
    QVERIFY(connect(this, SIGNAL(deleteCollectionTestCompleted()), &loop, SLOT(quit()), Qt::QueuedConnection));
    queue.resume();
    queue.finish();
    loop.exec();
    QCOMPARE(m_finishCount.loadAcquire(), 0);
    // The used Weaver instance needs to be shut down. The threads may still hold a reference to the previous job while
    // waiting for the next one or blocking because the queue is empty. If all threads have exited, no references to any jobs are
    // held anymore.
    queue.shutDown();
    QCOMPARE(InstanceCountingSequence::instances(), 0);
}

void DeleteTest::DeleteSequenceTest()
{
    const int NumberOfSequences = 100;
    m_finishCount.storeRelease(0);
    ThreadWeaver::Queue queue;
    queue.setMaximumNumberOfThreads(1);
    queue.suspend();
    for (int i = 0; i < NumberOfSequences; ++i) {
        QJobPointer seq(new QObjectDecorator(new InstanceCountingSequence));
        seq->sequence()->addJob(JobPointer(new BusyJob));
        seq->sequence()->addJob(JobPointer(new BusyJob));
        QVERIFY(connect(seq.data(), SIGNAL(done(ThreadWeaver::JobPointer)), SLOT(deleteSequence(ThreadWeaver::JobPointer))));

        queue.enqueue(seq);
        m_finishCount.fetchAndAddRelease(1);
    }
    QCOMPARE(m_finishCount.loadAcquire(), NumberOfSequences);
    QEventLoop loop;
    QVERIFY(connect(this, SIGNAL(deleteSequenceTestCompleted()), &loop, SLOT(quit()), Qt::QueuedConnection));
    queue.resume();
    queue.finish();
    loop.exec();
    QCOMPARE(m_finishCount.loadAcquire(), 0);
    // The used Weaver instance needs to be shut down. The threads may still hold a reference to the previous job while
    // waiting for the next one or blocking because the queue is empty. If all threads have exited, no references to any jobs are
    // held anymore.
    queue.shutDown();
    QCOMPARE(InstanceCountingSequence::instances(), 0);
}

void DeleteTest::deleteSequence(ThreadWeaver::JobPointer)
{
    if (m_finishCount.fetchAndAddRelease(-1) == 1) { // if it *was* 1...
        emit deleteSequenceTestCompleted();
    }
}

void DeleteTest::deleteCollection(JobPointer)
{
    if (m_finishCount.fetchAndAddRelease(-1) == 1) { // if it *was* 1...
        emit deleteCollectionTestCompleted();
    }
}

QMutex s_GlobalMutex;

QTEST_MAIN(DeleteTest)

