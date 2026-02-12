import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page{
    id: root
    title: "DCharVault"

    // signal to tell Main.qml to switch screens
    signal createClicked();

    // background color
    background: Rectangle{ color: "#1e1e1e"}

    // list
    ListView{
        id: diaryList
        anchors.fill: parent
        spacing: 10
        clip: true

        // FAKE DATA (We will delete this later and connect C++)
        model: ListModel {
            ListElement { date: "2025-02-09"; title: "Started Project"; preview: "Today I set up the repo..." }
            ListElement { date: "2025-02-10"; title: "C++ is fast"; preview: "Learned about StackView..." }
        }

        delegate: Rectangle{
            width: diaryList.width*0.95
            height: 80
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#2d2d2d"
            radius: 10
            ColumnLayout{
                anchors.fill: parent
                anchors.margins: 10
                Text { text: model.date; color: "#aaaaaa"; font.pixelSize: 12 }
                Text { text: model.title; color: "white"; font.bold: true; font.pixelSize: 16 }
                Text { text: model.preview; color: "#cccccc"; font.pixelSize: 14; elide: Text.ElideRight }
            }
        }
    }

    // the 'FAB' btn (Floating Action Btn)
    Button{
        text: "+"
        width: 60
        height: 60
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        background: Rectangle{
            color: "#007BFF" // blue accent
            radius: 30
        }
        contentItem: Text{
            text:"+"
            color: "white"
            font.pixelSize: 30
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        // When clicked, fire the signal
        onClicked: root.createClicked()
    }
}
