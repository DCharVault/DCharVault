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
            loginViewModel.createVault(newVaultPassword.secureInputComponent,
                                       currentFile)
        }
    }

    Rectangle {
        anchors.centerIn: parent
        width: 340
<<<<<<< HEAD
        height: mainColumn.implicitHeight + 40
        color: ThemeManager.bgCard
        border.color: ThemeManager.lineBorder
=======
        // Dynamically sizes the card based on which layout is active
        height: (isCreatingNew ? createColumn.implicitHeight : loginColumn.implicitHeight) + 40
        color: "#ffffff"
        radius: 12
        border.color: "#e5e7eb"
>>>>>>> 855b2e4 (UI Logic For Journal Creation)
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
<<<<<<< HEAD
                text: selectedDBUrl === "" ? "Select Vault File..." : "Change Vault File"
                Layout.preferredHeight: ThemeManager.controlHeight
=======
                text: selectedDBUrl === "" ? "Select Journal File..." : "Change Journal File"
>>>>>>> 855b2e4 (UI Logic For Journal Creation)
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
                text: selectedDBUrl
                      === "" ? "No Journal selected" : "Ready to unlock selected Journal."
                color: selectedDBUrl === "" ? "#ef4444" : "#10b981"
                font.pixelSize: 13
                Layout.alignment: Qt.AlignHCenter
            }

<<<<<<< HEAD
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
=======
            Label {
                text: "Enter Password"
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
>>>>>>> 855b2e4 (UI Logic For Journal Creation)
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

            // Missing button restored to toggle state
            Button {
                text: "Create New Journal"
                flat: true
                Layout.fillWidth: true
                onClicked: {
                    isCreatingNew = true
                }
            }
        } // End of loginColumn

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
                color: "#111827"
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 10
            }

            Label {
                text: "Journal Name: "
                Layout.fillWidth: true
            }

            TextField {
                id: newJournalName
                placeholderText: "e.g My Journal"
                Layout.fillWidth: true
                Layout.preferredHeight: 30
            }

            Label {
                text: "Set Master Password"
                Layout.fillWidth: true
            }

            VaultPasswordField {
                id: newVaultPassword // ID updated to match dialog reference
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

            // Back button moved to correct layout scope
            Button {
                text: "Back To Login"
                flat: true
                Layout.fillWidth: true
                onClicked: {
                    isCreatingNew = false
                    newJournalName.text = ""
                }
            }
        } // End of createColumn
    }
}
