#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QObject>

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
};

#endif // VIEWCONTROLLER_H
