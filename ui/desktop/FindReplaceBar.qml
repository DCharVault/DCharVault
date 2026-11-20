import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DCharVault

Popup {
    id: root

    property var controller: null
    property bool replaceMode: false

    padding: 8
    closePolicy: Popup.NoAutoClose

    background: Rectangle {
        color: ThemeManager.bgCard
        border.color: ThemeManager.lineBorder
        border.width: 1
        radius: ThemeManager.radiusDefault

        // Subtle drop shadow effect using border layering
        Rectangle {
            anchors.fill: parent
            anchors.margins: -1
            z: -1
            color: "transparent"
            border.color: ThemeManager.isDark ? "#20000000" : "#15000000"
            border.width: 2
            radius: ThemeManager.radiusDefault + 1
        }
    }

    enter: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 140 }
            NumberAnimation { property: "y"; from: root.y + 6; to: root.y; duration: 140; easing.type: Easing.OutQuad }
        }
    }

    exit: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 100 }
            NumberAnimation { property: "y"; from: root.y; to: root.y + 6; duration: 100; easing.type: Easing.InQuad }
        }
    }

    function openFind() {
        replaceMode = false
        root.open()
        findField.forceActiveFocus()
        findField.selectAll()
        if (findField.text.length > 0 && root.controller) {
            root.controller.findAll(findField.text)
        }
    }

    function openReplace() {
        replaceMode = true
        root.open()
        replaceField.forceActiveFocus()
        replaceField.selectAll()
        if (findField.text.length > 0 && root.controller) {
            root.controller.findAll(findField.text)
        }
    }

    function closeAndPurge() {
        root.close()
        if (root.controller) {
            root.controller.purge()
        }
    }

    function toggle() {
        if (root.visible) {
            closeAndPurge()
        } else {
            openFind()
        }
    }

    Timer {
        id: searchDebounceTimer
        interval: 150
        repeat: false
        onTriggered: {
            if (root.controller) {
                root.controller.findAll(findField.text)
            }
        }
    }

    ColumnLayout {
        id: contentLayout
        spacing: 6

        // ── Row 1: Find Controls ──
        RowLayout {
            spacing: 6

            // Search input field
            TextField {
                id: findField
                implicitWidth: 180
                implicitHeight: 30
                placeholderText: "Find..."
                color: ThemeManager.textMain
                placeholderTextColor: ThemeManager.textMuted
                font.pixelSize: 13
                leftPadding: 26
                rightPadding: 8
                verticalAlignment: TextInput.AlignVCenter
                selectByMouse: true

                background: Rectangle {
                    color: ThemeManager.bgInput
                    border.color: findField.activeFocus ? ThemeManager.colorAccent : ThemeManager.lineBorder
                    border.width: findField.activeFocus ? 2 : 1
                    radius: 6

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        text: "🔍"
                        font.pixelSize: 11
                    }
                }

                onTextChanged: searchDebounceTimer.restart()

                Keys.onReturnPressed: function(event) {
                    if (event.modifiers & Qt.ShiftModifier) {
                        if (root.controller) root.controller.findPrevious()
                    } else {
                        if (root.controller) root.controller.findNext()
                    }
                }
                Keys.onEnterPressed: function(event) {
                    if (event.modifiers & Qt.ShiftModifier) {
                        if (root.controller) root.controller.findPrevious()
                    } else {
                        if (root.controller) root.controller.findNext()
                    }
                }
                Keys.onEscapePressed: root.closeAndPurge()
            }

            // Match counter pill (e.g., "2 / 14")
            Rectangle {
                implicitWidth: matchCountText.implicitWidth + 14
                implicitHeight: 30
                radius: 6
                color: ThemeManager.bgInput
                border.color: ThemeManager.lineBorder
                border.width: 1

                Text {
                    id: matchCountText
                    anchors.centerIn: parent
                    text: {
                        if (!root.controller) return "0 / 0"
                        if (findField.text.length === 0) return "—"
                        if (!root.controller.hasMatches) return "0 / 0"
                        return root.controller.currentMatch + " / " + root.controller.matchCount
                    }
                    font.pixelSize: 11
                    font.bold: true
                    color: (root.controller && root.controller.hasMatches)
                        ? ThemeManager.colorAccent
                        : ThemeManager.textMuted
                }
            }

            // Previous match button
            ToolButton {
                id: prevBtn
                implicitWidth: 28
                implicitHeight: 30
                enabled: root.controller && root.controller.hasMatches
                opacity: enabled ? 1.0 : 0.4
                ToolTip.visible: hovered
                ToolTip.text: "Previous Match (Shift+Enter)"
                ToolTip.delay: 400

                background: Rectangle {
                    color: prevBtn.down ? ThemeManager.bgPrimaryActionPressed
                        : (prevBtn.hovered ? ThemeManager.bgButtonHover : "transparent")
                    radius: 6
                    border.color: ThemeManager.lineBorder
                    border.width: 1
                }
                contentItem: Text {
                    text: "◁"
                    color: ThemeManager.textMain
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: if (root.controller) root.controller.findPrevious()
            }

            // Next match button
            ToolButton {
                id: nextBtn
                implicitWidth: 28
                implicitHeight: 30
                enabled: root.controller && root.controller.hasMatches
                opacity: enabled ? 1.0 : 0.4
                ToolTip.visible: hovered
                ToolTip.text: "Next Match (Enter)"
                ToolTip.delay: 400

                background: Rectangle {
                    color: nextBtn.down ? ThemeManager.bgPrimaryActionPressed
                        : (nextBtn.hovered ? ThemeManager.bgButtonHover : "transparent")
                    radius: 6
                    border.color: ThemeManager.lineBorder
                    border.width: 1
                }
                contentItem: Text {
                    text: "▷"
                    color: ThemeManager.textMain
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: if (root.controller) root.controller.findNext()
            }

            // Case Sensitivity Toggle [Aa]
            ToolButton {
                id: caseBtn
                implicitWidth: 28
                implicitHeight: 30
                checkable: true
                checked: root.controller ? root.controller.caseSensitive : false
                ToolTip.visible: hovered
                ToolTip.text: "Match Case (Case Sensitive)"
                ToolTip.delay: 400

                background: Rectangle {
                    color: caseBtn.checked
                        ? ThemeManager.surfaceElevated
                        : (caseBtn.hovered ? ThemeManager.bgButtonHover : "transparent")
                    radius: 6
                    border.color: caseBtn.checked ? ThemeManager.colorAccent : ThemeManager.lineBorder
                    border.width: 1
                }
                contentItem: Text {
                    text: "Aa"
                    font.bold: caseBtn.checked
                    font.pixelSize: 11
                    color: caseBtn.checked ? ThemeManager.colorAccent : ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    if (root.controller) {
                        root.controller.caseSensitive = !root.controller.caseSensitive
                    }
                }
            }

            // Whole Word Toggle [W]
            ToolButton {
                id: wholeWordBtn
                implicitWidth: 28
                implicitHeight: 30
                checkable: true
                checked: root.controller ? root.controller.wholeWord : false
                ToolTip.visible: hovered
                ToolTip.text: "Match Whole Word"
                ToolTip.delay: 400

                background: Rectangle {
                    color: wholeWordBtn.checked
                        ? ThemeManager.surfaceElevated
                        : (wholeWordBtn.hovered ? ThemeManager.bgButtonHover : "transparent")
                    radius: 6
                    border.color: wholeWordBtn.checked ? ThemeManager.colorAccent : ThemeManager.lineBorder
                    border.width: 1
                }
                contentItem: Text {
                    text: "W"
                    font.bold: wholeWordBtn.checked
                    font.pixelSize: 11
                    color: wholeWordBtn.checked ? ThemeManager.colorAccent : ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    if (root.controller) {
                        root.controller.wholeWord = !root.controller.wholeWord
                    }
                }
            }

            // Toggle Replace Drawer
            ToolButton {
                id: toggleReplaceBtn
                implicitWidth: 68
                implicitHeight: 30
                ToolTip.visible: hovered
                ToolTip.text: "Toggle Replace Mode (Ctrl+H)"
                ToolTip.delay: 400

                background: Rectangle {
                    color: root.replaceMode
                        ? ThemeManager.surfaceElevated
                        : (toggleReplaceBtn.hovered ? ThemeManager.bgButtonHover : "transparent")
                    radius: 6
                    border.color: root.replaceMode ? ThemeManager.colorAccent : ThemeManager.lineBorder
                    border.width: 1
                }
                contentItem: Text {
                    text: root.replaceMode ? "Replace ▴" : "Replace ▾"
                    font.pixelSize: 11
                    font.bold: root.replaceMode
                    color: root.replaceMode ? ThemeManager.colorAccent : ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    root.replaceMode = !root.replaceMode
                    if (root.replaceMode) {
                        replaceField.forceActiveFocus()
                    }
                }
            }

            // Close Button [✕]
            ToolButton {
                id: closeBtn
                implicitWidth: 28
                implicitHeight: 30
                ToolTip.visible: hovered
                ToolTip.text: "Close (Esc)"
                ToolTip.delay: 400

                background: Rectangle {
                    color: closeBtn.hovered ? Qt.rgba(1, 0.2, 0.2, 0.2) : "transparent"
                    radius: 6
                }
                contentItem: Text {
                    text: "✕"
                    font.pixelSize: 12
                    color: ThemeManager.textMuted
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.closeAndPurge()
            }
        }

        // ── Row 2: Replace Controls (Expands when replaceMode is true) ──
        RowLayout {
            visible: root.replaceMode
            spacing: 6

            // Replace input field
            TextField {
                id: replaceField
                Layout.fillWidth: true
                implicitHeight: 30
                placeholderText: "Replace with..."
                color: ThemeManager.textMain
                placeholderTextColor: ThemeManager.textMuted
                font.pixelSize: 13
                leftPadding: 26
                rightPadding: 8
                verticalAlignment: TextInput.AlignVCenter
                selectByMouse: true

                background: Rectangle {
                    color: ThemeManager.bgInput
                    border.color: replaceField.activeFocus ? ThemeManager.colorAccent : ThemeManager.lineBorder
                    border.width: replaceField.activeFocus ? 2 : 1
                    radius: 6

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        text: "📝"
                        font.pixelSize: 11
                    }
                }

                Keys.onReturnPressed: {
                    if (root.controller) root.controller.replaceCurrent(replaceField.text)
                }
                Keys.onEnterPressed: {
                    if (root.controller) root.controller.replaceCurrent(replaceField.text)
                }
                Keys.onEscapePressed: root.closeAndPurge()
            }

            // Replace Current Button
            Button {
                id: replaceOneBtn
                implicitWidth: 70
                implicitHeight: 30
                enabled: root.controller && root.controller.hasMatches
                opacity: enabled ? 1.0 : 0.4
                ToolTip.visible: hovered
                ToolTip.text: "Replace Selected Match"
                ToolTip.delay: 400

                background: Rectangle {
                    color: replaceOneBtn.down ? ThemeManager.bgPrimaryActionPressed
                        : (replaceOneBtn.hovered ? ThemeManager.bgPrimaryActionHover : ThemeManager.bgPrimaryAction)
                    radius: 6
                    border.color: ThemeManager.borderPrimaryAction
                    border.width: 1
                }
                contentItem: Text {
                    text: "Replace"
                    font.pixelSize: 11
                    font.bold: true
                    color: "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    if (root.controller) {
                        root.controller.replaceCurrent(replaceField.text)
                    }
                }
            }

            // Replace All Button
            Button {
                id: replaceAllBtn
                implicitWidth: 44
                implicitHeight: 30
                enabled: root.controller && root.controller.hasMatches
                opacity: enabled ? 1.0 : 0.4
                ToolTip.visible: hovered
                ToolTip.text: "Replace All Matches in Document"
                ToolTip.delay: 400

                background: Rectangle {
                    color: replaceAllBtn.down ? ThemeManager.bgPrimaryActionPressed
                        : (replaceAllBtn.hovered ? ThemeManager.surfaceElevated : ThemeManager.bgCard)
                    radius: 6
                    border.color: ThemeManager.lineBorder
                    border.width: 1
                }
                contentItem: Text {
                    text: "All"
                    font.pixelSize: 11
                    font.bold: true
                    color: ThemeManager.textMain
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    if (root.controller) {
                        root.controller.replaceAll(replaceField.text)
                    }
                }
            }
        }
    }
}
