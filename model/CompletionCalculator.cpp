#include "CompletionCalculator.h"

#include<QTextDocument>
#include<QTextBlock>
#include<QTextBlockFormat>

#ifdef QT_DEBUG
#include<QDebug>
#endif

CompletionResult CompletionCalculator::calculate(const QString &richTextHtml)
{
    CompletionResult result;
    if (richTextHtml.isEmpty()) return result;

    // Fast parsing: count the injected zero-width tags directly
    result.completed = richTextHtml.count(QStringLiteral("\x200B" "CB:1" "\x200B"));
    int unchecked    = richTextHtml.count(QStringLiteral("\x200B" "CB:0" "\x200B"));
    result.total     = result.completed + unchecked;

#ifdef QT_DEBUG
    qDebug() << "[CompletionCalculator] Result:"
             << result.completed << "/" << result.total
             << "(" << result.percentage() << "%)";
#endif
    return result;
}