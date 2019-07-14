#include "capturecontroller.hpp"

#include <QReadWriteLock>
#include <QDebug>

#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp> // for CV_CAP_PROP...

CaptureWorker::CaptureWorker(const QString &device, CaptureController *captureController, QObject *parent) :
    QObject(parent), m_device(device), m_captureController(captureController), m_loopRunning(true)
{
}

void CaptureWorker::stop()
{
    m_loopRunning = false;
}

void CaptureWorker::doWork()
{
    bool ok = false;
    int deviceId = m_device.toInt(&ok);
    if (ok) {
        m_capture = new cv::VideoCapture(deviceId);
    } else {
        m_capture = new cv::VideoCapture(m_device.toStdString());
    }
    if (!m_capture->isOpened()) {
        qWarning() << "Can't capture" << m_device;
        m_captureController->setStatus(CaptureController::Status::Failed);
        emit workDone();
        return;
    }
    bool isVideoFile = static_cast<int>(m_capture->get(CV_CAP_PROP_FRAME_COUNT)) > 0;
    unsigned long sleepBetweenFrames = 0;
    if (isVideoFile)
        sleepBetweenFrames = static_cast<unsigned long>((1.0 / m_capture->get(CV_CAP_PROP_FPS)) * 1000000);
    m_captureController->setStatus(CaptureController::Status::Started);
    while(m_loopRunning) {
        if (!m_capture->grab()) {
            m_captureController->setStatus(CaptureController::Status::EofOrDisconnected);
            break;
        }
        m_captureController->m_lock->lockForWrite();
        m_capture->retrieve(m_frame);
        if (m_frame.empty()) { // last frame of video
            m_captureController->setStatus(CaptureController::Status::EofOrDisconnected);
            break;
        }
        m_captureController->m_lock->unlock();
        emit frameReady();

        if(isVideoFile)
            QThread::usleep(sleepBetweenFrames);
    }
    delete m_capture;
    emit workDone();
}

CaptureController::CaptureController(QObject *parent) :
    QObject(parent), m_worker(nullptr), m_lock(nullptr), m_status(Status::Stopped)
{
}

CaptureController::~CaptureController()
{
    stop();
}

const cv::Mat CaptureController::frameRef() const
{
    if (!m_worker)
        return cv::Mat();
    QReadLocker lock(m_lock);
    return m_worker->m_frame;
}

cv::Mat CaptureController::frameCopy() const
{
    if (!m_worker)
        return cv::Mat();
    QReadLocker lock(m_lock);
    cv::Mat frame;
    m_worker->m_frame.copyTo(frame);
    return frame;
}

CaptureController::Status CaptureController::status() const
{
    return m_status;
}

void CaptureController::start(const QString &device)
{
    if (m_status == Status::Started)
        stop();
    else if (m_status == Status::Starting) {
        qWarning() << "start() called while capture was in Starting mode, ignoring";
        return;
    }
    setStatus(Status::Starting);
    m_lock = new QReadWriteLock;
    m_worker = new CaptureWorker(device, this, nullptr);
    m_worker->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::started, m_worker, &CaptureWorker::doWork);
//    connect(&m_workerThread, &QThread::finished, this, &CaptureController::stop);
    connect(m_worker, &CaptureWorker::frameReady, this, &CaptureController::frameReady);
    connect(m_worker, &CaptureWorker::workDone, this, &CaptureController::stop);
    m_workerThread.start();
}

void CaptureController::stop()
{
    if (m_status == Status::Stopped)
        return;
    qDebug() << "Capture is stopping from" << m_status << "state ...";

    disconnect(m_worker, &CaptureWorker::frameReady, this, &CaptureController::frameReady);
    m_worker->stop();
    m_workerThread.quit();
    if(!m_workerThread.wait(100)) {
        qWarning() << "Capture thread did not terminate until timeout, trying terminate()";
        m_workerThread.terminate();
    }
    delete m_worker;
    m_worker = nullptr;
    delete m_lock;
    m_lock = nullptr;
    setStatus(Status::Stopped);
}

void CaptureController::setStatus(CaptureController::Status status)
{
    if (status == m_status)
        return;
    m_status = status;
    emit statusChanged();
}

