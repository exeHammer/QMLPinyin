import QtQuick 2.9
import QtQuick.Controls 2.2
import InputMethod 1.0

Rectangle {
    id: inputMethodViewerRoot

    property int candidateWordsPerPage: 5
    property int inputFontSize: 30
    property int candidateFontSize: 30
    property int candidatePageButtonSize: 50
    property int candidateWordsSpacing: 10
    property int candidateWordsStyle: Input.CandidateWords_Horizontal
    property string inputColor: "DarkCyan"
    property string candidateWordsColor: "DarkCyan"
    property string candidateFirstWordColor: "Red"
    property int margin: 10
    property int maxX: parent.width
    property int maxY: parent.height

    property bool chinese: true

    color: "AliceBlue"
    border.color: "Gray"

    property real objectX: 0
    //宽度过大，则x = 0。右侧超过，则以右侧为准设置，x = maxX - width
    //一般情况下，x = objectX
    x: (width < maxX) ? ((objectX + width < maxX) ? objectX : (maxX - width)) : 0
    opacity: 0
    visible: false
    width: (input.width > candidate.width ? input.width : candidate.width) + margin * 2
    height: input.height + candidate.height + margin * 3

    //background
    MouseArea {
        anchors.fill: parent
    }

    InputField {
        id: input
        fontSize: inputFontSize
        labelColor: inputColor
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: margin
    }
    CandidateBar {
        id: candidate
        anchors.top: input.bottom
        anchors.left: parent.left
        anchors.margins: margin
        fontSize: candidateFontSize
        wordsPerPage: candidateWordsPerPage
        wordsSpacing: candidateWordsSpacing
        style: candidateWordsStyle
        wordsColor: candidateWordsColor
        firstWordColor: candidateFirstWordColor
        pageButtonSize: candidatePageButtonSize
    }

    Component.onCompleted:  {
        Input.setInputMethodViewer(inputMethodViewerRoot)
        Input.setInputField(input)
        Input.setCandidateBar(candidate)
    }


    function show() {
        visible = true
    }
    function close() {
        input.clear()
        candidate.clear()
        visible = false
    }

    function setCoordinate(pos, offset) {
//        console.log(pos, offset)
        inputMethodViewerRoot.objectX = pos.x
        if (pos.y + offset + inputMethodViewerRoot.height > maxY) {
            inputMethodViewerRoot.y = pos.y - offset - inputMethodViewerRoot.height
        }
        else {
            inputMethodViewerRoot.y = pos.y + offset
        }
    }

    states: State {
        name: "visible"
        when: inputMethodViewerRoot.visible
        PropertyChanges {
            target: inputMethodViewerRoot
            opacity: 1
        }
    }
    transitions: Transition {
        from: ""
        to: "visible"
        ParallelAnimation {
            NumberAnimation {
                properties: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    }
}
