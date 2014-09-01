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

#include <algorithm> //for transform
#include <numeric>   //for accumulate

#include <QtDebug>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QMutexLocker>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Exception>
#include <ThreadWeaver/DebuggingAids>

#include "Model.h"
#include "PriorityDecorator.h"
#include "FileLoaderJob.h"

using namespace std;
using namespace ThreadWeaver;

Model::Model(QObject *parent)
    : QAbstractListModel(parent)
    , m_fileLoaderRestriction(4)
    , m_imageLoaderRestriction(4)
    , m_imageScalerRestriction(4)
    , m_fileWriterRestriction(4)
{
    ThreadWeaver::setDebugLevel(true, 0);
    connect(this, SIGNAL(signalElementChanged(int)), this, SLOT(slotElementChanged(int)));
}

int Model::fileLoaderCap() const
{
    return m_fileLoaderRestriction.cap();
}

void Model::setFileLoaderCap(int cap)
{
    m_fileLoaderRestriction.setCap(cap);
    Queue::instance()->reschedule();
}

int Model::imageLoaderCap() const
{
    return m_imageLoaderRestriction.cap();
}

void Model::setImageLoaderCap(int cap)
{
    m_imageLoaderRestriction.setCap(cap);
    Queue::instance()->reschedule();
}

int Model::computeThumbNailCap() const
{
    return m_imageScalerRestriction.cap();
}

void Model::setComputeThumbNailCap(int cap)
{
    m_imageScalerRestriction.setCap(cap);
}

int Model::saveThumbNailCap() const
{
    return m_fileWriterRestriction.cap();
}

void Model::setSaveThumbNailCap(int cap)
{
    m_imageScalerRestriction.setCap(cap);
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
        auto loadImage = [&image]() { image.loadImage(); };
        auto loadImageJob = new Lambda<decltype(loadImage)>(loadImage);
        {
            QMutexLocker l(loadImageJob->mutex());
            loadImageJob->assignQueuePolicy(&m_imageLoaderRestriction);
        }

        auto computeThumbNail = [&image]() { image.computeThumbNail(); };
        auto computeThumbNailJob = new Lambda<decltype(computeThumbNail)>(computeThumbNail);
        {
            QMutexLocker l(computeThumbNailJob->mutex());
            computeThumbNailJob->assignQueuePolicy(&m_imageScalerRestriction);
        }

        auto saveThumbNail = [&image]() { image.saveThumbNail(); };
        auto saveThumbNailJob = new Lambda<decltype(saveThumbNail)>(saveThumbNail);
        {
            QMutexLocker l(saveThumbNailJob->mutex());
            saveThumbNailJob->assignQueuePolicy(&m_fileWriterRestriction);
        }

        auto sequence = new Sequence();
        *sequence << new FileLoaderJob(&image, &m_fileLoaderRestriction)
                  << new PriorityDecorator(Image::Step_LoadImage, loadImageJob)
                  << new PriorityDecorator(Image::Step_ComputeThumbNail, computeThumbNailJob)
                  << new PriorityDecorator(Image::Step_SaveThumbNail, saveThumbNailJob);
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
    const Image& image = m_images.at(index.row());
    if (role == Qt::DisplayRole) {
        return image.description();
    } else if (role == Role_SortRole) {
        return image.processingOrder();
    } else if (role == Role_ImageRole) {
        return QVariant::fromValue(&image);
    } else if (role == Role_StepRole) {
        return QVariant::fromValue(image.progress().first);
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

