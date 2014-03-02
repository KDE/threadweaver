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
        int threadId = int();
        QString description;
        qint64 start;
        qint64 end;
    };

    explicit JobLoggingDecorator(const ThreadWeaver::JobPointer &job, JobLoggingDecoratorCollector* collector);
    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) Q_DECL_OVERRIDE;
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
