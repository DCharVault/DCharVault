import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Vault.Security 1.0
import DCharVault

Item {
    id: loginRoot
    property string selectedDBUrl: ""
    property bool isCreatingNew: false

    Rectangle {
        anchors.fill: parent
        color: ThemeManager.bgVault
    }

    FileDialog {
        id: vaultSelector
        title: "Select Journal"
        nameFilters: ["Database files (*.db)", "All files(*)"]
        fileMode: FileDialog.OpenFile
        onAccepted: {
            console.log("Qml: Vault Selected: " + currentFile)
            selectedDBUrl = currentFile
        }
    }

    FileDialog {
        id: saveVaultDialog
        title: "Save New Journal"
        nameFilters: ["Database files(*.db)"]
        fileMode: FileDialog.SaveFile
        defaultSuffix: "db"
        currentFile: newJournalName.text !== "" ? "file:///" + newJournalName.text + ".db" : ""
        onAccepted: {
            console.log("QML: Save location selected: " + currentFile)
            // Name matches the ID of the creation password field below
            loginViewModel.createVault(newJournalName.text,newVaultPassword.secureInputComponent,
                                       currentFile)
        }
    }

    Rectangle {
        anchors.centerIn: parent
        width: 340
        height: mainColumn.implicitHeight + 40
        color: ThemeManager.bgCard
        border.color: ThemeManager.lineBorder
        border.width: 1
        radius: 12

        // ==========================================
        // STATE 1: LOGIN OLD JOURNAL
        // ==========================================
        ColumnLayout {
            id: loginColumn
            anchors.centerIn: parent
            width: parent.width - 40
            spacing: 11
            visible: !isCreatingNew // Hides when creating

            Label {
                text: "Unlock DCharVault"
                font.pixelSize: 22
                font.bold: true
                color: ThemeManager.textMain
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 10
            }

            Button {
                text: selectedDBUrl === "" ? "Select Journal File..." : "Change Journal File"
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
                text: selectedDBUrl === "" ? "No Journal selected" : "Ready to unlock selected Journal."
                color: selectedDBUrl === "" ? "#ef4444" : "#10b981"
                font.pixelSize: 13
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                text: "Enter Password"
                color: ThemeManager.textMain
                Layout.fillWidth: true
            }

            VaultPasswordField {
                id: loginPassword
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                placeholderText: "Enter Password"
                onEnterPressed: {
                    console.log("QML: Enter pressed. Triggering C++ authentication.")
                    if (selectedDBUrl !== "") {
                        loginViewModel.authenticate(
                                    loginPassword.secureInputComponent,
                                    selectedDBUrl)
                    }
                }
            }

            Button {
                enabled: selectedDBUrl !== ""
                text: "Unlock Journal"
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
                    console.log("QML: Button pressed. Triggering C++ authentication.")
                    if (selectedDBUrl !== "") {
                        loginViewModel.authenticate(
                                    loginPassword.secureInputComponent,
                                    selectedDBUrl)
                    }
                }
            }

            Button {
                text: "Create New Journal"
                flat: true
                Layout.fillWidth: true
                onClicked: {
                    isCreatingNew = true
                }
            }
        }

        // ==========================================
        // STATE 2: CREATE A NEW JOURNAL
        // ==========================================
        ColumnLayout {
            id: createColumn
            anchors.centerIn: parent
            width: parent.width - 40
            spacing: 11
            visible: isCreatingNew // Hides when logging in

            Label {
                text: "Create New Journal"
                font.pixelSize: 22
                font.bold: true
                color: ThemeManager.textMain // Removed hardcoded #111827, replaced with ThemeManager
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 10
            }

            Label {
                text: "Journal Name: "
                color: ThemeManager.textMain
                Layout.fillWidth: true
            }

            TextField {
                id: newJournalName
                placeholderText: "e.g My Journal"
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                color: ThemeManager.textMain
            }

            Label {
                text: "Set Master Password"
                color: ThemeManager.textMain
                Layout.fillWidth: true
            }

            VaultPasswordField {
                id: newVaultPassword
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                placeholderText: "Enter Password"
                onEnterPressed: {
                    if (newJournalName.text !== "") {
                        saveVaultDialog.open()
                    }
                }
            }

            Button {
                text: "Create New Journal"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                enabled: newJournalName.text !== ""
                onClicked: {
                    console.log("QML: Open save dialog to pick location.")
                    if (Qt.platform.os === "android") {
                        // ANDROID: Skip the FileDialog.
                        loginViewModel.createVaultAndroid(
                                    newJournalName.text,
                                    newVaultPassword.secureInputComponent)
                    } else {
                        // LINUX/WINDOWS: Open the FileDialog so the user can browse
                        saveVaultDialog.open()
                    }
                }
            }

            Button {
                text: "Back To Login"
                flat: true
                Layout.fillWidth: true
                onClicked: {
                    isCreatingNew = false
                    newJournalName.text = ""
                }
            }
        }
    }
}
