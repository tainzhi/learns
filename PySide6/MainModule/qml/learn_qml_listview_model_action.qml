import QtQuick

Rectangle {
    width: 220
    height: 500
    x: 300

    gradient: Gradient {
        GradientStop { position: 0.0; color: '#f6f6f6'}
        GradientStop { position: 1.0; color: '#d7d7d7'}
    }

    ListView {
        focus: true
        spacing: 5
        clip: true
        anchors.fill: parent
        anchors.margins: 10
        model: actionModel
        delegate: Rectangle {
            id: delegate
            required property int index
            required property string name
            required property var hello
            width: ListView.view.width
            height: 40
            color: 'lightblue'

            Text {
                anchors.centerIn: parent
                font.pixelSize: 12
                text: delegate.name
            }

            MouseArea {
                anchors.fill: parent
                onClicked: delegate.hello(delegate.index)
            }
        }
    }

    ListModel {
        id:actionModel
        ListElement {
            name: 'zhangsan'
            hello: function(value) {
            console.log(value + ' zhangsan') }
        }
        ListElement {
            name: 'lisi'
            hello: function(value) { console.log(value + ' lisi') }
        }
        ListElement {
            name: 'wangwu'
            hello: function(value) { console.log(value + ' wangwu') }
        }
        ListElement {
            name: 'zhaoqi'
            hello: function(value) { console.log(value + ' zhaoqi') }
        }
    }
}