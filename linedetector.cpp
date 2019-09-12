#include <opencv2/opencv.hpp>

#include <QTimer>

#include "linedetector.h"
#include "capturecontroller.hpp"
#include "cvmatsurfacesource.hpp"

Q_LOGGING_CATEGORY(lineDetector, "vhrd.vision.linedetector")

LineDetector::LineDetector(CaptureController *captureController, QObject *parent) : QObject(parent),
    m_captureController(captureController)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(2000);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout,
            this,    &LineDetector::onTimeout);

    m_state = Unlocked;

    m_dz = 0;
    m_zerodxs = false;
    m_dxs0 = 0;
    m_ppmm = 348.432;
    m_s0 = 124;
    m_f = 3.6;
    m_lz = 130;
    m_lx = 180;

    m_angle = 0;

    m_hueLowRangeFrom = 0;
    m_hueLowRangeTo = 10;
    m_hueHighRangeFrom = 160;
    m_hueHighRangeTo = 179;
    m_saturationFrom = 10;
    m_saturationTo = 255;
    m_valueFrom = 10;
    m_valueTo = 255;
    m_integrateFrom = 0.2;
    m_integrateTo = 0.8;
    m_threshold = 0.6;
}

LineDetector::State LineDetector::state() const
{
    return m_state;
}

void LineDetector::onFrameReady()
{
    cv::Mat frame = m_captureController->frameCopy();

    cv::Mat rm = cv::getRotationMatrix2D(cv::Point(frame.cols / 2, frame.rows / 2), m_angle, 1.0);
    cv::Mat rotated;
    cv::warpAffine(frame, rotated, rm, cv::Size(frame.cols, frame.rows));
    frame = rotated;

    // Convert to HSV, filter
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::Mat upper_red_hue;
    cv::Mat lower_red_hue;
    cv::inRange(hsv,
                cv::Scalar(m_hueLowRangeFrom, m_saturationFrom, m_valueFrom),
                cv::Scalar(m_hueLowRangeTo, m_saturationTo, m_valueTo),
                lower_red_hue);
    cv::inRange(hsv,
                cv::Scalar(m_hueHighRangeFrom, m_saturationFrom, m_valueFrom),
                cv::Scalar(m_hueHighRangeTo, m_saturationTo, m_valueTo),
                upper_red_hue);
    cv::Mat red_hue;
    cv::addWeighted(lower_red_hue, 1.0, upper_red_hue, 1.0, 0.0, red_hue);

    // Mask V channel
    std::vector<cv::Mat> hsvSplitted;
    cv::split(hsv, hsvSplitted);
    cv::Mat masked;
    hsvSplitted[2].copyTo(masked, red_hue);

    // Integration limits
    quint16 colfrom = m_integrateFrom * frame.cols;
    quint16 colto = m_integrateTo * frame.cols;

    // Show as red channel
    cv::Mat desaturateMask(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
    cv::rectangle(desaturateMask, cv::Point(0, 0), cv::Point(colfrom, frame.rows), cv::Scalar(255), -1);
    cv::rectangle(desaturateMask, cv::Point(colto, 0), cv::Point(frame.cols, frame.rows), cv::Scalar(255), -1);
    cv::subtract(masked, cv::Scalar(127), masked, desaturateMask);

    cv::Mat zero(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
    std::vector<cv::Mat> channels;
    channels.push_back(zero);
    channels.push_back(zero);
    channels.push_back(masked);
    cv::Mat merged;
    cv::merge(channels, merged);
    CVMatSurfaceSource::imshow("second", merged);

    // Integrate
    QVector<float> linesSum;
    quint8 *mdata = static_cast<quint8 *>(masked.data);
    for (quint16 row = frame.rows - 1; row > 0; row--) {
        float s = 0;
        for (quint16 col = colfrom; col < colto; col++) {
            s += mdata[masked.step * row + col];
        }
        linesSum.push_back(s);
    }
    float max = 0;
    float thresholdAbs = m_threshold * frame.cols * 255;
    QVector<int> overThreshold;
    for (int i = 0; i < linesSum.size(); i++) {
        if (linesSum[i] > max) {
            max = linesSum[i];
        }
        if (linesSum[i] > thresholdAbs) {
            overThreshold.push_back(i);
        }
    }
    for (int i = 0; i < linesSum.size(); i++) {
        linesSum[i] = linesSum[i] / max;
    }
    emit integrationComplete(linesSum);

    // Find thicknes and center of a light beam
    if (overThreshold.count() >= 2) {
        int x1 = overThreshold.last();
        QPointF pt1((float)x1 / float(frame.rows), linesSum[x1]);
        int x2 = overThreshold.first();
        QPointF pt2((float)x2 / float(frame.rows), pt1.y());
        emit lineDetected(pt1, pt2);

        // Find dz
        float dxs = x1 + (float)(x2 - x1) / 2.0f;
        if (m_zerodxs) {
            m_dxs0 = dxs;
            m_zerodxs = false;
        }
        dxs -= m_dxs0;
        //float M = m_f / (m_s0 - m_f);
        //float dx = dxs / (M * m_ppmm);
        m_dz = (m_lz * dxs * (m_s0 - m_f) ) / (m_lx * m_f * m_ppmm - m_lz * dxs);
        emit dzChanged();

        if (m_state != Locked) {
            m_state = Locked;
            m_timer->stop();
            emit stateChanged();
        }
    } else {
        if (m_state == Locked) {
            m_state = Hold;
            m_timer->start();
            emit stateChanged();
        }
    }
}

void LineDetector::onTimeout()
{
    m_state = Unlocked;
    emit stateChanged();
}

float LineDetector::rotation() const
{
    return m_angle;
}

void LineDetector::setRotation(float angle)
{
    m_angle = angle;
}

float LineDetector::threshold() const
{
    return m_threshold;
}

void LineDetector::setThreshold(float threshold)
{
    m_threshold = threshold;
}

float LineDetector::dz() const
{
    return m_dz;
}

void LineDetector::zerodxs()
{
    m_zerodxs = true;
}

float LineDetector::integrateTo() const
{
    return m_integrateTo;
}

void LineDetector::setIntegrateTo(float integrateTo)
{
    m_integrateTo = integrateTo;
}

float LineDetector::integrateFrom() const
{
    return m_integrateFrom;
}

void LineDetector::setIntegrateFrom(float integrateFrom)
{
    m_integrateFrom = integrateFrom;
}

quint8 LineDetector::valueTo() const
{
    return m_valueTo;
}

void LineDetector::setValueTo(const quint8 &valueTo)
{
    m_valueTo = valueTo;
}

quint8 LineDetector::valueFrom() const
{
    return m_valueFrom;
}

void LineDetector::setValueFrom(const quint8 &valueFrom)
{
    m_valueFrom = valueFrom;
}

quint8 LineDetector::saturationTo() const
{
    return m_saturationTo;
}

void LineDetector::setSaturationTo(const quint8 &saturationTo)
{
    m_saturationTo = saturationTo;
}

quint8 LineDetector::saturationFrom() const
{
    return m_saturationFrom;
}

void LineDetector::setSaturationFrom(const quint8 &saturationFrom)
{
    m_saturationFrom = saturationFrom;
}

quint8 LineDetector::hueHighRangeTo() const
{
    return m_hueHighRangeTo;
}

void LineDetector::setHueHighRangeTo(const quint8 &hueHighRangeTo)
{
    if (hueHighRangeTo > 179)
        m_hueLowRangeTo = 179;
    else
        m_hueHighRangeTo = hueHighRangeTo;
}

quint8 LineDetector::hueHighRangeFrom() const
{
    return m_hueHighRangeFrom;
}

void LineDetector::setHueHighRangeFrom(const quint8 &hueHighRangeFrom)
{
    m_hueHighRangeFrom = hueHighRangeFrom;
}

quint8 LineDetector::hueLowRangeTo() const
{
    return m_hueLowRangeTo;
}

void LineDetector::setHueLowRangeTo(const quint8 &hueLowRangeTo)
{
    if (hueLowRangeTo > 179)
        m_hueLowRangeTo = 179;
    else
        m_hueLowRangeTo = hueLowRangeTo;
}

quint8 LineDetector::hueLowRangeFrom() const
{
    return m_hueLowRangeFrom;
}

void LineDetector::setHueLowRangeFrom(const quint8 &hueLowRangeFrom)
{
    m_hueLowRangeFrom = hueLowRangeFrom;
}
