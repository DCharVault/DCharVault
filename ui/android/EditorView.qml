import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: textEditorPage
    title: "Editor"

    // Signal to tell HomeView to open the drawer
    signal menuClicked()

    // 1. The Top Bar for Actions
    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            // Changed from "<- Back" to a Menu button
            ToolButton {
                text: "☰"
                font.pixelSize: 20
                onClicked: textEditorPage.menuClicked()
            }

            // Spacer to push the Save button to the right
            Item { Layout.fillWidth: true }

            ToolButton {
                text: "Save"
                onClicked: console.log("Save button hit. Wire up your backend.")
            }
        }
    }

    // 2. The Editor Area
    ScrollView {
        anchors.fill: parent
        anchors.margins: 12

        TextArea {
            id: editorArea
            placeholderText: "Start typing..."
            wrapMode: TextEdit.Wrap
            font.pixelSize: 16

            // Native selection handles and persistent selection
            selectByMouse: true
            persistentSelection: true

            Component.onCompleted: forceActiveFocus()
        }
    }
}
