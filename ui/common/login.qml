import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 300
    height: 200
    visible: true
    title: "QML Sample Login"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 10

        TextField {
            id: passwordInput
            placeholderText: "Enter Password..."
            Layout.fillWidth: true
        }

        Button {
            text: "Unlock Vault"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                // Assuming your TextField has the id: passwordInput
                loginViewModel.attemptLogin(passwordInput.text)
            }
        }
    }

    Connections {
        target: loginViewModel
        function onLoginSuccess() {
            console.log("QML: Vault unlocked! Time to change screens.")
            // TODO : Actually change the screen to the diary list
        }
        function onLoginFailed() {
            console.log("QML: Wrong password or crypto error.")
            // TODO tomorrow: Show a red error text on the UI
        }
    }
}
