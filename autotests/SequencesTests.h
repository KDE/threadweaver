#ifndef SEQUENCESTESTS_H
#define SEQUENCESTESTS_H

#include <QObject>

class SequencesTests : public QObject
{
    Q_OBJECT

public:
    SequencesTests();

private Q_SLOTS:
    void RecursiveStopTest();
};

#endif // SEQUENCESTESTS_H
