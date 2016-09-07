/* -*- C++ -*-

   A decorator to make jobs progress into QObjects in ThreadWeaver.

   $ Author: Marcus Tillmanns $
   $ Copyright: (C) 2016-2017 Marcus Tillmanns $
   $ Contact: maddimax@gmail.com
   $

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

#include "qobjectprogressdecorator.h"

#include <QtCore/QDebug>

namespace ThreadWeaver 
{
	
QObjectProgressDecorator::QObjectProgressDecorator(JobInterface* decoratee, QObject* parent)
  : QObject(parent)
  , IdDecorator(decoratee)
{
    decoratee->setProgressInterface(this);
}

QObjectProgressDecorator::QObjectProgressDecorator(JobInterface* decoratee, bool autoDelete, QObject* parent)
  : QObject(parent)
  , IdDecorator(decoratee, autoDelete)
{
    decoratee->setProgressInterface(this);
}

void QObjectProgressDecorator::setProgress(const JobPointer& self, float progress)
{
    Q_EMIT progressChanged(self, progress);
}

void QObjectProgressDecorator::setProgressMessage(const JobPointer& self, const QString &message)
{
    Q_EMIT progressMessageChanged(self, message);
}

}
