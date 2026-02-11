import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page{
    id: editior
    title: "New Entry"

    signal backClicked()
    signal saveClicked(string text)
    background: Rectangle { color: "#1e1e1e" }

    header: ToolBar {
        background: Rectangle { color: "#2d2d2d" }
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: "< Back"
                onClicked: editor.backClicked()
            }
            Label {
                text: "Write Entry"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
                color: "white"
            }
            ToolButton {
                text: "Save"
                // later we will pass the text back to C++ here
                onClicked: editor.saveClicked(contentArea.text)
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        TextArea {
            id: contentArea
            placeholderText: "Dear Diary..."
            color: "white"
            font.pixelSize: 16
            background: null // Transparent background
            wrapMode: Text.Wrap
        }
    }

}
