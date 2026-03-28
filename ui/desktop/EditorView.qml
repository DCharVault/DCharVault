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

    property int colorMode: 0 // 0 == text color, 1 == highlight color
    property int currentEntryId: -1
    property string originalTitle: ""

    // --- API ---
    property alias entryTitle: titleField.text
    property alias entryContent: editorArea.text
    property alias readOnly: editorArea.readOnly

    Connections{
        target: diaryViewModel
        function onEntrySavedSuccessfully(savedId, finalizeTitle){
            console.log("QML: Success! Entry locked and saved in SQLite.")
            diaryListModel.loadEntries()
            root.currentEntryId = savedId
            editorArea.textDocument.modified = false
            titleField.text = finalizeTitle
            originalTitle = finalizeTitle
        }
        function onEntrySaveFailed(errorMessage){
            console.error("QML Error: " + errorMessage)
        }
        function onEntryDeletedSuccessfully(){
            console.log("QML: Entry deleted. Clearing editor.")
            diaryListModel.loadEntries()
            root.currentEntryId = -1
            titleField.text = ""
            editorArea.text = ""
            editorArea.textDocument.modified = false
        }
    }

    Action {
        id: saveAction
        text: "Save"
        shortcut: StandardKey.Save // ctrl+S
        enabled: editorArea.textDocument.modified || titleField.text!==originalTitle
        onTriggered: {
            console.log("QML: Sending entry to viewModel: diaryViewModel.cpp");
            console.log("CRITICAL DEBUG -> Hitting Save! currentEntryId is:", root.currentEntryId);
            diaryViewModel.saveNewEntry(root.currentEntryId,titleField.text,editorArea.text);
            originalTitle = titleField.text
            editorArea.textDocument.modified = false
        }
    }
    Action{
        id: deleteAction
        text: "delete"
        shortcut: StandardKey.Delete
        enabled: root.currentEntryId!=-1 && editorArea.text!=="[[encryption failed]]"// only when we have a entry not blanked and text is [[encryption failed]]

        onTriggered: {
            console.log("QML: Requesting deletion of ID:", root.currentEntryId)
            deleteConfirmDialog.open()
        }
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

    ActionGroup { id: alignnmentGroup }
    Action { id: alignLeftAction; text: "Align Left"; shortcut: "Ctrl+L"; checkable: true; ActionGroup.group: alignnmentGroup; checked: editorArea.cursorSelection.alignment === Qt.AlignLeft; onTriggered: { editorArea.cursorSelection.alignment = Qt.AlignLeft; root.refreshCursor() } }
    Action { id: alignCentreAction; text: "Align Centre"; shortcut: "Ctrl+E"; checkable: true; ActionGroup.group: alignnmentGroup; checked: editorArea.cursorSelection.alignment === Qt.AlignCenter; onTriggered: { editorArea.cursorSelection.alignment = Qt.AlignCenter; root.refreshCursor() } }
    Action { id: alignRightAction; text: "Align Right"; shortcut: "Ctrl+R"; checkable: true; ActionGroup.group: alignnmentGroup; checked: editorArea.cursorSelection.alignment === Qt.AlignRight; onTriggered: { editorArea.cursorSelection.alignment = Qt.AlignRight; root.refreshCursor() } }

    Action { id: boldAction; text: "Bold"; shortcut: StandardKey.Bold; checkable: true; checked: editorArea.cursorSelection.font.bold; onTriggered: editorArea.cursorSelection.font.bold = checked }
    Action { id: italicAction; text: "Italic"; shortcut: StandardKey.Italic; checkable: true; checked: editorArea.cursorSelection.font.italic; onTriggered: editorArea.cursorSelection.font.italic = checked }
    Action { id: underlineAction; text: "Underline"; shortcut: StandardKey.Underline; checkable: true; checked: editorArea.cursorSelection.font.underline; onTriggered: editorArea.cursorSelection.font.underline = checked }

    ColorDialog {
        id: colorPickerDialog
        title: root.colorMode === 0 ? "Select Text Color" : "Select Highlighter Color"
        selectedColor: root.colorMode === 0 ? editorArea.cursorSelection.color : "#FFFF00"
        onAccepted: {
            if (colorMode == 0) {
                editorArea.cursorSelection.color = selectedColor
            } else {
                editorArea.cursorSelection.font.backgroundColor = selectedColor
            }
            editorArea.forceActiveFocus()
        }
    }
    MessageDialog{
        id: deleteConfirmDialog
        title: "Delete Entry"
        text: "Are you sure you want to permanently delete this page?"
        informativeText: "This action cannot be undone. The encrypted data will be wiped from the vault."
        buttons: MessageDialog.Yes | MessageDialog.No
        onButtonClicked: function(button,role){
            if(button === MessageDialog.Yes){
                console.log("QML: User confirmed. Requesting deletion of ID:", root.currentEntryId)
                diaryViewModel.deleteEntry(root.currentEntryId);
            }
        }
    }

    // Toolbar Header
    header: EditorToolbar {
        id: toolbar
        isBold: boldAction.checked
        isItalic: italicAction.checked
        isUnderline: underlineAction.checked

        onBoldClicked: boldAction.trigger()
        onItalicClicked: italicAction.trigger()
        onUnderlineClicked: underlineAction.trigger()

        onFontSizeSelected: size => {
            editorArea.cursorSelection.font.pointSize = size
            editorArea.forceActiveFocus()
        }

        onColorClicked: {
            root.colorMode = 0
            colorPickerDialog.open()
        }
        onHighlighterClicked: {
            root.colorMode = 1
            colorPickerDialog.open()
        }
        onDeleteEntryClicked: deleteAction.trigger()
        onDoneClicked: Qt.inputMethod.hide()
        visible: true
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16 // MATCHED TO SIDEBAR MARGIN
        spacing: 12 // Slightly more breathing room

        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
            spacing: 8 // Unified spacing between title, line, and date

            // --- HEADER ---
            TextField {
                id: titleField
                Layout.fillWidth: true
                font.pixelSize: 22
                font.bold: true
                color: "#222222" // Matched to sidebar title color
                background: null
                selectByMouse: true
                padding: 0

            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#e0e0e0" // Matched to sidebar border color exactly
            }

            // date label
            Text {
                id: dateLabel
                text: Qt.formatDateTime(new Date(), "dddd, MMMM d • h:mm AP")
                color: "#777777" // Matched to sidebar metadata color
                font.pixelSize: 13 // Matched to sidebar metadata size
            }
        }

        // --- CONTENT EDITOR ---
        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            flickableDirection: Flickable.VerticalFlick
            clip: true

            ScrollBar.vertical: ScrollBar {}

            TextArea.flickable: TextArea {
                id: editorArea
                topPadding: 8

                // CRITICAL FIX: Use RichText to allow colors, backgrounds, and sizes to save properly
                textFormat: TextEdit.RichText

                font.pointSize: 12
                font.family: "Georgia" // Great choice for reading!
                wrapMode: Text.Wrap
                background: null
                persistentSelection: true
                color: "#333333" // Softer black for long-form reading

                onCursorPositionChanged: {
                    if (editorArea.inputMethodComposing) return

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
