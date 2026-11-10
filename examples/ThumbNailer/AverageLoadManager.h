#ifndef AVERAGELOADMANAGER_H
#define AVERAGELOADMANAGER_H

#include <QObject>

#include <utility>

class QTimer;

/*!
 * \class AverageLoadManager
 *
 * \inmodule ThreadWeaver
 */
class AverageLoadManager : public QObject
{
    Q_OBJECT
public:
    /*!
     */
    explicit AverageLoadManager(QObject *parent = nullptr);

    /*!
     */
    void activate(bool enabled);
    /*!
     */
    bool available() const;
    /*!
     */
    std::pair<int, int> workersRange() const;

Q_SIGNALS:
    /*!
     */
    void recommendedWorkerCount(int);

private Q_SLOTS:
    /*!
     */
    void update();

private:
    QTimer *m_timer;
    int m_min, m_max;
};

#endif // AVERAGELOADMANAGER_H
