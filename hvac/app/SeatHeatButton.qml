/*
 * Copyright (C) 2016 The Qt Company Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import AGL.Demo.Controls 1.0


Item {
    id: root
    implicitWidth: 318
    implicitHeight: 219

    property string side: 'Left'
    property int headLevel: 0

    Column {
        id: background
        anchors.centerIn: parent
        Image {
            id: chair
            source: './images/HMI_HVAC_%1_Chair_OFF.svg'.arg(root.side)
            states: [
                State {
                    when: root.headLevel > 0
                    PropertyChanges {
                        target: chair
                        source: './images/HMI_HVAC_%1_Chair_ON.svg'.arg(root.side)
                    }
                }
            ]
        }
        Image {
            id: indicator
            width: 178
            height: 18
            source: './images/HMI_HVAC_ChairIndicator_OFF.svg'
            states: [
                State {
                    when: root.headLevel === 1
                    PropertyChanges {
                        target: indicator
                        source: './images/HMI_HVAC_ChairIndicator_One.svg'
                    }
                },
                State {
                    when: root.headLevel === 2
                    PropertyChanges {
                        target: indicator
                        source: './images/HMI_HVAC_ChairIndicator_Two.svg'
                    }
                }
            ]
        }

    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.headLevel = (root.headLevel + 1) % 3
        }
    }
}
