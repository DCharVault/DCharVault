#ifndef DCHARVAULT_TEXTANALYZER_H
#define DCHARVAULT_TEXTANALYZER_H


#include <QQuickTextDocument>

class TextAnalyzer : public QObject
{
    Q_PROPERTY(quint64 m_charsCount READ getCharsCount NOTIFY countChanged)
    Q_PROPERTY(quint64 m_wordsCount READ getWordsCount NOTIFY countChanged)

    Q_OBJECT
public:
    explicit TextAnalyzer(QObject *parent = nullptr);

    Q_INVOKABLE void calculateCount(QQuickTextDocument *doc);

    [[nodiscard]] quint64 getCharsCount() const;
    [[nodiscard]] quint64 getWordsCount() const;
signals:
    void countChanged();
private:
    quint64 m_charsCount;
    quint64 m_wordsCount;
};

#endif //DCHARVAULT_TEXTANALYZER_H
