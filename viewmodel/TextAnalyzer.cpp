#include "TextAnalyzer.h"

#include <QTextBlock>
#include <QTextDocument>
#include <QTextBoundaryFinder>

TextAnalyzer::TextAnalyzer(QObject *parent) : QObject(parent), m_charsCount(0), m_wordsCount(0){}

void TextAnalyzer::calculateCount(QQuickTextDocument* doc)
{
    if (!doc || !doc->textDocument()) return;

    QTextDocument* textDoc = doc->textDocument();
    m_wordsCount = 0;
    m_charsCount = 0;
    for (QTextBlock textBlock = textDoc->begin(); textBlock.isValid(); textBlock = textBlock.next()) // iterate through the document one paragraph (block) at a time
    {
        QString text = textBlock.text();
        m_charsCount += text.length();
        QTextBoundaryFinder finder(QTextBoundaryFinder::Word, text);
        while (finder.toNextBoundary()!=-1)
        {
            //count when the finder hits the actual start of a word
            if (finder.boundaryReasons() && QTextBoundaryFinder::StartOfItem)
            {
                m_wordsCount++;
            }
        }
    }

    emit countChanged();
}

[[nodiscard]] quint64 TextAnalyzer::getCharsCount() const
{
    return m_charsCount;
}
[[nodiscard]] quint64 TextAnalyzer::getWordsCount() const
{
    return m_wordsCount;
}