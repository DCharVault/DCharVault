#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <sodium.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // --- SECURITY CHECK ---
    if (sodium_init() < 0) {
        qCritical() << "FATAL: Libsodium failed to initialize!";
        return -1;
    }
    qInfo() << "SUCCESS: Libsodium initialized. Architecture is Secure.";

    // --- UI LAUNCHER ---
    QQmlApplicationEngine engine;

    // MAGICAL FIX: Load directly from the compiled module
    // This works regardless of where the file is on the Z: drive
    engine.loadFromModule("DCharVault", "Main");

    // Check if it loaded
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
