import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import DCharVault

ApplicationWindow {
    id: window

    property bool isDark: true

    visible: true
    width: 800
    height: 700
    title: "AegisJournal - Editor Test"

    // --- GLOBAL STYLE ---
    Material.theme: isDark ? Material.Dark : Material.Light
    Material.accent: Material.DeepOrange
    Material.primary: "#FFFFFF"

    HomeView {
        anchors.fill: parent
    }
}
