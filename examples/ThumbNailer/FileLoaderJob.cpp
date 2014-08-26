#include <QMutexLocker>

#include "Image.h"
#include "FileLoaderJob.h"

using namespace ThreadWeaver;

FileLoaderJob::FileLoaderJob(Image *image, ThreadWeaver::ResourceRestrictionPolicy *throttle)
    : m_image(image)
{
    QMutexLocker l(mutex());
    assignQueuePolicy(throttle);
}

int FileLoaderJob::priority() const
{
    return Image::Step_LoadFile;
}

void FileLoaderJob::run(JobPointer, Thread*)
{
    Q_ASSERT(m_image);
    m_image->loadFile();
}
