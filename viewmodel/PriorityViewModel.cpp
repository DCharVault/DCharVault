#include "PriorityViewModel.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

PriorityViewModel::PriorityViewModel(DiaryManager &manager, QObject *parent)
    : QObject(parent), m_manager(manager) {}

QVariantList PriorityViewModel::labels() const { return m_labels; }

void PriorityViewModel::load()
{
    m_labels.clear();
    const QString json = m_manager.loadConfig(CONFIG_KEY);
    if (json.isEmpty()) {
        emit labelsChanged();
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isArray()) {
        emit labelsChanged();
        return;
    }
    const QJsonArray arr = doc.array();
    for (const auto &val : arr) {
        QJsonObject obj = val.toObject();
        QVariantMap m;
        m["name"]  = obj.value("name").toString();
        m["color"] = obj.value("color").toString();
        m_labels.append(m);
    }
    emit labelsChanged();
}

void PriorityViewModel::addLabel(const QString &name, const QString &color)
{
    QVariantMap m;
    m["name"]  = name;
    m["color"] = color;
    m_labels.append(m);
    save();
    emit labelsChanged();
}

void PriorityViewModel::removeLabel(int index)
{
    if (index < 0 || index >= m_labels.size()) return;
    m_labels.removeAt(index);
    save();
    emit labelsChanged();
}

void PriorityViewModel::save()
{
    QJsonArray arr;
    for (const auto &v : m_labels) {
        QVariantMap m = v.toMap();
        QJsonObject obj;
        obj["name"]  = m.value("name").toString();
        obj["color"] = m.value("color").toString();
        arr.append(obj);
    }
    QString json = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
    m_manager.saveConfig(CONFIG_KEY, json);
}