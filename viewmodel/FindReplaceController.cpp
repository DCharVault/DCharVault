#include "FindReplaceController.h"

#include <QTextBlock>
#include <QColor>
#include <QBrush>
#include <utility>

namespace {
    struct ScopedWiper {
        QString &target;
        ~ScopedWiper() {
            FindReplaceController::secureWipeQString(target);
        }
    };
}

FindReplaceController::FindReplaceController(QObject *parent)
    : QObject(parent)
{
}

FindReplaceController::~FindReplaceController()
{
    purge();
}

void FindReplaceController::findAll(QString searchTerm)
{
    ScopedWiper wiper(searchTerm);

    QTextDocument *doc = document();
    if (!doc || searchTerm.isEmpty())
    {
        clearHighlights();
        m_matches.clear();
        m_currentIndex = -1;
        m_lastSearchTerm = SecureString();
        emit matchesChanged();
        emit currentMatchChanged();
        return;
    }

    clearHighlights();
    m_matches.clear();

    QTextDocument::FindFlags flags;
    if (m_caseSensitive) flags |= QTextDocument::FindCaseSensitively;
    if (m_wholeWord) flags |= QTextDocument::FindWholeWords;

    QTextCursor searchCursor(doc);
    searchCursor.setPosition(0);

    while (!searchCursor.isNull() && !searchCursor.atEnd())
    {
        searchCursor = doc->find(searchTerm, searchCursor, flags);
        if (!searchCursor.isNull())
        {
            m_matches.append(searchCursor);
        }
    }
    m_currentIndex = m_matches.isEmpty() ? -1 : 0;
    emit matchesChanged();
    emit currentMatchChanged();

    // Securely cache search term in guarded memory
    QByteArray utf8search = searchTerm.toUtf8();
    SecureString tempSecure(utf8search.begin(), utf8search.end());
    m_lastSearchTerm = std::move(tempSecure);
    sodium_memzero(utf8search.data(), utf8search.size());

    applyHighlights();

    if (m_currentIndex >= 0)
    {
        navigateToMatch();
    }
}

void FindReplaceController::findNext()
{
    if (m_matches.isEmpty())
    {
        return;
    }
    clearHighlights();
    m_currentIndex++;
    if (m_currentIndex >= m_matches.length())
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
    if (m_currentIndex < 0)
    {
        m_currentIndex = m_matches.length() - 1;
    }
    navigateToMatch();
    applyHighlights();
}

void FindReplaceController::replaceCurrent(QString replacement)
{
    ScopedWiper wiper(replacement);

    if (m_matches.isEmpty() || m_currentIndex < 0 || m_currentIndex >= m_matches.length())
    {
        return;
    }

    clearHighlights();
    QTextCursor currentCursorSelection = m_matches[m_currentIndex];
    currentCursorSelection.beginEditBlock();
    currentCursorSelection.insertText(replacement);
    currentCursorSelection.endEditBlock();

    const int nextSearchPos = currentCursorSelection.position();

    // Re-index remaining occurrences to avoid offset skew
    reindexMatches(nextSearchPos);

    emit textModified();

    if (!m_matches.isEmpty())
    {
        navigateToMatch();
        applyHighlights();
    }
}

void FindReplaceController::replaceAll(QString replacement)
{
    ScopedWiper wiper(replacement);

    if (m_matches.isEmpty())
    {
        return;
    }
    clearHighlights();

    QTextDocument *doc = document();
    if (!doc)
        return;

    QTextCursor editCursor(doc);
    editCursor.beginEditBlock();

    for (int i = m_matches.length() - 1; i >= 0; --i) {
        QTextCursor cursor = m_matches.at(i);
        cursor.insertText(replacement);
    }

    editCursor.endEditBlock();

    m_matches.clear();
    m_currentIndex = -1;
    m_lastSearchTerm = SecureString();

    emit matchesChanged();
    emit currentMatchChanged();
    emit textModified();
}

void FindReplaceController::purge()
{
    clearHighlights();
    m_matches.clear();
    m_currentIndex = -1;

    m_lastSearchTerm = SecureString(); // forced deallocation via sodium_free

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

    purge();

    if (m_textDocument)
        disconnect(m_textDocument, nullptr, this, nullptr);

    m_textDocument = doc;

    if (m_textDocument) {
        connect(m_textDocument, &QObject::destroyed, this, [this] {
            m_textDocument = nullptr;
            purge();
            emit textDocumentChanged();
        });
    }

    emit textDocumentChanged();
}

int FindReplaceController::matchCount() const
{
    return m_matches.length();
}

int FindReplaceController::currentMatch() const
{
    return m_currentIndex + 1;
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
    if (cs == m_caseSensitive) return;
    m_caseSensitive = cs;
    emit optionsChanged();

    if (!m_lastSearchTerm.empty())
    {
        clearHighlights();
        reindexMatches();
        applyHighlights();
    }
}

bool FindReplaceController::wholeWord() const
{
    return m_wholeWord;
}

void FindReplaceController::setWholeWord(bool w)
{
    if (w == m_wholeWord) return;
    m_wholeWord = w;
    emit optionsChanged();

    if (!m_lastSearchTerm.empty())
    {
        clearHighlights();
        reindexMatches();
        applyHighlights();
    }
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

    QTextDocument *doc = document();
    if (!doc)
        return;

    const bool wasUndo = doc->isUndoRedoEnabled();
    const bool wasModified = doc->isModified();
    doc->setUndoRedoEnabled(false);

    QTextCharFormat fmtClear;
    fmtClear.setBackground(Qt::NoBrush);

    for (int i = 0; i < m_matches.length(); ++i)
    {
        QTextCursor cursor = m_matches.at(i);
        cursor.mergeCharFormat(fmtClear);
    }

    doc->setUndoRedoEnabled(wasUndo);
    doc->setModified(wasModified);
}

void FindReplaceController::applyHighlights()
{
    if (m_matches.isEmpty())
    {
        return;
    }

    QTextDocument *doc = document();
    if (!doc)
        return;

    const bool wasUndo = doc->isUndoRedoEnabled();
    const bool wasModified = doc->isModified();
    doc->setUndoRedoEnabled(false);

    QTextCharFormat fmtMatches;
    QTextCharFormat fmtActiveMatch;

    // Theme-compatible translucent colors for seamless dark/light mode integration
    fmtMatches.setBackground(QBrush(QColor(224, 165, 38, 100)));     // Amber tint
    fmtActiveMatch.setBackground(QBrush(QColor(76, 175, 110, 180)));  // Mint emerald

    for (int i = 0; i < m_matches.length(); ++i) {
        QTextCursor cursor = m_matches.at(i);
        if (i == m_currentIndex)
        {
            cursor.mergeCharFormat(fmtActiveMatch);
        } else
        {
            cursor.mergeCharFormat(fmtMatches);
        }
    }

    doc->setUndoRedoEnabled(wasUndo);
    doc->setModified(wasModified);
}

void FindReplaceController::navigateToMatch()
{
    if (m_currentIndex < 0 || m_currentIndex >= m_matches.length()) {
        return;
    }

    emit currentMatchChanged();

    QTextCursor currentCursorSelection = m_matches[m_currentIndex];
    int absolutePos = currentCursorSelection.selectionStart();

    emit scrollToPosition(absolutePos);
}

void FindReplaceController::reindexMatches(int preferredTargetPos)
{
    QTextDocument *doc = document();
    if (!doc || m_lastSearchTerm.empty())
    {
        m_matches.clear();
        m_currentIndex = -1;
        emit matchesChanged();
        emit currentMatchChanged();
        return;
    }

    QString searchTerm = QString::fromUtf8(m_lastSearchTerm.data(), static_cast<qsizetype>(m_lastSearchTerm.size()));
    ScopedWiper wiper(searchTerm);

    m_matches.clear();

    QTextDocument::FindFlags flags;
    if (m_caseSensitive) flags |= QTextDocument::FindCaseSensitively;
    if (m_wholeWord) flags |= QTextDocument::FindWholeWords;

    QTextCursor searchCursor(doc);
    searchCursor.setPosition(0);

    int closestIndex = -1;
    while (!searchCursor.isNull() && !searchCursor.atEnd())
    {
        searchCursor = doc->find(searchTerm, searchCursor, flags);
        if (!searchCursor.isNull())
        {
            if (closestIndex == -1 && preferredTargetPos >= 0 && searchCursor.selectionStart() >= preferredTargetPos)
            {
                closestIndex = m_matches.length();
            }
            m_matches.append(searchCursor);
        }
    }

    if (m_matches.isEmpty())
    {
        m_currentIndex = -1;
    } else if (closestIndex >= 0 && closestIndex < m_matches.length())
    {
        m_currentIndex = closestIndex;
    } else
    {
        m_currentIndex = 0;
    }

    emit matchesChanged();
    emit currentMatchChanged();
}

void FindReplaceController::secureWipeQString(QString &str)
{
    if (str.capacity() == 0 || str.data() == nullptr) return;

    str.detach();
    sodium_memzero(str.data(), str.capacity() * sizeof(QChar));
    str.clear();
}
