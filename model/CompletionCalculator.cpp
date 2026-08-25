#include "CompletionCalculator.h"

#include<QTextDocument>
#include<QTextBlock>
#include<QTextBlockFormat>

#ifndef QT_DEBUG
#include<QDebug>
#endif

CompletionResult CompletionCalculator::calculate(const QString &richTextHtml)
{
    CompletionResult result;
    if (richTextHtml.isEmpty()) return result;

    QTextDocument doc;
    doc.setHtml(richTextHtml);
    for (QTextBlock block = doc.begin(); block != doc.end(); block = block.next()) {
        const QTextBlockFormat::MarkerType marker = block.blockFormat().marker();
        if (marker == QTextBlockFormat::MarkerType::Unchecked) {
            ++result.total;
        } else if (marker == QTextBlockFormat::MarkerType::Checked) {
            ++result.total;
            ++result.completed;
        }
    }
#ifdef QT_DEBUG
    qDebug() << "[CompletionCalculator] Result:"
             << result.completed << "/" << result.total
             << "(" << result.percentage() << "%)";
#endif
    return result;
}