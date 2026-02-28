import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Item {
    id: root

    signal entrySelected(int index)
    signal createClicked()
    signal createDiaryClicked()

    Rectangle {
        anchors.fill: parent
        color: "#F7F7F7"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // --- 1. HEADER: Journal Switcher ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 15

                ComboBox {
                    Layout.fillWidth: true
                    model: ["Personal Diary", "Work Notes", "Code Ideas"]
                    background: null
                    contentItem: Text {
                        text: parent.displayText
                        font.bold: true
                        font.pixelSize: 18
                        color: "#333333"
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }

                Button {
                    text: "+"
                    Layout.preferredWidth: 45
                    Layout.preferredHeight: 45
                    background: Rectangle {
                        color: "#212121"
                        radius: width / 2
                    }
                    contentItem: Text {
                        text: "+"
                        color: "white"
                        font.pixelSize: 24
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: root.createDiaryClicked()
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#E0E0E0" }

        // --- 2. MIDDLE: The Entry List ---
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            topMargin: 5
            spacing: 2
            model: 10

            delegate: ItemDelegate {
                width: ListView.view.width
                height: 75
                highlighted: ListView.isCurrentItem
                onClicked: {
                    listView.currentIndex = index
                    root.entrySelected(index)
                }

                background: Rectangle {
                    color: parent.highlighted ? "#E3F2FD" : (parent.hovered ? "#F0F0F0" : "transparent")
                    radius: 8
                    anchors.margins: 4
                    anchors.fill: parent
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: "Entry Title " + (index + 1)
                            font.bold: true
                            font.pixelSize: 15
                            color: "#212121"
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                        Text {
                            text: "Feb " + (10 + index)
                            color: "#757575"
                            font.pixelSize: 11
                        }
                    }

                    Text {
                        text: "This is a preview of what I wrote properly..."
                        color: "#616161"
                        font.pixelSize: 13
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }
            }
            ScrollBar.vertical: ScrollBar { }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#E0E0E0" }

        // --- 3. FOOTER: Search & Add ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            color: "#FFFFFF"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                TextField {
                    Layout.fillWidth: true
                    placeholderText: "Search..."
                    font.pixelSize: 14
                    background: Rectangle {
                        color: "#F5F5F5"
                        radius: 8
                    }
                }

                Button {
                    text: "+"
                    Layout.preferredWidth: 45
                    Layout.preferredHeight: 45
                    background: Rectangle {
                        color: "#212121"
                        radius: width / 2
                    }
                    contentItem: Text {
                        text: "+"
                        color: "white"
                        font.pixelSize: 24
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: root.createClicked()
                }

                ToolButton {
                    text: window.isDark ? "☀" :"️🌙"
                    onClicked: window.isDark = !window.isDark
                }
            }
        }
    }
}
