#include "TextAnalyzer.h"

#include <QTextBlock>
#include <QTextDocument>

TextAnalyzer::TextAnalyzer(QObject *parent)
    : QObject(parent), m_charsCount(0), m_wordsCount(0) {}

void TextAnalyzer::reset()
{
    if (m_charsCount != 0 || m_wordsCount != 0) {
        m_charsCount = 0;
        m_wordsCount = 0;
        emit countChanged();
    }
}

void TextAnalyzer::calculateCount(QQuickTextDocument *doc)
{
    if (!doc || !doc->textDocument()) {
        reset();
        return;
    }

    const QTextDocument *textDoc = doc->textDocument();
    quint64 newChars = 0;
    quint64 newWords = 0;

    for (QTextBlock block = textDoc->begin(); block.isValid(); block = block.next()) {
        QString blockText = block.text();
        bool inToken = false;
        bool tokenHasAlnum = false;

        for (const QChar ch : std::as_const(blockText)) {
            // Ignore zero-width checkbox markers if ever present
            if (ch == u'\u200B') {
                continue;
            }

            if (!ch.isSpace()) {
                newChars++;
                inToken = true;
                if (!tokenHasAlnum && ch.isLetterOrNumber()) {
                    tokenHasAlnum = true;
                }
            } else {
                if (inToken && tokenHasAlnum) {
                    newWords++;
                }
                inToken = false;
                tokenHasAlnum = false;
            }
        }

        if (inToken && tokenHasAlnum) {
            newWords++;
        }

        if (!blockText.isEmpty()) {
            blockText.fill(u'\0');
        }
    }

    if (newChars != m_charsCount || newWords != m_wordsCount) {
        m_charsCount = newChars;
        m_wordsCount = newWords;
        emit countChanged();
    }
}

[[nodiscard]] quint64 TextAnalyzer::getCharsCount() const noexcept
{
    return m_charsCount;
}

[[nodiscard]] quint64 TextAnalyzer::getWordsCount() const noexcept
{
    return m_wordsCount;
}