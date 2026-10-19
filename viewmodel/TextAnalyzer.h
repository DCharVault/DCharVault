#ifndef DCHARVAULT_TEXTANALYZER_H
#define DCHARVAULT_TEXTANALYZER_H

#include <QObject>
#include <QQuickTextDocument>
#include <QQmlEngine>

class TextAnalyzer : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(quint64 charsCount READ getCharsCount NOTIFY countChanged)
    Q_PROPERTY(quint64 wordsCount READ getWordsCount NOTIFY countChanged)

public:
    explicit TextAnalyzer(QObject *parent = nullptr);

    Q_INVOKABLE void calculateCount(QQuickTextDocument *doc);
    Q_INVOKABLE void reset();

    [[nodiscard]] quint64 getCharsCount() const noexcept;
    [[nodiscard]] quint64 getWordsCount() const noexcept;

 signals:
        void countChanged();
private:
    quint64 m_charsCount = 0;
    quint64 m_wordsCount = 0;
};

#endif //DCHARVAULT_TEXTANALYZER_H
