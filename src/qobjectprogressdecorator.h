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

#ifndef THREADWEAVER_QOBJECTPROGRESSDECORATOR_H
#define THREADWEAVER_QOBJECTPROGRESSDECORATOR_H

#include <QtCore/QObject>

#include "iddecorator.h"
#include "progressinterface.h"

namespace ThreadWeaver
{
class THREADWEAVER_EXPORT QObjectProgressDecorator : public QObject, public ProgressInterface, public IdDecorator
{
    Q_OBJECT
public:
    explicit QObjectProgressDecorator(JobInterface *decoratee, QObject *parent = 0);
    explicit QObjectProgressDecorator(JobInterface *decoratee, bool autoDelete, QObject *parent = 0);

    void setProgress(const JobPointer& self, float progress) Q_DECL_OVERRIDE;
    void setProgressMessage(const JobPointer& self, const QString& message) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void progressChanged(ThreadWeaver::JobPointer self, float progress);
    void progressMessageChanged(ThreadWeaver::JobPointer self, const QString& progressMessage);
};

typedef QSharedPointer<QObjectProgressDecorator> QProgressJobPointer;

}


#endif // THREADWEAVER_QOBJECTPROGRESSDECORATOR_H
