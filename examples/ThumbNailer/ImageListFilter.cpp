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

#include "ImageListFilter.h"

#include "Model.h"

ImageListFilter::ImageListFilter(Image::Steps step, QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_step(step)
{
    setSortRole(Model::Role_SortRole);
    setDynamicSortFilter(true);
    sort(0, Qt::AscendingOrder);
}

bool ImageListFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    const QModelIndex index(sourceModel()->index(source_row, 0, source_parent));
    const int step = index.data(Model::Role_StepRole).value<int>();
    return step == m_step;
}
