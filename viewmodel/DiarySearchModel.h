#ifndef DIARYSEARCHMODEL_H
#define DIARYSEARCHMODEL_H

#include <QSortFilterProxyModel>
#include "DiaryListModel.h"

class DiarySearchModel : public QSortFilterProxyModel {
    Q_OBJECT
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
public:
    explicit DiarySearchModel(QObject* parent = nullptr);
    QString searchQuery() const;
    void setSearchQuery(const QString& query);
signals:
    void searchQueryChanged();
private:
    QString m_searchQuery;
};

#endif // DIARYSEARCHMODEL_H
