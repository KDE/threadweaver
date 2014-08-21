#include <QFile>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Exception>

#include "Image.h"
#include "Model.h"

Image::Image(const QString inputFileName, const QString outputFileName, Model *model)
    : m_inputFileName(inputFileName)
    , m_outputFileName(outputFileName)
    , m_model(model)
{
}

Progress Image::progress() const
{
    return qMakePair(m_progress, Step_NumberOfSteps);
}

const QString Image::inputFileName() const
{
    return m_inputFileName;
}

const QString Image::outputFileName() const
{
    return m_outputFileName;
}

void Image::loadFile()
{
    QFile file(m_inputFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        throw ThreadWeaver::JobFailed(tr("Unable to load input file!"));
    }
    m_imageData = file.readAll();
    m_progress.storeRelease(1);
    announceProgress();
}

void Image::loadImage()
{
    if (!m_image.loadFromData(m_imageData)) {
        throw ThreadWeaver::JobFailed(tr("Unable to parse image data!"));
    }
    m_imageData.clear();
    m_progress.storeRelease(2);
    announceProgress();
}

void Image::computeThumbNail()
{
    m_thumbnail = m_image.scaled(160, 100,  Qt::KeepAspectRatioByExpanding);
    m_image = QImage();
    m_progress.storeRelease(3);
    announceProgress();
}

void Image::saveThumbNail()
{
    if (!m_thumbnail.save(m_outputFileName)) {
        throw ThreadWeaver::JobFailed(tr("Unable to save output file!"));
    }
    m_progress.storeRelease(4);
    announceProgress();
}

void Image::announceProgress()
{
    if (m_model) m_model->progressChanged();
}
