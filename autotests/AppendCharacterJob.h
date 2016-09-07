/* -*- C++ -*-

   Helper class for unit tests.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2005-2013 Mirko Boehm $
   $ Contact: mirko@kde.org
         http://www.kde.org
         http://creative-destruction.me $

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

#ifndef APPENDCHARACTER_JOB
#define APPENDCHARACTER_JOB

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include <ThreadWeaver/JobPointer>
#include <ThreadWeaver/Job>
#include <ThreadWeaver/DebuggingAids>
#include <ThreadWeaver/ProgressInterface>

// define in test binary:

extern QMutex s_GlobalMutex;

class AppendCharacterJob : public ThreadWeaver::Job
{
public:
    AppendCharacterJob(QChar c = QChar(), QString *stringref = 0)
        : ThreadWeaver::Job()
    {
        setValues(c, stringref);
    }

    void setValues(QChar c, QString *stringref)
    {
        m_c = c;
        m_stringref = stringref;
    }

    void run(ThreadWeaver::JobPointer, ThreadWeaver::Thread *) Q_DECL_OVERRIDE
    {
        QMutexLocker locker(&s_GlobalMutex);
        m_stringref->append(m_c);
        using namespace ThreadWeaver;
        TWDEBUG(3, "AppendCharacterJob::run: %c appended, result is %s.\n",
                m_c.toLatin1(), qPrintable(*m_stringref));
    }

    QChar character() const
    {
        return m_c;
    }

protected:
    QString *stringRef() const
    {
        return m_stringref;
    }

private:
    QChar m_c;
    QString *m_stringref;
};

class AppendCharacterJobWithProgress : public AppendCharacterJob
{
public:
  AppendCharacterJobWithProgress(QChar c = QChar(), QString *stringref = 0)
      : AppendCharacterJob(c, stringref)
  {
  }

  void run(ThreadWeaver::JobPointer job, ThreadWeaver::Thread* thread) Q_DECL_OVERRIDE
  {
      progressInterface()->setProgress(job, 0.0);
      progressInterface()->setProgressMessage(job, "Starting to append");
      AppendCharacterJob::run(job, thread);
      progressInterface()->setProgress(job, 1.0);
      progressInterface()->setProgressMessage(job, "Job done");
  }
};

class FailingAppendCharacterJob : public AppendCharacterJob
{
public:
    FailingAppendCharacterJob(QChar c = QChar(), QString *stringref = 0)
        : AppendCharacterJob(c, stringref)
    {
    }

    void run(ThreadWeaver::JobPointer job, ThreadWeaver::Thread* thread) Q_DECL_OVERRIDE
    {
        AppendCharacterJob::run(job, thread);
        setStatus(Job::Status_Failed);
    }
};

class BusyJob : public ThreadWeaver::Job
{
public:
    BusyJob()
        : ThreadWeaver::Job()
    {
        using namespace ThreadWeaver;
        TWDEBUG(3, "BusyJob ctor\n");
    }

    ~BusyJob()
    {
        using namespace ThreadWeaver;
        TWDEBUG(3, "~BusyJob\n");
    }

    void run(ThreadWeaver::JobPointer, ThreadWeaver::Thread *) Q_DECL_OVERRIDE
    {
        using namespace ThreadWeaver;
        TWDEBUG(3, "BusyJob: entered run()\n");
        for (int i = 0; i < 100; ++i) {
            int k = (i << 3) + (i >> 4);
            Q_UNUSED(k);
        }
    }
};

#endif

