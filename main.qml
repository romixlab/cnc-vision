import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.0
import QtMultimedia 5.9
import io.opencv 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    //color: "#161616"

    CaptureController {
        id: captureCtrl
        onStatusChanged: console.log(status)
    }

    VideoOutput {
        anchors.centerIn: parent
        width: 1920
        height: 1080
        source: CVMatSurfaceSource {
            name: "main"
        }
    }

    Button {
        onClicked: {
            captureCtrl.start("abcd")
        }
    }
}
