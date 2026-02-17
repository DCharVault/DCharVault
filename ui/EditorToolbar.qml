import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

ToolBar {
    id: root

    // 1-- inputs and outputs
    // the parent tell us: "Are we on Mobile?"
    property bool isMobile: false

    property alias currentFontSize: sizeField.text

    // hardcoded height:
    height: 63

    // We tell parent: "User clicked Bold!"
    signal boldClicked()
    signal italicClicked()
    signal underlineClicked()
    signal colorClicked()
    signal highlighterClicked()
    signal doneClicked()
    signal fontSelected(string fontName)
    signal menuClicked()
    signal fontSizeSelected(int sizeFont)

    // 2 Visuals
    Material.background: Material.theme === Material.Dark ? "#FFFFFF" : "#7B3F00"
    Material.elevation: isMobile ? 8 : 0 // Shadow only on mobile

    // a tiny border line for desktop mode
    Rectangle {
        visible: !root.isMobile
        width: parent.width
        height: 5
        anchors.bottom: parent.bottom
        color: Material.theme === Material.Dark ? "#FFFFFF" : "#7B3F00"
    }

    // 3 Buttons
    RowLayout {
        anchors.fill: parent
        spacing: 5

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

        //Font Family(hidden on mobile
        ComboBox {
            visible: !root.isMobile
            model: ["Segoe UI", "Georgia", "Roboto"]
            Layout.preferredWidth: 150
            Layout.preferredHeight: 50
            Layout.leftMargin: 5 // Breathing room
        }

        // --- Font Size Stepper ---
        Rectangle{
            id: stepperPill

            // unified dimensions and style
            Layout.preferredWidth: 100 // compact width
            Layout.preferredHeight: 36
            Layout.alignment: Qt.AlignVCenter

            radius: 4
            color: "#F5F5F5"
            border.color: "#E0E0E0"
            border.width: 1

            // layout internal
            RowLayout{
                anchors.fill: parent
                spacing: 0
                // Decrease Button
                ToolButton{
                    text: "-"
                    Layout.preferredWidth: 24
                    Layout.fillHeight: true
                    background: null
                    onClicked: {
                        var newSize = parseInt(sizeField.text)-2
                        if(newSize>=6){
                            root.fontSizeSelected(newSize)
                            sizeField.text = newSize
                        }
                    }
                }
                //Divider
                Rectangle{width: 1; height: 20; color: "#D0D0D0"; Layout.alignment: Qt.AlignVCenter}
                // input value box
                TextField{
                    id: sizeField
                    text: "16" // default
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    font.pixelSize: 14
                    color: "#333333"
                    background: null

                    // only allow for numbers 6 to 88
                    validator: IntValidator{bottom: 6; top: 88}
                    // apply when press enter
                    onAccepted: {
                        root.fontSizeSelected(parseInt(text))
                        focus = false // hide keyboard
                    }
                }
                //Divider
                Rectangle{width: 1; height: 20; color: "#D0D0D0"; Layout.alignment: Qt.AlignVCenter}
                // Increase Button
                ToolButton{
                    text: "+"
                    Layout.preferredWidth: 24
                    Layout.fillHeight: true
                    background: null
                    onClicked: {
                        var newSize = parseInt(sizeField.text)+2
                        if(newSize<=72){
                            root.fontSizeSelected(newSize)
                            sizeField.text = newSize
                        }
                    }
                }
            }
        }

        ToolButton {
            text: "<b>B</b>"
            focusPolicy: Qt.NoFocus // stop button from stealing focus from textEditorArea
            onClicked: root.boldClicked()
        }
        ToolButton {
            text: "<i>I</i>"
            focusPolicy: Qt.NoFocus
            onClicked: root.italicClicked()
        }
        ToolButton {
            text: "<u>U</u>"
            focusPolicy: Qt.NoFocus
            onClicked: root.underlineClicked()
        }
        ToolSeparator {}

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
