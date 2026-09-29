import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vault.Core 1.0
import DCharVault

Rectangle {
    id: root
    property alias analyzer: vaultAnalyzer

    color: "transparent"
    implicitWidth: contentLayout.width + 16

    TextAnalyzer {
        id: vaultAnalyzer
    }

    RowLayout {
        id: contentLayout
        anchors.centerIn: parent
        spacing: 8

        Text {
            font.pixelSize: 12
            font.family: "Open Sans"
            color: ThemeManager.textMuted
            text: vaultAnalyzer.m_wordsCount + " Words"
        }
        Text {
            font.pixelSize: 12
            color: ThemeManager.lineBorder
            text: "•"
        }
        Text {
            font.pixelSize: 12
            font.family: "Open Sans"
            color: ThemeManager.textMuted
            text: vaultAnalyzer.m_charsCount + " Chars"
        }
    }
}