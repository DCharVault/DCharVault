import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import DCharVault

Item {
    id: root

    signal entrySelected(var entryId, string entryTitle)
    signal createClicked
    signal createDiaryClicked
    signal settingsClicked

    readonly property int globalMargin: 16

    Component.onCompleted: {
        console.log("Sidebar loaded. Requesting data from C++...")
        diaryListModel.loadEntries()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 1. TOP HEADER (Notebook Name & Search)
        Rectangle {
            Layout.fillWidth: true
            height: 110
            color: ThemeManager.bgCard

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: root.globalMargin
                anchors.rightMargin: root.globalMargin
                anchors.topMargin: 12
                anchors.bottomMargin: 12
                spacing: 12

                // Title Row
                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "My Notebook ▾"
                        font.pixelSize: 20
                        font.bold: true
                        color: ThemeManager.textMain
                        Layout.fillWidth: true
                    }
                }

                // Search Input
                TextField {
                    id: searchInput
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    color: ThemeManager.textMain
                    verticalAlignment: TextInput.AlignVCenter
                    leftPadding: 12
                    placeholderText: ""

                    Timer {
                        id: searchDebounceTimer
                        interval: 300
                        repeat: false
                        onTriggered: {
                            diarySearchModel.searchQuery = searchInput.text
                        }
                    }

                    onTextEdited: {
                        searchDebounceTimer.restart()
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        text: "🔍 Search..."
                        color: ThemeManager.textMuted
                        font.pixelSize: 14

                        visible: !searchInput.activeFocus
                                 && searchInput.text === ""
                    }

                    background: Rectangle {
                        color: ThemeManager.bgInput
                        radius: ThemeManager.radiusDefault
                        border.color: searchInput.activeFocus ? ThemeManager.colorAccent : ThemeManager.lineBorder
                        border.width: searchInput.activeFocus ? 2 : 1
                        Behavior on border.color {
                            ColorAnimation {
                                duration: 150
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
                color: ThemeManager.lineBorder
            }
        }

        // NOTE LIST
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: ThemeManager.bgVault

            ListView {
                id: noteList
                anchors.fill: parent
                clip: true

                leftMargin: root.globalMargin
                rightMargin: root.globalMargin
                topMargin: 12
                bottomMargin: 12
                spacing: 8

                ScrollBar.vertical: ScrollBar {
                    width: 8
                    policy: ScrollBar.AsNeeded
                    active: true
                }

                model: diarySearchModel

                delegate: Rectangle {
                    id: delegateRoot
                    // 12px right gutter for scrollbar
                    width: ListView.view.width - ListView.view.leftMargin
                           - ListView.view.rightMargin - 12
                    height: 64
                    radius: ThemeManager.radiusDefault

                    readonly property int  cCompleted: model.completionCompleted ?? 0
                    readonly property int  cTotal:     model.completionTotal     ?? 0
                    readonly property string cState:   model.completionState     ?? "none"
                    readonly property bool isSelected: noteList.currentIndex === index
                    readonly property bool isHovered:  delegateMouseArea.containsMouse

                    color: isSelected ? ThemeManager.surfaceElevated
                                      : (isHovered ? ThemeManager.bgButtonHover : ThemeManager.bgCard)
                    border.color: ThemeManager.lineBorder
                    border.width: isSelected ? 0 : 1

                    Behavior on color { ColorAnimation { duration: 150 } }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: 1
                        width: 3
                        color: ThemeManager.colorAccent
                        visible: delegateRoot.isSelected
                        radius: 2
                    }

                    // Progress Ring (right side)
                    ProgressRing {
                        id: progressRing
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.rightMargin: 12
                        completionCompleted: delegateRoot.cCompleted
                        completionTotal: delegateRoot.cTotal
                        completionState: delegateRoot.cState
                    }

                    //"X / Y tasks" tooltip on hover
                    ToolTip {
                        id: progressTooltip
                        visible: delegateRoot.isHovered && delegateRoot.cState !== "none"
                        delay: 400
                        text: delegateRoot.cState === "complete"
                              ? "All %1 tasks complete ✓".arg(delegateRoot.cTotal)
                              : "%1 / %2 tasks done".arg(delegateRoot.cCompleted).arg(delegateRoot.cTotal)
                        parent: delegateRoot
                        x: delegateRoot.width - width - 4
                        y: delegateRoot.height + 4
                    }

                    ColumnLayout {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        // Reserve 32px on the right for the progress ring
                        anchors.right: progressRing.left
                        anchors.margins: 12
                        anchors.leftMargin: 12 + (delegateRoot.isSelected ? 4 : 0)
                        anchors.rightMargin: 4
                        spacing: 2

                        Text {
                            text: model.title
                            font.pixelSize: 16
                            font.bold: true
                            color: ThemeManager.textMain
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            Text {
                                text: Qt.formatDateTime(
                                          new Date(model.createdAt * 1000),
                                          "MMM d, yyyy")
                                font.pixelSize: 13
                                color: ThemeManager.textMuted
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }

                            // "X / Y" compact counter — shown only when selected
                            Text {
                                visible: delegateRoot.isSelected && delegateRoot.cState !== "none"
                                text: "%1 / %2".arg(delegateRoot.cCompleted).arg(delegateRoot.cTotal)
                                font.pixelSize: 11
                                color: delegateRoot.cState === "complete"
                                       ? ThemeManager.colorAccent
                                       : ThemeManager.textMuted
                                Layout.alignment: Qt.AlignRight
                            }
                        }
                    }

                    MouseArea {
                        id: delegateMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            noteList.currentIndex = index
                            root.entrySelected(model.id, model.title)
                        }
                    }
                }
            }
        }

        // BOTTOM BAR
        Rectangle {
            Layout.fillWidth: true
            height: 64
            color: ThemeManager.bgCard

            // Top Border
            Rectangle {
                width: parent.width
                height: 1
                anchors.top: parent.top
                color: ThemeManager.lineBorder
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: root.globalMargin
                anchors.rightMargin: root.globalMargin
                spacing: 12

                // LEFT SIDE: Settings Button
                ToolButton {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                    background: Rectangle {
                        color: parent.down ? ThemeManager.bgButtonHover : (parent.hovered ? ThemeManager.surfaceElevated : "transparent")
                        radius: ThemeManager.radiusDefault
                    }
                    contentItem: Text {
                        text: "⚙"
                        font.pixelSize: 20
                        color: ThemeManager.textMain
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: root.settingsClicked()
                }

                //Responsive New Note Button
                Button {
                    id: newNoteBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    clip: true

                    background: Rectangle {
                        color: parent.down ? ThemeManager.bgPrimaryActionPressed : (parent.hovered ? ThemeManager.bgPrimaryActionHover : ThemeManager.bgPrimaryAction)

                        border.color: ThemeManager.borderPrimaryAction
                        border.width: ThemeManager.isDark ? 1 : 0

                        radius: ThemeManager.radiusPill
                        scale: parent.pressed ? 0.98 : (parent.hovered ? 1.02 : 1.0)

                        Behavior on scale {
                            NumberAnimation {
                                duration: 150
                            }
                        }

                        Behavior on color {
                            ColorAnimation {
                                duration: 150
                            }
                        }
                    }

                    contentItem: Item {
                        anchors.fill: parent

                        Row {
                            anchors.centerIn: parent
                            spacing: 8

                            Text {
                                text: "+"
                                font.pixelSize: 18
                                color: "#FFFFFF"
                                font.bold: true
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: "New Note"
                                font.pixelSize: 14
                                color: "#FFFFFF"
                                font.bold: true
                                anchors.verticalCenter: parent.verticalCenter
                                visible: newNoteBtn.width > 110
                            }
                        }
                    }

                    onClicked: root.createClicked()
                }
            }
        }
    }
}
