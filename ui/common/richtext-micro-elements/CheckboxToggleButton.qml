import QtQuick
import QtQuick.Controls
import DCharVault

ToolButton{
    id: root
    property bool isActive: false
    signal toggled()

    text: "☑"
    font.pixelSize: 16
    checkable: true
    checked: root.isActive

    ToolTip.visible: hovered
    ToolTip.text: root.isActive ? "Remove Checkbox" : "Add Checkbox (To-Do)"
    ToolTip.delay: 600

    contentItem: Text{
        text: parent.text
        font: parent.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: root.isActive ? ThemeManager.colorAccent : ThemeManager.textMain

        Behavior on color {
            ColorAnimation { duration: 120 }
        }
    }
    background: Rectangle {
        radius: ThemeManager.radiusDefault
        color: root.isActive
            ? Qt.alpha(ThemeManager.colorAccent, 0.15)
            : (root.hovered ? ThemeManager.surfaceElevated : "transparent")
        Behavior on color {
            ColorAnimation { duration: 120 }
        }
    }
    onClicked: {
        root.toggled()
        // Re-bind checked so it always reflects the real document state
        // rather than the toggle animation state
        checked = Qt.binding(function() { return root.isActive })
    }
}