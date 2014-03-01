#include "src/queue.h"

#include "WaitForIdleAndFinished.h"

using namespace ThreadWeaver;

WaitForIdleAndFinished::WaitForIdleAndFinished(Queue *weaver)
    : weaver_(weaver)
{
    Q_ASSERT(weaver);
    weaver_->finish();
    Q_ASSERT(weaver_->isIdle());
    weaver_->resume();
}

WaitForIdleAndFinished::~WaitForIdleAndFinished()
{
    weaver_->resume();
    weaver_->dequeue();
    weaver_->finish();
    weaver_->suspend();
    Q_ASSERT(weaver_->isIdle());
}
