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
    property bool isStrikethrough: false
    property bool isCheckbox: false

    signal boldClicked
    signal italicClicked
    signal underlineClicked
    signal colorClicked
    signal highlighterClicked
    signal deleteEntryClicked
    signal doneClicked
    signal fontSelected(string fontName)
    signal fontSizeSelected(int sizeFont)

    signal bulletListClicked
    signal numberedListClicked
    signal blockquoteClicked

    signal strikethroughClicked
    signal clearFormattingClicked
    signal exportClicked
    signal checkboxClicked
    signal priorityLabelInserted(string name, string color)

    property string currentBlockType: "normal"
    onCurrentBlockTypeChanged: {
        for (var i = 0; i < blockTypeCombo.model.length; ++i) {
            if (blockTypeCombo.model[i].value === currentBlockType) {
                blockTypeCombo.currentIndex = i
                break
            }
        }
    }
    signal blockTypeSelected(string blockType)

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
                radius: ThemeManager.radiusPill
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
                            if (newSize <= 88) {
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

            DCharComboBox {
                id: blockTypeCombo
                Layout.preferredWidth: 140
                Layout.alignment: Qt.AlignVCenter
                model: ListModel {
                    ListElement {
                        text: "Normal"
                        value: "normal"
                    }
                    ListElement {
                        text: "Heading 1"
                        value: "heading1"
                    }
                    ListElement {
                        text: "Heading 2"
                        value: "heading2"
                    }
                    ListElement {
                        text: "Heading 3"
                        value: "heading3"
                    }
                }
                onActivated: {
                    root.blockTypeSelected(currentValue)
                }
            }


            ToolButton {
                text: "<s>S</s>"
                checkable: true
                checked: root.isStrikethrough
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    root.strikethroughClicked()
                    checked = Qt.binding(function () {
                        return root.isStrikethrough
                    })
                }
            }

            ToolButton {
                text: "•"
                font.pixelSize: 20
                checkable: true
                checked: root.currentBlockType === "bulletList"
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    font: parent.font
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    root.bulletListClicked()
                    checked = Qt.binding(function () {
                        return root.currentBlockType === "bulletList"
                    })
                }
            }
            ToolButton {
                text: "1."
                checkable: true
                checked: root.currentBlockType === "numberedList"
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    root.numberedListClicked()
                    checked = Qt.binding(function () {
                        return root.currentBlockType === "numberedList"
                    })
                }
            }
            ToolButton {
                text: "❞"
                checkable: true
                checked: root.currentBlockType === "blockquote"
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    root.blockquoteClicked()
                    checked = Qt.binding(function () {
                        return root.currentBlockType === "blockquote"
                    })
                }
            }

            CheckboxToggleButton {
                isActive: root.isCheckbox
                onToggled: root.checkboxClicked()
            }

            ToolButton {
                id: priorityBtn
                text: "🏷"
                font.pixelSize: 15
                ToolTip.visible: hovered
                ToolTip.text: "Insert Priority Label"
                ToolTip.delay: 600
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: priorityPopup.open()

                Popup {
                    id: priorityPopup
                    y: parent.height + 4
                    x: -8
                    width: 260
                    padding: 12
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                    background: Rectangle {
                        color: ThemeManager.bgCard
                        border.color: ThemeManager.lineBorder
                        border.width: 1
                        radius: ThemeManager.radiusDefault
                    }

                    // Reload labels each time the popup opens
                    property var labels: []
                    onOpened: labels = priorityViewModel.getPriorityLabels()

                    Connections {
                        target: priorityViewModel
                        function onLabelsChanged() {
                            priorityPopup.labels = priorityViewModel.getPriorityLabels()
                        }
                    }

                    Column {
                        width: parent.width
                        spacing: 10

                        // Header
                        Text {
                            text: "Priority Labels"
                            font.pixelSize: 13
                            font.bold: true
                            color: ThemeManager.textMain
                            width: parent.width
                        }

                        // Existing labels list
                        Column {
                            width: parent.width
                            spacing: 6
                            Repeater {
                                model: priorityPopup.labels
                                delegate: Row {
                                    width: parent.width
                                    spacing: 6

                                    // Colored chip — click to insert
                                    Rectangle {
                                        id: chip
                                        height: 26
                                        width: parent.width - 32
                                        radius: 4
                                        color: modelData.color

                                        Text {
                                            anchors.centerIn: parent
                                            text: modelData.name.toUpperCase()
                                            font.pixelSize: 11
                                            font.bold: true
                                            color: {
                                                let c = Qt.color(modelData.color)
                                                let lum = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b
                                                return lum < 0.5 ? "#FFFFFF" : "#1A0F18"
                                            }
                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: {
                                                root.priorityLabelInserted(modelData.name, modelData.color)
                                                priorityPopup.close()
                                            }
                                        }

                                        Behavior on opacity { NumberAnimation { duration: 120 } }
                                    }

                                    Rectangle {
                                        width: 26
                                        height: 26
                                        radius: 4
                                        color: delMA.containsMouse
                                            ? Qt.rgba(1, 0.2, 0.2, 0.2)
                                            : "transparent"
                                        Behavior on color { ColorAnimation { duration: 100 } }

                                        Text {
                                            anchors.centerIn: parent
                                            text: "×"
                                            font.pixelSize: 14
                                            color: ThemeManager.textMuted
                                        }

                                        MouseArea {
                                            id: delMA
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: {
                                                priorityViewModel.deletePriorityLabel(modelData.name)
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        Rectangle {
                            width: parent.width
                            height: 1
                            color: ThemeManager.lineBorder
                            visible: priorityPopup.labels.length > 0
                        }

                        Column {
                            id: addLabelSection
                            width: parent.width
                            spacing: 8

                            Text {
                                text: "+ New Label"
                                font.pixelSize: 12
                                font.bold: true
                                color: ThemeManager.colorAccent
                                width: parent.width
                            }

                            Row {
                                width: parent.width
                                spacing: 6

                                TextField {
                                    id: newLabelName
                                    width: parent.width - newLabelColor.width - saveBtn.width - 12
                                    height: 28
                                    placeholderText: "Label name..."
                                    color: ThemeManager.textMain
                                    font.pixelSize: 12
                                    leftPadding: 8
                                    background: Rectangle {
                                        color: ThemeManager.bgInput
                                        radius: 4
                                        border.color: parent.activeFocus ? ThemeManager.colorAccent : ThemeManager.lineBorder
                                        border.width: 1
                                    }
                                    onAccepted: {
                                        if (text.trim() !== "")
                                            saveBtn.doSave()
                                    }
                                }

                                Rectangle {
                                    id: newLabelColor
                                    width: 28
                                    height: 28
                                    radius: 4
                                    color: colorCycler.currentColor
                                    border.color: ThemeManager.lineBorder
                                    border.width: 1

                                    property var palette: [
                                        "#E05252", "#E07D52", "#D4B84A",
                                        "#4CAF6E", "#4A90D9", "#7B52E0",
                                        "#D45296", "#52BDD4"
                                    ]
                                    property int colorIndex: 0
                                    property string currentColor: palette[colorIndex]

                                    MouseArea {
                                        id: colorCycler
                                        anchors.fill: parent
                                        property string currentColor: newLabelColor.currentColor
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            newLabelColor.colorIndex = (newLabelColor.colorIndex + 1) % newLabelColor.palette.length
                                        }
                                    }

                                    Behavior on color { ColorAnimation { duration: 150 } }
                                }

                                Rectangle {
                                    id: saveBtn
                                    width: 28
                                    height: 28
                                    radius: 4
                                    color: saveBtnMA.containsMouse
                                        ? ThemeManager.bgPrimaryActionHover
                                        : ThemeManager.bgPrimaryAction
                                    Behavior on color { ColorAnimation { duration: 120 } }

                                    Text {
                                        anchors.centerIn: parent
                                        text: "✓"
                                        font.pixelSize: 13
                                        font.bold: true
                                        color: "#FFFFFF"
                                    }

                                    function doSave() {
                                        const labelName = newLabelName.text.trim()
                                        if (labelName === "") return
                                        priorityViewModel.savePriorityLabel(labelName, newLabelColor.currentColor)
                                        newLabelName.text = ""
                                        newLabelColor.colorIndex = 0
                                    }

                                    MouseArea {
                                        id: saveBtnMA
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: saveBtn.doSave()
                                    }
                                }
                            }
                        }
                    }
                }
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
                text: "Tx"
                font.pixelSize: 16
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.clearFormattingClicked()
            }

            ToolButton {
                text: "⤴"
                font.pixelSize: 16
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.exportClicked()
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
