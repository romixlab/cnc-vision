#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "capturecontroller.hpp"
#include "cvmatsurfacesource.hpp"
#include "cameracalibrator.h"
#include "linedetector.h"
#include "linedetectordatasource.h"
#include "gcodeplayer.h"
#include "rayreceiver.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<CVMatSurfaceSource>("io.opencv", 1, 0, "CVMatSurfaceSource");
    //qmlRegisterType<CaptureController>("io.opencv", 1, 0, "CaptureController");

    CaptureController captureController;

    CameraCalibrator cameraCalibrator(&captureController);
    QObject::connect(&captureController, &CaptureController::frameReady,
                     &cameraCalibrator,  &CameraCalibrator::onFrameReady);

    qmlRegisterUncreatableType<LineDetector>("tech.vhrd.vision", 1, 0, "LineDetector", "Only for enums");
    LineDetector lineDetector(&captureController);
    QObject::connect(&captureController, &CaptureController::frameReady,
                     &lineDetector,      &LineDetector::onFrameReady);
    LineDetectorDataSource lineDetectorDataSource;
    QObject::connect(&lineDetector,           &LineDetector::integrationComplete,
                     &lineDetectorDataSource, &LineDetectorDataSource::updateIntegratedPlot);
    QObject::connect(&lineDetector,           &LineDetector::lineDetected,
                     &lineDetectorDataSource, &LineDetectorDataSource::drawBeamThickness);

    engine.rootContext()->setContextProperty("captureController", &captureController);
    engine.rootContext()->setContextProperty("cameraCalibrator", &cameraCalibrator);
    engine.rootContext()->setContextProperty("lineDetector", &lineDetector);
    engine.rootContext()->setContextProperty("lineDetectorDataSource", &lineDetectorDataSource);

    GcodePlayer::registerQmlTypes();
    GcodePlayerModel::registerQmlTypes();

    RayReceiver receiver;
    engine.rootContext()->setContextProperty("ray", &receiver);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
