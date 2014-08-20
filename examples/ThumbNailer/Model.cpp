#include <algorithm>

#include <QtDebug>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Exception>

#include "Model.h"

Model::Model(QObject *parent) :
    QObject(parent)
{
}

void Model::prepareConversions(const QFileInfoList &filenames, const QString &outputDirectory)
{
    using namespace std;
    Q_ASSERT(m_images.isEmpty());
    m_images.resize(filenames.size());
    auto initializeImage = [=] (const QFileInfo& file) {
        return Image(file.absoluteFilePath(),
                     QFileInfo(outputDirectory, file.fileName()).absoluteFilePath());
    };
    transform(filenames.begin(), filenames.end(), m_images.begin(), initializeImage);
}

bool Model::computeThumbNailsBlockingInLoop()
{
    for(Image& image : m_images) {
        try {
            image.loadFile();
            image.loadImage();
            image.computeThumbNail();
            image.saveThumbNail();

        } catch (const ThreadWeaver::Exception& ex) {
            qDebug() << ex.message();
            return false;
        }
    }
    return true;
}

bool Model::computeThumbNailsBlockingConcurrent()
{
    using namespace ThreadWeaver;
    auto queue = stream();
    for(Image& image : m_images) {
        auto sequence = new Sequence();
        *sequence << make_job( [&image]() { image.loadFile(); } );
        *sequence << make_job( [&image]() { image.loadImage(); } );
        *sequence << make_job( [&image]() { image.computeThumbNail(); } );
        *sequence << make_job( [&image]() { image.saveThumbNail(); } );
        queue << sequence;
    }
    queue.flush();
    Queue::instance()->finish();
    // figure out result:
    for(auto image : m_images) {
        if (image.progress().first != Image::Step_NumberOfSteps) return false;
    }
    return true;
}

