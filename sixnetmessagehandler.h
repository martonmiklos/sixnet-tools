#ifndef SIXNETMESSAGEHANDLER_H
#define SIXNETMESSAGEHANDLER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

#include "sixnetmessage.h"

class SixnetMessageDispatcher : public QObject
{
    Q_OBJECT
public:
    SixnetMessageDispatcher(QHostAddress address, int port, int timeout = 1500, QObject *parent = 0);
    void connectToHost();
    void disconnectFromHost();

    void sendMessage(SixnetMessage *msg);
    void setHostAddress(QHostAddress address) {m_address = address;}

    bool connected() const {return m_connected;}

private:
    QList<SixnetMessage*> m_waitingMessages;
    SixnetMessage *m_sentMessage;

    void sendNextMessageFromWaiting();

    int m_sequence;

    QUdpSocket *m_socket;
    QHostAddress m_address;
    QTimer m_timer;

    unsigned int m_port;
    bool m_connected;

signals:
    void debug(QString);

public slots:
private slots:
    void socketStateChanged(QAbstractSocket::SocketState newState);
    void socketReadyRead();
    void timeout();
};

#endif // SIXNETMESSAGEHANDLER_H
