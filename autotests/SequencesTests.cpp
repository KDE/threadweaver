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
