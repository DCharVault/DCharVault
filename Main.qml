import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
// We import the folder where .qml lives
import "ui"

ApplicationWindow {
    id: window

    property bool isDark: true



    visible: true
    width: 800
    height: 700
    title: "AegisJournal - Editor Test"

    // --- GLOBAL STYLE ---
        // This applies to the whole app (Sidebar, Editor, everything)
        Material.theme: isDark ? Material.Dark : Material.Light
        Material.accent: Material.DeepOrange // The color of the "+" button and active fields
        Material.primary: "#FFFFFF"


    HomeView{
        anchors.fill: parent
    }
}
