#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QAtomicInt>
#include <QCoreApplication>

#include "Progress.h"

/** @brief Image loads an image from a path, and then calculates and saves a thumbnail for it. */
class Image
{
    Q_DECLARE_TR_FUNCTIONS(Image)

public:
    enum Steps {
        Step_NotStarted,
        Step_LoadFile,
        Step_LoadImage,
        Step_ComputeThumbNail,
        Step_SaveImage,
        Step_NumberOfSteps = Step_SaveImage
    };

    Image(const QString inputFileName = QString(), const QString outputFileName = QString());
    Progress progress() const;
    const QString inputFileName() const;
    const QString outputFileName() const;

    void loadFile();
    void loadImage();
    void computeThumbNail();
    void saveThumbNail();

private:
    QString m_inputFileName;
    QString m_outputFileName;
    QAtomicInt m_progress;

    QByteArray m_imageData;
    QImage m_image;
    QImage m_thumbnail;
};

#endif // IMAGE_H
