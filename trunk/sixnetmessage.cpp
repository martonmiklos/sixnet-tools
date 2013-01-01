#include "sixnetmessage.h"

SixnetMessage::SixnetMessage(unsigned char dstAddress,
                                         SixnetMessageType command,
                                         unsigned int size,
                                         QObject *parent) :
    QObject(parent),
    m_dstAddress(dstAddress),
    m_session(0x0C),
    m_command(command),
    m_size(size)
{
}

QByteArray SixnetMessage::serialize()
{
    unsigned char message[14] = {
        '}',                                // fixed CRC message
        0x0C,                               // size: 14 bytes
        (unsigned char) m_dstAddress,       // destination address
        0xF0,                               // our address always 0xF0
        (unsigned char) m_session,          // session
        (unsigned char) m_sequence,         // sequence
        (unsigned char) (m_command >> 8),   // cmd H
        (unsigned char) m_command,          // cmd L
        (unsigned char) (m_size >> 24),     // size
        (unsigned char) (m_size >> 16),     // size
        (unsigned char) (m_size >> 8),      // size
        (unsigned char) m_size,             // size
        0x1D,
        0x0F
    };

    QByteArray ret((const char*)message, 14);
    return ret;
}

int SixnetMessage::channelData(unsigned int channel)
{
    int ret = 0;
    if (channel < m_size) {
        switch (m_command) {
        case SixnetMessageReadAnalog:
            ret = (unsigned char)m_receievedData.at(channel*2+1);
            ret += ((unsigned char)m_receievedData.at(channel*2)) << 8;
            break;
        case SixnetMessageReadDigital:
            ret = (m_receievedData.at(channel/8) & (1<<(channel-((unsigned int)(channel/8)*8))))>0?1:0;
            break;
        }
    }
    return ret;
}

bool SixnetMessage::checkAnswer(QByteArray data)
{
    emit debug(tr("IN %1").arg(QString(data.toHex())));
    if (data.size() >= 8) { // NACK answers have only one byte command + msg body
        if ((unsigned char) data.at(0) != '}') {
            m_error = tr("Wrong lead character: %1 instead of 0x7D").arg(data.at(0));
            return false;
        }

        if ((unsigned char)data.size() != (data.at(1) + 2)) {
            m_error = tr("Truncated message! Size: %1 instead of %2").arg(data.size()).arg(data.at(1));
            return false;
        }

        if ((unsigned char)data.at(2) != 0xF0) {
            m_error = tr("Destination is not 0xF0 but %1").arg(data.at(2));
            return false;
        }

        if ((unsigned char)data.at(4) != m_session) {
            m_error = tr("Invalid session: %1").arg(data.at(3));
            return false;
        }

        if ((unsigned char)data.at(5) != m_sequence) {
            m_error = tr("Sequence mismatch: %1 awaited: %2").arg(data.at(4)).arg(m_sequence);
            return false;
        }

        if ((unsigned char)data.at(6) != 1) { // we got ACK
            return false;
        }

        unsigned int receievedDataSize = data.at(11);
        receievedDataSize += (data.at(10) << 8);
        receievedDataSize += (data.at(9) << 16);
        receievedDataSize += (data.at(8) << 24);

        switch (m_command) {
        case SixnetMessageReadDigital:
            m_receievedData = data.mid(12, receievedDataSize/8);
            break;
        case SixnetMessageReadAnalog:
            m_receievedData = data.mid(12, receievedDataSize*2);
            break;
        }

        emit debug(tr("DATA (%1): %2").arg(receievedDataSize).arg(QString(m_receievedData.toHex())));
        emit dataReaded();
    }

    return true;
}
