import QtQuick
import DCharVault

Canvas{
    id: root

    property int completionCompleted: 0;
    property int completionTotal: 0;
    property string completionState: "none"

    width: 18
    height: 18

    onPaint:{
        const ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        const cx = width / 2
        const cy = height / 2
        const r  = (width / 2) - 1.5
        if (root.completionState === "none") {
            // Neutral faint circle — no trackable items
            ctx.beginPath()
            ctx.arc(cx, cy, r, 0, 2 * Math.PI)
            ctx.strokeStyle = ThemeManager.textMuted
            ctx.globalAlpha = 0.30
            ctx.lineWidth = 1.5
            ctx.stroke()
            ctx.globalAlpha = 1.0
        } else if (root.completionState === "complete") {
            // Fully filled accent circle — all items done
            ctx.beginPath()
            ctx.arc(cx, cy, r, 0, 2 * Math.PI)
            ctx.fillStyle = ThemeManager.colorAccent
            ctx.fill()
        } else {
            // Partial arc — in progress
            const pct = root.completionTotal > 0
                ? root.completionCompleted / root.completionTotal
                : 0
            const startAngle = -Math.PI / 2          // top of circle
            const endAngle   = startAngle + pct * 2 * Math.PI
            // Background track
            ctx.beginPath()
            ctx.arc(cx, cy, r, 0, 2 * Math.PI)
            ctx.strokeStyle = ThemeManager.lineBorder
            ctx.lineWidth = 2
            ctx.stroke()
            // Progress arc
            ctx.beginPath()
            ctx.arc(cx, cy, r, startAngle, endAngle)
            ctx.strokeStyle = ThemeManager.colorAccent
            ctx.lineWidth = 2
            ctx.lineCap = "round"
            ctx.stroke()
        }
    }
    onCompletionStateChanged: requestPaint()
    onCompletionTotalChanged: requestPaint()
    onCompletionCompletedChanged: requestPaint()

    Connections {
        target: ThemeManager
        function onThemeChanged() { root.requestPaint() }
    }

    Component.onCompleted: requestPaint()
    onVisibleChanged: if (visible) requestPaint()
}