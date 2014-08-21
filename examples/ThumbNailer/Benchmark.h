#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <QObject>
#include <QFileInfoList>

class Benchmark : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void processThumbNailsAsBenchmarkInLoop();
    void processThumbNailsAsBenchmarkWithThreadWeaver();

private:
    const QFileInfoList images();
};

#endif // BENCHMARK_H
