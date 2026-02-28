import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle { color: "#FAFAFA" }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        // Custom Handle
        handle: Rectangle {
            implicitWidth: 1
            color: "#E0E0E0"
        }

        // --- LEFT PANE (Desktop Only) ---
        SidebarView {
            SplitView.preferredWidth: 300
            SplitView.minimumWidth: 200
            SplitView.maximumWidth: 400
        }

        // --- RIGHT PANE (Editor) ---
        EditorView {
            SplitView.fillWidth: true
        }
    }
}
