#ifndef PRIORITYVIEWMODEL_H
#define PRIORITYVIEWMODEL_H

#include <QObject>
#include <QVariantList>
#include "model/DiaryManager.h"

class PriorityViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList labels READ labels NOTIFY labelsChanged)

public:
    explicit PriorityViewModel(DiaryManager &manager, QObject *parent = nullptr);

    QVariantList labels() const;

    Q_INVOKABLE void addLabel(const QString &name, const QString &color);
    Q_INVOKABLE void removeLabel(int index);
    Q_INVOKABLE void load();

    signals:
        void labelsChanged();

private:
    void save();
    DiaryManager &m_manager;
    QVariantList  m_labels;
    static constexpr const char* CONFIG_KEY = "priority_labels";
};

#endif // PRIORITYVIEWMODEL_H