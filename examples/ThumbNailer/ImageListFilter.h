/* -*- C++ -*-

   This file is part of ThreadWeaver.

   Author: Mirko Boehm
   Copyright: (C) 2005-2014 Mirko Boehm
   Contact: mirko@kde.org
         https://www.kde.org
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

#ifndef IMAGELISTFILTER_H
#define IMAGELISTFILTER_H

#include <QSortFilterProxyModel>
#include "Image.h"

class ImageListFilter : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit ImageListFilter(Image::Steps step, QObject* parent = nullptr);
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;
private:
    Image::Steps m_step;
};
#endif // IMAGELISTFILTER_H
