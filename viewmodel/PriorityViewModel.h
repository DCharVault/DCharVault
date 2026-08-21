#ifndef PRIORITYVIEWMODEL_H
#define PRIORITYVIEWMODEL_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include "model/DiaryManager.h"

/**
 * @brief PriorityViewModel — manages user-defined priority labels.
 *
 * Labels are stored as a JSON array in vault_config under key "priority_labels",
 * encrypted/decrypted via DiaryManager's existing config mechanism.
 *
 * Each label is a {name, color} pair.
 * Example config value (plaintext before encryption):
 *   [{"name":"Work","color":"#4A90D9"},{"name":"Urgent","color":"#FF6B6B"}]
 */

class PriorityViewModel : public QObject{
    Q_OBJECT
public:
    explicit PriorityViewModel(DiaryManager &manager, QObject *parent=nullptr);

    /**
     * @brief Returns the list of all user-defined priority labels.
     * Each item is a QVariantMap with keys "name" (string) and "color" (string).
     */
    Q_INVOKABLE QVariantList getPriorityLabels();
    /**
     * @brief Save (add or update) a priority label.
     * If a label with the same name already exists, its color is updated.
     * @param name  Display name of the label (trimmed, case-sensitive).
     * @param color Hex color string, e.g. "#FF6B6B".
     * @return true on success.
     */
    Q_INVOKABLE bool savePriorityLabel(const QString &name, const QString &color);
    /**
     * @brief Delete a priority label by name.
     * @return true if the label was found and deleted.
     */
    Q_INVOKABLE bool deletePriorityLabel(const QString &name);
    /**
     * @brief Build the HTML badge string to insert into the editor.
     * Returns a styled <span> that visually represents the label inline.
     */
    Q_INVOKABLE QString buildLabelHtml(const QString &name, const QString &color);

signals:
    void labelsChanged();

private:
    DiaryManager &m_manager;
    static constexpr const char *kConfigKey = "priority_labels";
    /** Load the current label list from vault_config. */
    QVariantList loadLabels() const;
    /** Persist the label list to vault_config. */
    bool saveLabels(const QVariantList &labels);
};

#endif // PRIORITYVIEWMODEL_H
