#include <QPixmap>

#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , m_image(new QLabel(this))
    , m_caption(new QLabel(tr("Hello Internet!"), m_image))
    , m_status(new QLabel(tr("Please wait..."), m_image))
{
    setFixedSize(800, 450);
    //set style sheet for the text labels:
    m_caption->setContentsMargins(9, 9, 9, 9);
    m_caption->setStyleSheet(QStringLiteral("background-color: rgba(255, 255, 255, 200); font: 14pt \"Serif\";"));
    m_status->setContentsMargins(9, 9, 9, 9);
    m_status->setStyleSheet(QStringLiteral("background-color: rgba(255, 255, 255, 200); font: 10pt \"Sans Serif\";"));
}

MainWidget::~MainWidget()
{
}

void MainWidget::resizeEvent(QResizeEvent *)
{
    m_image->setGeometry(0, 0, width(), height());
    m_caption->move(0, 0);
    m_caption->setFixedWidth(width());
    m_status->setFixedWidth(width());
    m_status->setFixedHeight(m_status->sizeHint().height());
    m_status->move(0, height()-m_status->height());
}

void MainWidget::setImage(QImage image)
{
    m_image->setPixmap(QPixmap::fromImage(image));
}

void MainWidget::setCaption(QString text)
{
    m_caption->setText(text);
}

void MainWidget::setStatus(QString text)
{
    m_status->setText(text);
}
