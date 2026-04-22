import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import DCharVault

ToolBar {
    id: root
    palette.buttonText: ThemeManager.textMain
    background: Rectangle {
        implicitHeight: 56
        color: ThemeManager.bgCard
    }
    property alias currentFontSize: sizeField.text
    property bool isBold: false
    property bool isItalic: false
    property bool isUnderline: false

    signal boldClicked
    signal italicClicked
    signal underlineClicked
    signal colorClicked
    signal highlighterClicked
    signal deleteEntryClicked
    signal doneClicked
    signal fontSelected(string fontName)
    signal fontSizeSelected(int sizeFont)

    // A tiny border line for desktop mode
    Rectangle {
        width: parent.width
        height: 1
        color: ThemeManager.lineBorder
        anchors.bottom: parent.bottom
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
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            // --- COMPACT Font Size Stepper ---
            Rectangle {
                id: stepperPill
                Layout.preferredWidth: stepperRow.implicitWidth + 8
                Layout.preferredHeight: 32
                Layout.alignment: Qt.AlignVCenter
                radius: ThemeManager.radiusDefault
                color: ThemeManager.bgInput
                border.color: ThemeManager.lineBorder

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
                            color: ThemeManager.textMain
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

                    Rectangle {
                        width: 1
                        height: 16
                        color: ThemeManager.lineBorder
                    }

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
                        color: ThemeManager.textMain
                        validator: IntValidator {
                            bottom: 6
                            top: 88
                        }
                        onAccepted: {
                            root.fontSizeSelected(parseInt(text))
                            focus = false
                        }
                    }

                    Rectangle {
                        width: 1
                        height: 16
                        color: ThemeManager.lineBorder
                    }

                    ToolButton {
                        text: "+"
                        Layout.preferredWidth: 32
                        Layout.fillHeight: true
                        contentItem: Text {
                            text: parent.text
                            color: ThemeManager.textMain
                            font: parent.font
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
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
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.boldClicked()
            }
            ToolButton {
                text: "<i>I</i>"
                checkable: true
                checked: root.isItalic
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.italicClicked()
            }
            ToolButton {
                text: "<u>U</u>"
                checkable: true
                checked: root.isUnderline
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.underlineClicked()
            }

            ToolButton {
                text: "A"
                font.bold: true
                palette.buttonText: "red"
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.colorClicked()
            }

            ToolButton {
                text: "🖊️"
                palette.buttonText: "#FFA500"
                font.pixelSize: 16
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.highlighterClicked()
            }

            ToolButton {
                text: "X"
                palette.button: "#FFA500"
                font.pixelSize: 16
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.deleteEntryClicked()
            }
        }
    }
}
