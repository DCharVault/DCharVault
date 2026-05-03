import QtQuick
import QtQuick.Controls
import Vault.Security 1.0
Rectangle {
    id: root
    property string placeholderText: "Enter Password"

    //Expose underlying C++ component so loginViewModel can access it
    property alias secureInputComponent: secureInput

    //custom signal so parent file knows when Enter is pressed
    signal enterPressed()


    color: ThemeManager.bgInput
    border.color: secureInput.activeFocus ? ThemeManager.colorAccent : ThemeManager.lineBorder
    border.width: secureInput.activeFocus ? 2 : 1
    radius: 7

    SecurePasswordInput {
        id: secureInput
        anchors.fill: parent
        focus: true

        Text {
            anchors.fill: parent
            anchors.margins: 12
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 24
            color: "#111111"

            // Dynamically creates a string of dots exactly as long as input password
            text: "".padStart(secureInput.passwordLength, "•")

            // Placeholder text for empty condition
            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: root.placeholderText
                color: "#9ca3af"
                font.pixelSize: 14
                visible: secureInput.passwordLength === 0
            }
        }

        onEnterPressed: {
            root.enterPressed()
        }
    }
}
