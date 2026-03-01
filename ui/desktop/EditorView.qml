import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Dialogs

Page {
    id: root

    function refreshCursor() {
        let start = editorArea.selectionStart
        let end = editorArea.selectionEnd
        editorArea.deselect()
        editorArea.select(start, end)
        editorArea.forceActiveFocus()
    }


    property int colorMode: 0 // 0 == text color, 1== highlight color

    // --- API ---
    property alias entryTitle: titleField.text
    property alias entryContent: editorArea.text
    property alias readOnly: editorArea.readOnly

    Action {
        id: saveAction
        text: "Save"
        shortcut: StandardKey.Save // ctrl+S
        enabled: editorArea.textDocument.modified
        onTriggered: console.log(
                         "Save Action Triggered! Currently in Development!!")
    }
    Action {
        id: reloadAction
        text: "Reload From Disk"
        shortcut: "Ctrl+Shift+R"
        onTriggered: console.log("Reload Triggered! Currently in Development!!")
    }
    Action {
        id: undoAction
        text: "Undo"
        shortcut: StandardKey.Undo // ctrl+Z
        enabled: editorArea.canUndo
        onTriggered: editorArea.undo()
    }
    Action {
        id: redoAction
        text: "Redo"
        shortcut: StandardKey.Redo // ctrl+Y or ctrl+shift+Z
        enabled: editorArea.canRedo
        onTriggered: editorArea.redo()
    }
    Action {
        id: cutAction
        text: "Cut"
        shortcut: StandardKey.Cut // ctrl+X
        enabled: editorArea.selectedText.length > 0
        onTriggered: editorArea.cut()
    }
    Action {
        id: copyAction
        text: "Copy"
        shortcut: StandardKey.Copy // ctrl+C
        enabled: editorArea.selectedText.length > 0
        onTriggered: editorArea.copy()
    }
    Action {
        id: pasteAction
        text: "Paste"
        shortcut: StandardKey.Paste // ctrl+V
        enabled: true
        onTriggered: editorArea.paste()
    }

    // Alignment TRIO's
    ActionGroup {
        id: alignnmentGroup
    }
    Action {
        id: alignLeftAction
        text: "Align Left"
        shortcut: "Ctrl+L"
        checkable: true
        ActionGroup.group: alignnmentGroup
        checked: editorArea.cursorSelection.alignment === Qt.AlignLeft
        onTriggered: {
            editorArea.cursorSelection.alignment = Qt.AlignLeft
            root.refreshCursor()
        }
    }
    Action {
        id: alignCentreAction
        text: "Align Centre"
        shortcut: "Ctrl+E"
        checkable: true
        ActionGroup.group: alignnmentGroup
        checked: editorArea.cursorSelection.alignment === Qt.AlignCenter
        onTriggered: {
            editorArea.cursorSelection.alignment = Qt.AlignCenter
            root.refreshCursor()
        }
    }
    Action {
        id: alignRightAction
        text: "Align Right"
        shortcut: "Ctrl+R"
        checkable: true
        ActionGroup.group: alignnmentGroup
        checked: editorArea.cursorSelection.alignment === Qt.AlignRight
        onTriggered: {
            editorArea.cursorSelection.alignment = Qt.AlignRight
            root.refreshCursor()
        }
    }

    Action {
        id: boldAction
        text: "Bold"
        shortcut: StandardKey.Bold // handles ctrl+B auto
        checkable: true
        checked: editorArea.cursorSelection.font.bold
        onTriggered: editorArea.cursorSelection.font.bold = checked
    }
    Action {
        id: italicAction
        text: "Italic"
        shortcut: StandardKey.Italic // handles ctrl+I auto
        checkable: true
        checked: editorArea.cursorSelection.font.italic
        onTriggered: editorArea.cursorSelection.font.italic = checked
    }
    Action {
        id: underlineAction
        text: "Underline"
        shortcut: StandardKey.Underline // handle ctrl+U auto
        checkable: true
        checked: editorArea.cursorSelection.font.underline
        onTriggered: editorArea.cursorSelection.font.underline = checked
    }

    ColorDialog {
        id: colorPickerDialog
        title: root.colorMode === 0 ? "Select Text Color" : "Select Highlighter Color"

        selectedColor: root.colorMode === 0 ? editorArea.cursorSelection.color : "#FFFF00" // sync start color with current text
        onAccepted: {
            if (colorMode == 0) {
                editorArea.cursorSelection.color = selectedColor
            } else {
                editorArea.cursorSelection.font.backgroundColor = selectedColor
            }
            editorArea.forceActiveFocus() // keep keyboard open
        }
    }

    // 2 Toolbar logic
    header: EditorToolbar {
        id: toolbar
        // Sync Toolbar State
        isBold: boldAction.checked
        isItalic: italicAction.checked
        isUnderline: underlineAction.checked

        // Trigger Actions
        onBoldClicked: boldAction.trigger()
        onItalicClicked: italicAction.trigger()
        onUnderlineClicked: underlineAction.trigger()

        onFontSizeSelected: size => {
                                // RichText handles point sizes natively and reliably
                                editorArea.cursorSelection.font.pointSize = size
                                editorArea.forceActiveFocus()
                            }

        onColorClicked: {
            root.colorMode = 0 // mode text color
            colorPickerDialog.open()
        }
        onHighlighterClicked: {
            root.colorMode = 1 // mode highlighter color
            colorPickerDialog.open()
        }

        onDoneClicked: Qt.inputMethod.hide()

        visible: true
    }

    // The editor content
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        ColumnLayout {

            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
            spacing: 4
            // --- HEADER ---
            TextField {
                id: titleField
                Layout.fillWidth: true
                font.pixelSize: 24
                font.bold: true
                background: null
                selectByMouse: true
                padding: 0
                readonly property real textWidth: contentWidth
                color: Material.theme === Material.Dark ? "#FFFFFF" : "#7B3F00"
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1 // 1px thin line
                color: "#D0D0D0" // Subtle grey
                opacity: 0.6 // Make it soft
                Layout.topMargin: 0
                Layout.bottomMargin: 15 // Push the title down a bit
            }
            // date label
            Text {
                id: dateLabel
                text: Qt.formatDateTime(new Date(), "dddd, MMMM d • h:mm AP")
                color: "#C4A484"
                font.pixelSize: 12
            }
        }

        // --- CONTENT EDITOR ---
        // Replacing ScrollView with Flickable stops the screen shattering
        // and fixes the detached selection handles.
        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            flickableDirection: Flickable.VerticalFlick
            clip: true

            ScrollBar.vertical: ScrollBar {}

            TextArea.flickable: TextArea {
                id: editorArea
                topPadding: 10

                // Using Markdown for clean C++ data.
                // (Note: Markdown will NOT save font sizes or colors)
                textFormat: TextEdit.MarkdownText

                font.pointSize: 12
                font.family: "Georgia"
                wrapMode: Text.Wrap

                background: null
                color: Material.theme === Material.Dark ? "#FFFFFF" : "#7B3F00"

                // Qt 6.8 Native Selection Handling
                persistentSelection: true

                onCursorPositionChanged: {
                    if (editorArea.inputMethodComposing)
                        return

                    let size = editorArea.cursorSelection.font.pointSize
                    if (size !== undefined && size > 0) {
                        toolbar.currentFontSize = Math.round(size).toString()
                    }

                    toolbar.isBold = editorArea.cursorSelection.font.bold
                    toolbar.isItalic = editorArea.cursorSelection.font.italic
                    toolbar.isUnderline = editorArea.cursorSelection.font.underline
                }
            }
        }
    }
}
