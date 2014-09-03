#ifndef AVERAGELOADMANAGER_H
#define AVERAGELOADMANAGER_H

#include <QObject>

class QTimer;

class AverageLoadManager : public QObject
{
    Q_OBJECT
public:
    explicit AverageLoadManager(QObject *parent = 0);

    void activate(bool enabled);
    bool available() const;

Q_SIGNALS:
    void recommendedWorkerCount(int);

private Q_SLOTS:
    void update();

private:
    QTimer* m_timer;

};

#endif // AVERAGELOADMANAGER_H
