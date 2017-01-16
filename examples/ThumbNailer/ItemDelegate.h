/* -*- C++ -*-

   This file declares the SMIVItemDelegate class.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2005, Mirko Boehm $
   $ Contact: mirko@kde.org
         http://www.kde.org
         http://www.hackerbuero.org $
   $ License: LGPL with the following explicit clarification:
         This code may be linked against any version of the Qt toolkit
         from Trolltech, Norway. $

   $Id: SMIVItemDelegate.h 30 2005-08-16 16:16:04Z mirko $
*/

#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QItemDelegate>
#include <QtCore/QSize>

class ItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ItemDelegate(QObject* parent = nullptr);
    static const int FrameWidth;
    static const int TextMargin;
    static const int Margin;
private:
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const Q_DECL_OVERRIDE;
};

#endif // SMIVITEMDELEGATE
