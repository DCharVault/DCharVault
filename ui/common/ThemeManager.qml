pragma Singleton

import QtQuick 6.8

QtObject {
    id: root
    property bool isDark: true

    onIsDarkChanged: {
        if (typeof loginViewModel !== "undefined") {
            loginViewModel.updateTitleBar(isDark)
        }
    }

    readonly property color bgVault: isDark ? "#0D0D0D" : "#F9FAFB"
    readonly property color bgCard: isDark ? "#1A1A1A" : "#FFFFFF"
    readonly property color bgInput: isDark ? "#0A0A0A" : "#FFFFFF"
    readonly property color bgButton: isDark ? "#2D2D2D" : "#F3F4F6"
    readonly property color bgButtonHover: isDark ? "#3A3A3A" : "#E5E7EB"

    readonly property color colorAccent: "#DC4D01"

    readonly property color textMain: isDark ? "#F3F4F6" : "#111827"
    readonly property color textMuted: isDark ? "#9CA3AF" : "#6B7280"
    readonly property color lineBorder: isDark ? "#333333" : "#E5E7EB"

    readonly property int marginGlobal: 16
    readonly property int radiusDefault: 8
    readonly property int controlHeight: 36 // spacious for desktop, perfect for mobile thumbs

    function toggleTheme() {
        isDark = !isDark
    }
}
