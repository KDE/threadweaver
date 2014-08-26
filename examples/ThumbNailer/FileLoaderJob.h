#ifndef FILELOADERJOB_H
#define FILELOADERJOB_H

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/ResourceRestrictionPolicy>

class Image;

class FileLoaderJob : public ThreadWeaver::Job
{
public:
    explicit FileLoaderJob(Image* image, ThreadWeaver::ResourceRestrictionPolicy* throttle);
    int priority() const override;
    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;

private:
    Image* m_image;

};

#endif // FILELOADERJOB_H
