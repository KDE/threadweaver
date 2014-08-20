#include <algorithm> //for transform
#include <numeric>   //for accumulate

#include <QtDebug>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Exception>

#include "Model.h"

using namespace std;

Model::Model(QObject *parent) :
    QObject(parent)
{
}

void Model::prepareConversions(const QFileInfoList &filenames, const QString &outputDirectory)
{
    Q_ASSERT(m_images.isEmpty());
    m_images.resize(filenames.size());
    auto initializeImage = [=] (const QFileInfo& file) {
        auto const out = QFileInfo(outputDirectory, file.fileName()).absoluteFilePath();
        return Image(file.absoluteFilePath(), out, this);
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

Progress Model::progress() const
{
    auto sumItUp = [](const Progress& sum, const Image& image) {
        auto const values = image.progress();
        return qMakePair(sum.first + values.first,
                         sum.second + values.second);
    };
    auto const soFar = accumulate(m_images.begin(), m_images.end(),
                                  Progress(), sumItUp);
    return soFar;
}

void Model::progressChanged()
{
    //NI (will be used once this becomes a list model
    auto const p = progress();
    qDebug() << Q_FUNC_INFO << p.first << "/" << p.second;
}

