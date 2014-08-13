#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QObject>
#include <QUrl>
#include <QByteArray>

class MainWidget;

class ViewController : public QObject
{
    Q_OBJECT
public:
    explicit ViewController(MainWidget* mainwidget);
    ~ViewController();

Q_SIGNALS:
    void setImage(QImage image);
    void setCaption(QString text);
    void setStatus(QString text);

private:
    void loadPlaceholderFromResource();
    void loadPostFromTumblr();
    void loadImageFromTumblr();

    QByteArray download(const QUrl& url);

    QUrl m_imageUrl;
    const QString m_apiPostUrl;
    const QString m_fullPostUrl;
};

#endif // VIEWCONTROLLER_H
