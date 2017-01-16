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

#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QAtomicInt>
#include <QCoreApplication>
#include <QReadWriteLock>

#include "Progress.h"

class Model;

/** @brief Image loads an image from a path, and then calculates and saves a thumbnail for it. */
class Image
{
    Q_DECLARE_TR_FUNCTIONS(Image)

public:
    enum Steps {
        Step_NotStarted,
        Step_LoadFile,
        Step_LoadImage,
        Step_ComputeThumbNail,
        Step_SaveThumbNail,
        Step_NumberOfSteps = Step_SaveThumbNail,
        Step_Complete = Step_SaveThumbNail
    };

    Image(const QString inputFileName = QString(), const QString outputFileName = QString(),
          Model* model = nullptr, int id = 0);
    Progress progress() const;
    QString description() const;
    QString details() const;
    QString details2() const;
    int processingOrder() const;

    const QString inputFileName() const;
    const QString outputFileName() const;
    QImage thumbNail() const;

    void loadFile();
    void loadImage();
    void computeThumbNail();
    void saveThumbNail();

    static const int ThumbHeight;
    static const int ThumbWidth;
private:
    void announceProgress(Steps step);
    void error(Steps step, const QString& message);

    QString m_inputFileName;
    QString m_outputFileName;
    QString m_description;
    QString m_details;
    QString m_details2;
    QAtomicInt m_progress;
    QAtomicInt m_failedStep;
    QAtomicInt m_processingOrder;

    QByteArray m_imageData;
    QImage m_image;
    QImage m_thumbnail;
    Model* m_model;
    int m_id;

    static QReadWriteLock Lock;
    static int ProcessingOrder;
};

Q_DECLARE_METATYPE(Image)
Q_DECLARE_METATYPE(const Image*)

#endif // IMAGE_H
