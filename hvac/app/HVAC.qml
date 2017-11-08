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
import 'api' as API

ApplicationWindow {
    id: root

    API.Binding {
        id: binding
        url: bindingAddress
        onFanSpeedChanged: fanSpeedSlider.value = fanSpeed
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: width / 10
        anchors.bottomMargin: width / 10
        RowLayout {
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            Image {
                source: './images/HMI_HVAC_Fan_Icon.svg'
            }
            Item {
                width: root.width * 0.8
                Slider {
                    id: fanSpeedSlider
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    from: 0.0
                    to: 255.0
                    stepSize: 1.0
                    onValueChanged: {
                        binding.fanSpeed = value
                    }
                }
                Label {
                    anchors.left: fanSpeedSlider.left
                    anchors.top: fanSpeedSlider.bottom
                    font.pixelSize: 32
                    text: 'FAN SPEED'
                }
            }
        }
        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Layout.Center
            spacing: 20
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 20
                SeatHeatButton {
                    id: leftSeat
                    side: 'Left'
                }
                HeatDegree {
                    onCurrentItemChanged: {
                        console.log("Left Temp changed",degree)
                        binding.leftTemperature = degree
                    }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 20
                ToggleButton {
                    onImage: './images/HMI_HVAC_Active.svg'
                    offImage: './images/HMI_HVAC_Inactive.svg'
                    Label {
                        anchors.centerIn: parent
                        color: parent.checked ? '#66FF99' : '#848286'
                        text: 'A/C'
                        font.pixelSize: parent.height / 3
                    }
                    onCheckedChanged: {
                        console.debug('A/C', checked)
                    }
                }
                ToggleButton {
                    onImage: './images/HMI_HVAC_Active.svg'
                    offImage: './images/HMI_HVAC_Inactive.svg'
                    Label {
                        anchors.centerIn: parent
                        color: parent.checked ? '#66FF99' : '#848286'
                        text: 'AUTO'
                        font.pixelSize: parent.height / 3
                    }
                    onCheckedChanged: {
                        console.debug('AUTO', checked)
                    }
                }
                ToggleButton {
                    onImage: './images/HMI_HVAC_Circulation_Active.svg'
                    offImage: './images/HMI_HVAC_Circulation_Inactive.svg'
                    onCheckedChanged: {
                        console.debug('Circulation', checked)
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 20
                SeatHeatButton {
                    id: rightSeat
                    side: 'Right'
                }
                HeatDegree {
                    onCurrentItemChanged: {
                        console.log("Right Temp changed",degree)
                        binding.rightTemperature = degree
                    }
                }
            }
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            spacing: root.width / 20
            Repeater {
                model: ['AirDown', 'AirUp', 'AirRight', 'Rear', 'Front']
                ToggleButton {
                    onImage: './images/HMI_HVAC_%1_Active.svg'.arg(model.modelData)
                    offImage: './images/HMI_HVAC_%1_Inactive.svg'.arg(model.modelData)
                    onCheckedChanged: {
                        console.debug(model.modelData, checked)
                    }
                }
            }
        }
    }
}
