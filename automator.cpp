#include "automator.h"

Automator::Automator(QObject *parent) : QObject(parent)
{
    m_working = false;
    m_enabled = false;
    m_mcConnected = false;
    m_lastdzValid = false;
    m_lastCoordsValid = false;
    m_message = "Waiting for pause";
}

bool Automator::working() const
{
    return m_working;
}

void Automator::ondzChanged(float dz)
{
    m_lastdz = dz;
}

void Automator::ondzValidChanged(bool valid)
{
    m_lastdzValid = valid;
    checkWorkingState();
}

void Automator::onMcConnectionStateChanged(bool connected)
{
    m_mcConnected = connected;
    checkWorkingState();
}

void Automator::onRayConnectionStateChanged(bool connected)
{
    m_lastCoordsValid = connected;
}

void Automator::onCoordsChanged(float x, float y, float z, float b)
{
    Q_UNUSED(z);
    m_mcs_x = x;
    m_mcs_y = y;
    m_mcs_b = b;
}

void Automator::onMcStateChanged(RayReceiver::State s)
{
    if (!m_working)
        return;
    if (s == RayReceiver::Paused) {
        float targetB = m_mcs_b - m_lastdz;
        QString correction = QString("G90 G0 B%1").arg(targetB);
        m_message = correction;
        emit messageChanged();
    }
}

void Automator::checkWorkingState()
{
    bool working = m_lastdzValid && m_mcConnected && m_lastCoordsValid && m_enabled;
    if (working != m_working) {
        m_working = working;
        emit workingChanged();
    }
}

bool Automator::enabled() const
{
    return m_enabled;
}

void Automator::setEnabled(bool enabled)
{
    m_enabled = enabled;
    emit enabledChanged();
}

QString Automator::message() const
{
    return m_message;
}
