#include "PriorityViewModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QColor>

#include <QDebug>

PriorityViewModel::PriorityViewModel(DiaryManager &manager, QObject *parent)
    : QObject(parent)
    , m_manager(manager)
{
}

QVariantList PriorityViewModel::loadLabels() const
{
    const QString json = m_manager.loadConfig(QString::fromLatin1(kConfigKey), QStringLiteral("[]"));

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray()) {
        qWarning() << "[PriorityViewModel] Failed to parse priority labels JSON:" << err.errorString();
        return {};
    }

    QVariantList result;
    for (const QJsonValue &val : doc.array()) {
        if (!val.isObject())
            continue;
        QJsonObject obj = val.toObject();
        QVariantMap entry;
        entry[QStringLiteral("name")]  = obj[QStringLiteral("name")].toString();
        entry[QStringLiteral("color")] = obj[QStringLiteral("color")].toString();
        result.append(entry);
    }
    return result;
}

bool PriorityViewModel::saveLabels(const QVariantList &labels)
{
    QJsonArray arr;
    for (const QVariant &item : labels) {
        const QVariantMap map = item.toMap();
        QJsonObject obj;
        obj[QStringLiteral("name")]  = map[QStringLiteral("name")].toString();
        obj[QStringLiteral("color")] = map[QStringLiteral("color")].toString();
        arr.append(obj);
    }

    const QString json = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
    const DiaryError err = m_manager.saveConfig(QString::fromLatin1(kConfigKey), json);
    if (err != DiaryError::None) {
        qWarning() << "[PriorityViewModel] Failed to save priority labels. Error:" << static_cast<int>(err);
        return false;
    }
    return true;
}

QVariantList PriorityViewModel::getPriorityLabels()
{
    return loadLabels();
}

bool PriorityViewModel::savePriorityLabel(const QString &name, const QString &color)
{
    const QString trimmedName = name.trimmed();
    if (trimmedName.isEmpty() || color.isEmpty()) {
        qWarning() << "[PriorityViewModel] savePriorityLabel: name or color is empty.";
        return false;
    }

    QVariantList labels = loadLabels();

    // Update existing label if name matches, otherwise append
    bool found = false;
    for (int i = 0; i < labels.size(); ++i) {
        QVariantMap entry = labels[i].toMap();
        if (entry[QStringLiteral("name")].toString().compare(trimmedName, Qt::CaseInsensitive) == 0) {
            entry[QStringLiteral("color")] = color;
            labels[i] = entry;
            found = true;
            break;
        }
    }

    if (!found) {
        QVariantMap newEntry;
        newEntry[QStringLiteral("name")]  = trimmedName;
        newEntry[QStringLiteral("color")] = color;
        labels.append(newEntry);
    }

    const bool ok = saveLabels(labels);
    if (ok)
        emit labelsChanged();
    return ok;
}

bool PriorityViewModel::deletePriorityLabel(const QString &name)
{
    QVariantList labels = loadLabels();
    const int sizeBefore = labels.size();

    labels.erase(std::remove_if(labels.begin(), labels.end(),
        [&](const QVariant &item) {
            return item.toMap()[QStringLiteral("name")].toString()
                       .compare(name, Qt::CaseInsensitive) == 0;
        }),
        labels.end());

    if (labels.size() == sizeBefore) {
        qWarning() << "[PriorityViewModel] Label not found for deletion:" << name;
        return false;
    }

    const bool ok = saveLabels(labels);
    if (ok)
        emit labelsChanged();
    return ok;
}

QString PriorityViewModel::buildLabelHtml(const QString &name, const QString &color)
{
    if (name.isEmpty())
        return {};

    // Pick readable text color based on background luminance
    QColor bg(color);
    const double luminance = 0.299 * bg.redF() + 0.587 * bg.greenF() + 0.114 * bg.blueF();
    const QString textColor = (luminance < 0.5) ? QStringLiteral("#FFFFFF") : QStringLiteral("#1A0F18");

    return QStringLiteral(
        " <span style=\""
        "background-color:%1;"
        "color:%2;"
        "border-radius:3px;"
        "padding:1px 6px 2px 6px;"
        "font-size:10pt;"
        "font-weight:bold;"
        "font-family:'Open Sans',sans-serif;"
        "\">%3</span> "
    ).arg(color, textColor, name.toUpper());
}