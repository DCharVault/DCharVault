#include "FindReplaceController.h"

#include <QTextBlock>
#include <pstl/algorithm_impl.h>

FindReplaceController::FindReplaceController(QObject *parent)
    : QObject(parent)
{
}

FindReplaceController::~FindReplaceController()
{
}

void FindReplaceController::findAll(QString searchTerm)
{
    QTextDocument *doc = document();
    if (!doc)
    {
        return;
    }
    clearHighlights();

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
    if (m_matches.isEmpty())
    {
        m_currentIndex = -1;
    }
    else
    {
        m_currentIndex = 0;
    }
    emit matchesChanged();
    emit currentMatchChanged();

    QByteArray utf8search = searchTerm.toUtf8();
    SecureString tempSecure(utf8search.begin(), utf8search.end());
    m_lastSearchTerm = std::move(tempSecure);
    sodium_memzero(utf8search.data(),utf8search.size());

    applyHighlights();

    secureWipeQString(searchTerm);
}

void FindReplaceController::findNext()
{
    if (m_matches.isEmpty())
    {
        return;
    }
    clearHighlights();
    m_currentIndex++;
    if (m_currentIndex>=m_matches.length())
    {
        m_currentIndex = 0;
    }
    navigateToMatch();
    applyHighlights();
}

void FindReplaceController::findPrevious()
{
    if (m_matches.isEmpty())
    {
        return;
    }
    clearHighlights();
    m_currentIndex--;
    if (m_currentIndex<0)
    {
        m_currentIndex = m_matches.length()-1;
    }
    navigateToMatch();
    applyHighlights();
}

void FindReplaceController::replaceCurrent(QString replacement)
{
    if (m_matches.isEmpty() || m_currentIndex<0 || m_currentIndex >= m_matches.length())
    {
        return;
    }

    clearHighlights();
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
    emit textModified();
    applyHighlights();
    secureWipeQString(replacement);
}

void FindReplaceController::replaceAll(QString replacement)
{
    if (m_matches.isEmpty())
    {
        return;
    }
    clearHighlights();

    QTextCursor firstCursor = m_matches.first();
    firstCursor.beginEditBlock();

    for (int i = m_matches.length()-1; i>=0; i--) {
        QTextCursor cursor = m_matches.at(i);
        cursor.insertText(replacement);
    }

    firstCursor.endEditBlock();
    m_matches.clear();
    m_currentIndex = -1;
    emit matchesChanged();
    emit currentMatchChanged();
    emit textModified();
    applyHighlights();
    secureWipeQString(replacement);
}

void FindReplaceController::purge()
{
    clearHighlights();
    m_matches.clear();
    m_currentIndex = -1;

    m_lastSearchTerm = SecureString(); //forced deallocation

    emit matchesChanged();
    emit currentMatchChanged();
}

QQuickTextDocument* FindReplaceController::textDocument() const
{
    return m_textDocument;
}

void FindReplaceController::setTextDocument(QQuickTextDocument *doc)
{
    if (m_textDocument == doc) return;

    m_textDocument = doc;
    emit textDocumentChanged();
}

int FindReplaceController::matchCount() const
{
    return m_matches.length();
}

int FindReplaceController::currentMatch() const
{
    return m_currentIndex+1;
}

bool FindReplaceController::hasMatches() const
{
    return !m_matches.isEmpty();
}

bool FindReplaceController::caseSensitive() const
{
    return m_caseSensitive;
}

void FindReplaceController::setCaseSensitive(bool cs)
{
    if (cs==m_caseSensitive) return;
    m_caseSensitive = cs;
    emit optionsChanged();
}

bool FindReplaceController::wholeWord() const
{
    return false;
}

void FindReplaceController::setWholeWord(bool w)
{
    if (w==m_wholeWord) return;
    m_wholeWord = w;
    emit optionsChanged();
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
    if (m_matches.isEmpty())
    {
        return;
    }
    QTextCharFormat fmtClear = QTextCharFormat();
    fmtClear.clearBackground();

    for (int i=0; i<m_matches.length(); i++)
    {
        QTextCursor cursor = m_matches.at(i);
        cursor.mergeCharFormat(fmtClear);
    }
}

void FindReplaceController::applyHighlights()
{
    if (m_matches.isEmpty())
    {
        return;
    }
    QTextCharFormat fmtMatches = QTextCharFormat();
    fmtMatches.setBackground(QBrush(QBrush(QColor(Qt::yellow))));

    QTextCharFormat fmtActiveMatch = QTextCharFormat();
    fmtActiveMatch.setBackground(QBrush(QBrush(QColor(Qt::green))));

    for (int i=0; i<m_matches.length(); ++i) {
        QTextCursor cursor = m_matches.at(i);
        if (i==m_currentIndex)
        {
            cursor.mergeCharFormat(fmtActiveMatch);
        }else
        {
            cursor.mergeCharFormat(fmtMatches);
        }
    }
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
    if (str.isEmpty()) return;

    str.detach();
    sodium_memzero(str.data(),str.capacity()*sizeof(QChar));
    str.clear();
}