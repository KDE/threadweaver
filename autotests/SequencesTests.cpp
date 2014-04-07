/* -*- C++ -*-

   This file contains a testsuite for sequences in ThreadWeaver.

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

#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/DebuggingAids>

#include "WaitForIdleAndFinished.h"
#include "AppendCharacterJob.h"
#include "SequencesTests.h"

using namespace ThreadWeaver;

QMutex s_GlobalMutex;

SequencesTests::SequencesTests()
{
    setDebugLevel(true, 2);
}

void SequencesTests::RecursiveStopTest()
{
    QSKIP("Broken in current master, won't finish", SkipAll);
    WAITFORIDLEANDFINISHED(Queue::instance());
    QString result;
    Sequence innerSequence;
    innerSequence << new AppendCharacterJob('b', &result)
                  << new FailingAppendCharacterJob('c', &result)
                  << new AppendCharacterJob('d', &result);

    Sequence outerSequence;
    outerSequence << new AppendCharacterJob('a', &result)
                  << innerSequence
                  << new AppendCharacterJob('e', &result);

    stream() << outerSequence;
    Queue::instance()->finish();
    QCOMPARE(result, QString::fromLatin1("abc"));
}

QTEST_MAIN(SequencesTests);
