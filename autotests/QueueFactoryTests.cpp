#include <QString>
#include <QtTest>

#include <ThreadWeaver.h>
#include <QueueAPI.h>
#include <QueueSignals.h>
#include <Queue.h>
#include <Weaver.h>
#include <IdDecorator.h>

using namespace ThreadWeaver;
QAtomicInt counter;

class CountingJobDecorator : public IdDecorator
{
public:
    explicit CountingJobDecorator(const JobPointer &job)
        : IdDecorator(job.data(), false)
        , original_(job)
    {}

    void run(JobPointer self, Thread *thread) Q_DECL_OVERRIDE {
        counter.fetchAndAddRelease(1);
        IdDecorator::run(self, thread);
        counter.fetchAndAddAcquire(1);
    }

    JobPointer original_;
};

class JobCountingWeaver : public Weaver
{
    Q_OBJECT
public:
    explicit JobCountingWeaver(QObject *parent = 0) : Weaver(parent) {}
    void enqueue(const QVector<JobPointer> &jobs) Q_DECL_OVERRIDE {
        QVector<JobPointer> decorated;
        std::transform(jobs.begin(), jobs.end(), std::back_inserter(decorated),
        [](const JobPointer & job)
        {
            return JobPointer(new CountingJobDecorator(job));
        });
        Weaver::enqueue(decorated);
    }
};

class CountingGlobalQueueFactory : public Queue::GlobalQueueFactory
{
    Queue *create(QObject *parent = 0) Q_DECL_OVERRIDE {
        return new Queue(new JobCountingWeaver, parent);
    }
};

int argc = 0;

class QueueFactoryTests : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testQueueFactory()
    {
        counter.storeRelease(0);
        QCoreApplication app(argc, (char **)0);
        Queue queue(new JobCountingWeaver(this));
        queue.enqueue(make_job([]() {}));  // nop
        queue.finish();
        QCOMPARE(counter.loadAcquire(), 2);
    }

    void testGlobalQueueFactory()
    {
        Queue::setGlobalQueueFactory(new CountingGlobalQueueFactory());
        QCoreApplication app(argc, (char **)0);
        counter.storeRelease(0);
        Queue::instance()->enqueue(make_job([]() {}));  // nop
        Queue::instance()->finish();
        QCOMPARE(counter.loadAcquire(), 2);
    }
};

QTEST_APPLESS_MAIN(QueueFactoryTests)

#include "QueueFactoryTests.moc"
