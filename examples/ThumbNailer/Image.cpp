#include <QFile>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Exception>

#include "Image.h"

Image::Image(const QString inputFileName, const QString outputFileName)
    : m_inputFileName(inputFileName)
    , m_outputFileName(outputFileName)
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
        throw ThreadWeaver::Exception(tr("Unable to load input file!"));
    }
    m_imageData = file.readAll();
    m_progress.storeRelease(1);
}

void Image::loadImage()
{
    if (!m_image.loadFromData(m_imageData)) {
        throw ThreadWeaver::Exception(tr("Unable to parse image data!"));
    }
    m_progress.storeRelease(2);
}

void Image::computeThumbNail()
{
    m_thumbnail = m_image.scaled(160, 100,  Qt::KeepAspectRatioByExpanding);
    m_progress.storeRelease(3);
}

void Image::saveThumbNail()
{
    if (!m_thumbnail.save(m_outputFileName)) {
        throw ThreadWeaver::Exception(tr("Unable to save output file!"));
    }
    m_progress.storeRelease(4);
}
