#include "chatmessage.h"
#include <QByteArray>

ChatMessage::ChatMessage(QObject *parent) :
    QObject(parent)
{
}

ndn::Block ChatMessage::getEncodedBlock(const QByteArray& msg)
{
    ndn::EncodingEstimator estimator;
    int len = ChatMessage::encode(estimator, msg);
    ndn::EncodingBuffer buf(len, 0);
    ChatMessage::encode(buf, msg);

    return buf.block();
}

QByteArray ChatMessage::getDecodedString(ndn::Block& block)
{
    block.parse();
    auto i = block.elements_begin();
    QByteArray msg((char*)i->value(), i->value_size());
    return msg;
}
