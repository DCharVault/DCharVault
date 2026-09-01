#ifndef PRIORITYVIEWMODEL_H
#define PRIORITYVIEWMODEL_H

#include <QObject>
#include <QVariantList>
#include "model/DiaryManager.h"

class PriorityViewModel : public QObject
{
    Q_OBJECT

public:
    explicit PriorityViewModel(DiaryManager &manager, QObject *parent = nullptr);

    Q_INVOKABLE QVariantList getPriorityLabels();
    Q_INVOKABLE bool savePriorityLabel(const QString &name, const QString &color);
    Q_INVOKABLE bool deletePriorityLabel(const QString &name);
    Q_INVOKABLE QString buildLabelHtml(const QString &name, const QString &color);

    signals:
        void labelsChanged();

private:
    QVariantList loadLabels() const;
    bool saveLabels(const QVariantList &labels);

    DiaryManager &m_manager;
    static constexpr const char *kConfigKey = "priority_labels";
};

#endif // PRIORITYVIEWMODEL_H