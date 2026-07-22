#ifndef RICHTEXTCONTROLLER_H
#define RICHTEXTCONTROLLER_H

#include <QObject>
#include <QQuickTextDocument>
#include <QColor>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QTextListFormat>
#include <QTextList>
#include <QUrl>
#include <QQmlEngine>
#include <QVariantMap>
#include <QStringList>

/**
 * @brief RichTextController — Full formatting engine for the DCharVault editor.
 *
 * Replaces and extends TextHighlighter. Provides Q_INVOKABLE methods for all
 * rich text formatting operations, plus format detection for toolbar state sync.
 */
class RichTextController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)

    /**
     * @brief Controls whether remote resource loading is blocked on paste.
     *        Default: true (all remote resources are blocked).
    */

    Q_PROPERTY(bool blockRemoteResources READ blockRemoteResources WRITE setBlockRemoteResources NOTIFY blockRemoteResourcesChanged)
    /**
     * @brief Read-only list of system font families, populated once on first access.
     *        Used by QML toolbar to populate font family dropdown.
     */
    Q_PROPERTY(QStringList availableFontFamilies READ availableFontFamilies CONSTANT)

public:
    explicit RichTextController(QObject *parent = nullptr);

    QQuickTextDocument* textDocument() const;
    void setTextDocument(QQuickTextDocument *doc);
    QStringList availableFontFamilies() const;

    bool blockRemoteResources() const;
    Q_INVOKABLE void setBlockRemoteResources(bool block);

    //  Text Style Formatting

    /** @brief Set bold state on selection. Merge-safe: preserves other formatting. */
    Q_INVOKABLE void setBold(int selStart, int selEnd, bool enable);

    /** @brief Set italic state on selection. Merge-safe. */
    Q_INVOKABLE void setItalic(int selStart, int selEnd, bool enable);

    /** @brief Set underline state on selection. Merge-safe. */
    Q_INVOKABLE void setUnderline(int selStart, int selEnd, bool enable);

    /** @brief Set font point size on selection. Merge-safe: won't strip bold/italic/etc. */
    Q_INVOKABLE void setFontSize(int selStart, int selEnd, qreal pointSize);

    /** @brief Set text foreground color on selection. Merge-safe. */
    Q_INVOKABLE void setTextColor(int selStart, int selEnd, const QColor &color);

    /** @brief Toggle strikethrough on the selection [selStart, selEnd). */
    Q_INVOKABLE void toggleStrikethrough(int selStart, int selEnd);

    /** @brief Explicitly set strikethrough state on the selection. Merge-safe. */
    Q_INVOKABLE void setStrikethrough(int selStart, int selEnd, bool enable);


    //  Block-Level Formatting

    /**
     * @brief Apply heading level (1-3) to blocks in [selStart, selEnd).
     *        Level 0 resets to normal paragraph.
     *        H1 = 28pt bold, H2 = 22pt bold, H3 = 18pt bold.
     */
    Q_INVOKABLE void setHeading(int selStart, int selEnd, int level);

    /**
     * @brief Toggle bullet (unordered) list on the block at cursorPos.
     *        If already a bullet list, removes it. Otherwise creates one.
     */
    Q_INVOKABLE void toggleBulletList(int cursorPos);

    /**
     * @brief Toggle numbered (ordered) list on the block at cursorPos.
     *        If already a numbered list, removes it. Otherwise creates one.
     */
    Q_INVOKABLE void toggleNumberedList(int cursorPos);

    /**
     * @brief Toggle blockquote on blocks in [selStart, selEnd).
     *        Blockquote = left indent + left border hint via increased margin.
     */
    Q_INVOKABLE void toggleBlockquote(int selStart, int selEnd);

    /** @brief Insert a horizontal rule (<hr>) at cursorPos. */
    Q_INVOKABLE void insertHorizontalRule(int cursorPos);

    /**
     * @brief Increase indent level of the block at cursorPos.
     *        Max indent depth: 8 levels.
     */
    Q_INVOKABLE void indent(int cursorPos);

    /**
     * @brief Decrease indent level of the block at cursorPos.
     *        Min indent depth: 0.
     */
    Q_INVOKABLE void outdent(int cursorPos);

    //  Font Family

    /**
     * @brief Set the font family for selection [selStart, selEnd).
     *        Only system-installed fonts are accepted; invalid names are rejected.
     */
    Q_INVOKABLE void setFontFamily(int selStart, int selEnd, const QString &family);

    //  Hyperlinks

    /**
     * @brief Insert/apply a hyperlink to selection [selStart, selEnd).
     *        URL scheme whitelist: http://, https://, mailto: ONLY.
     *        Returns true if URL was accepted, false if rejected (scheme violation).
     */
    Q_INVOKABLE bool insertHyperlink(int selStart, int selEnd, const QString &url);

    /** @brief Remove hyperlink from selection, keeping the text. */
    Q_INVOKABLE void removeHyperlink(int selStart, int selEnd);

    // Highlight (migrated from TextHighlighter)

    /** @brief Apply background highlight color to selection. */
    Q_INVOKABLE void applyHighlight(int selStart, int selEnd, const QColor &color);

    /** @brief Remove background highlight from selection. */
    Q_INVOKABLE void removeHighlight(int selStart, int selEnd);

    //  Clear Formatting

    /**
     * @brief Strip ALL character formatting (bold, italic, underline, strikethrough,
     *        color, highlight, font size, font family, links) from selection.
     *        Resets to default paragraph font.
     */
    Q_INVOKABLE void clearCharFormatting(int selStart, int selEnd);

    /**
     * @brief Strip ALL block formatting (heading, list, blockquote, indent, alignment)
     *        from blocks in selection. Resets to normal paragraph.
     */
    Q_INVOKABLE void clearBlockFormatting(int selStart, int selEnd);

    /**
     * @brief Strip ALL formatting (character + block) from selection.
     *        Convenience method that calls clearCharFormatting + clearBlockFormatting.
     */
    Q_INVOKABLE void clearAllFormatting(int selStart, int selEnd);

    //  Format Detection (Toolbar State Sync)

    /**
     * @brief Returns a QVariantMap describing the character format at cursorPos.
     *
     * Keys: "bold" (bool), "italic" (bool), "underline" (bool),
     *        "strikethrough" (bool), "fontSize" (double), "fontFamily" (string),
     *        "textColor" (color string), "highlightColor" (color string, empty if none),
     *        "isLink" (bool), "linkUrl" (string)
     */
    Q_INVOKABLE QVariantMap currentCharFormat(int cursorPos);

    /**
     * @brief Returns a string describing the block type at cursorPos.
     *
     * Returns one of: "normal", "heading1", "heading2", "heading3",
     *                  "bulletList", "numberedList", "blockquote"
     */
    Q_INVOKABLE QString currentBlockType(int cursorPos);

    /**
     * @brief Returns the current indent level (0-8) of the block at cursorPos.
     */
    Q_INVOKABLE int currentIndentLevel(int cursorPos);

    /**
     * @brief Returns true if the block at cursorPos is completely empty.
     */
    Q_INVOKABLE bool isBlockEmpty(int cursorPos);

signals:
    void textDocumentChanged();
    void blockRemoteResourcesChanged();

private:
    QQuickTextDocument *m_textDocument = nullptr;
    bool m_blockRemoteResources = true;
    /**
     * @brief Install a resource provider on the QTextDocument that blocks
     *        remote resources (images, fonts, CSS) to prevent IP leaking.
     */
    void installResourceGuard(QTextDocument *doc);

    /** @brief Get a valid QTextDocument pointer, or nullptr if unavailable. */
    QTextDocument* document() const;

    /** @brief Validate URL scheme against whitelist. */
    static bool isUrlSchemeAllowed(const QString &url);

    /** @brief Blockquote left margin in pixels. */
    static constexpr qreal kBlockquoteMargin = 40.0;

    /** @brief Indent step in pixels per level. */
    static constexpr qreal kIndentStep = 40.0;

    /** @brief Maximum indent depth. */
    static constexpr int kMaxIndentLevel = 8;

    /** @brief Maximum allowed size for data: URIs (5 MB). Prevents clipboard-based DoS. */
    static constexpr int kMaxDataUriBytes = 5 * 1024 * 1024;

    /** @brief Heading font sizes: index 0 unused, 1=H1, 2=H2, 3=H3. */
    static constexpr int kHeadingSizes[] = {0, 28, 22, 18};
};

#endif // RICHTEXTCONTROLLER_H
