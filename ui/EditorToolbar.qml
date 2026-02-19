import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

ToolBar {
    id: root

    // 1-- inputs and outputs
    // the parent tell us: "Are we on Mobile?"
    property bool isMobile: true

    property alias currentFontSize: sizeField.text

    property bool isBold: false
    property bool isItalic: false
    property bool isUnderline: false

    // hardcoded height:
    // height: 63

    // We tell parent: "User clicked Bold!"
    signal boldClicked
    signal italicClicked
    signal underlineClicked
    signal colorClicked
    signal highlighterClicked
    signal doneClicked
    signal fontSelected(string fontName)
    signal menuClicked
    signal fontSizeSelected(int sizeFont)

    // 2 Visuals
    // Material.background: Material.theme === Material.Dark ? "#FFFFFF" : "#7B3F00"
    Material.elevation: isMobile ? 8 : 0 // Shadow only on mobile

    // a tiny border line for desktop mode
    Rectangle {
        visible: !root.isMobile
        width: parent.width
        height: 5
        anchors.bottom: parent.bottom
        // color: Material.theme === Material.Dark ? "#FFFFFF" : "#7B3F00"
    }

    ScrollView {
        anchors.fill: parent
        // Hide the scrollbar (optional, looks cleaner)
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AlwaysOff

        // Allow horizontal scrolling
        contentWidth: toolbarLayout.implicitWidth
        contentHeight: parent.height
        clip: true // Cut off content that goes outside
        // 3 Buttons
        RowLayout {
            id: toolbarLayout
            // 3. CRITICAL FIX: Do NOT use anchors.fill: parent
            // Only anchor top/bottom/left. Let the Right side grow freely.
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left

            spacing: 5
            anchors.margins: 5

            // 1. MOBILE MENU BUTTON (Hamburger)
            ToolButton {
                visible: root.isMobile
                text: "☰" // Hamburger Icon
                font.pixelSize: 20
                onClicked: root.menuClicked() // Emit signal
            }

            ToolButton {
                id: insert
                text: "+"
                font.bold: true
                font.pixelSize: 20
                Layout.preferredHeight: 50
            }

            // --- Font Size Stepper ---
            // --- COMPACT Font Size Stepper (Fixed) ---
            Rectangle {
                id: stepperPill

                // Auto-width based on content + a little breathing room
                Layout.preferredWidth: stepperRow.implicitWidth + 8
                Layout.preferredHeight: 32 // Standard touch-friendly height
                Layout.alignment: Qt.AlignVCenter

                radius: 4
                color: "#F5F5F5"
                border.color: "#E0E0E0"

                RowLayout {
                    id: stepperRow
                    anchors.centerIn: parent
                    spacing: 0

                    // Decrease Button
                    ToolButton {
                        text: "−"
                        Layout.preferredWidth: 32 // Square button
                        Layout.fillHeight: true

                        // Center the text perfectly
                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: "#333"
                        }
                        background: null

                        onClicked: {
                            let newSize = parseInt(sizeField.text) - 2
                            if (newSize >= 6) {
                                root.fontSizeSelected(newSize)
                                sizeField.text = newSize.toString()
                            }
                        }
                    }

                    // Vertical Divider (Optional, keeps it clean)
                    Rectangle {
                        width: 1
                        height: 16
                        color: "#DDD"
                    }

                    // Input value box
                    TextField {
                        id: sizeField
                        text: "12"

                        // Enough width for "88" without cutting off
                        Layout.preferredWidth: 36
                        Layout.fillHeight: true

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        // CRITICAL FIXES:
                        padding: 0
                        leftPadding: 0
                        rightPadding: 0
                        background: null
                        selectByMouse: true

                        font.pixelSize: 14
                        color: "#333333"

                        validator: IntValidator {
                            bottom: 6
                            top: 88
                        }
                        onAccepted: {
                            root.fontSizeSelected(parseInt(text))
                            focus = false
                        }
                    }

                    // Vertical Divider
                    Rectangle {
                        width: 1
                        height: 16
                        color: "#DDD"
                    }

                    // Increase Button
                    ToolButton {
                        text: "+"
                        Layout.preferredWidth: 32 // Square button
                        Layout.fillHeight: true

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: "#333"
                        }
                        background: null

                        onClicked: {
                            let newSize = parseInt(sizeField.text) + 2
                            if (newSize <= 72) {
                                root.fontSizeSelected(newSize)
                                sizeField.text = newSize.toString()
                            }
                        }
                    }
                }
            }

            ToolButton {
                text: "<b>B</b>"
                checkable: root.isBold
                checked: root.isBold
                onClicked: root.boldClicked()
            }
            ToolButton {
                text: "<i>I</i>"
                checkable: root.isItalic
                checked: root.isItalic
                onClicked: root.italicClicked()
            }
            ToolButton {
                text: "<u>U</u>"
                checkable: root.isUnderline
                checked: root.isUnderline
                onClicked: root.underlineClicked()
            }

            // Group 3: Colors (The new request)
            ToolButton {
                text: "A"
                font.bold: true
                palette.buttonText: "red" // Visual clue
                onClicked: root.colorClicked()
                // To make this real, we'd open a ColorDialog here
            }

            ToolButton {
                text: "🖊️" // Highlighter
                palette.buttonText: "#FFA500"
                font.pixelSize: 16
                onClicked: root.highlighterClicked()
            }

            // spacer pushes next items to right
            // "Done" Button (Only for Mobile)
            Item {
                Layout.fillWidth: true
            }
        }
    }
}
