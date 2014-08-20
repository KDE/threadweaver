#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVector>
#include <QFileInfoList>

#include <Image.h>

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = 0);

    void prepareConversions(const QFileInfoList& filenames, const QString& outputDirectory);
    bool computeThumbNailsBlockingInLoop();
    bool computeThumbNailsBlockingConcurrent();

Q_SIGNALS:
    void completed();

private:
    QVector<Image> m_images;
};

#endif // MODEL_H
