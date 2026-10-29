#include "FindReplaceController.h"

FindReplaceController::FindReplaceController(QObject *parent)
    : QObject(parent)
{
}

FindReplaceController::~FindReplaceController()
{
}

void FindReplaceController::findAll(const QString &searchTerm)
{
}

void FindReplaceController::findNext()
{
}

void FindReplaceController::findPrevious()
{
}

void FindReplaceController::replaceCurrent(const QString &replacement)
{
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
    return nullptr;
}

void FindReplaceController::clearHighlights()
{
}

void FindReplaceController::applyHighlights()
{
}

void FindReplaceController::navigateToMatch(int index)
{
}

void FindReplaceController::secureWipeQString(QString &str)
{
}