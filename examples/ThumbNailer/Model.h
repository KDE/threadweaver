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

#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QFileInfoList>

#include <ThreadWeaver/ResourceRestrictionPolicy>

#include "Progress.h"
#include "Image.h"

class Model : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        Role_SortRole = Qt::UserRole,
        Role_ImageRole,
        Role_StepRole
    };

    explicit Model(QObject *parent = nullptr);

    int fileLoaderCap() const;
    void setFileLoaderCap(int cap);

    int imageLoaderCap() const;
    void setImageLoaderCap(int cap);

    int computeThumbNailCap() const;
    void setComputeThumbNailCap(int cap);

    int saveThumbNailCap() const;
    void setSaveThumbNailCap(int cap);

    void clear();
    void prepareConversions(const QFileInfoList& filenames, const QString& outputDirectory);
    bool computeThumbNailsBlockingInLoop();
    bool computeThumbNailsBlockingConcurrent();

    void queueUpConversion(const QStringList& files, const QString& outputDirectory);
    Progress progress() const;
    void progressChanged();
    void elementChanged(int id);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void completed();
    void progress(int, int);
    void signalElementChanged(int);

private Q_SLOTS:
    void slotElementChanged(int id);

private:
    QVector<Image> m_images;
    ThreadWeaver::ResourceRestrictionPolicy m_fileLoaderRestriction;
    ThreadWeaver::ResourceRestrictionPolicy m_imageLoaderRestriction;
    ThreadWeaver::ResourceRestrictionPolicy m_imageScalerRestriction;
    ThreadWeaver::ResourceRestrictionPolicy m_fileWriterRestriction;
};

#endif // MODEL_H
