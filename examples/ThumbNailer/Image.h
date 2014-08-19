#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QImage>

/** @brief Image loads an image from a path, and then calculates and saves a thumbnail for it. */
class Image : public QObject
{
    Q_OBJECT
public:
    Image(const QString inputFileName, const QString outputFileName, QObject* parent = 0);

    void loadFile();
    void loadImage();
    void computeThumbNail();
    void saveThumbnail();

Q_SIGNALS:
    void progress(int current, int total);

private:
    const QString m_inputFileName;
    const QString m_outputFileName;

    QByteArray m_imageData;
    QImage m_image;
    QImage m_thumbnail;
};

#endif // IMAGE_H
