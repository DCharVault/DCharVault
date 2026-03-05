#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <sodium.h>
#include <QQmlContext>// Required to inject C++ into QML

#include "model/DiaryManager.h"
#include "viewmodel/LoginViewModel.h"
#include "viewmodel/DiaryViewModel.h"

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

    // UI LAUNCHER
    QQmlApplicationEngine engine;

    // --- 3. INJECT THE BRIDGE INTO QML ---
    // This creates a global variable named "loginViewModel" inside your QML files,
    // pointing directly to your C++ loginVM object.
    engine.rootContext()->setContextProperty("loginViewModel", &loginVM);
    engine.rootContext()->setContextProperty("diaryViewModel",&diaryVM);

    // MAGICAL FIX: Load directly from the compiled module
    // This works regardless of where the file is on the Z: drive
    engine.loadFromModule("DCharVault", "Main");

    // Check if it loaded
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
