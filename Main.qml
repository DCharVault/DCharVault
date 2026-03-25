import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import DCharVault

ApplicationWindow {
    id: rootWindow

    width: 800
    height: 700
    visible: true
    title: "DCharVault"

    StackView{
        id: startView
        anchors.fill: parent
        initialItem: "ui/common/login.qml"
    }
    Connections{
        target: loginViewModel
        function onLoginSuccess(){
            console.log("Main.qml: Vault unlocked. Sliding to HomeView.")
            //without locking the vault. 'replace' destroys the login screen and slides in the new one
            startView.replace("ui/desktop/HomeView.qml", StackView.PushTransition)
        }
    }
    function onVaultLocked(){
        console.log("Main.qml: Vault locked. Sliding back to Login.")
        // slide back to the login screen again and destroy the HomeView (clearing memory).
        stackView.replace("ui/common/login.qml", StackView.PopTransition)
    }

    // // This property tracks whether the vault is locked or unlocked
    // property bool isVaultUnlocked: false

    // // The Loader dynamically loads QML files based on our state
    // Loader {
    //     id: mainLoader
    //     anchors.fill: parent
    //     // If unlocked, load the Editor. If locked, load the Login view.
    //     // NOTE: Adjust these file paths to match your actual folder structure!
    //     source: isVaultUnlocked ? "ui/desktop/HomeView.qml" : "ui/common/login.qml"
    // }

    // // Listen for the C++ login signal right here at the root level
    // Connections {
    //     target: loginViewModel

    //     function onLoginSuccess() {
    //         console.log("Main.qml: Vault unlocked. Switching to EditorView.")
    //         // This single line changes the state and tells the Loader to swap the UI
    //         isVaultUnlocked = true
    //     }
    // }
}
