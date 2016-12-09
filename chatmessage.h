#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QObject>
#include "common.h"
#include <ndn-cxx/data.hpp>

class ChatMessage : public QObject
{
    Q_OBJECT
public:
    static ndn::Block getEncodedBlock(const QByteArray& msg);
    static QByteArray getDecodedString(ndn::Block& block);
private:
    explicit ChatMessage(QObject *parent = 0);

    template<bool T>
    static int encode(ndn::EncodingImpl<T>& encoder, QByteArray& msg)
    {
        int len = encoder.prependByteArrayBlock(TYPE, (uint8_t*)msg.data(), msg.length());
        return len;
    }

public:
    static const int TYPE = 1;
signals:

public slots:

};

#endif // CHATMESSAGE_H
