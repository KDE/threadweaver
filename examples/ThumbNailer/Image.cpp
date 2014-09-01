/* -*- C++ -*-

   This file is part of ThreadWeaver.

   Author: Mirko Boehm
   Copyright: (C) 2005-2014 Mirko Boehm
   Contact: mirko@kde.org
         http://www.kde.org
         http://creative-destruction.me

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QtDebug>
#include <QFile>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Exception>

#include "Image.h"
#include "Model.h"

//const int Image::ThumbHeight = 75;
//const int Image::ThumbWidth = 120;
const int Image::ThumbHeight = 60;
const int Image::ThumbWidth = 80;
QReadWriteLock Image::Lock;
int Image::ProcessingOrder;

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

int Image::processingOrder() const
{
    return m_processingOrder.loadAcquire();
}

const QString Image::inputFileName() const
{
    return m_inputFileName;
}

const QString Image::outputFileName() const
{
    return m_outputFileName;
}

QImage Image::thumbNail() const
{
    QReadLocker r(&Lock);
    return m_thumbnail;
}

void Image::loadFile()
{
    m_processingOrder.storeRelease(ProcessingOrder++);
    QFile file(m_inputFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        error(Step_LoadFile, tr("Unable to load input file!"));
    }
    m_imageData = file.readAll();
    m_progress.storeRelease(1);
    announceProgress();
}

void Image::loadImage()
{
    m_processingOrder.storeRelease(ProcessingOrder++);
    if (!m_image.loadFromData(m_imageData)) {
        error(Step_LoadImage, tr("Unable to parse image data!"));
    }
    m_imageData.clear();
    m_progress.storeRelease(2);
    announceProgress();
}

void Image::computeThumbNail()
{
    m_processingOrder.storeRelease(ProcessingOrder++);
    QImage thumb = m_image.scaled(ThumbWidth, ThumbHeight,  Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    if (thumb.isNull()) {
        error(Step_ComputeThumbNail, tr("Unable to compute thumbnail!"));
    }
    {   // thumb is implicitly shared, no copy:
        QWriteLocker l(&Lock);
        m_thumbnail = thumb;
    }

    m_image = QImage();
    m_progress.storeRelease(3);
    announceProgress();
}

void Image::saveThumbNail()
{
    QImage thumb;
    {
        QReadLocker r(&Lock);
        thumb = m_thumbnail;
    }
    if (!thumb.save(m_outputFileName)) {
        error(Step_SaveThumbNail, tr("Unable to save output file!"));
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

void Image::error(Image::Steps step, const QString &message)
{
    m_failedStep.store(step);
    m_progress.storeRelease(Step_Complete);
    throw ThreadWeaver::JobFailed(message);
}
