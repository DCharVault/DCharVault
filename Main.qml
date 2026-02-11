import QtQuick
import QtQuick.Controls
import QtQuick.Window

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("DCharVault")
    color: "#1e1e1e"

    StackView{
        id: stack
        anchors.fill: parent

        // start with home view
        initialItem: HomeView{
            // handle the signal from HomeView
            onCreateClicked:{
                stack.push("ui/EditorView.qml")
            }
        }
    }
}
