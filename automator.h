#ifndef AUTOMATOR_H
#define AUTOMATOR_H

#include <QObject>
#include "rayreceiver.h"
#include <QTimer>

class Automator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool working READ working NOTIFY workingChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(bool autosendB READ autosendB WRITE setAutosendB)
    Q_PROPERTY(bool autosendPower READ autosendPower WRITE setAutosendPower)
    Q_PROPERTY(float minPower READ minPower WRITE setMinPower)
    Q_PROPERTY(float maxPower READ maxPower WRITE setMaxPower)
    Q_PROPERTY(float lastSentPower READ lastSentPower NOTIFY changePower)
public:
    explicit Automator(QObject *parent = nullptr);

    bool working() const;

    bool enabled() const;
    void setEnabled(bool enabled);

    QString message() const;
    Q_INVOKABLE float compensate(float dz) const;

    bool autosendB() const;
    void setAutosendB(bool autosendB);

    bool autosendPower() const;
    void setAutosendPower(bool autosendPower);

    float maxPower() const;
    void setMaxPower(float maxPower);

    float minPower() const;
    void setMinPower(float minPower);

    float lastSentPower() const;

signals:
    void workingChanged();
    void enabledChanged();
    void messageChanged();
    void sendToMC(const QString &command);
    void changePower(float power);

public slots:
    void ondzChanged(float dz);
    void ondzValidChanged(bool valid);
    void onMcConnectionStateChanged(bool connected);
    void onRayConnectionStateChanged(bool connected);
    void onCoordsChanged(float x, float y, float z, float b);
    void onMcStateChanged(RayReceiver::State s);

private:
    void checkWorkingState();
    bool m_working;
    float m_lastdz;
    bool m_lastdzValid;
    bool m_lastCoordsValid;
    bool m_enabled;
    bool m_mcConnected;
    float m_mcs_x;
    float m_mcs_y;
    float m_mcs_b;
    float m_mcs_b_initial;
    QString m_message;
    bool m_autosendB;
    bool m_autosendPower;
    float m_maxPower;
    float m_minPower;
    float m_lastSentPower;
    QTimer m_powerTimer;
};

#endif // AUTOMATOR_H
