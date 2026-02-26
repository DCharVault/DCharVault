import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Item {
    id: root

    // --- SIGNALS ---
    // We tell the parent what happened. We don't change views ourselves.
    signal entrySelected(int index)
    signal createClicked()
    signal createDiaryClicked()
    signal menuClicked() // Only used on mobile to close drawer

    // Background
    Rectangle {
        anchors.fill: parent
        color: "#F7F7F7" // Slightly darker than the white editor
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

                // The "Logo" or Menu Icon
                ToolButton {
                    text: "☰"
                    font.pixelSize: 18
                    onClicked: root.menuClicked()
                }

                // The Dropdown (Personal / Work)
                ComboBox {
                    Layout.fillWidth: true
                    model: ["Personal Diary", "Work Notes", "Code Ideas"]

                    // Style it to look clean (No box)
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

                // The Big "Add" Button
                Button {
                    text: "+"
                    Layout.preferredWidth: 45
                    Layout.preferredHeight: 45

                    // Make it a Circle
                    background: Rectangle {
                        color: "#212121" // Black button
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

        // Divider
        Rectangle { Layout.fillWidth: true; height: 1; color: "#E0E0E0" }

        // --- 2. MIDDLE: The Entry List ---
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            topMargin: 5
            spacing: 2

            // Dummy Model (We will link C++ here later)
            model: 10

            delegate: ItemDelegate {
                width: ListView.view.width
                height: 75

                // Highlight color when selected
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

                // The Content of one row
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 4

                    // Row 1: Title + Date
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

                    // Row 2: Preview Text
                    Text {
                        text: "This is a preview of what I wrote properly..."
                        color: "#616161"
                        font.pixelSize: 13
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }
            }

            // Scrollbar
            ScrollBar.vertical: ScrollBar { }
        }

        // Divider
        Rectangle { Layout.fillWidth: true; height: 1; color: "#E0E0E0" }

        // --- 3. FOOTER: Search & Add ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            color: "#FFFFFF" // White bottom bar

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                // Search Box
                TextField {
                    Layout.fillWidth: true
                    placeholderText: "Search..."
                    font.pixelSize: 14

                    background: Rectangle {
                        color: "#F5F5F5"
                        radius: 8
                    }
                }

                // The Big "Add" Button
                Button {
                    text: "+"
                    Layout.preferredWidth: 45
                    Layout.preferredHeight: 45

                    // Make it a Circle
                    background: Rectangle {
                        color: "#212121" // Black button
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

                // light/dark mode toggle
                ToolButton{
                    text: window.isDark ? "☀" :"️🌙"
                    onClicked: window.isDark = !window.isDark
                }
            }
        }
    }
}
