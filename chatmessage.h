#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QObject>
#include "common.h"
#include <ndn-cxx/data.hpp>

class ChatMessage : public QObject
{
    Q_OBJECT
public:
    static ndn::Block getEncodedBlock(const std::string& msg);
    static std::string getDecodedString(ndn::Block& block);
private:
    explicit ChatMessage(QObject *parent = 0);

    template<bool T>
    static int encode(ndn::EncodingImpl<T>& encoder, const std::string& msg)
    {
        auto cstr = (uint8_t*)msg.c_str();
        int len = encoder.prependByteArrayBlock(TYPE, cstr, msg.length());
        return len;
    }

public:
    static const int TYPE = 1;
signals:

public slots:

};

#endif // CHATMESSAGE_H
