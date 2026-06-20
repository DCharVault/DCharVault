#include "TextHighlighter.h"
#include <QBrush>

QQuickTextDocument* TextHighlighter::textDocument() const { return m_textDocument; }

void TextHighlighter::setTextDocument(QQuickTextDocument *doc) {
    if (m_textDocument == doc)
        return;

    if (m_textDocument)
        disconnect(m_textDocument, nullptr, this, nullptr);

    m_textDocument = doc;

    if (m_textDocument) {
        connect(m_textDocument, &QObject::destroyed, this, [this] {
            m_textDocument = nullptr;
            emit textDocumentChanged();
        });
    }

    emit textDocumentChanged();
}

    /**
     * @brief Apply a background color to the text between \p selStart and \p selEnd.
     *
     * Call this from QML after the user picks a highlight color:
     *   textHighlighter.applyHighlight(editorArea.selectionStart,
     *                                   editorArea.selectionEnd,
     *                                   selectedColor)
     */
void TextHighlighter::applyHighlight(int selStart, int selEnd, const QColor &color) {
    if (!m_textDocument || !m_textDocument->textDocument())
        return;
    if (selStart == selEnd)
        return; // nothing selected

    QTextCursor cursor(m_textDocument->textDocument());
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setBackground(QBrush(color));
    cursor.mergeCharFormat(fmt);
}

    /**
     * @brief Remove the background highlight from the text between
     *        \p selStart and \p selEnd.
     */
void TextHighlighter::removeHighlight(int selStart, int selEnd) {
    if (!m_textDocument || !m_textDocument->textDocument())
        return;
    if (selStart == selEnd)
        return;

    QTextCursor cursor(m_textDocument->textDocument());
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setBackground(Qt::NoBrush);
    cursor.mergeCharFormat(fmt);
}


