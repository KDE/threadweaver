#ifndef JOBLOGGINGDECORATOR_H
#define JOBLOGGINGDECORATOR_H

#include <QVector>
#include <QDateTime>
#include <QMutex>

#include "src/iddecorator.h"

class JobLoggingDecoratorCollector;

class JobLoggingDecorator : public IdDecorator
{
public:
    struct JobData {
        int threadId = int();
        QString description;
        QDateTime start;
        QDateTime end;
    };

    explicit JobLoggingDecorator(const JobPointer &job, JobLoggingDecoratorCollector* collector);
    void run(JobPointer self, Thread *thread) Q_DECL_OVERRIDE;
private:
    JobData data_;
    JobLoggingDecoratorCollector* collector_;
};

class JobLoggingDecoratorCollector {
public:
    void storeJobData(const JobData& data);

private:
    QVector<JobLoggingDecorator> jobData_;
    QMutex mutex_;
};

#endif // JOBLOGGINGDECORATOR_H
