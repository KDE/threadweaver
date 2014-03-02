#ifndef JOBLOGGINGWEAVER_H
#define JOBLOGGINGWEAVER_H

#include "src/weaver.h"
#include "src/jobpointer.h"
#include "JobLoggingDecorator.h"

class JobLoggingWeaver : public ThreadWeaver::Weaver
{
    Q_OBJECT
public:
    explicit JobLoggingWeaver(QObject* parent = 0);
    void enqueue(const QVector<ThreadWeaver::JobPointer> &jobs) Q_DECL_OVERRIDE;

private:
    JobLoggingDecoratorCollector collector_;
};

#endif // JOBLOGGINGWEAVER_H
