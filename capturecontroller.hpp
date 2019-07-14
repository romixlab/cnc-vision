#ifndef CAPTURECONTROLLER_HPP
#define CAPTURECONTROLLER_HPP

#include <QObject>
#include <QThread>
#include <opencv2/core.hpp>

namespace cv {
class VideoCapture;
}

class QReadWriteLock;
class CaptureController;
class CaptureWorker : public QObject
{
    Q_OBJECT
public:
    explicit CaptureWorker(const QString &device, CaptureController *captureController, QObject *parent = nullptr);

    /**
     * @brief stop Stops capture and exits doWork() loop
     */
    void stop();
signals:
    void frameReady();
    void workDone();

public slots:
    void doWork();

private:
    friend class CaptureController;
    QString m_device;
    cv::VideoCapture *m_capture;
    CaptureController *m_captureController;
    cv::Mat m_frame;
    bool m_loopRunning;
};

class CaptureController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
public:
    explicit CaptureController(QObject *parent = nullptr);
    ~CaptureController();

    const cv::Mat frameRef() const;
    cv::Mat frameCopy() const;

    /**
     * @brief The Status enum
     */
    enum class Status {
        Starting,          ///< Capture is in startup
        Started,           ///< Capture successfully started and frameReady() signals is emitting
        Stopped,           ///< Initial state or after @see stop() was called
        EofOrDisconnected, ///< Video file ended or camera was disconnected
        Failed             ///< After Starting if it wasn't successful
    };
    Q_ENUM(Status)
    Status status() const;

public slots:
    void start(const QString &device);
    void stop();
signals:
    void statusChanged();
    void frameReady();
    //void statisticsReady(QVariant ?

private:
    friend class CaptureWorker;
    CaptureWorker* m_worker;
    QThread m_workerThread;
    mutable QReadWriteLock* m_lock;

    void setStatus(Status status);
    Status m_status;
};

#endif // CAPTURECONTROLLER_HPP
