#ifndef WAITFORIDLEANDFINISHED_H
#define WAITFORIDLEANDFINISHED_H

namespace ThreadWeaver {
class Queue;
}

/** @brief Helper class for tests to ensure that after the scope if left, the queue is idle and resumed.
 * Upon destruction, ensure the weaver is idle and suspended.
 */
class WaitForIdleAndFinished
{
public:
    explicit WaitForIdleAndFinished(ThreadWeaver::Queue *weaver);
    ~WaitForIdleAndFinished();
private:
    ThreadWeaver::Queue *weaver_;
};

#endif // WAITFORIDLEANDFINISHED_H
