import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DCharVault

Rectangle{
    id: root
    property string icon: ""
    property string label: "Metric"
    property string value: "0"
    property string tooltipText: ""
    property bool interactive: false

    // theming
    property int   cornerRadius: ThemeManager.radiusDefault - 4
    property color bgColor:      mouseArea.containsMouse && root.interactive
        ? ThemeManager.bgButtonHover
        : ThemeManager.bgInput
    property color borderColor:  ThemeManager.lineBorder
    property color valueColor:   ThemeManager.textMain
    property color labelColor:   ThemeManager.textMuted

    signal clicked()

    // geometry
    implicitWidth:  contentRow.implicitWidth + 20
    implicitHeight: 28
    radius:         root.cornerRadius
    color:          root.bgColor
    border.color:   root.borderColor
    border.width:   1

    Behavior on color { ColorAnimation { duration: 130 } }
    Behavior on border.color { ColorAnimation { duration: 130 } }

    RowLayout{
        id: contentRow
        anchors.centerIn: parent
        spacing: 5
        Text {
            visible:        root.icon !== ""
            text:           root.icon
            font.pixelSize: 12
            color:          root.labelColor
            verticalAlignment: Text.AlignVCenter
        }
        Text {
            text:           root.value
            font.pixelSize: 12
            font.bold:      true
            color:          root.valueColor
            verticalAlignment: Text.AlignVCenter
        }
        Text {
            visible:        root.label !== ""
            text:           root.label
            font.pixelSize: 12
            color:          root.labelColor
            verticalAlignment: Text.AlignVCenter
        }
    }

    ToolTip.visible: root.tooltipText !== "" && mouseArea.containsMouse
    ToolTip.text: root.tooltipText
    ToolTip.delay: 400
    MouseArea{
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape:  root.interactive ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked:{
            if(root.interactive)
                root.clicked()
        }
    }
}