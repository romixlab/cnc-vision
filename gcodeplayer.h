#ifndef GCODEPLAYER_H
#define GCODEPLAYER_H

#include <QObject>
#include <QTcpSocket>
#include "gcodeplayermodel.h"

class GcodePlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GcodePlayerModel* model READ model CONSTANT)
    Q_PROPERTY(int currentLineNumber READ currentLineNumber WRITE setCurrentLineNumber NOTIFY currentLineChanged)
    Q_PROPERTY(int linesCount READ linesCount NOTIFY linesCountChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged);
    Q_PROPERTY(ConnectionState connectionState READ connectionState NOTIFY connectionStateChanged)

public:
    explicit GcodePlayer(QObject *parent = nullptr);

    enum State {
        Stopped,
        Playing,
        Paused,
        Error
    };
    Q_ENUM(State)

    enum ConnectionState {
        Disconnected,
        Connecting,
        Connected
    };
    Q_ENUM(ConnectionState)

    static void registerQmlTypes();

    GcodePlayerModel *model() const;
    Q_INVOKABLE void loadFile(const QUrl &fileUrl);

    int currentLineNumber() const;
    void setCurrentLineNumber(int currentLineNumber);

    int linesCount() const;

    State state() const;
    ConnectionState connectionState() const;



signals:
    void currentLineChanged();
    void linesCountChanged();
    void stateChanged();
    void connectionStateChanged();
    void connectionStateChanged(bool connected);


public slots:
    void connectToMC();
    void play();
    void pause();
    void stop();
    void send(const QString &command);

private slots:
    void onSocketStateChanged(QAbstractSocket::SocketState state);
    void onSocketError(QAbstractSocket::SocketError error);
    void onMCResponse();

private:
    void sendNextLine();
    void processMCResponse(const QString &line);

    GcodePlayerModel *m_model;
    int m_currentLineNumber;
    int m_linesCount;
    State m_state;
    ConnectionState m_connectionState;
    QTcpSocket *m_tcp;
    QString m_tcpLine;
    bool m_querySent;
};

#endif // GCODEPLAYER_H
