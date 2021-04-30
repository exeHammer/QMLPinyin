import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Item {
    id: inputFieldRoot
    property string text
    property string selectedWords
    property string selectedSource
    property int fontSize
    property string labelColor

    width: selected.width + toBeSelected.width
    height: toBeSelected.height

    Label {
        id: selected
        property string source
        anchors.left: parent.left
        font.pixelSize: fontSize
        color: labelColor
        text: inputFieldRoot.selectedWords
    }
    Label {
        id: toBeSelected
        anchors.left: selected.right
        font.pixelSize: fontSize
        color: labelColor
        text: inputFieldRoot.text
    }

    function backspace() {
        //若有汉字，则还原其拼音
        if (selectedWords) {
            selectedWords = ""
            text = selectedSource + text
            selectedSource = ""
            return
        }
        text = text.substring(0, text.length - 1)
        if (text[text.length-1] === "'") {
            backspace()
        }
    }

    function appendSelected(word, src) {
        selectedWords += word
        selectedSource += src
    }

    function getSelected() {
        return selected.text
    }

    function clear() {
        text = ""
        selectedWords = ""
        selectedSource = ""
    }
}
