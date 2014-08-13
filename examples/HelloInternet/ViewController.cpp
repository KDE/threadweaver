#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QUrl>
#include <QThread>
#include <QByteArray>
#include <QDomDocument>
#include <QFile>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Exception>
//#include <threadweaver/DebuggingAids.h>

#include "ViewController.h"
#include "MainWidget.h"

ViewController::ViewController(MainWidget *mainwidget)
    : QObject() // no parent
{
    connect(this, SIGNAL(setImage(QImage)), mainwidget, SLOT(setImage(QImage)));
    connect(this, SIGNAL(setCaption(QString)), mainwidget, SLOT(setCaption(QString)));
    connect(this, SIGNAL(setStatus(QString)), mainwidget, SLOT(setStatus(QString)));

    //ThreadWeaver::setDebugLevel(true, 3);
    using namespace ThreadWeaver;
    auto sequence = new Sequence;
    *sequence << make_job( [=]() { loadPlaceholderFromResource(); } )
              << make_job( [=]() { loadPostFromTumblr(); } )
              << make_job( [=]() { loadImageFromTumblr(); } );
    stream() << sequence;
}

ViewController::~ViewController()
{
    ThreadWeaver::Queue::instance()->finish();
}

void ViewController::loadPlaceholderFromResource()
{
    QThread::msleep(1000);
    auto const path = QStringLiteral("://resources/IMG_20140813_004131.png");
    Q_ASSERT(QFile::exists(path));
    const QImage i(path);
    Q_ASSERT(!i.isNull());
    emit setImage(i);
    emit setStatus(tr("Downloading post..."));
}

void ViewController::loadPostFromTumblr()
{
    const QUrl url(QStringLiteral
                   ("http://fickedinger.tumblr.com/api/read?post=94635924143&num=1"));

    auto const data = download(url);
    emit setStatus(tr("Post downloaded..."));

    QDomDocument doc;
    if (!doc.setContent(data)) {
        return;
    }

    auto textOfFirst = [&doc](const QString& name) {
        auto elements = doc.elementsByTagName(name);
        if (elements.isEmpty()) return QString();
        return elements.at(0).toElement().text();
    };

    auto const caption = textOfFirst(QStringLiteral("photo-caption"));
    if (caption.isEmpty()) {
        throw ThreadWeaver::JobFailed(tr("Post does not contain a caption!"));
    }
    emit setCaption(caption);
    auto const imageUrl = textOfFirst(QStringLiteral("photo-url"));
    if (imageUrl.isEmpty()) {
        throw ThreadWeaver::JobFailed(tr("Post does not contain an image!"));
    }
    m_imageUrl = QUrl(imageUrl);
    emit setStatus(tr("Downloading image..."));
}

void ViewController::loadImageFromTumblr()
{
    auto const data = download(m_imageUrl);
    emit setStatus(tr("Image downloaded..."));
    const QImage image=QImage::fromData(data);
    if (!image.isNull()) {
        emit setImage(image);
        emit setStatus(tr("Download complete."));
    } else {
        throw ThreadWeaver::JobFailed(tr("Image format error!"));
    }
}

QByteArray ViewController::download(const QUrl &url)
{
    QNetworkAccessManager manager;
    QEventLoop loop;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    auto reply = manager.get(QNetworkRequest(url));
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        const QByteArray data = reply->readAll();
        return data;
    } else {
        throw ThreadWeaver::JobFailed(tr("Unable to download data for \"%1\"!").arg(url.toString()));
    }
}
