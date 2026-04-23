#include"ClipboardSanitizer.h"
#include<QGuiApplication>
#include<QString>
#include<QMimeData>
#include <algorithm>
//wrap debug logs to prevent console leakage in Release builds
#ifdef QT_DEBUG
#include <QDebug>
#endif

ClipboardSanitizer::ClipboardSanitizer(int timeoutMs, QObject *parent) : QObject(parent), m_clipboard(QGuiApplication::clipboard()), m_ownsClipboard(false){
    // Hard crash immediately on boot if OS denies clipboard access
    Q_ASSERT_X(m_clipboard != nullptr, "ClipboardSanitizer", "FATAL: System clipboard is inaccessible.");

    m_wipeTimer.setSingleShot(true);
    m_wipeTimer.setInterval(timeoutMs);

    connect(&m_wipeTimer, &QTimer::timeout, this, &ClipboardSanitizer::executeSanitization);
    connect(m_clipboard, &QClipboard::dataChanged, this, &ClipboardSanitizer::onSystemClipboardChanged);
}

ClipboardSanitizer::~ClipboardSanitizer() {
    // If app is closed, then only wipe if apps sensitive data is still in there.
    if (m_ownsClipboard) {
        executeSanitization();
    }
}

void ClipboardSanitizer::notifyCopied() {
    if (!m_ownsClipboard) {
        // first copy in a series — start a fresh timer
        m_ownsClipboard = true;
        m_wipeTimer.start();
    }
    // (user can copy multiple times within the same 30s window)
#ifdef QT_DEBUG
    qDebug() << "[SEC] Vault data copied. Sanitization locked and scheduled.";
#endif
}

void ClipboardSanitizer::onSystemClipboardChanged() {
    if (QGuiApplication::applicationState() != Qt::ApplicationActive) {
        // User is in another app. Relinquish ownership.
        if (m_ownsClipboard) {
            m_ownsClipboard = false;
            m_wipeTimer.stop();
#ifdef QT_DEBUG
            qDebug() << "[SEC] External copy detected. Relinquishing ownership.";
#endif
        }
    } else {
        // The app is active. We just copied something via Ctrl+C.
        if (!m_ownsClipboard) {
            m_ownsClipboard = true;
            m_wipeTimer.start();
#ifdef QT_DEBUG
            qDebug() << "[SEC] Auto-detected Ctrl+C. Timer started.";
#endif
        }
    }
}

void ClipboardSanitizer::wipeNow(){
    if(m_ownsClipboard){
        executeSanitization();
#ifdef QT_DEBUG
        qDebug() << "[SEC] Manual wipe triggered.";
#endif
    }
}

void ClipboardSanitizer::overwriteWithGarbage() {
    if (m_clipboard) {
        //to match the original data size
        const QMimeData *original = m_clipboard->mimeData();
        int estimatedSize = 1024;
        if (original && original->hasText()) {
            estimatedSize = std::max(1024, (int)original->text().size());
        }
        // overwrite with at least 1KB of garbage, or matched size
        int overwriteSize = std::max(1024, estimatedSize);
        m_clipboard->setText(QString(overwriteSize, '*'));
        m_clipboard->clear();
#ifdef QT_DEBUG
        qDebug() << "[SEC] Clipboard overwritten (" << estimatedSize << " bytes) and cleared.";
#endif
    }
}

void ClipboardSanitizer::executeSanitization() {
    if (m_ownsClipboard && m_clipboard) {
        m_ownsClipboard = false;
        overwriteWithGarbage();

#ifdef QT_DEBUG
        qDebug() << "[SEC] Clipboard memory overwritten and sanitized.";
#endif
    }
}
