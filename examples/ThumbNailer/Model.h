#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVector>
#include <QFileInfoList>

#include "Progress.h"
#include "Image.h"

class Model : public QObject
{
    Q_OBJECT
public:

    explicit Model(QObject *parent = 0);

    void prepareConversionsForBenchmark(const QFileInfoList& filenames, const QString& outputDirectory);
    bool computeThumbNailsBlockingInLoop();
    bool computeThumbNailsBlockingConcurrent();

    void queueUpConversion(const QStringList& files, const QString& outputDirectory);
    Progress progress() const;
    void progressChanged();

Q_SIGNALS:
    void completed();
    void progress(int, int);

private:
    QVector<Image> m_images;
};

#endif // MODEL_H
