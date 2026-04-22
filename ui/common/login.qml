import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Vault.Security 1.0
import DCharVault

Item {
    id: loginRoot
    property string selectedDBUrl: ""
    // loginRoot background
    Rectangle {
        anchors.fill: parent
        color: ThemeManager.bgVault
    }

    FileDialog {
        id: vaultSelector
        title: "Select Encrypted Vault"
        nameFilters: ["Database files (*.db)", "All files(*)"]
        onAccepted: {
            console.log("Qml: Vault Selected: " + currentFile)
            selectedDBUrl = currentFile
        }
    }

    //login card (white centred)
    Rectangle {
        anchors.centerIn: parent
        width: 340
        height: mainColumn.implicitHeight + 40
        color: ThemeManager.bgCard
        border.color: ThemeManager.lineBorder
        border.width: 1
        radius: 12

        ColumnLayout {
            id: mainColumn
            anchors.centerIn: parent
            width: parent.width - 40
            spacing: 11

            Label {
                text: "Unlock DCharVault"
                font.pixelSize: 22
                font.bold: true
                color: ThemeManager.textMain
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 10
            }

            Button {
                text: selectedDBUrl === "" ? "Select Vault File..." : "Change Vault File"
                Layout.preferredHeight: ThemeManager.controlHeight
                Layout.fillWidth: true

                background: Rectangle {
                    color: parent.hovered ? ThemeManager.bgButtonHover : ThemeManager.bgButton
                    radius: ThemeManager.radiusDefault
                    border.color: ThemeManager.lineBorder
                }
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: vaultSelector.open()
            }

            Text {
                text: selectedDBUrl === "" ? "No vault selected" : "Ready to unlock selected vault."
                color: selectedDBUrl === "" ? "#ef4444" : "#10b981"
                font.pixelSize: 13
                Layout.alignment: Qt.AlignHCenter
            }

            // The Visual Box
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: ThemeManager.controlHeight
                color: ThemeManager.bgInput
                border.color: secureInput.activeFocus ? ThemeManager.colorAccent : ThemeManager.lineBorder
                border.width: secureInput.activeFocus ? 2 : 1
                radius: 7
                // The invisible C++ key interceptor
                SecurePasswordInput {
                    id: secureInput
                    anchors.fill: parent
                    focus: true
                    Text {
                        anchors.fill: parent
                        anchors.margins: 12
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 24
                        color: ThemeManager.textMain
                        // This dynamically creates a string of dots exactly as long as your password
                        text: "".padStart(secureInput.passwordLength, "•")
                        // Placeholder text when empty
                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            text: "Enter Password"
                            color: ThemeManager.textMuted
                            font.pixelSize: 14
                            visible: secureInput.passwordLength === 0
                        }
                    }

                    // When the user hits Enter on their keyboard
                    onEnterPressed: {
                        console.log("QML: Enter pressed. Triggering C++ authentication.")
                        // Pass the actual component to C++, NOT a text string
                        if (selectedDBUrl != "") {
                            loginViewModel.authenticate(secureInput,
                                                        selectedDBUrl)
                        }
                    }
                }
            }

            Button {
                enabled: selectedDBUrl !== ""
                text: "Unlock Vault"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                background: Rectangle{
                    color: parent.enabled ? (parent.hovered ? Qt.lighter(ThemeManager.colorAccent, 1.1) : ThemeManager.colorAccent)
                                                              : ThemeManager.bgButton
                    radius: ThemeManager.radiusDefault
                    border.color: ThemeManager.lineBorder
                }
                contentItem: Text{
                    text: parent.text
                    color: parent.enabled ? "#FFFFFF" : ThemeManager.textMuted
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    loginViewModel.authenticate(secureInput, selectedDBUrl)
                }
            }
        }
    }
}
