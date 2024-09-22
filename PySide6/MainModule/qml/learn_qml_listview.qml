import QtQuick
import QtQuick.Controls

ListView {
    id:lv
    anchors.fill: parent
    anchors.margins: 20
    clip: true
    model: 100
    spacing: 10
    // 2倍BlueBox的高度，cache 2 items
    cacheBuffer: 80

    delegate: BlueBox {
        required property int index
        width: lv.width/2
        height: 40
        text: index

        Component.onCompleted: {
            console.log(index + " added")
        }
        Component.onDestruction: {
            console.log(index + " deleted")
        }
    }
}