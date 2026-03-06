import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import DCharVault

ApplicationWindow {
    id: window

    visible: true
    width: 800
    height: 700
    title: "AegisJournal - Editor Test"

    Material.accent: Material.DeepOrange
    Material.primary: "#FFFFFF"

    HomeView {
        anchors.fill: parent
    }
}
