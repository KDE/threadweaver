/* -*- C++ -*-

   This file implements the SMIVItemDelegate class.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2005, Mirko Boehm $
   $ Contact: mirko@kde.org
         http://www.kde.org
         http://www.hackerbuero.org $
   $ License: LGPL with the following explicit clarification:
         This code may be linked against any version of the Qt toolkit
         from Trolltech, Norway. $

   $Id: SMIVItemDelegate.cpp 30 2005-08-16 16:16:04Z mirko $
*/

#include <QtCore/QModelIndex>
#include <QPainter>

#include "ItemDelegate.h"

const int ItemDelegate::FrameWidth = 2;
const int ItemDelegate::ThumbHeight = 60;
const int ItemDelegate::ThumbWidth = 80;
const int ItemDelegate::TextMargin = 6;
const int ItemDelegate::Margin = 3;

ItemDelegate::ItemDelegate( QObject *parent )
    : QItemDelegate ( parent )
{
}

void ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem & ,
                                 const QModelIndex & ) const
{
    static const int Width = ThumbWidth + 2*FrameWidth + 2*Margin + 2;
    static const int Height = ThumbHeight + 2*FrameWidth + 2*Margin + 2;
    return QSize(Width, Height);
}

