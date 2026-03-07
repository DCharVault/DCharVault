#ifndef DIARYLISTMODEL_H
#define DIARYLISTMODEL_H

#include <QAbstractListModel>
#include <vector>
#include "model/DiaryManager.h"

class DiaryListModel : public QAbstractListModel {
    Q_OBJECT

public:

    enum EntryRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        CreatedAtRole,
        UpdatedAtRole,
        BookmarkedRole
    };

    explicit DiaryListModel(DiaryManager& manager, QObject *parent = nullptr);

    Q_INVOKABLE void loadEntries();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    DiaryManager& m_manager;
    std::vector<DiaryEntrySummary> m_entries;
};

#endif // DIARYLISTMODEL_H
