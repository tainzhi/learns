import QtQuick
import QtQuick.Controls
Column {
    spacing: 8
    // Repeater {
    //     // model: 10
    //     model: ['hello', 'world', 'hello', 'piks', 'YT', 'BJ', 'halloy']
    //     delegate: BlueBox {
    //         // required property int index
    //         width: 100
    //         height: 64
    //         // text: index
    //         text: '[' + index + ']' + modelData
    //     }
    //     onItemAdded: console.log('item' + index + ' added')
    // }
    Repeater {
        model: ListModel {
            ListElement { name:'hello'; surfaceColor: 'gray' }
            ListElement { name:'hello'; surfaceColor: 'yello' }
            ListElement { name:'hello'; surfaceColor: 'blue' }
            ListElement { name:'hello'; surfaceColor: 'orange' }
            ListElement { name:'hello'; surfaceColor: 'lightBlue' }
            ListElement { name:'hello'; surfaceColor: 'white' }
            ListElement { name:'hello'; surfaceColor: 'purple' }
        }
        delegate: BlueBox {
            id: blueBox
            required property string name
            required property color surfaceColor
            width: 120;height:32
            radius: 3
            text: name
            Rectangle {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 8
                width:16;height:width;radius:8
                color:parent.surfaceColor
            }
        }
    }
}