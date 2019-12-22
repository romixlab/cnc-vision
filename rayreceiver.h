#ifndef RAYRECEIVER_H
#define RAYRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>

typedef struct {
    float mcs_x;
    float mcs_y;
    float mcs_z;
    float mcs_b;
    uint32_t state;
    uint32_t played;
    int32_t total;
} ray_payload_t;

class RayReceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectionStateChanged)
public:
    explicit RayReceiver(QObject *parent = nullptr);

    enum State {
        NotPlaying = 2,
        Paused = 0,
        Playing = 3
    };

    bool connected() const;

signals:
    void stateChanged(State s);
    void coordsChanged(float x, float y, float z, float b);
    void connectionStateChanged(bool connected);

public slots:
    void setLaserPower(float pwr);
    void setTopExhaust(bool enabled);
    void setBottomExhaust(bool enabled);

private slots:
    void onReadyRead();
    void onTimerTimeout();

private:
    void processPayload(QNetworkDatagram datagram);

    QUdpSocket *m_udp;
    ray_payload_t m_payload;
    QTimer m_timer;
    bool m_connected;
    State m_lastState;
};

#endif // RAYRECEIVER_H
