import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import DCharVault

ApplicationWindow {
    id: rootWindow
    width: 800
    height: 600
    visible: true
    title: "DCharVault"

    // This property tracks whether the vault is locked or unlocked
    property bool isVaultUnlocked: false

    // The Loader dynamically loads QML files based on our state
    Loader {
        id: mainLoader
        anchors.fill: parent
        // If unlocked, load the Editor. If locked, load the Login view.
        // NOTE: Adjust these file paths to match your actual folder structure!
        source: isVaultUnlocked ? "ui/desktop/HomeView.qml" : "ui/common/login.qml"
    }

    // Listen for the C++ login signal right here at the root level
    Connections {
        target: loginViewModel

        function onLoginSuccess() {
            console.log("Main.qml: Vault unlocked. Switching to EditorView.")
            // This single line changes the state and tells the Loader to swap the UI
            isVaultUnlocked = true
        }
    }
}
