/* -*- C++ -*-

   This file declares the progress interface class

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

#ifndef PROGRESSINTERFACE_H
#define PROGRESSINTERFACE_H

#include "threadweaver_export.h"

#include "jobinterface.h"

namespace ThreadWeaver
{
	class THREADWEAVER_EXPORT ProgressInterface
	{
	public:
		virtual ~ProgressInterface() {}
		
		virtual void setProgress(const JobPointer& self, float progress) = 0;
		virtual void setProgressMessage(const JobPointer& self, const QString& message) = 0;
	};
}

#endif // PROGRESSINTERFACE_H
