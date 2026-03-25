import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item { // Or Page, or Rectangle (whatever your root is)
    id: root

    // Define the custom signal
    signal backClicked()

    // Somewhere in your UI, you should have a back button:
    Button {
        text: "<- Back"
        onClicked: {
            root.backClicked() // This fires the signal that your router is listening for
        }
    }
}
