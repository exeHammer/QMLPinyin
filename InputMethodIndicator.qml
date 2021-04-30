import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtTest 1.0

import InputMethod 1.0

Rectangle {
    id: indicatorRoot
    property bool chinese: true
    property int maxX: 1024
    property int maxY: 600
    property int welt: 100
    x: maxX - width
    y: maxY - height
    height: 50
    width: icon.width + text.width + 10 * 2
    color: "LightGoldenRodYellow"
    border.color: "black"

    Drag.active: dragArea.drag.active

    Image {
        id: icon
        width: height
        height: parent.height
        source: "icons/keyboard-color.png"
    }
    Text {
        id: text
        anchors.left: icon.right
        anchors.verticalCenter: icon.verticalCenter
        anchors.leftMargin: 10
        text: chinese ? "中" : "En"
        font.pixelSize: 20
    }
    NumberAnimation on x{
        id:aniX
        duration: 400
        easing.type: Easing.OutCubic
    }
    NumberAnimation on y{
        id:aniY
        duration: 400
        easing.type: Easing.OutCubic
    }
    MouseArea {
        id: dragArea
        anchors.fill: parent
        drag.target: parent
        drag.maximumX: maxX-indicatorRoot.width
        drag.minimumX: 0
        drag.maximumY: maxY-indicatorRoot.height
        drag.minimumY: 0

        onClicked: {
            testEvent.keyClick(Qt.Key_Space, Qt.ControlModifier, -1)
        }
        TestEvent {
            id: testEvent
        }

        onReleased: {
            //小于阈值welt自动贴边
            if(maxX - indicatorRoot.x - indicatorRoot.width < welt){
                aniX.to = maxX - indicatorRoot.width
                aniX.start()
            }
            else if (indicatorRoot.x < welt) {
                aniX.to = 0
                aniX.start()
            }
            if(maxY - indicatorRoot.y - indicatorRoot.height < welt){
                aniY.to = maxY - indicatorRoot.height
                aniY.start()
            }
            else if (indicatorRoot.y < welt) {
                aniY.to = 0
                aniY.start()
            }
        }
    }

    function call(activeFocusControl) {
        if (Input.classIsListen(helper.getClassName(activeFocusControl), true)) {
            visible = true
        }
        else {
            visible = false
        }
    }
}
