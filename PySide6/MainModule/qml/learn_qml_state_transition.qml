import QtQuick
import QtQuick.Controls

Item {
    id: root
    width: 150; height: 260

    property color black: 'black'
        property color red: 'red'
            property color green: 'green'

                states: [
                    State {
                        name:'stop'
                        PropertyChanges {target: light1; color:root.red}
                        PropertyChanges {target: light2; color:root.black}k}

                    },
                    State {
                        name:'go'
                        PropertyChanges {target: light1; color:root.black}
                        PropertyChanges {target: light2; color:root.red}

                    }
                ]

                transitions: [
                    Transition {
                        // from: 'stop' to 'go'
                        from: '*';to:'*'
                        ColorAnimation {
                            target: light1; properties: 'color';duration: 1000
                        }
                        ColorAnimation {
                            target: light2; properties: 'color';duration: 1000
                        }
                    }
                ]


                Rectangle {
                    anchors.fill: parent
                    color: "#333333"
                }

                Rectangle {
                    id: light1
                    x:25;y:25
                    width:100; height: width
                    radius: width/2
                    color: root.black
                }

                Rectangle {
                    id: light2
                    x:25;y:135
                    width:100;height:width
                    radius: width/2
                    color: root.red
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: false
                    onEntered: {}
                    onExited: {}
                    onWheel: {}
                    onClicked: parent.state = (parent.state == 'stop' ? 'go': 'stop')


                }
            }