#ifndef SECUREPASSWORDINPUT_H
#define SECUREPASSWORDINPUT_H

#include"model/SecureAllocator.h"

#include<QQuickItem>
#include<QKeyEvent>

class SecurePasswordInput : public QQuickItem{
    Q_OBJECT

    // QML can only see length not actul characters
    Q_PROPERTY(int passwordLength READ passwordLength NOTIFY passwordLengthChanged)

public:
    explicit SecurePasswordInput(QQuickItem *parent = nullptr);
    ~SecurePasswordInput();

    int passwordLength() const;

    //to securely wipe buffer when done
    Q_INVOKABLE void clearPassword();

    // loginViewModel now can pull secure string purely in C++
    const SecureString& getSecureBuffer() const;

signals:
    void passwordLengthChanged();
    void enterPressed();

protected:
    // interceptor: catches raw clicks from OS
    // intercept logic
    void keyPressEvent(QKeyEvent* event) override;

    //catch mouse clicks so invisible box can gain keyboard focus
    void mousePressEvent(QMouseEvent *event) override;

private:
    SecureString m_secureBuffer;
};

#endif // SECUREPASSWORDINPUT_H
