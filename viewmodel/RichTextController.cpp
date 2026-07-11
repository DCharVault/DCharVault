#include "RichTextController.h"

#include<utility>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextList>
#include <QBrush>
#include <QFontDatabase>
#include <QUrl>

//Construction & Document Binding

RichTextController::RichTextController(QObject *parent)
    : QObject(parent)
{
}

QQuickTextDocument* RichTextController::textDocument() const
{
    return m_textDocument;
}

void RichTextController::setTextDocument(QQuickTextDocument *doc)
{
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

QTextDocument* RichTextController::document() const
{
    if (!m_textDocument)
        return nullptr;
    return m_textDocument->textDocument();
}

QStringList RichTextController::availableFontFamilies() const
{
    static const QStringList families = QFontDatabase::families();
    return families;
}

void RichTextController::setBold(int selStart, int selEnd, bool enable)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setFontWeight(enable ? QFont::Bold : QFont::Normal);
    cursor.mergeCharFormat(fmt);
}

void RichTextController::setItalic(int selStart, int selEnd, bool enable)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setFontItalic(enable);
    cursor.mergeCharFormat(fmt);
}

void RichTextController::setUnderline(int selStart, int selEnd, bool enable)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setFontUnderline(enable);
    cursor.mergeCharFormat(fmt);
}

void RichTextController::setFontSize(int selStart, int selEnd, qreal pointSize)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    // Clamp to safe range
    pointSize = qBound(6.0, pointSize, 88.0);

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setFontPointSize(pointSize);
    cursor.mergeCharFormat(fmt);
}

void RichTextController::setTextColor(int selStart, int selEnd, const QColor &color)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setForeground(QBrush(color));
    cursor.mergeCharFormat(fmt);
}

// ─── Strikethrough ───────────────────────────────────────────────────────────

void RichTextController::toggleStrikethrough(int selStart, int selEnd)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    // Read current state at selection start to determine toggle direction
    QTextCursor probe(doc);
    probe.setPosition(selStart);
    bool currentlyStruck = probe.charFormat().fontStrikeOut();

    setStrikethrough(selStart, selEnd, !currentlyStruck);
}

void RichTextController::setStrikethrough(int selStart, int selEnd, bool enable)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setFontStrikeOut(enable);
    cursor.mergeCharFormat(fmt);
}

// Headings

void RichTextController::setHeading(int selStart, int selEnd, int level)
{
    QTextDocument *doc = document();
    if (!doc)
        return;

    // Clamp level: 0 = normal, 1-3 = headings
    level = qBound(0, level, 3);

    // Clamp inputs to prevent signed integer UB on selEnd - 1
    const int docLen = doc->characterCount();
    selStart = qBound(0, selStart, docLen);
    selEnd   = qBound(0, selEnd,   docLen);
    if (selStart > selEnd) std::swap(selStart, selEnd);

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    if (selEnd > selStart)
        cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    cursor.beginEditBlock();

    // Iterate through all blocks in the selection
    QTextBlock startBlock = doc->findBlock(selStart);
    QTextBlock endBlock = doc->findBlock(qMax(selStart, selEnd - 1));
    if (!endBlock.isValid())
        endBlock = doc->lastBlock();

    for (QTextBlock block = startBlock; block.isValid(); block = block.next()) {
        QTextCursor blockCursor(doc);
        blockCursor.setPosition(block.position());
        blockCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

        // Set block heading level
        QTextBlockFormat blockFmt = block.blockFormat();
        blockFmt.setHeadingLevel(level);
        blockCursor.setBlockFormat(blockFmt);

        // Set character format for heading style
        QTextCharFormat charFmt;
        if (level > 0 && level <= 3) {
            charFmt.setFontWeight(QFont::Bold);
            charFmt.setFontPointSize(kHeadingSizes[level]);
        } else {
            // Reset to normal: clear bold weight and font size
            charFmt.setFontWeight(QFont::Normal);
            charFmt.setFontPointSize(0.0);  // 0.0 = inherit document default; mergeCharFormat will push it through
        }
        blockCursor.mergeCharFormat(charFmt);

        if (block == endBlock)
            break;
    }

    cursor.endEditBlock();
}

//Bullet List

void RichTextController::toggleBulletList(int cursorPos)
{
    QTextDocument *doc = document();
    if (!doc)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(cursorPos);

    QTextList *currentList = cursor.currentList();

    cursor.beginEditBlock();

    if (currentList && currentList->format().style() == QTextListFormat::ListDisc) {
        // Already a bullet list — remove it
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(0);
        cursor.setBlockFormat(blockFmt);
        currentList->remove(cursor.block());
    } else {
        // If in a different list type, remove from that first
        if (currentList) {
            currentList->remove(cursor.block());
        }
        // Create bullet list
        QTextListFormat listFmt;
        listFmt.setStyle(QTextListFormat::ListDisc);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}

// Numbered List

void RichTextController::toggleNumberedList(int cursorPos)
{
    QTextDocument *doc = document();
    if (!doc)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(cursorPos);

    QTextList *currentList = cursor.currentList();

    cursor.beginEditBlock();

    if (currentList && currentList->format().style() == QTextListFormat::ListDecimal) {
        // Already a numbered list — remove it
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(0);
        cursor.setBlockFormat(blockFmt);
        currentList->remove(cursor.block());
    } else {
        // If in a different list type, remove from that first
        if (currentList) {
            currentList->remove(cursor.block());
        }
        // Create numbered list
        QTextListFormat listFmt;
        listFmt.setStyle(QTextListFormat::ListDecimal);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}

// Blockquote

void RichTextController::toggleBlockquote(int selStart, int selEnd)
{
    QTextDocument *doc = document();
    if (!doc)
        return;

    // Use selStart as the reference if no selection
    if (selEnd <= selStart)
        selEnd = selStart + 1;

    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    QTextBlock startBlock = doc->findBlock(selStart);
    QTextBlock endBlock = doc->findBlock(selEnd - 1);
    if (!endBlock.isValid())
        endBlock = doc->lastBlock();

    // Check if the first block is already a blockquote (by margin heuristic)
    bool isBlockquote = (startBlock.blockFormat().leftMargin() >= kBlockquoteMargin);

    for (QTextBlock block = startBlock; block.isValid(); block = block.next()) {
        QTextCursor blockCursor(doc);
        blockCursor.setPosition(block.position());

        QTextBlockFormat blockFmt = block.blockFormat();

        if (isBlockquote) {
            // Remove blockquote styling
            blockFmt.setLeftMargin(0);
            blockFmt.clearProperty(QTextFormat::BlockLeftMargin);
        } else {
            // Apply blockquote: increased left margin
            blockFmt.setLeftMargin(kBlockquoteMargin);
        }

        blockCursor.setBlockFormat(blockFmt);

        if (block == endBlock)
            break;
    }

    cursor.endEditBlock();
}

//Horizontal Rule

void RichTextController::insertHorizontalRule(int cursorPos)
{
    QTextDocument *doc = document();
    if (!doc)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(cursorPos);
    cursor.beginEditBlock();

    // Move to end of current block, then insert new block with HR
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.insertBlock();
    cursor.insertHtml(QStringLiteral("<hr/>"));
    // Insert another block after the rule so the cursor has somewhere to go
    cursor.insertBlock();

    // Reset the new block to normal formatting
    QTextBlockFormat normalBlock;
    cursor.setBlockFormat(normalBlock);
    QTextCharFormat normalChar;
    normalChar.setFontWeight(QFont::Normal);
    normalChar.clearProperty(QTextFormat::FontPointSize);
    cursor.setCharFormat(normalChar);

    cursor.endEditBlock();
}

//Indent / Outdent

void RichTextController::indent(int cursorPos)
{
    QTextDocument *doc = document();
    if (!doc)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(cursorPos);

    QTextList *list = cursor.currentList();
    if (list) {
        // For list items, increase the list indent
        QTextListFormat listFmt = list->format();
        const int currentIndent = qMax(0, listFmt.indent() - 1);
        if (currentIndent < kMaxIndentLevel) {
            listFmt.setIndent(currentIndent + 2);
            list->setFormat(listFmt);
        }
    } else {
        // For normal blocks, increase text indent
        QTextBlockFormat blockFmt = cursor.blockFormat();
        const int currentIndent = blockFmt.indent();
        if (currentIndent < kMaxIndentLevel) {
            blockFmt.setIndent(currentIndent + 1);
            // Also set left margin for visual effect
            blockFmt.setLeftMargin((currentIndent + 1) * kIndentStep);
            cursor.setBlockFormat(blockFmt);
        }
    }
}

void RichTextController::outdent(int cursorPos)
{
    QTextDocument *doc = document();
    if (!doc)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(cursorPos);

    QTextList *list = cursor.currentList();
    if (list) {
        // For list items, decrease the list indent
        QTextListFormat listFmt = list->format();
        const int currentIndent = qMax(0, listFmt.indent() - 1);
        if (currentIndent > 0) {
            listFmt.setIndent(currentIndent);
            list->setFormat(listFmt);
        }
    } else {
        // For normal blocks, decrease text indent
        QTextBlockFormat blockFmt = cursor.blockFormat();
        const int currentIndent = blockFmt.indent();
        if (currentIndent > 0) {
            blockFmt.setIndent(currentIndent - 1);
            blockFmt.setLeftMargin(qMax(0.0, (currentIndent - 1) * kIndentStep));
            cursor.setBlockFormat(blockFmt);
        }
    }
}

//Font Family

void RichTextController::setFontFamily(int selStart, int selEnd, const QString &family)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    // Sec-TradeOff: Validate against system-installed fonts only
    // This prevents injection of external/network font references
    const QStringList systemFonts = availableFontFamilies();
    if (!systemFonts.contains(family, Qt::CaseInsensitive))
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setFontFamilies({family});
    cursor.mergeCharFormat(fmt);
}

//Hyperlinks

bool RichTextController::isUrlSchemeAllowed(const QString &url)
{
    // Only accept URLs up to 2048 characters.
    if (url.length() > 2048)
        return false;

    // Sec-TradeOff: Strict URL scheme whitelist.
    // Only http://, https://, and mailto: are allowed.
    // Blocks javascript:, file://, data:, vbscript:, etc.
    QUrl parsed(url, QUrl::StrictMode);

    if (!parsed.isValid())
        return false;

    const QString scheme = parsed.scheme().toLower();
    return (scheme == QLatin1String("http")
         || scheme == QLatin1String("https")
         || scheme == QLatin1String("mailto"));
}

bool RichTextController::insertHyperlink(int selStart, int selEnd, const QString &url)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return false;

    // SECURITY: Validate URL scheme
    if (!isUrlSchemeAllowed(url))
        return false;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setAnchor(true);
    fmt.setAnchorHref(url);
    fmt.setFontUnderline(true);
    fmt.setForeground(QBrush(QColor("#4A90D9"))); // Subtle link blue
    cursor.mergeCharFormat(fmt);

    return true;
}

void RichTextController::removeHyperlink(int selStart, int selEnd)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setAnchor(false);
    fmt.setAnchorHref(QString());
    fmt.setFontUnderline(false);
    fmt.clearForeground();
    cursor.mergeCharFormat(fmt);
}

// Highlight (migrated from TextHighlighter)

void RichTextController::applyHighlight(int selStart, int selEnd, const QColor &color)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setBackground(QBrush(color));
    cursor.mergeCharFormat(fmt);
}

void RichTextController::removeHighlight(int selStart, int selEnd)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;
    fmt.setBackground(Qt::NoBrush);
    cursor.mergeCharFormat(fmt);
}

// Clear Formatting

void RichTextController::clearCharFormatting(int selStart, int selEnd)
{
    QTextDocument *doc = document();
    if (!doc || selStart == selEnd)
        return;

    QTextCursor cursor(doc);
    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);

    // Reset to a completely clean character format
    QTextCharFormat cleanFmt;
    // Set explicit defaults so we override everything
    cleanFmt.setFontWeight(QFont::Normal);
    cleanFmt.setFontItalic(false);
    cleanFmt.setFontUnderline(false);
    cleanFmt.setFontStrikeOut(false);
    cleanFmt.setForeground(QBrush());         // Inherit default
    cleanFmt.setBackground(Qt::NoBrush);      // No highlight
    cleanFmt.setAnchor(false);                // Remove links
    cleanFmt.setAnchorHref(QString());
    cleanFmt.clearProperty(QTextFormat::FontPointSize);
    cleanFmt.clearProperty(QTextFormat::FontFamilies);

    // Use setCharFormat (not merge) to replace completely
    cursor.setCharFormat(cleanFmt);
}

void RichTextController::clearBlockFormatting(int selStart, int selEnd)
{
    QTextDocument *doc = document();
    if (!doc)
        return;

    if (selEnd <= selStart)
        selEnd = selStart + 1;

    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    QTextBlock startBlock = doc->findBlock(selStart);
    QTextBlock endBlock = doc->findBlock(selEnd - 1);
    if (!endBlock.isValid())
        endBlock = doc->lastBlock();

    for (QTextBlock block = startBlock; block.isValid(); block = block.next()) {
        QTextCursor blockCursor(doc);
        blockCursor.setPosition(block.position());

        // Remove from any list
        QTextList *list = block.textList();
        if (list) {
            list->remove(block);
        }

        // Reset block format to defaults
        QTextBlockFormat cleanBlock;
        cleanBlock.setHeadingLevel(0);
        cleanBlock.setIndent(0);
        cleanBlock.setLeftMargin(0);
        cleanBlock.setAlignment(Qt::AlignLeft);
        blockCursor.setBlockFormat(cleanBlock);

        if (block == endBlock)
            break;
    }

    cursor.endEditBlock();
}

void RichTextController::clearAllFormatting(int selStart, int selEnd)
{
    QTextDocument *doc = document();
    if (!doc)
        return;

    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    clearBlockFormatting(selStart, selEnd);
    clearCharFormatting(selStart, selEnd);

    cursor.endEditBlock();
}

//Format Detection

QVariantMap RichTextController::currentCharFormat(int cursorPos)
{
    QVariantMap result;
    QTextDocument *doc = document();
    if (!doc) {
        // Return safe defaults
        result[QStringLiteral("bold")] = false;
        result[QStringLiteral("italic")] = false;
        result[QStringLiteral("underline")] = false;
        result[QStringLiteral("strikethrough")] = false;
        result[QStringLiteral("fontSize")] = 12.0;
        result[QStringLiteral("fontFamily")] = QStringLiteral("Open Sans");
        result[QStringLiteral("textColor")] = QString();
        result[QStringLiteral("highlightColor")] = QString();
        result[QStringLiteral("isLink")] = false;
        result[QStringLiteral("linkUrl")] = QString();
        return result;
    }

    QTextCursor cursor(doc);
    cursor.setPosition(cursorPos);
    QTextCharFormat fmt = cursor.charFormat();

    result[QStringLiteral("bold")] = (fmt.fontWeight() >= QFont::Bold);
    result[QStringLiteral("italic")] = fmt.fontItalic();
    result[QStringLiteral("underline")] = fmt.fontUnderline();
    result[QStringLiteral("strikethrough")] = fmt.fontStrikeOut();

    double fontSize = fmt.fontPointSize();
    result[QStringLiteral("fontSize")] = (fontSize > 0) ? fontSize : 12.0;

    QStringList families = fmt.fontFamilies().toStringList();
    result[QStringLiteral("fontFamily")] = families.isEmpty()
        ? QStringLiteral("Open Sans")
        : families.first();

    // Text color
    if (fmt.foreground().style() != Qt::NoBrush) {
        result[QStringLiteral("textColor")] = fmt.foreground().color().name();
    } else {
        result[QStringLiteral("textColor")] = QString();
    }

    // Highlight color
    if (fmt.background().style() != Qt::NoBrush) {
        result[QStringLiteral("highlightColor")] = fmt.background().color().name();
    } else {
        result[QStringLiteral("highlightColor")] = QString();
    }

    // Hyperlink
    result[QStringLiteral("isLink")] = fmt.isAnchor();
    result[QStringLiteral("linkUrl")] = fmt.anchorHref();

    return result;
}

QString RichTextController::currentBlockType(int cursorPos)
{
    QTextDocument *doc = document();
    if (!doc)
        return QStringLiteral("normal");

    QTextCursor cursor(doc);
    cursor.setPosition(cursorPos);

    QTextBlock block = cursor.block();
    QTextBlockFormat blockFmt = block.blockFormat();

    // Check heading level first
    int headingLevel = blockFmt.headingLevel();
    if (headingLevel >= 1 && headingLevel <= 3)
        return QStringLiteral("heading%1").arg(headingLevel);

    // Check if in a list
    QTextList *list = block.textList();
    if (list) {
        QTextListFormat::Style style = list->format().style();
        if (style == QTextListFormat::ListDisc
            || style == QTextListFormat::ListCircle
            || style == QTextListFormat::ListSquare) {
            return QStringLiteral("bulletList");
        }
        if (style == QTextListFormat::ListDecimal
            || style == QTextListFormat::ListLowerAlpha
            || style == QTextListFormat::ListUpperAlpha
            || style == QTextListFormat::ListLowerRoman
            || style == QTextListFormat::ListUpperRoman) {
            return QStringLiteral("numberedList");
        }
    }

    // Check blockquote (by left margin heuristic)
    if (blockFmt.leftMargin() >= kBlockquoteMargin)
        return QStringLiteral("blockquote");

    return QStringLiteral("normal");
}

int RichTextController::currentIndentLevel(int cursorPos)
{
    QTextDocument *doc = document();
    if (!doc)
        return 0;

    QTextCursor cursor(doc);
    cursor.setPosition(cursorPos);

    QTextBlock block = cursor.block();

    // Check list indent first
    QTextList *list = block.textList();
    if (list)
        return qMax(0, list->format().indent() - 1);

    // Fall back to block indent
    return block.blockFormat().indent();
}

bool RichTextController::isBlockEmpty(int cursorPos)
{
    QTextDocument *doc = document();
    if(!doc)
        return true;

    QTextCursor cursor(doc);
    cursor.setPosition(cursorPos);
    return cursor.block().text().isEmpty();
}
