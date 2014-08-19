#include <QFile>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Exception>

#include "Image.h"

Image::Image(const QString inputFileName, const QString outputFileName, QObject *parent)
    : QObject(parent)
    , m_inputFileName(inputFileName)
    , m_outputFileName(outputFileName)
{
}

void Image::loadFile()
{
    QFile file(m_inputFileName);
    if (file.open(QIODevice::ReadOnly)) {
        throw ThreadWeaver::Exception(tr("Unable to load input file!"));
    }
    m_imageData = file.readAll();
    emit progress(1, 4);
}

void Image::loadImage()
{
    if (!m_image.loadFromData(m_imageData)) {
        throw ThreadWeaver::Exception(tr("Unable to parse image data!"));
    }
    emit progress(2, 4);
}

void Image::computeThumbNail()
{
    m_thumbnail = m_image.scaled(160, 100,  Qt::KeepAspectRatioByExpanding);
    emit progress(3, 4);
}

void Image::saveThumbnail()
{
    if (!m_thumbnail.save(m_outputFileName)) {
        throw ThreadWeaver::Exception(tr("Unable to save output file!"));
    }
    emit progress(4, 4);
}
