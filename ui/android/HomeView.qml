import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle { color: "#FAFAFA" }

    // --- THE DRAWER (Mobile Only) ---
    Drawer {
        id: mobileDrawer
        width: Math.min(parent.width * 0.8, 300)
        height: parent.height
        interactive: true // Swipe-able on mobile

        SidebarView {
            anchors.fill: parent
            onEntrySelected: mobileDrawer.close()
        }
    }

    // --- THE MAIN VIEW (Editor) ---
    EditorView {
        anchors.fill: parent
        // This expects your Android EditorView to have a signal menuClicked()
        onMenuClicked: mobileDrawer.open()
    }
}
