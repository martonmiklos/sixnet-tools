#include "sixnetmessagehandler.h"

SixnetMessageDispatcher::SixnetMessageDispatcher(QHostAddress address, int port, int timeout, QObject *parent) :
    QObject(parent),
    m_sentMessage(NULL),
    m_sequence(0),
    m_address(address),
    m_port(port),
    m_connected(false)
{
    m_socket = new QUdpSocket(this);

    m_socket->bind(QHostAddress::LocalHost, port);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));

    m_socket->connectToHost(address, port);
    m_timer.setInterval(timeout);
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void SixnetMessageDispatcher::connectToHost()
{
    if (m_socket->state() == QUdpSocket::UnconnectedState) {
        m_socket->connectToHost(m_address, m_port);
    }
}

void SixnetMessageDispatcher::disconnectFromHost()
{
    if (m_socket->state() != QUdpSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

void SixnetMessageDispatcher::sendMessage(SixnetMessage *msg)
{
    m_waitingMessages.append(msg);
    sendNextMessageFromWaiting();
}

void SixnetMessageDispatcher::sendNextMessageFromWaiting()
{
    if ((m_sentMessage != NULL) || (m_waitingMessages.size() == 0))
        return;

    m_sentMessage = m_waitingMessages.takeFirst();
    m_sentMessage->setSequence(m_sequence);
    connect(m_sentMessage, SIGNAL(debug(QString)), this, SIGNAL(debug(QString)));

    emit debug(tr("OUT %1").arg(QString(m_sentMessage->serialize().toHex())));

    m_socket->write(m_sentMessage->serialize());
    m_timer.start();

    m_sequence++;
}

void SixnetMessageDispatcher::socketStateChanged(QAbstractSocket::SocketState newState)
{
    m_connected = (newState == QUdpSocket::ConnectedState);
    if (newState == QUdpSocket::ConnectedState) {
        sendNextMessageFromWaiting();
    }
}

void SixnetMessageDispatcher::socketReadyRead()
{
    if (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        m_timer.stop();
        m_socket->readDatagram(datagram.data(), datagram.size());
        if (m_sentMessage != NULL) {
            if(!m_sentMessage->checkAnswer(datagram))
                emit debug(tr("Error: %1").arg(m_sentMessage->error()));
            m_sentMessage = NULL;
        }
    }
}

void SixnetMessageDispatcher::timeout()
{
    if (m_sentMessage != NULL) {
        emit debug(tr("timeout"));
        m_sentMessage->deleteLater();
        m_sentMessage = NULL;
    }
    sendNextMessageFromWaiting();
}
