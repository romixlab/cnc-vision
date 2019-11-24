import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtMultimedia 5.9
import io.opencv 1.0
import tech.vhrd.vision 1.0
import tech.vhrd 1.0
import QtCharts 2.3
import QtQuick.Dialogs 1.2

Window {
    visible: true
    width: 1920
    height: 1080
    title: qsTr("Hello World")
    color: "#161616"

//    CaptureController {
//        id: captureCtrl
//        onStatusChanged: console.log(status)
//    }

    VideoOutput {
        id: mainVideoOutput
        anchors.right: parent.right
        anchors.top: parent.top
        width: parent.width / 2
        height: parent.height / 2
        source: CVMatSurfaceSource {
            name: "main"
        }
    }

    VideoOutput {
        id: secondVideoOutput
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: parent.width / 2
        height: parent.height / 2
        source: CVMatSurfaceSource {
            id: secondVideoSource
            name: "second"
        }
        onWidthChanged: osdRescale(testRect, secondVideoSource, secondVideoOutput)
        onHeightChanged: osdRescale(testRect, secondVideoSource, secondVideoOutput)
    }

    Item {
        id: testRect
        anchors.left: secondVideoOutput.left
        anchors.top: secondVideoOutput.top
        width: secondVideoOutput.width
        height: secondVideoOutput.height

        ColumnLayout {
            id: settingsLayout
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width * 0.4
            spacing: 8

            RowLayout {
                Layout.maximumHeight: 24
                Text {
                    text: "Hue range low:"
                    color: "gray"
                    Layout.preferredWidth: settingsLayout.width * 0.4
                }

                RangeSlider {
                    from: 0
                    to: 179
                    stepSize: 1
                    first.value: 0
                    second.value: 10
                    first.onMoved: lineDetector.hueLowRangeFrom = Math.floor(first.value)
                    second.onMoved: lineDetector.hueLowRangeTo = Math.floor(second.value)
                }
            }
            RowLayout {
                Layout.maximumHeight: 24
                Text {
                    text: "Hue range high:"
                    color: "gray"
                    Layout.preferredWidth: settingsLayout.width * 0.4
                }

                RangeSlider {
                    from: 0
                    to: 179
                    stepSize: 1
                    first.value: 170
                    second.value: 179
                    first.onMoved: lineDetector.hueHighRangeFrom = Math.floor(first.value)
                    second.onMoved: lineDetector.hueHighRangeTo = Math.floor(second.value)
                }
            }
            RowLayout {
                Layout.maximumHeight: 24
                Text {
                    text: "Saturation range:"
                    color: "gray"
                    Layout.preferredWidth: settingsLayout.width * 0.4
                }

                RangeSlider {
                    from: 0
                    to: 255
                    stepSize: 1
                    first.value: 40
                    second.value: 255
                    first.onMoved: lineDetector.saturationFrom = Math.floor(first.value)
                    second.onMoved: lineDetector.saturationTo = Math.floor(second.value)
                }
            }
            RowLayout {
                Layout.maximumHeight: 24
                Text {
                    text: "Value range:"
                    color: "gray"
                    Layout.preferredWidth: settingsLayout.width * 0.4
                }

                RangeSlider {
                    from: 0
                    to: 255
                    stepSize: 1
                    first.value: 40
                    second.value: 255
                    first.onMoved: lineDetector.valueFrom = Math.floor(first.value)
                    second.onMoved: lineDetector.valueTo = Math.floor(second.value)
                }
            }
            RowLayout {
                Layout.maximumHeight: 24
                Text {
                    text: "Integration range:"
                    color: "gray"
                    Layout.preferredWidth: settingsLayout.width * 0.4
                }

                RangeSlider {
                    from: 0
                    to: 1
                    stepSize: 0.01
                    first.value: 0.2
                    second.value: 0.8
                    first.onMoved: lineDetector.integrateFrom = first.value
                    second.onMoved: lineDetector.integrateTo = second.value
                }
            }
            RowLayout {
                Layout.maximumHeight: 24
                Text {
                    text: "Threshold:"
                    color: "gray"
                    Layout.preferredWidth: settingsLayout.width * 0.4
                }

                Slider {
                    id: thresholdSlider
                    from: 0
                    to: 1
                    stepSize: 0.01
                    onValueChanged: lineDetector.threshold = value
                }
            }
            RowLayout {
                Layout.maximumHeight: 24
                Text {
                    text: "Fine rotation:"
                    color: "gray"
                    Layout.preferredWidth: settingsLayout.width * 0.4
                }

                Slider {
                    from: -15
                    to: 15
                    stepSize: 0.1
                    onValueChanged: lineDetector.rotation = value
                }
            }


            Item {
                Layout.fillHeight: true
            }
        }

        ChartView {
            height: parent.width * 0.3
            width: parent.height + 112
            rotation: -90
            anchors.bottom: parent.top
            anchors.bottomMargin: 47
            anchors.right: parent.right
            anchors.rightMargin: -54
            transformOrigin: Item.BottomRight
            animationOptions: ChartView.NoAnimation
            backgroundColor: "transparent"
            legend.visible: false
            //antialiasing: true

            ValueAxis {
                id: axisX
                min: 0
                max: 1

                gridVisible: false
                lineVisible: false
                labelsVisible: false
            }
            ValueAxis {
                id: axisY
                min: 0
                max: 1
                gridVisible: false
                lineVisible: false
                labelsVisible: false
            }
            LineSeries {
                id: integratedPlotSeries
                name: "test"
                axisX: axisX
                axisY: axisY
                useOpenGL: true
                width: 1.0
            }
            LineSeries {
                id: thresholdPlotSeries
                axisX: axisX
                axisY: axisY
                useOpenGL: true
                width: 2.0
                color: "red"
            }

            Component.onCompleted: {
                lineDetectorDataSource.setIntegratedPlotSeries(integratedPlotSeries)
                lineDetectorDataSource.setThresholdPlotSeries(thresholdPlotSeries)
            }
        }

        Text {
            id: stateLabel
            text: "UNLOCKED"
            color: "#f44336"
            font.bold: true
            font.pointSize: 18
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 32
            anchors.rightMargin: 32

            Connections {
                target: lineDetector
                onStateChanged: {
                    var lstate = lineDetector.state;
                    if (lstate === LineDetector.Locked) {
                        stateLabel.text = "LOCKED"
                        stateLabel.color = "#4caf50"
                    } else if (lstate === LineDetector.Hold) {
                        stateLabel.text = "HOLD"
                        stateLabel.color = "#cddc39"
                    } else {
                        stateLabel.text = "UNLOCKED"
                        stateLabel.color = "#f44336"
                    }
                }
            }
        }

        Text {
            id: dzLabel
            font.bold: true
            font.pointSize: 18
            anchors.top: stateLabel.top
            anchors.right: stateLabel.left
            anchors.rightMargin: 8
            color: "gray"
            text: lineDetector.dz.toFixed(3)
        }

        Button {
            anchors.right: stateLabel.left
            anchors.top: dzLabel.bottom
            anchors.topMargin: 0
            anchors.rightMargin: 8
            text: "0"
            height: 34
            onClicked: lineDetector.zerodxs()
        }
    }


    function osdRescale(item, videoSource, videoOutput) {
        if (videoSource.width === 0)
            return
        var sourceAspectRatio = videoSource.width / videoSource.height
        var sourceVisibleWidth = videoOutput.width
        var sourceVisibleHeight = sourceVisibleWidth / sourceAspectRatio
        if (sourceVisibleHeight > videoOutput.height) {
            sourceVisibleHeight = videoOutput.height
            sourceVisibleWidth = sourceVisibleHeight * sourceAspectRatio
            item.anchors.leftMargin = (videoOutput.width - sourceVisibleWidth) / 2
            item.anchors.topMargin = 0
        } else {
            item.anchors.topMargin = (videoOutput.height - sourceVisibleHeight) / 2
            item.anchors.leftMargin = 0
        }

        item.width = sourceVisibleWidth
        item.height = sourceVisibleHeight
    }

    Connections {
        target: secondVideoSource
        onDimensionsChanged: osdRescale(testRect, secondVideoSource, secondVideoOutput)
    }

    GcodePlayer {
        id: player
        onCurrentLineChanged: commandsListView.positionViewAtIndex(currentLineNumber, ListView.Center)
    }

    FileDialog {
        id: playerFileDialog
        title: "Choose gcode file"
        folder: shortcuts.home
        onAccepted: {
            player.loadFile(fileUrl)
        }
    }

    Item {
        id: playerItem
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: parent.width / 2
        height: parent.height / 2

        RowLayout {
            id: playerButtonsLayout
            anchors.leftMargin: 16
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right
            height: 48
            spacing: 16

            Button {
                text: "Load"
                font.pointSize: 20
                onClicked: playerFileDialog.visible = true
            }

            Button {
                text: "Play"
                font.pointSize: 20
                onClicked: player.play();
            }

            Button {
                text: "Pause"
                font.pointSize: 20
                onClicked: player.pause();
            }

            Button {
                text: "Stop"
                font.pointSize: 20
                onClicked: player.stop();
            }

            Text {
                id: connectionStatusLabel
                font.bold: true
                font.pointSize: 16

                function checkState() {
                    var lstate = player.connectionState;
                    if (lstate === GcodePlayer.Connected) {
                        connectionStatusLabel.text = "MC:connected"
                        connectionStatusLabel.color = "#4caf50"
                    } else if (lstate === GcodePlayer.Connecting) {
                        connectionStatusLabel.text = "MC:connecting"
                        connectionStatusLabel.color = "#cddc39"
                    } else if (lstate === GcodePlayer.Disconnected) {
                        connectionStatusLabel.text = "MC:disconnected"
                        connectionStatusLabel.color = "#f44336"
                    }
                }

                Connections {
                    id: c1
                    target: player
                    onConnectionStateChanged: connectionStatusLabel.checkState()
                }

                Component.onCompleted: connectionStatusLabel.checkState()

                MouseArea {
                    anchors.fill: parent
                    onClicked: player.connectToMC()
                }
            }

            Text {
                id: playerStatusLabel
                text: "STOPPED"
                color: "orange"
                font.bold: true
                font.pointSize: 18

                Connections {
                    target: player
                    onStateChanged: {
                        var lstate = player.state;
                        if (lstate === GcodePlayer.Playing) {
                            playerStatusLabel.text = "PLAYING"
                            playerStatusLabel.color = "#4caf50"
                        } else if (lstate === GcodePlayer.Paused) {
                            playerStatusLabel.text = "PAUSED"
                            playerStatusLabel.color = "#cddc39"
                        } else if (lstate === GcodePlayer.Stopped) {
                            playerStatusLabel.text = "STOPPED"
                            playerStatusLabel.color = "orange"
                        } else if (lstate === GcodePlayer.Error) {
                            playerStatusLabel.text = "ERROR"
                            playerStatusLabel.color = "#f44336"
                        }
                    }
                }
            }

            Text {
                font.bold: true
                font.pointSize: 18
                color: "#ccc"
                text: player.currentLineNumber + " / " + player.linesCount + " (" + (player.currentLineNumber / player.linesCount).toFixed(1) + " %)"
            }

            Item {
                Layout.fillWidth: true
            }
        }

        ListView {
            id: commandsListView
            anchors.left: parent.left
            anchors.top: playerButtonsLayout.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            clip: true

            model: player.model

            delegate: Rectangle {
                height: 16
                width: parent.width
                color: model.lineNumber % 2 === 0 ? "#222" : "#333"
                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    Item {
                        width: 6
                    }

                    Rectangle {
                        width: parent.height * 0.6
                        height: width
                        //radius: height
                        color: {
                            if (model.status === GcodePlayerItem.Pending) {
                                return "orange"
                            } else if (model.status === GcodePlayerItem.Ok) {
                                return "#4caf50"
                            } else if (model.status === GcodePlayerItem.Warning) {
                                return "red"
                            } else if (model.status === GcodePlayerItem.InternalCommand) {
                                return "blue"
                            }

                            return "gray"
                        }
                    }
                    Item {
                        width: 64
                        height: parent.height
                        //color: "#333"
                        Text {
                            anchors.fill: parent
                            text: String(model.lineNumber).padStart(6, '0')
                            color: "#666"
                            horizontalAlignment: Text.AlignHCenter
                            font.family: "Monaco"
                        }
                    }
                    Item {
                        height: parent.height
                        Layout.fillWidth: true

                        Text {
                            anchors.fill: parent
                            //anchors.leftMargin: 8
                            text: {
                                if (model.status === GcodePlayerItem.Warning) {
                                    return model.code.trim() + " (" + model.response + ")"
                                } else {
                                    return model.code
                                }
                            }

                            color: "#ccc"
                            horizontalAlignment: Text.AlignLeft
                            font.family: "Monaco"
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                minimumSize: 0.1
            }
        }
    }

    Item {
        anchors.top: parent.top
        anchors.left: buttonsLayout.right
        anchors.bottom: playerItem.top
        anchors.right: mainVideoOutput.left
        anchors.margins: 20

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                Slider {
                    property real lastValue: 0.45
                    id: laserPowerSlider
                    from: 0.4
                    to: 5
                    stepSize: 0.01
                    onPressedChanged: {
                        if (value !== lastValue) {
                            lastValue = value
                            var normalized = value / 5.0
                            ray.setLaserPower(normalized)
                        }
                    }
                }

                Text {
                    color: "#ccc"
                    font.bold: true
                    text: laserPowerSlider.value.toFixed(2)
                }
            }

            RowLayout {
                Button {
                    text: "TE"
                    onClicked: ray.setTopExhaust(true)
                }
                Button {
                    text: "TD"
                    onClicked: ray.setTopExhaust(false)
                }
                Button {
                    text: "BE"
                    onClicked: ray.setBottomExhaust(true)
                }
                Button {
                    text: "BD"
                    onClicked: ray.setBottomExhaust(false)
                }
            }

            RowLayout {
                Layout.fillHeight: true
            }
        }
    }

    ColumnLayout {
        id: buttonsLayout
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 16
        spacing: 10
        width: 200

        Button {
            text: "Capture"
            onClicked: {
                captureController.start("abcd")
            }
        }

        Button {
            text: "Take pic"
            onClicked: cameraCalibrator.takePicture()
        }

        Button {
            text: "Save pics"
            onClicked: cameraCalibrator.savePictures("./pictures")
        }

        Button {
            text: "Load pics"
            onClicked: cameraCalibrator.loadPictures("./pictures")
        }

        Button {
            text: "Calibrate"
            onClicked: cameraCalibrator.calibrate()
        }

        Button {
            text: "Save calib"
            onClicked: cameraCalibrator.saveCalibrationData("undistort.yaml")
        }

        Button {
            text: "Load calib"
            onClicked: cameraCalibrator.loadCalibrationData("undistort.yaml")
        }

        Button {
            text: "Apply calib"
            onClicked: cameraCalibrator.applyCalibrationData()
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
