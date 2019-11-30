#ifndef AUTOMATOR_H
#define AUTOMATOR_H

#include <QObject>
#include "rayreceiver.h"

class Automator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool working READ working NOTIFY workingChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
public:
    explicit Automator(QObject *parent = nullptr);

    bool working() const;

    bool enabled() const;
    void setEnabled(bool enabled);

    QString message() const;

signals:
    void workingChanged();
    void enabledChanged();
    void messageChanged();

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
    QString m_message;
};

#endif // AUTOMATOR_H
