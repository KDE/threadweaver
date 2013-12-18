#ifndef APPENDCHARACTER_JOB
#define APPENDCHARACTER_JOB

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include <JobPointer.h>
#include <Job.h>
#include <DebuggingAids.h>

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

    void run(ThreadWeaver::JobPointer, ThreadWeaver::Thread *)
    {
        QMutexLocker locker(&s_GlobalMutex);
        m_stringref->append(m_c);
        using namespace ThreadWeaver;
        debug(3, "AppendCharacterJob::run: %c appended, result is %s.\n",
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

class FailingAppendCharacterJob : public AppendCharacterJob
{
public:
    FailingAppendCharacterJob(QChar c = QChar(), QString *stringref = 0)
        : AppendCharacterJob(c, stringref)
    {
    }

    bool success() const
    {
        return false;
    }
};

class BusyJob : public ThreadWeaver::Job
{
public:
    BusyJob()
        : ThreadWeaver::Job()
    {
    }

    ~BusyJob()
    {
//        using namespace ThreadWeaver;
//        debug(0, "~BusyJob\n");
    }

    void run(ThreadWeaver::JobPointer, ThreadWeaver::Thread *)
    {
//        debug(0, "BusyJob: entered run()\n");
        for (int i = 0; i < 100; ++i) {
            int k = (i << 3) + (i >> 4);
            Q_UNUSED(k);
        }
    }
};

#endif

