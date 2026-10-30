#ifndef DCHARVAULT_FINDREPLACECONTROLLER_H
#define DCHARVAULT_FINDREPLACECONTROLLER_H

#include <QObject>
#include <QQmlEngine>
#include <QQuickTextDocument>
#include <QTextCursor>
#include <QList>
#include "model/SecureAllocator.h"

class FindReplaceController : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
    Q_PROPERTY(int matchCount READ matchCount NOTIFY matchesChanged)
    Q_PROPERTY(int currentMatch READ currentMatch NOTIFY currentMatchChanged)
    Q_PROPERTY(bool hasMatches READ hasMatches NOTIFY matchesChanged)
    Q_PROPERTY(bool caseSensitive READ caseSensitive WRITE setCaseSensitive NOTIFY optionsChanged)
    Q_PROPERTY(bool wholeWord READ wholeWord WRITE setWholeWord NOTIFY optionsChanged)

public:
    explicit FindReplaceController(QObject *parent = nullptr);
    ~FindReplaceController();

    Q_INVOKABLE void findAll(const QString &searchTerm);
    Q_INVOKABLE void findNext();
    Q_INVOKABLE void findPrevious();
    Q_INVOKABLE void replaceCurrent(const QString &replacement);
    Q_INVOKABLE void replaceAll(const QString &replacement);

    Q_INVOKABLE void purge(); // wipe all state

    QQuickTextDocument* textDocument() const;
    void setTextDocument(QQuickTextDocument *doc);
    int matchCount() const;
    int currentMatch() const;
    bool hasMatches() const;
    bool caseSensitive() const;
    void setCaseSensitive(bool cs);
    bool wholeWord() const;
    void setWholeWord(bool w);

signals:
    void textDocumentChanged();
    void matchesChanged();
    void currentMatchChanged();
    void optionsChanged();
    void scrollToPosition(int position);

private:
    QQuickTextDocument *m_textDocument = nullptr;
    QList<QTextCursor> m_matches; // cached match cursors
    int m_currentIndex = -1;
    bool m_caseSensitive = false;
    bool m_wholeWord = false;

    SecureString m_lastSearchTerm;

    QTextDocument* document() const;
    void clearHighlights();
    void applyHighlights();
    void navigateToMatch();

    static void secureWipeQString(QString &str);
};

#endif //DCHARVAULT_FINDREPLACECONTROLLER_H
