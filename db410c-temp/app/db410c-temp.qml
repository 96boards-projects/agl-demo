import QtQuick 2.6
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtWebSockets 1.0

import 'MessageId.js' as MessageId

Window {
    	id: root

         // WINDOW PROPERTIES

        visible: true
        width: 340
        height: 240

        property string status_str: ""

    	WebSocket {
        	id: websocket
		url: bindingAddress
                onTextMessageReceived: {
                         // VERB RESPONSE VALIDATION
                        var json = JSON.parse(message)
                        var request = json[2].request
                        if (json[0] != MessageId.retok) {
                                console.log("Return value is not ok !")
                                return
                        }

			console.log("got value", request.info)
			temp.text = request.info
		}
                onStatusChanged: {
                        if (websocket.status == WebSocket.Error) {
                                status_str = "WebSocket error: " + websocket.errorString
                        } else if (websocket.status == WebSocket.Open) {
				var request_str = [MessageId.call, '9999', 'db410c/temp', '']
		            	console.debug(JSON.stringify(request_str))
            			websocket.sendTextMessage(JSON.stringify(request_str))
                        }
                }
		active: true
	}

        Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.margins: 20

                 // TITLE SECTION
                Text {
			id: lab
                        text: "DB410c Temperature App"
                        font.pixelSize: 16
                        font.bold: true
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: 0
                }

                 // COUNT SECTION
                Text {
                        id: temp
                        text: '0'
			font.pixelSize: 26
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: 60
		}
		Timer {
			id: timer
			interval: 3000
			running: true
			repeat: true
			onTriggered: {
				var request_str = [MessageId.call, '9999', 'db410c/temp', '']
           			console.debug(JSON.stringify(request_str))
            			websocket.sendTextMessage(JSON.stringify(request_str))
			}
		}
	}
}
