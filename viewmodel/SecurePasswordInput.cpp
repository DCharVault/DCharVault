#include"SecurePasswordInput.h"

#include<QDebug>

SecurePasswordInput::SecurePasswordInput(QQuickItem *parent) : QQuickItem(parent){
    // Tell the QML engine that this invisible box is allowed to receive keyboard focus
    setFlag(ItemIsFocusScope, true);
    setAcceptedMouseButtons(Qt::LeftButton);
}

SecurePasswordInput::~SecurePasswordInput(){
    clearPassword();
}

int SecurePasswordInput::passwordLength() const{
    return m_secureBuffer.length();
}

void SecurePasswordInput::clearPassword(){
    // Manually overwrite existing characters with zeros before clearing
    std::fill(m_secureBuffer.begin(),m_secureBuffer.end(),'\0');
    m_secureBuffer.clear();// Overwrites with zeros via SecureAllocator
    emit passwordLengthChanged();
}

const SecureString& SecurePasswordInput::getSecureBuffer() const{
    return m_secureBuffer;
}

// intercept clicks to grab keyboard focus
void SecurePasswordInput::mousePressEvent(QMouseEvent *event){
    forceActiveFocus();
    QQuickItem::mousePressEvent(event);
}

// intercept logic
void SecurePasswordInput::keyPressEvent(QKeyEvent *event){
    int key = event->key();
    // handle backspace
    if(key==Qt::Key_Backspace){
        if(!m_secureBuffer.empty()){
            m_secureBuffer.pop_back();
            emit passwordLengthChanged();
        }
        event->accept();
        return;
    }
    // handle enter/return key
    if(key==Qt::Key_Enter || key==Qt::Key_Return){
        emit enterPressed();
        event->accept();
        return;
    }
    // handle printable charaters
    QString text = event->text();
    if(!text.isEmpty() && text.at(0).isPrint()){
        // take the raw ASCII/UTF-8 byte
        QByteArray bytes = text.toUtf8(); // converts a char to utf8, some special characters or symbols are multi-byte that why we need QByteArray
        for (char b : std::as_const(bytes)) {
            m_secureBuffer.push_back(b);
        }
        emit passwordLengthChanged();
        event->accept();
        return;
    }
    // ignore other keys (Shift, ctrl, esc etc)
    //if user presses key(like Volume Up, or F1) skip all custom logic and pass event back to QQuickItem::keyPressEvent(event) so standard Qt engine can handle it normally.
    QQuickItem::keyPressEvent(event);
}

