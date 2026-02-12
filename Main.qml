import QtQuick
import QtQuick.Controls

// We import the folder where EditorView.qml lives
import "ui"

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 900
    title: "AegisJournal - Editor Test"

    // Set a nice background color so the "paper" stands out
    color: "#F2F2F2"

    // Load ONLY the EditorView
    EditorView {
        anchors.fill: parent
        anchors.margins: 0 // Edges touch the window

        // Test Data: Pre-fill some text to see if it looks right
        entryTitle: "Testing the Editor"
        entryContent: "<b>Hello!</b><br>This is a test of the <i>rich text</i> editor.<br><br>Type here to test the scrolling..."
    }
}
