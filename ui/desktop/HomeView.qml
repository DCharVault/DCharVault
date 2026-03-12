import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle {
        color: "#FAFAFA"
    }

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
            onEntrySelected: function (entryId, entryTitle) {
                console.log("QML: User clicked entry ID:", entryId)
                mainEditor.currentEntryId = entryId
                mainEditor.entryTitle = entryTitle
                let secretContent = diaryViewModel.loadEntryContent(entryId)
                mainEditor.entryContent = secretContent
            }
            onCreateClicked: {
                console.log("QML: Preparing empty editor for new note")
                mainEditor.currentEntryId = -1
                mainEditor.entryTitle = ""
                mainEditor.entryContent = ""
            }
        }

        EditorView {
            id: mainEditor
            SplitView.fillWidth: true
        }
    }
}
