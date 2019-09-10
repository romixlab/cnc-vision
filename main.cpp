#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "capturecontroller.hpp"
#include "cvmatsurfacesource.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<CVMatSurfaceSource>("io.opencv", 1, 0, "CVMatSurfaceSource");
    qmlRegisterType<CaptureController>("io.opencv", 1, 0, "CaptureController");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
