import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vault.Security 1.0

Item {
    id: root
    property int screenMargin: 24

    // Mobile apps usually auto-resolve their private path.
    // We will hardcode a standard internal path for testing right now.
    property string mobileVaultPath: "app_private_vault.db"

    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width - (root.screenMargin * 2)
        spacing: 20
        Label {
            text: "Enter Master Password"
            font.pixelSize: 18
            Layout.alignment: Qt.AlignHCenter
            color: "gray"
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56 // Standard Android Touch Target Size
            color: "transparent"
            border.color: "lightgray"
            border.width: 1
            radius: 8
            SecurePasswordInput {
                id: secureInput
                anchors.fill: parent
                anchors.margins: 4
                onEnterPressed: {
                    loginViewModel.authenticate(secureInput, mobileVaultPath)
                }
            }
        }
        Button {
            text: "Unlock Vault"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            enabled: secureInput.passwordLength > 0
            onClicked: {
                // Hitting the C++ Engine you just secured
                loginViewModel.authenticate(secureInput, mobileVaultPath)
            }
        }
    }
}
