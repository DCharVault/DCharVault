import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle { color: "#FAFAFA" }

    // 1. The Secure Router. It only loads one component into memory at a time.
    Loader {
        id: appRouter
        anchors.fill: parent
        sourceComponent: loginComponent // Boot straight to the login screen
    }

    // --- STATE 1: UNAUTHENTICATED ---
    Component {
        id: loginComponent

        // This is the LoginView we designed earlier
        AndroidLogin{
            // Wait for the C++ engine to fire the success signal
            Connections {
                target: loginViewModel
                function onLoginSuccess() {
                    console.log("QML: Vault Unlocked. Incinerating Login Screen.")
                    // 2. Overwrite the Loader. This physically destroys LoginView.
                    appRouter.sourceComponent = authenticatedStackComponent
                }
            }
        }
    }

    // --- STATE 2: AUTHENTICATED ---
    Component {
        id: authenticatedStackComponent

        // 3. The internal navigation for the actual diary
        StackView {
            id: mainStack
            anchors.fill: parent

            // The first screen they see after logging in
            initialItem: EditorView {
                // When they tap a note, slide the editor over it
                onEntrySelected: function(entryId) {
                    mainStack.push(editorComponent, { "currentEntryId": entryId })
                }
            }
        }
    }

    // --- STATE 3: THE EDITOR ---
    Component {
        id: editorComponent
        EditorView {
            onBackClicked: {
                // Slide back to the list
                appRouter.item.pop()
            }
        }
    }
}
