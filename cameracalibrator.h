#ifndef CAMERACALIBRATOR_H
#define CAMERACALIBRATOR_H

#include <opencv2/core.hpp>

#include <QObject>
#include <QLoggingCategory>

class CaptureController;

class CameraCalibrator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 horizontalCornersCount READ horizontalCornersCount WRITE setHorizontalCornersCount)
    Q_PROPERTY(quint32 verticalCornersCount READ verticalCornersCount WRITE setVerticalCornersCount)
public:
    explicit CameraCalibrator(CaptureController *captureController, QObject *parent = nullptr);

    quint32 horizontalCornersCount() const;
    void setHorizontalCornersCount(quint32 count);
    quint32 verticalCornersCount() const;
    void setVerticalCornersCount(quint32 count);

signals:

public slots:
    void takePicture();
    void onFrameReady();
    void calibrate();
    void applyCalibrationData();
    void savePictures(const QString &toFolder);
    void loadPictures(const QString &fromFolder);
    void saveCalibrationData(const QString &filename);
    void loadCalibrationData(const QString &filename);

private:
    void findChessboard(const cv::Mat &frame);

    struct picture_t {
        cv::Mat picture;
        std::vector<cv::Point2f> corners;
    };
    QList<picture_t> m_pictures;
    bool m_takePicture;
    CaptureController *m_captureController;

    quint32 m_horizontalCornersCount;
    quint32 m_verticalCornersCount;

    cv::Mat m_intrinsic;
    cv::Mat m_distCoeffs;
};

Q_DECLARE_LOGGING_CATEGORY(cameraCalibrator)

#endif // CAMERACALIBRATOR_H
