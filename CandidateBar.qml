import QtQuick 2.9
import QtQuick.Controls 2.2

import InputMethod 1.0

Item {
    id: candidateBarRoot
    width: wordsLoader.width + pageRow.width + wordsSpacing
    height: wordsLoader.height

    property var wordslist : []
    property int page: 0
    property int style
    property int wordsPerPage
    property int fontSize
    property int pageButtonSize
    property int wordsSpacing
    property string wordsColor
    property string firstWordColor

    signal wordClicked(int index)

    Loader {
        id: wordsLoader
        sourceComponent: {
            if (style == Input.CandidateWords_Horizontal)
                return wordsRowComp
            else if (style == Input.CandidateWords_Vertical) {
                return wordsColComp
            }
        }
    }

    Row {
        id: pageRow
        anchors.left: wordsLoader.right
        anchors.leftMargin: wordsSpacing
        anchors.bottom: wordsLoader.bottom
        visible: wordslist.length
        ItemDelegate {
            width: preText.width + wordsSpacing
            height: preText.height + wordsSpacing
            onClicked: {
                prePage()
            }
            Label {
                id: preText
                text: "<"
                anchors.centerIn: parent
                font.pixelSize: pageButtonSize
                color: wordsColor
            }
        }
        ItemDelegate {
            width: nextText.width + wordsSpacing
            height: nextText.height + wordsSpacing
            onClicked: {
                nextPage()
            }
            Label {
                id: nextText
                text: ">"
                anchors.centerIn: parent
                font.pixelSize: pageButtonSize
                color: wordsColor

            }
        }

    }

    Component {
        id: wordsRowComp
        Row {
            Repeater {
                model: wordsCount
                delegate: wordTextComp
            }
        }
    }
    Component {
        id: wordsColComp
        Column {
            Repeater {
                model: wordsCount
                delegate: wordTextComp
            }
        }
    }
    // 每页最多wordsPerPage个候选词
    readonly property int wordsCount: wordslist.length < (page+1) * wordsPerPage
                             ? wordslist.length - page * wordsPerPage
                             : wordsPerPage
    Component {
        id: wordTextComp
        ItemDelegate {
            width: word.width + wordsSpacing
            height: word.height + wordsSpacing
            onClicked: {
                candidateBarRoot.wordClicked(index + 1)
            }
            Text {
                id: word
                anchors.centerIn: parent
                font.pixelSize: fontSize
                text: (index+1) + "." +wordslist[index+page*wordsPerPage]
                color: index == 0 ? firstWordColor : wordsColor
            }
        }
    }

    function clear() {
        wordslist = []
        page = 0
    }

    function nextPage() {
        if (wordslist.length > wordsPerPage * (page+1)) {
            page += 1
        }
    }
    function prePage() {
        if (page > 0) {
            page -= 1
        }
    }

    function getText(index) {
        return wordslist[(page*wordsPerPage+index-1)]
    }
}
