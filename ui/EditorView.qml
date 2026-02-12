import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // --- API ---
    property alias entryTitle: titleField.text
    property alias entryContent: editorArea.text
    property alias readOnly: editorArea.readOnly

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10

        // --- 1. THE SCROLLABLE TOOLBAR (Mobile Optimized) ---
        // We wrap the RowLayout in a ScrollView.
        // On Desktop: It looks like a normal bar.
        // On Mobile: You can swipe left/right to see more tools.
        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 50 // Fixed height for the bar
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff // Hide the ugly bar, just allow swipe

            RowLayout {
                spacing: 5

                // Group 1: Font Face
                ComboBox {
                    model: ["Segoe UI", "Georgia", "Arial", "Courier New"]
                    Layout.preferredWidth: 120
                    onActivated: (index) => {
                        // In real implementation, this sends a signal to C++ document handler
                        editorArea.font.family = currentText
                        console.log("Font changed to: " + currentText)
                    }
                }

                // Group 2: Formatting
                ToolButton { text: "<b>B</b>"; onClicked: forceActiveFocus() }
                ToolButton { text: "<i>I</i>"; onClicked: forceActiveFocus() }
                ToolButton { text: "<u>U</u>"; onClicked: forceActiveFocus() }

                ToolSeparator {}

                // Group 3: Colors (The new request)
                ToolButton {
                    text: "A"
                    font.bold: true
                    palette.buttonText: "red" // Visual clue
                    onClicked: console.log("Text Color: Red")
                    // To make this real, we'd open a ColorDialog here
                }

                ToolButton {
                    text: "🖊️" // Highlighter
                    palette.buttonText: "#FFA500"
                    onClicked: console.log("Highlight: Yellow")
                }

                ToolSeparator {}

                // Group 4: Sizing
                ComboBox {
                    model: [12, 14, 16, 18, 24, 32]
                    Layout.preferredWidth: 70
                    currentIndex: 2
                }

                // Group 5: Alignment (Extra options hidden on small screens)
                ToolButton { text: "Left" }
                ToolButton { text: "Center" }
            }
        }

        // Divider
        Rectangle { Layout.fillWidth: true; height: 1; color: "#DDDDDD" }

        // --- 2. HEADER ---
        TextField {
            id: titleField
            Layout.fillWidth: true
            placeholderText: "Title your day..."
            font.pixelSize: 24
            font.bold: true
            background: null
            selectByMouse: true
        }

        Text {
            id: dateLabel
            text: Qt.formatDateTime(new Date(), "dddd, MMMM d • h:mm AP")
            color: "#C4A484"
            font.pixelSize: 12
        }

        // --- 3. EDITOR ---
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            TextArea {
                id: editorArea
                placeholderText: "Start writing..."
                font.pixelSize: 16
                font.family: "Georgia"
                // Line height removed per previous fix
                wrapMode: Text.Wrap
                textFormat: TextEdit.RichText
                selectByMouse: true
                background: null
                color: "#7B3F00"
            }
        }
    }
}
