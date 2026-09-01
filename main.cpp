#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QWindow>
#include <sodium.h>
#include <QQmlContext>
#include <QQuickStyle>
#include "model/DiaryManager.h"
#include"viewmodel/SecurePasswordInput.h"
#include "viewmodel/LoginViewModel.h"
#include "viewmodel/DiaryViewModel.h"
#include "viewmodel/SessionViewModel.h"
#include "viewmodel/DiaryListModel.h"
#include "viewmodel/SecureNetworkManager.h"
#include "viewmodel/DiarySearchModel.h"
#include "viewmodel/PriorityViewModel.h"

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

    if (sodium_init() < 0) {
        qCritical() << "FATAL: Libsodium failed to initialize!";
        return -1;
    }
    qInfo() << "SUCCESS: Libsodium initialized. Architecture is Secure.";

    DiaryManager diaryManager;

    LoginViewModel loginVM(diaryManager);
    DiaryViewModel diaryVM(diaryManager);
    PriorityViewModel priorityVM(diaryManager);
    SessionViewModel diarySM(&diaryManager);
    DiaryListModel diaryListModel(diaryManager);

    DiarySearchModel diarySearchModel;
    diarySearchModel.setSourceModel(&diaryListModel);

    QObject::connect(&loginVM, &LoginViewModel::loginSuccess,
                     &diarySM, &SessionViewModel::onVaultOpened);

    QObject::connect(&app, &QGuiApplication::applicationStateChanged,
                     &diarySM, &SessionViewModel::onApplicationStateChanged);


    QQuickStyle::setStyle("Basic");

    SecureNetworkManagerFactory namFactory;

    QQmlApplicationEngine engine;
    engine.setNetworkAccessManagerFactory(&namFactory);

    qmlRegisterType<SecurePasswordInput>("Vault.Security",1,0,"SecurePasswordInput");

    engine.rootContext()->setContextProperty("loginViewModel", &loginVM);
    engine.rootContext()->setContextProperty("diaryViewModel",&diaryVM);
    engine.rootContext()->setContextProperty("priorityViewModel", &priorityVM);
    engine.rootContext()->setContextProperty("diarySessionModel",&diarySM);
    engine.rootContext()->setContextProperty("diaryListModel",&diaryListModel);
    engine.rootContext()->setContextProperty("diarySearchModel",&diarySearchModel);

    engine.loadFromModule("DCharVault", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;



#ifdef Q_OS_WIN
    // capture the list once as a const variable
    const QList<QObject *> rootObjects = engine.rootObjects();

    if (!rootObjects.isEmpty()) {
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

