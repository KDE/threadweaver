#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QFileInfoList>

#include "Progress.h"
#include "Image.h"

class Model : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        Role_SortRole = Qt::UserRole
    };

    explicit Model(QObject *parent = 0);

    void clear();
    void prepareConversions(const QFileInfoList& filenames, const QString& outputDirectory);
    bool computeThumbNailsBlockingInLoop();
    bool computeThumbNailsBlockingConcurrent();

    void queueUpConversion(const QStringList& files, const QString& outputDirectory);
    Progress progress() const;
    void progressChanged();
    void elementChanged(int id);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

Q_SIGNALS:
    void completed();
    void progress(int, int);
    void signalElementChanged(int);

private Q_SLOTS:
    void slotElementChanged(int id);

private:
    QVector<Image> m_images;
};

#endif // MODEL_H
