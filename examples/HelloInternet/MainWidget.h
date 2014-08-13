#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QImage>

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    void resizeEvent(QResizeEvent *);

public Q_SLOTS:
    void setImage(QImage image);
    void setCaption(QString text);
    void setStatus(QString text);

private:
    QLabel* m_image;
    QLabel* m_caption;
    QLabel* m_status;
};

#endif // MAINWIDGET_H
