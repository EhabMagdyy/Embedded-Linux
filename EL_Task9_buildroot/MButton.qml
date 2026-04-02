import QtQuick

Rectangle {
    id: root

    width: 50
    height: 30
    radius: cornerRadius

    // actual properties
    property color gradStart: "blue"
    property color gradEnd: "red"

    gradient: Gradient {
        orientation: Gradient.Vertical
        GradientStop { position: 0.0; color: root.gradStart }
        GradientStop { position: 1.0; color: root.gradEnd }
    }



    property alias label: btnText.text
    property alias btnWidth: root.width
    property alias btnHeight: root.height
    property alias textColor: btnText.color
    property alias cornerRadius: root.radius
    property alias fontSize: btnText.font.pixelSize

    signal btnClicked()

    Text {
        id: btnText
        anchors.centerIn: parent
        text: "B"
        color: root.textColor
        font.pixelSize: root.height * 0.5
    }

    MouseArea {
        id: btnMouseArea
        anchors.fill: parent
        // saving color and height & width properties to restore on hover exit and press release
        property color originalGradStart;
        property color originalGradEnd;
        property real originalWidth;
        property real originalHeight;

        hoverEnabled: true
        onEntered: {
            // save original colors before changing
            btnMouseArea.originalGradStart = root.gradStart
            btnMouseArea.originalGradEnd = root.gradEnd
            // lighten colors on hover by increasing lightness by 20%
            root.gradStart = Qt.lighter(root.gradStart, 1.4)
            root.gradEnd = Qt.lighter(root.gradEnd, 1.4)
        }
        onExited: {
            root.gradStart = btnMouseArea.originalGradStart
            root.gradEnd = btnMouseArea.originalGradEnd
        }
        onPressed: {
            // reduce height and width by 10% to simulate press
            btnMouseArea.originalWidth = root.width
            btnMouseArea.originalHeight = root.height

            root.width = root.width * 0.9
            root.height = root.height * 0.9
        }
        onReleased: {
            // restore height and width
            root.width = btnMouseArea.originalWidth
            root.height = btnMouseArea.originalHeight
        }

        onClicked: root.btnClicked()
    }
}