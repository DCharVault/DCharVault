/*
 * SECURITY LIMITATIONS:
 *
 * This sanitizer can only control the primary clipboard. It CANNOT prevent:
 * - Windows Clipboard History (Win+V) from persisting copies
 * - macOS Universal Clipboard from syncing to iCloud
 * - Third-party clipboard managers from logging all clipboard operations
 * - Wayland clipboard paste models on Linux
 * - Screenshots/screen recording software from capturing paste events
 *
 * Users should disable Clipboard History and clipboard managers if handling
 * highly sensitive data.
 */


#ifndef CLIPBOARDSANITIZER_H
#define CLIPBOARDSANITIZER_H

#include<QObject>
#include<QTimer>
#include<QClipboard>

class ClipboardSanitizer : public QObject{
    Q_OBJECT
public:
    explicit ClipboardSanitizer(int timeoutMs = 30000, QObject *parent = nullptr);
    ~ClipboardSanitizer();

    Q_INVOKABLE void notifyCopied();
    Q_INVOKABLE void wipeNow();

private slots:
    void onSystemClipboardChanged();
    void executeSanitization();

signals:
    void clipboardSanitized(); // emitted after wipe
    void externalClipboardDetected(); // emitted when external change detected

private:
    void overwriteWithGarbage();

    QTimer m_wipeTimer;
    QClipboard *m_clipboard;

    bool m_ownsClipboard;
};

#endif // CLIPBOARDSANITIZER_H
