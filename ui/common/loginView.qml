import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import Vault.Security 1.0

Item{
    id: rootLogin
    width: 200
    height: 600
    visible: true
    title: "Vault Login"
    property string selectedDBfile = ""
    ColumnLayout{
        anchors.centerIn = parent
        spacing: 10
        Text{
            text: diaryName === "" ? "Pick Your Journal" : "Change Vault";
        }
    }
}