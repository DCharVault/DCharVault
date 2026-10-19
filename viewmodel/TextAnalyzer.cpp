#include "TextAnalyzer.h"

#include <QTextBlock>
#include <QTextDocument>
#include <QTextBoundaryFinder>
#include <QRegularExpression>

TextAnalyzer::TextAnalyzer(QObject *parent) : QObject(parent), m_charsCount(0), m_wordsCount(0){}

void TextAnalyzer::calculateCount(QQuickTextDocument* doc)
{
    if (!doc || !doc->textDocument())
    {
        if (m_charsCount!=0 || m_wordsCount!=0) {
            m_charsCount = 0;
            m_wordsCount = 0;
            emit countChanged();
        }
        return;
    }

    QTextDocument* textDoc = doc->textDocument();
    QString fullText = textDoc->toPlainText();

    quint64 newChars = 0;
    for (QChar ch : fullText) {
        if (!ch.isSpace()) {
            newChars++;
        }
    }

    QStringList rawWords = fullText.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    quint64 newWords = 0;
    QRegularExpression alphanumericCheck(".*[A-Za-z0-9].*");
    for (const QString &word : rawWords) {
        if (alphanumericCheck.match(word).hasMatch()) {
            newWords++;
        }
    }

    if (newChars != m_charsCount || newWords != m_wordsCount) {
        m_charsCount = newChars;
        m_wordsCount = newWords;
        emit countChanged();
    }
}

[[nodiscard]] quint64 TextAnalyzer::getCharsCount() const
{
    return m_charsCount;
}
[[nodiscard]] quint64 TextAnalyzer::getWordsCount() const
{
    return m_wordsCount;
}