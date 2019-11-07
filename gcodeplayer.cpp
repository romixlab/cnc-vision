#include <QQmlEngine>

#include <QFile>
#include <QTcpSocket>
#include <QTimer>
#include <QDebug>

#include "gcodeplayer.h"

GcodePlayer::GcodePlayer(QObject *parent) : QObject(parent)
{
    m_model = new GcodePlayerModel(this);
    m_currentLineNumber = 0;
    m_linesCount = 0;
    m_state = Stopped;

    m_tcp = new QTcpSocket(this);
    m_connectionState = Disconnected;
    emit connectionStateChanged();
    connect(m_tcp, &QTcpSocket::stateChanged,
            this,  &GcodePlayer::onSocketStateChanged);
    connect(m_tcp, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this,  &GcodePlayer::onSocketError);
    connect(m_tcp, &QIODevice::readyRead,
            this,  &GcodePlayer::onMCResponse);
    m_querySent = false;
}

void GcodePlayer::registerQmlTypes()
{
    qmlRegisterType<GcodePlayer>("tech.vhrd", 1, 0, "GcodePlayer");
}

GcodePlayerModel *GcodePlayer::model() const
{
    return m_model;
}

void GcodePlayer::loadFile(const QUrl &fileUrl)
{
    if (m_state == Playing || m_state == Paused) {
        qWarning() << "Stop first";
        return;
    }
    m_model->removeAll();

    QString fileName;
    if (fileUrl.isLocalFile())
        fileName = fileUrl.toLocalFile();
    else
        return;
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open" << fileName << f.errorString();
        return;
    }
    int lineNumber = 1;
    while (!f.atEnd()) {
        QString code = QString::fromLocal8Bit(f.readLine());
        m_model->addItem(GcodePlayerItem(GcodePlayerItem::Pending, lineNumber, code));

        lineNumber++;
    }

    m_currentLineNumber = 1;
    emit currentLineChanged();
    m_linesCount = lineNumber - 1;
    emit linesCountChanged();
    m_state = Stopped;
    emit stateChanged();
}

int GcodePlayer::currentLineNumber() const
{
    return m_currentLineNumber;
}

void GcodePlayer::setCurrentLineNumber(int currentLineNumber)
{
    m_currentLineNumber = currentLineNumber;
}

int GcodePlayer::linesCount() const
{
    return m_linesCount;
}

GcodePlayer::State GcodePlayer::state() const
{
    return m_state;
}

GcodePlayer::ConnectionState GcodePlayer::connectionState() const
{
    return m_connectionState;
}

void GcodePlayer::connectToMC()
{
    if (m_connectionState == Disconnected) {
        m_tcp->connectToHost("10.0.1.1", 5555);
        QTimer::singleShot(2000, [=](){
            if (this->m_connectionState != Connected)
                this->m_tcp->abort();
        });
    }
}

void GcodePlayer::play()
{
    if (m_state == Stopped) {
        if (m_linesCount > 0) {
            m_currentLineNumber = 1;
            emit currentLineChanged();
            m_model->changeAllStates(GcodePlayerItem::Pending);
            sendNextLine();
            m_state = Playing;
            emit stateChanged();
        } else {
            qWarning() << "Nothing to play";
        }
    } else if (m_state == Paused) {
        sendNextLine();
        m_state = Playing;
        emit stateChanged();
    } else {
        qWarning() << "Can't play from state" << m_state;
    }
}

void GcodePlayer::pause()
{
    m_state = Paused;
    emit stateChanged();
}

void GcodePlayer::stop()
{
    m_state = Stopped;
    emit stateChanged();
}

void GcodePlayer::onSocketStateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::ConnectedState) {
        m_connectionState = Connected;
    } else if (state == QAbstractSocket::UnconnectedState) {
        m_connectionState = Disconnected;
    } else {
        m_connectionState = Connecting;
    }
    emit connectionStateChanged();
}

void GcodePlayer::onSocketError(QAbstractSocket::SocketError error)
{

    switch (error) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
//            QMessageBox::information(this, tr("Fortune Client"),
//                                     tr("The host was not found. Please check the "
//                                        "host name and port settings."));
            break;
        case QAbstractSocket::ConnectionRefusedError:
//            QMessageBox::information(this, tr("Fortune Client"),
//                                     tr("The connection was refused by the peer. "
//                                        "Make sure the fortune server is running, "
//                                        "and check that the host name and port "
//                                        "settings are correct."));
            break;
        default:
//            QMessageBox::information(this, tr("Fortune Client"),
//                                     tr("The following error occurred: %1.")
//                                     .arg(tcpSocket->errorString()));
        break;
        }

}

void GcodePlayer::onMCResponse()
{
    QByteArray bytes = m_tcp->readAll();
    for (int i = 0; i < bytes.length(); ++i) {
        if (bytes[i] == '\r')
            continue;
        if (bytes[i] == '\n') {
            if (!m_tcpLine.isEmpty()) {
                processMCResponse(m_tcpLine);
                m_tcpLine.clear();
            }
            continue;
        }
        m_tcpLine.append(QChar(bytes[i]));
    }
}

void GcodePlayer::sendNextLine()
{
    GcodePlayerItem line = m_model->getItem(m_currentLineNumber - 1);
    m_tcp->write(line.m_code.toLocal8Bit());
    m_querySent = true;
}

void GcodePlayer::processMCResponse(const QString &line)
{
    if (m_querySent) {
        qDebug() << "mc q:" << line;
        m_querySent = false;

        GcodePlayerItem item = m_model->getItem(m_currentLineNumber - 1);
        if (line == "ok") {
            item.m_status = GcodePlayerItem::Ok;
            m_model->replaceItem(m_currentLineNumber - 1, item);
        } else {
            item.m_response = line;
            item.m_status = GcodePlayerItem::Warning;
            m_model->replaceItem(m_currentLineNumber - 1, item);
        }
        m_currentLineNumber++;
        if (m_currentLineNumber > m_linesCount) {
            m_state = Stopped;
            emit stateChanged();
        } else {
            emit currentLineChanged();
            if (m_state == Playing)
                sendNextLine();
        }
    } else {
        qDebug() << "mc:" << line;
    }
}
