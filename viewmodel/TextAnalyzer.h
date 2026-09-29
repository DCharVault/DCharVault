#ifndef DCHARVAULT_TEXTANALYZER_H
#define DCHARVAULT_TEXTANALYZER_H


#include <QQuickTextDocument>

class TextAnalyzer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint64 charsCount READ getCharsCount NOTIFY countChanged)
    Q_PROPERTY(quint64 wordsCount READ getWordsCount NOTIFY countChanged)

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
