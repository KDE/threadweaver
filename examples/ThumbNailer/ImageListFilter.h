#ifndef IMAGELISTFILTER_H
#define IMAGELISTFILTER_H

#include <QSortFilterProxyModel>
#include "Image.h"

class ImageListFilter : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit ImageListFilter(Image::Steps step, QObject* parent = 0);
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
private:
    Image::Steps m_step;
};
#endif // IMAGELISTFILTER_H
