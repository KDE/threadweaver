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

#include "ViewController.h"
#include "MainWidget.h"

ViewController::ViewController(MainWidget *mainwidget)
    : QObject() // no parent
    , m_apiPostUrl(QStringLiteral("http://fickedinger.tumblr.com/api/read?id=94635924143"))
    , m_fullPostUrl(QStringLiteral("http://fickedinger.tumblr.com/post/94635924143/hello-developers-have-fun-using-the-kde"))
//@@snippet_begin(hellointernet-sequence)
{
    connect(this, SIGNAL(setImage(QImage)), 
	    mainwidget, SLOT(setImage(QImage)));
    connect(this, SIGNAL(setCaption(QString)), 
	    mainwidget, SLOT(setCaption(QString)));
    connect(this, SIGNAL(setStatus(QString)),
	    mainwidget, SLOT(setStatus(QString)));

    using namespace ThreadWeaver;
    auto s = new Sequence;
    *s << make_job( [=]() { loadPlaceholderFromResource(); } )
       << make_job( [=]() { loadPostFromTumblr(); } )
       << make_job( [=]() { loadImageFromTumblr(); } );
    stream() << s;
}
//@@snippet_end

ViewController::~ViewController()
{
    ThreadWeaver::Queue::instance()->finish();
}

//@@snippet_begin(hellointernet-loadresource)
void ViewController::loadPlaceholderFromResource()
{
    QThread::msleep(500);
    showResourceImage("IMG_20140813_004131.png");
    emit setStatus(tr("Downloading post..."));
}
//@@snippet_end

//@@snippet_begin(hellointernet-loadpost)
void ViewController::loadPostFromTumblr()
{
    const QUrl url(m_apiPostUrl);

    auto const data = download(url);
    emit setStatus(tr("Post downloaded..."));

    QDomDocument doc;
    if (!doc.setContent(data)) {
        error(tr("Post format not recognized!"));
    }

    auto textOfFirst = [&doc](const char* name) {
        auto const s = QString::fromLatin1(name);
        auto elements = doc.elementsByTagName(s);
        if (elements.isEmpty()) return QString();
        return elements.at(0).toElement().text();
    };

    auto const caption = textOfFirst("photo-caption");
    if (caption.isEmpty()) {
        error(tr("Post does not contain a caption!"));
    }
    emit setCaption(caption);
    auto const imageUrl = textOfFirst("photo-url");
    if (imageUrl.isEmpty()) {
        error(tr("Post does not contain an image!"));
    }
    m_imageUrl = QUrl(imageUrl);
    showResourceImage("IMG_20140813_004131-colors-cubed.png");
    emit setStatus(tr("Downloading image..."));
    QThread::msleep(500);
}
//@@snippet_end

void ViewController::loadImageFromTumblr()
{
    auto const data = download(m_imageUrl);
    emit setStatus(tr("Image downloaded..."));
    const QImage image=QImage::fromData(data);
    if (!image.isNull()) {
        emit setImage(image);
        emit setStatus(tr("Download complete (see %1).")
                       .arg(m_fullPostUrl));
    } else {
        error(tr("Image format error!"));
    }
}

QByteArray ViewController::download(const QUrl &url)
{
    QNetworkAccessManager manager;
    QEventLoop loop;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)),
                     &loop, SLOT(quit()));
    auto reply = manager.get(QNetworkRequest(url));
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        const QByteArray data = reply->readAll();
        return data;
    } else {
        error(tr("Unable to download data for \"%1\"!").
              arg(url.toString()));
        return QByteArray();
    }
}

//@@snippet_begin(hellointernet-error)
void ViewController::error(const QString &message)
{
    showResourceImage("IMG_20140813_004131-colors-cubed.png");
    emit setCaption(tr("Error"));
    emit setStatus(tr("%1").arg(message));
    throw ThreadWeaver::JobFailed(message);
}
//@@snippet_end

void ViewController::showResourceImage(const char* file)
{
    const QString path(QStringLiteral("://resources/%1")
                       .arg(QString::fromLatin1(file)));
    Q_ASSERT(QFile::exists(path));
    const QImage i(path);
    Q_ASSERT(!i.isNull());
    emit setImage(i);
}
