#include "FindReplaceController.h"

#include <QTextBlock>

FindReplaceController::FindReplaceController(QObject *parent)
    : QObject(parent)
{
}

FindReplaceController::~FindReplaceController()
{
}

void FindReplaceController::findAll(const QString &searchTerm)
{
    QTextDocument *doc = document();
    if (!doc)
    {
        return;
    }

    Qt::CaseSensitivity sensitive = (m_caseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

    for (QTextBlock currBlock = doc->firstBlock(); currBlock.isValid(); currBlock = currBlock.next())
    {
        int posWord = currBlock.text().indexOf(searchTerm,0,sensitive);
        if (posWord < 0)
        {
            continue;
        }

        int startIndex = 0;
        while (startIndex >= 0 && posWord >= 0)
        {
            QTextCursor cursor(doc);
            cursor.setPosition(currBlock.position()+posWord);
            cursor.setPosition(currBlock.position()+posWord+searchTerm.length(),QTextCursor::KeepAnchor);
            m_matches.append(cursor);
            startIndex = posWord+searchTerm.length();
            posWord = currBlock.text().indexOf(searchTerm,startIndex,sensitive);
        }
    }

}

void FindReplaceController::findNext()
{
    if (m_matches.isEmpty())
    {
        return;
    }

    m_currentIndex++;
    if (m_currentIndex>=m_matches.length())
    {
        m_currentIndex = 0;
    }
    navigateToMatch();
}

void FindReplaceController::findPrevious()
{
    if (m_matches.isEmpty())
    {
        return;
    }

    m_currentIndex--;
    if (m_currentIndex<0)
    {
        m_currentIndex = m_matches.length()-1;
    }
    navigateToMatch();
}

void FindReplaceController::replaceCurrent(const QString &replacement)
{
    if (m_matches.isEmpty() || m_currentIndex<0 || m_currentIndex >= m_matches.length())
    {
        return;
    }

    QTextCursor currentCursorSelection = m_matches[m_currentIndex];
    currentCursorSelection.insertText(replacement);

    m_matches.removeAt(m_currentIndex);
    if (m_currentIndex>=m_matches.length())
    {
        m_currentIndex = 0;
    }

    if (m_matches.isEmpty())
    {
        m_currentIndex = -1;
        emit currentMatchChanged();
    }else
    {
        navigateToMatch();
    }
    emit matchesChanged();
}

void FindReplaceController::replaceAll(const QString &replacement)
{
}

void FindReplaceController::purge()
{
}

QQuickTextDocument* FindReplaceController::textDocument() const
{
    return nullptr;
}

void FindReplaceController::setTextDocument(QQuickTextDocument *doc)
{
}

int FindReplaceController::matchCount() const
{
    return 0;
}

int FindReplaceController::currentMatch() const
{
    return 0;
}

bool FindReplaceController::hasMatches() const
{
    return false;
}

bool FindReplaceController::caseSensitive() const
{
    return false;
}

void FindReplaceController::setCaseSensitive(bool cs)
{
}

bool FindReplaceController::wholeWord() const
{
    return false;
}

void FindReplaceController::setWholeWord(bool w)
{
}

QTextDocument* FindReplaceController::document() const
{
    if (!m_textDocument)
    {
        return nullptr;
    }
    return m_textDocument->textDocument();
}

void FindReplaceController::clearHighlights()
{
}

void FindReplaceController::applyHighlights()
{
}

void FindReplaceController::navigateToMatch()
{
    if (m_currentIndex < 0 || m_currentIndex >= m_matches.length()) {
        return;
    }

    emit currentMatchChanged();

    QTextCursor currentCursorSelection = m_matches[m_currentIndex];
    int absolutePos = currentCursorSelection.position();

    emit scrollToPosition(absolutePos);
}

void FindReplaceController::secureWipeQString(QString &str)
{
}