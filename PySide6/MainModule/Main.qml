import QtQuick
import QtQuick.Controls

import MainModule

ApplicationWindow {
    width: 800
    height: 600
    visible: true
    title: 'LearnPySide6-Main'

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    property string curtime: '00:00:00'
        Bridge {
            id: bridge
        }

        Rectangle {
            width: 60; height: 40
            color: 'blue'
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log(bridge.getColor('get color'))
                    bridge.update_time()
                }
            }
        }

        Text {
            x: 100; y: 100
            text : curtime
        }

        Connections {
            target: bridge


            // Python name	| QML name
            // mySignal	onMySignal
            // mysignal	onMysignal
            // my_signal	onMy_signal
            // 在Bridge中定义了timer_signal, 所以这里是 onTimer_signal
            function onTimer_signal(msg)
            {
                curtime = msg
            }
        }

        // LearnQmlListViewModelAction {}
        Rectangle {
            width: 60; height: 40
            x: 300
            color: 'yellow'
            Text {
                anchors.centerIn: parent
                font.pixelSize: 12
                color: 'white'
                text: 'Initial ListView'
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log('add list item')
                    // actionModel.addItemsToListModel([
                    //     {name: '1', hello: function(value) {}},
                    //     {name: '2', hello: function(value) {}},
                    //     {name: '3', hello: function(value) {}}
                    // ])
                }
            }
        }
        Rectangle {
            width: 60; height: 40
            x: 300
            color: 'red'
            Text {
                anchors.centerIn: parent
                font.pixelSize: 12
                color: 'white'
                text: 'Add Item'
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    function action(value) {
                        console.log(value + ' new add zhangsan')
                    }
                    console.log('add list item')
                    actionModel.append( { name: 'new add zhangsan', hello: action() })
            }
        }
    }

Rectangle {
    width: 60; height: 40
    x: 200
    color: 'blue'

    Text {
        anchors.centerIn: parent
        font.pixelSize: 12
        color: 'white'
        text: 'Remote Item'
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log('add list item')
            actionModel.clear()
        }
    }
}

Rectangle {
    width: 220
    height: 500
    x: 300
    y: 100

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

}
