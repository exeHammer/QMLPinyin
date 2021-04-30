import QtQuick 2.9
import QtQuick.Controls 2.2

import InputMethod 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Column {
        Repeater {
            model: 10
            delegate: TextField {
                placeholderText: index
            }
        }
    }
    Component.onCompleted: Input.setlistenRoot(this)
    onActiveFocusControlChanged: {
        console.log("activeFocusControl:", activeFocusControl.objectName)
        if (inputMethodIndicator.active)
            inputMethodIndicator.item.call(activeFocusControl)
    }

    InputMethodViewer {
        id: inputMethod
        onChineseChanged: {
            if (inputMethodIndicator.active) {
                inputMethodIndicator.item.chinese = chinese
            }
        }
    }
    Loader {
        id: inputMethodIndicator
        active: true
        source: "InputMethodIndicator.qml"
    }
}
