#ifndef SIXNETMESSAGE_H
#define SIXNETMESSAGE_H

#include <QObject>
#include <QDebug>

class SixnetMessage : public QObject
{
    Q_OBJECT
public:
    enum SixnetMessageType {
        SixnetMessageReadAnalog = 0x0C80,
        SixnetMessageReadDigital = 0x0A80
    };

    SixnetMessage(unsigned char dstAddress, SixnetMessageType command, unsigned int size, QObject *parent = 0);

    bool checkAnswer(QByteArray data);
    QByteArray serialize();
    int channelCount() const {return m_size;}

    int dstAddress() const {return m_dstAddress;}
    void setSequence(int seq) {m_sequence = seq;}

    int channelData(unsigned int channel);
    QString error() const {return m_error;}

    SixnetMessageType messageType() const {return m_command;}

private:
    QByteArray m_receievedData;
    unsigned char m_dstAddress, m_sequence, m_session;
    SixnetMessageType m_command;
    unsigned int m_size;
    QByteArray m_data;
    QString m_error;


signals:
    void dataReaded();
    void debug(QString);
};

#endif // SIXNETMESSAGE_H
