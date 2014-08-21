#ifndef PRIORITYDECORATOR_H
#define PRIORITYDECORATOR_H

#include <ThreadWeaver/ThreadWeaver>

class PriorityDecorator : public ThreadWeaver::IdDecorator {
public:
    explicit PriorityDecorator(int priority, ThreadWeaver::JobInterface* job, bool autoDelete = true);

    //FIXME temp
    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread);

    int priority() const override;

private:
    int m_priority;
};

#endif // PRIORITYDECORATOR_H
