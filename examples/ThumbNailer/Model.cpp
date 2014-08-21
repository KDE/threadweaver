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
using namespace ThreadWeaver;

Model::Model(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, SIGNAL(signalElementChanged(int)), this, SLOT(slotElementChanged(int)));
}

void Model::clear()
{
    beginResetModel();
    m_images.clear();
    endResetModel();
}

void Model::prepareConversions(const QFileInfoList &filenames, const QString &outputDirectory)
{
    beginResetModel();
    Q_ASSERT(m_images.isEmpty());
    m_images.resize(filenames.size());
    int counter = 0;
    auto initializeImage = [=, &counter] (const QFileInfo& file) {
        auto const out = QFileInfo(outputDirectory, file.fileName()).absoluteFilePath();
        return Image(file.absoluteFilePath(), out, this, counter++);
    };
    transform(filenames.begin(), filenames.end(), m_images.begin(), initializeImage);
    endResetModel();
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

void Model::queueUpConversion(const QStringList &files, const QString &outputDirectory)
{
    QFileInfoList fileInfos;
    transform(files.begin(), files.end(), back_inserter(fileInfos),
              [](const QString& file) { return QFileInfo(file); } );
    prepareConversions(fileInfos, outputDirectory);
    //FIXME duplicated code
    auto queue = stream();
    for(Image& image : m_images) {
        auto sequence = new Sequence();
        *sequence << make_job( [&image]() { image.loadFile(); } );
        *sequence << make_job( [&image]() { image.loadImage(); } );
        *sequence << make_job( [&image]() { image.computeThumbNail(); } );
        *sequence << make_job( [&image]() { image.saveThumbNail(); } );
        queue << sequence;
    }
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
    auto const p = progress();
    emit progress(p.first, p.second);
}

void Model::elementChanged(int id)
{
    signalElementChanged(id);
}

int Model::rowCount(const QModelIndex &parent) const
{
    return m_images.size();
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (index.row() < 0 || index.row() >= rowCount()) return QVariant();
    if (role == Qt::DisplayRole) {
        return m_images.at(index.row()).description();
    }
    return QVariant();
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

void Model::slotElementChanged(int id)
{
    if (id >= 0 && id < m_images.count()) {
        auto const i = index(id, 0);
        emit dataChanged(i, i);
    }
}

