import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle { color: "#FAFAFA" }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        handle: Rectangle {
            implicitWidth: 2
            color: "#E0E0E0"
        }

        SidebarView {
            SplitView.preferredWidth: 230
            SplitView.minimumWidth: 0
            SplitView.maximumWidth: parent.width
        }

        EditorView {
            SplitView.fillWidth: true
        }
    }
}
