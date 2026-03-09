#include "DiaryListModel.h"
#include <QDebug>

DiaryListModel::DiaryListModel(DiaryManager& manager, QObject *parent)
    : QAbstractListModel(parent), m_manager(manager) {}

void DiaryListModel::loadEntries() {
    qDebug() << "DiaryListModel: Fetching decrypted metadata from engine...";

    // begin/end ResetModel are MANDATORY. They tell QML "Stop drawing, the data is changing!"
    beginResetModel();
    m_entries = m_manager.readEntrySummaries();
    endResetModel();

    qDebug() << "DiaryListModel: Loaded" << m_entries.size() << "entries into UI model.";
}

int DiaryListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_entries.size());
}

QVariant DiaryListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_entries.size())
        return QVariant();

    const DiaryEntrySummary& entry = m_entries[index.row()];

    switch (role) {
    case IdRole: return QVariant::fromValue(entry.id);
    case TitleRole: return entry.title;
    case CreatedAtRole: return QVariant::fromValue(entry.createdAt);
    case UpdatedAtRole: return QVariant::fromValue(entry.updatedAt);
    case BookmarkedRole: return QVariant::fromValue(entry.bookmarked);
    default: return QVariant();
    }
}

QHash<int, QByteArray> DiaryListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TitleRole] = "title";
    roles[CreatedAtRole] = "createdAt";
    roles[UpdatedAtRole] = "updatedAt";
    roles[BookmarkedRole] = "bookmarked";
    return roles;
}
