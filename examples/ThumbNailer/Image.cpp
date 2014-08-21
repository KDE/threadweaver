#include <QtDebug>
#include <QFile>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Exception>

#include "Image.h"
#include "Model.h"

Image::Image(const QString inputFileName, const QString outputFileName, Model *model, int id)
    : m_inputFileName(inputFileName)
    , m_outputFileName(outputFileName)
    , m_model(model)
    , m_id(id)
{
}

Progress Image::progress() const
{
    return qMakePair(m_progress, Step_NumberOfSteps);
}

QString Image::description() const
{
    const QString result = tr("[%1]: %2")
            .arg(m_progress.loadAcquire())
            .arg(inputFileName());
    return result;
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
        m_failedStep.store(Step_LoadFile);
        m_progress.storeRelease(Step_Complete);
        throw ThreadWeaver::JobFailed(tr("Unable to load input file!"));
    }
    m_imageData = file.readAll();
    m_progress.storeRelease(1);
    announceProgress();
}

void Image::loadImage()
{
    if (!m_image.loadFromData(m_imageData)) {
        m_failedStep.store(Step_LoadImage);
        m_progress.storeRelease(Step_Complete);
        throw ThreadWeaver::JobFailed(tr("Unable to parse image data!"));
    }
    m_imageData.clear();
    m_progress.storeRelease(2);
    announceProgress();
}

void Image::computeThumbNail()
{
    m_thumbnail = m_image.scaled(160, 100,  Qt::KeepAspectRatioByExpanding);
    if (m_thumbnail.isNull()) {
        m_failedStep.store(Step_ComputeThumbNail);
        m_progress.storeRelease(Step_Complete);
        throw ThreadWeaver::JobFailed(tr("Unable to compute thumbnail!"));
    }

    m_image = QImage();
    m_progress.storeRelease(3);
    announceProgress();
}

void Image::saveThumbNail()
{
    if (!m_thumbnail.save(m_outputFileName)) {
        m_failedStep.store(Step_SaveImage);
        m_progress.storeRelease(Step_Complete);
        throw ThreadWeaver::JobFailed(tr("Unable to save output file!"));
    }
    m_progress.storeRelease(4);
    announceProgress();
}

void Image::announceProgress()
{
    if (m_model) {
        m_model->progressChanged();
        m_model->elementChanged(m_id);
    }
}
