import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

ToolBar {
    id: root

    property alias currentFontSize: sizeField.text
    property bool isBold: false
    property bool isItalic: false
    property bool isUnderline: false

    signal boldClicked()
    signal italicClicked()
    signal underlineClicked()
    signal colorClicked()
    signal highlighterClicked()
    signal doneClicked()
    signal fontSelected(string fontName)
    signal fontSizeSelected(int sizeFont)

    Material.background: Material.theme === Material.Dark ? "#FFFFFF" : "#7B3F00"
    Material.elevation: 0

    // A tiny border line for desktop mode
    Rectangle {
        width: parent.width
        height: 5
        anchors.bottom: parent.bottom
        color: Material.theme === Material.Dark ? "#FFFFFF" : "#7B3F00"
    }

    ScrollView {
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        contentWidth: toolbarLayout.implicitWidth
        contentHeight: parent.height
        clip: true

        RowLayout {
            id: toolbarLayout
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            spacing: 5
            anchors.margins: 5

            ToolButton {
                id: insert
                text: "+"
                font.bold: true
                font.pixelSize: 20
                Layout.preferredHeight: 50
            }

            // --- COMPACT Font Size Stepper ---
            Rectangle {
                id: stepperPill
                Layout.preferredWidth: stepperRow.implicitWidth + 8
                Layout.preferredHeight: 32
                Layout.alignment: Qt.AlignVCenter
                radius: 4
                color: "#F5F5F5"
                border.color: "#E0E0E0"

                RowLayout {
                    id: stepperRow
                    anchors.centerIn: parent
                    spacing: 0

                    ToolButton {
                        text: "−"
                        Layout.preferredWidth: 32
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
                            let newSize = parseInt(sizeField.text) - 2
                            if (newSize >= 6) {
                                root.fontSizeSelected(newSize)
                                sizeField.text = newSize.toString()
                            }
                        }
                    }

                    Rectangle { width: 1; height: 16; color: "#DDD" }

                    TextField {
                        id: sizeField
                        text: "12"
                        Layout.preferredWidth: 36
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        padding: 0
                        leftPadding: 0
                        rightPadding: 0
                        background: null
                        selectByMouse: true
                        font.pixelSize: 14
                        color: "#333333"
                        validator: IntValidator { bottom: 6; top: 88 }
                        onAccepted: {
                            root.fontSizeSelected(parseInt(text))
                            focus = false
                        }
                    }

                    Rectangle { width: 1; height: 16; color: "#DDD" }

                    ToolButton {
                        text: "+"
                        Layout.preferredWidth: 32
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
                checkable: true
                checked: root.isBold
                onClicked: root.boldClicked()
            }
            ToolButton {
                text: "<i>I</i>"
                checkable: true
                checked: root.isItalic
                onClicked: root.italicClicked()
            }
            ToolButton {
                text: "<u>U</u>"
                checkable: true
                checked: root.isUnderline
                onClicked: root.underlineClicked()
            }

            ToolButton {
                text: "A"
                font.bold: true
                palette.buttonText: "red"
                onClicked: root.colorClicked()
            }

            ToolButton {
                text: "🖊️"
                palette.buttonText: "#FFA500"
                font.pixelSize: 16
                onClicked: root.highlighterClicked()
            }

            Item { Layout.fillWidth: true }
        }
    }
}
