#include "DiarySearchModel.h"

DiarySearchModel::DiarySearchModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setFilterRole(DiaryListModel::TitleRole);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

QString DiarySearchModel::searchQuery() const
{
    return m_searchQuery;
}

void DiarySearchModel::setSearchQuery(const QString& query)
{
    if (m_searchQuery == query)
        return;
    m_searchQuery = query;
    emit searchQueryChanged();
    if (query.isEmpty()) {
        setFilterWildcard("");
    } else {
        setFilterWildcard("*" + query + "*");
    }
}
