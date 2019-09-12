#include <opencv2/opencv.hpp>

#include <QDir>

#include "cameracalibrator.h"
#include "capturecontroller.hpp"
#include "cvmatsurfacesource.hpp"



Q_LOGGING_CATEGORY(cameraCalibrator, "vhrd.vision.camera_calibrator");

CameraCalibrator::CameraCalibrator(CaptureController *captureController, QObject *parent) : QObject(parent),
    m_takePicture(false), m_captureController(captureController),
    m_horizontalCornersCount(9), m_verticalCornersCount(6)
{
}

quint32 CameraCalibrator::horizontalCornersCount() const
{
    return m_horizontalCornersCount;
}

void CameraCalibrator::setHorizontalCornersCount(quint32 count)
{
    m_horizontalCornersCount = count;
    m_pictures.clear();
}

quint32 CameraCalibrator::verticalCornersCount() const
{
    return m_verticalCornersCount;
}

void CameraCalibrator::setVerticalCornersCount(quint32 count)
{
    m_verticalCornersCount = count;
    m_pictures.clear();
}

void CameraCalibrator::takePicture()
{
    m_takePicture = true;
}

void CameraCalibrator::onFrameReady()
{
    if (!m_takePicture)
        return;
    m_takePicture = false;

    qCDebug(cameraCalibrator) << "Processing frame";
    cv::Mat frame = m_captureController->frameCopy();
    findChessboard(frame);
}

void CameraCalibrator::calibrate()
{
    if (m_pictures.length() <= 1) {
        qCWarning(cameraCalibrator) << "Not enough pictures for calibration";
        return;
    }
    std::vector<cv::Point3f> points;
    for (quint32 i = 0; i < m_horizontalCornersCount; ++i) {
        for (quint32 j = 0; j < m_verticalCornersCount; ++j) {
            points.push_back(cv::Point3f(i, j, 0));
        }
    }
    std::vector<std::vector<cv::Point3f>> object_points;
    std::vector<std::vector<cv::Point2f>> image_points;

    foreach (const picture_t &p, m_pictures) {
        object_points.push_back(points);
        image_points.push_back(p.corners);
    }

    m_intrinsic = cv::Mat(3, 3, CV_32FC1);
    m_intrinsic.ptr<float>(0)[0] = 1; // fx
    m_intrinsic.ptr<float>(1)[1] = 1; // fy

    cv::Mat distCoeffs;
    std::vector<cv::Mat> rvecs;
    std::vector<cv::Mat> tvecs;

    float rms = cv::calibrateCamera(object_points,
                                    image_points,
                                    m_pictures[0].picture.size(),
                                    m_intrinsic,
                                    m_distCoeffs,
                                    rvecs,
                                    tvecs );
    qCDebug(cameraCalibrator) << "rms: " << rms;
}

void CameraCalibrator::applyCalibrationData()
{
    if (m_intrinsic.empty() || m_distCoeffs.empty()) {
        qCWarning(cameraCalibrator) << "not yet calibrated";
        return;
    }
    qCDebug(cameraCalibrator) << "Enabling undistort";
    m_captureController->enableUndistort(m_intrinsic, m_distCoeffs);
}

void CameraCalibrator::savePictures(const QString &toFolder)
{
    QDir dir = QDir(toFolder);
    QString absPath = dir.absolutePath();
    if (!dir.exists()) {
        qCDebug(cameraCalibrator) << "Creating" << absPath;
        dir.mkpath(absPath);
    } else {
        qCDebug(cameraCalibrator) << "Saving images to" << absPath;
    }
    quint32 i = 0;
    foreach (const picture_t &p, m_pictures) {
        QString filename = QString("%1/%2.jpg").arg(absPath).arg(i);
        bool ok = false;
        try {
            ok = cv::imwrite(filename.toStdString(), p.picture);
        } catch(...) {

        }
        qCDebug(cameraCalibrator) << "Saving" << i+1 << "of" << m_pictures.size() << ":" << ok;
        i += 1;
    }
}

void CameraCalibrator::loadPictures(const QString &fromFolder)
{
    qCDebug(cameraCalibrator) << "Loading frames from" << fromFolder;
    QDir dir(fromFolder);
    QString absPath = dir.absolutePath();
    QStringList images = dir.entryList(QStringList() << "*.jpg" << "*.JPG", QDir::Files);
    quint32 i = 0;
    foreach (const QString &image, images) {
        cv::Mat frame = cv::imread((absPath + "/" + image).toStdString());
        qCDebug(cameraCalibrator) << "Loading" << i+1 << "of" << images.size() << ":" << !frame.empty();
        findChessboard(frame);
        i += 1;
    }
}

void CameraCalibrator::saveCalibrationData(const QString &filename)
{
    cv::FileStorage fs(filename.toStdString(), cv::FileStorage::WRITE);
    fs << "intrinsic" << m_intrinsic;
    fs << "distort" << m_distCoeffs;
    fs.release();
}

void CameraCalibrator::loadCalibrationData(const QString &filename)
{
    cv::FileStorage fs(filename.toStdString(), cv::FileStorage::READ);
    fs["intrinsic"] >> m_intrinsic;
    fs["distort"] >> m_distCoeffs;
    fs.release();
}

void CameraCalibrator::findChessboard(const cv::Mat &frame)
{
    if (frame.empty()) {
        qCWarning(cameraCalibrator) << "empty frame";
    }
    cv::Mat gray = cv::Mat(frame.rows, frame.cols, CV_8UC1);
    cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
    //CVMatSurfaceSource::imshow("second", gray);

    auto board_size = cv::Size(m_verticalCornersCount, m_horizontalCornersCount);
    std::vector<cv::Point2f> corners;
    bool found = cv::findChessboardCorners(frame, board_size, corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS);

    if (found) {
        qCDebug(cameraCalibrator) << "Pattern found!";
        m_pictures.append(picture_t { frame.clone(), corners });

        cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 30, 0.1));
        drawChessboardCorners(frame, board_size, corners, found);
        CVMatSurfaceSource::imshow("second", frame);

    }
}
