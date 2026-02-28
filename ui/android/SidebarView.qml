import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // HomeView expects this signal to exist so it can close the drawer
    signal entrySelected()

    Rectangle {
        anchors.fill: parent
        color: "#EEEEEE"

        ColumnLayout {
            anchors.centerIn: parent

            Label {
                text: "Mobile Sidebar Placeholder"
            }

            Button {
                text: "Close Drawer Test"
                onClicked: root.entrySelected()
            }
        }
    }
}
