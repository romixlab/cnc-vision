#ifndef LINEDETECTOR_H
#define LINEDETECTOR_H

#include <QObject>
#include <QLoggingCategory>

class CaptureController;
class QTimer;

class LineDetector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint8 hueLowRangeFrom READ hueLowRangeFrom WRITE setHueLowRangeFrom NOTIFY hsvThresholdsChanged)
    Q_PROPERTY(quint8 hueLowRangeTo READ hueLowRangeTo WRITE setHueLowRangeTo NOTIFY hsvThresholdsChanged)
    Q_PROPERTY(quint8 hueHighRangeFrom READ hueHighRangeFrom WRITE setHueHighRangeFrom NOTIFY hsvThresholdsChanged)
    Q_PROPERTY(quint8 hueHighRangeTo READ hueHighRangeTo WRITE setHueHighRangeTo NOTIFY hsvThresholdsChanged)
    Q_PROPERTY(quint8 saturationFrom READ saturationFrom WRITE setSaturationFrom NOTIFY hsvThresholdsChanged)
    Q_PROPERTY(quint8 saturationTo READ saturationTo WRITE setSaturationTo NOTIFY hsvThresholdsChanged)
    Q_PROPERTY(quint8 valueFrom READ valueFrom WRITE setValueFrom NOTIFY hsvThresholdsChanged)
    Q_PROPERTY(quint8 valueTo READ valueTo WRITE setValueTo NOTIFY hsvThresholdsChanged)
    Q_PROPERTY(float integrateFrom READ integrateFrom WRITE setIntegrateFrom NOTIFY integrationLimitsChanged)
    Q_PROPERTY(float integrateTo READ integrateTo WRITE setIntegrateTo NOTIFY integrationLimitsChanged)
    Q_PROPERTY(float threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(float dz READ dz NOTIFY dzChanged)
    Q_PROPERTY(float rotation READ rotation WRITE setRotation)
public:
    explicit LineDetector(CaptureController *captureController, QObject *parent = nullptr);

    enum State {
        Unlocked,
        Locked,
        Hold
    };
    Q_ENUM(State)
    State state() const;

    quint8 hueLowRangeFrom() const;
    void setHueLowRangeFrom(const quint8 &hueLowRangeFrom);

    quint8 hueLowRangeTo() const;
    void setHueLowRangeTo(const quint8 &hueLowRangeTo);

    quint8 hueHighRangeFrom() const;
    void setHueHighRangeFrom(const quint8 &hueHighRangeFrom);

    quint8 hueHighRangeTo() const;
    void setHueHighRangeTo(const quint8 &hueHighRangeTo);

    quint8 saturationFrom() const;
    void setSaturationFrom(const quint8 &saturationFrom);

    quint8 saturationTo() const;
    void setSaturationTo(const quint8 &saturationTo);

    quint8 valueFrom() const;
    void setValueFrom(const quint8 &valueFrom);

    quint8 valueTo() const;
    void setValueTo(const quint8 &valueTo);

    float integrateFrom() const;
    void setIntegrateFrom(float integrateFrom);

    float integrateTo() const;
    void setIntegrateTo(float integrateTo);

    float threshold() const;
    void setThreshold(float threshold);

    float dz() const;

    Q_INVOKABLE void zerodxs();

    float rotation() const;
    void setRotation(float angle);

signals:
    void hsvThresholdsChanged();
    void integrationLimitsChanged();
    void thresholdChanged();
    void integrationComplete(const QVector<float> &data);
    void lineDetected(const QPointF &pt1, const QPointF &pt2);
    void stateChanged();
    void dzChanged();

public slots:
    void onFrameReady();

private slots:
    void onTimeout();

private:
    CaptureController *m_captureController;
    quint8 m_hueLowRangeFrom;
    quint8 m_hueLowRangeTo;
    quint8 m_hueHighRangeFrom;
    quint8 m_hueHighRangeTo;
    quint8 m_saturationFrom;
    quint8 m_saturationTo;
    quint8 m_valueFrom;
    quint8 m_valueTo;
    float m_integrateFrom;
    float m_integrateTo;
    float m_threshold;
    QTimer *m_timer;
    State m_state;
    float m_dz;
    bool m_zerodxs;
    float m_dxs0;
    float m_ppmm;
    float m_s0;
    float m_f;
    float m_lz;
    float m_lx;
    float m_angle;
};

Q_DECLARE_LOGGING_CATEGORY(lineDetector)

#endif // LINEDETECTOR_H
