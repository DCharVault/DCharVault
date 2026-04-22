#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QWindow>
#include <sodium.h>
#include <QQmlContext>// Required to inject C++ into QML
#include <QQuickStyle>
#include "model/DiaryManager.h"
#include"viewmodel/SecurePasswordInput.h"
#include "viewmodel/LoginViewModel.h"
#include "viewmodel/DiaryViewModel.h"
#include "viewmodel/DiaryListModel.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
//MinGW lacks definition defined manually as 20.
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // SECURITY CHECK
    if (sodium_init() < 0) {
        qCritical() << "FATAL: Libsodium failed to initialize!";
        return -1;
    }
    qInfo() << "SUCCESS: Libsodium initialized. Architecture is Secure.";

    // Boot Backend
    DiaryManager diaryManager;
    // Boot View Model
    LoginViewModel loginVM(diaryManager);
    DiaryViewModel diaryVM(diaryManager);
    DiaryListModel diaryListModel(diaryManager);

    // UI LAUNCHER
    QQuickStyle::setStyle("Basic");
    QQmlApplicationEngine engine;

    // register type SecurePasswordInput
    qmlRegisterType<SecurePasswordInput>("Vault.Security",1,0,"SecurePasswordInput");

    // --- 3. INJECT THE BRIDGE INTO QML ---
    // This creates a global variable named "loginViewModel" inside your QML files,
    // pointing directly to your C++ loginVM object.
    engine.rootContext()->setContextProperty("loginViewModel", &loginVM);
    engine.rootContext()->setContextProperty("diaryViewModel",&diaryVM);
    engine.rootContext()->setContextProperty("diaryListModel",&diaryListModel);

    engine.loadFromModule("DCharVault", "Main");
    // Check if it loaded
    if (engine.rootObjects().isEmpty())
        return -1;



#ifdef Q_OS_WIN
    // capture the list once as a const variable
    const QList<QObject *> rootObjects = engine.rootObjects();

    if (!rootObjects.isEmpty()) {
        // use constFirst() or at(0) — neither triggers a detach/copy
        QWindow *window = qobject_cast<QWindow *>(rootObjects.constFirst());
        if (window) {
            HWND hwnd = (HWND)window->winId();
            BOOL dark = TRUE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
        }
    }
#endif


    return app.exec();
}

