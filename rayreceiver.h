#ifndef RAYRECEIVER_H
#define RAYRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>

typedef struct {
    float mcs_x;
    float mcs_y;
    float mcs_z;
    float mcs_b;
    uint8_t state;
    uint32_t played;
    uint32_t total;
} ray_payload_t;

class RayReceiver : public QObject
{
    Q_OBJECT
public:
    explicit RayReceiver(QObject *parent = nullptr);

signals:

public slots:
    void setLaserPower(float pwr);
    void setTopExhaust(bool enabled);
    void setBottomExhaust(bool enabled);

private slots:
    void onReadyRead();

private:
    void processPayload(QNetworkDatagram datagram);

    QUdpSocket *m_udp;
    ray_payload_t m_payload;
};

#endif // RAYRECEIVER_H
