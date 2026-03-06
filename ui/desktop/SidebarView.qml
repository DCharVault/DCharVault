import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Item {
    id: root

    signal entrySelected(int index)
    signal createClicked
    signal createDiaryClicked

    // Global margin for consistent alignment
    readonly property int globalMargin: 16

    RowLayout {
        anchors.fill: parent
        spacing: 0

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // ------------------------------------------
            // 1. TOP HEADER (Notebook Name)
            // ------------------------------------------
            Rectangle {
                Layout.fillWidth: true
                height: 60 // Slightly taller to give the button room
                color: "#ffffff"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: root.globalMargin
                    anchors.rightMargin: root.globalMargin

                    Text {
                        text: "My Notebook"
                        font.pixelSize: 20 // Slightly larger to feel like a header
                        font.bold: true
                        color: "#222222"
                        Layout.fillWidth: true
                    }

                    // Dropdown button fixed to prevent overflow
                    ToolButton {
                        text: "▾"
                        font.pixelSize: 24
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        // Make the button background transparent so it doesn't create an ugly grey circle
                        background: Rectangle {
                            color: parent.down ? "#eeeeee" : (parent.hovered ? "#f5f5f5" : "transparent")
                            radius: 4
                        }
                    }
                }

                // Bottom border for the header
                Rectangle {
                    width: parent.width
                    height: 1
                    anchors.bottom: parent.bottom
                    color: "#e0e0e0"
                }
            }

            // ------------------------------------------
            // 2. NOTE LIST (Note name & metadata)
            // ------------------------------------------
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "#f9f9f9" // Very subtle background color to make the white cards pop

                ListView {
                    id: noteList
                    anchors.fill: parent
                    clip: true

                    // The secret to perfect list margins:
                    leftMargin: root.globalMargin
                    rightMargin: root.globalMargin
                    topMargin: 12
                    bottomMargin: 12
                    spacing: 8

                    model: ListModel {
                        ListElement { title: "Project Brainstorm"; meta: "Today • 2 KB" }
                        ListElement { title: "Weekly Meeting Notes For Team"; meta: "Yesterday • 5 KB" }
                        ListElement { title: "Grocery List"; meta: "Oct 24 • 1 KB" }
                        ListElement { title: "QML Ideas"; meta: "Oct 22 • 12 KB" }
                    }

                    delegate: Rectangle {
                        // Dynamically calculate width based on the ListView's margins
                        width: ListView.view.width - ListView.view.leftMargin - ListView.view.rightMargin
                        height: 70

                        // Rounded borders
                        radius: 8
                        border.color: noteList.currentIndex === index ? "#DC4D01" : "#e0e0e0"
                        border.width: noteList.currentIndex === index ? 2 : 1
                        color: noteList.currentIndex === index ? "#e3f2fd" : "#ffffff"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 4

                            // Note Name
                            Text {
                                text: model.title
                                font.pixelSize: 16
                                font.bold: true
                                color: "#222222"
                                Layout.fillWidth: true
                                elide: Text.ElideRight // <-- STOPS TEXT OVERFLOW
                            }

                            // Metadata (Date, size, preview, etc.)
                            Text {
                                text: model.meta
                                font.pixelSize: 13
                                color: "#777777"
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: noteList.currentIndex = index
                        }
                    }
                }
            }

            // ------------------------------------------
            // 3. BOTTOM BAR (Search, Filter, Add Note)
            // ------------------------------------------
            Rectangle {
                Layout.fillWidth: true
                height: 64 // Slightly taller for better touch targets
                color: "#ffffff"

                // Top border for the bottom bar
                Rectangle {
                    width: parent.width
                    height: 1
                    anchors.top: parent.top
                    color: "#e0e0e0"
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: root.globalMargin
                    anchors.rightMargin: root.globalMargin
                    spacing: 12

                    // Search Input
                    TextField {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40
                        placeholderText: "🔍 Search..."
                        background: Rectangle {
                            color: "#f0f0f0"
                            radius: 8 // Match the card radius
                        }
                        verticalAlignment: TextInput.AlignVCenter
                        leftPadding: 12
                    }

                    // Add New Note Button
                    Button {
                        text: "+"
                        font.pixelSize: 24
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40

                        // Custom background to match the aesthetic
                        background: Rectangle {
                            color: parent.down ? "#d5d5d5" : (parent.hovered ? "#e0e0e0" : "#eeeeee")
                            radius: 8
                        }

                        ToolTip.visible: hovered
                        ToolTip.text: "Add new note"
                    }
                }
            }
        }
    }
}
