#include "chatmessage.h"

ChatMessage::ChatMessage(QObject *parent) :
    QObject(parent)
{
}

ndn::Block ChatMessage::getEncodedBlock(const std::string& msg)
{
    ndn::EncodingEstimator estimator;
    int len = ChatMessage::encode(estimator, msg);
    ndn::EncodingBuffer buf(len, 0);
    ChatMessage::encode(buf, msg);

    return buf.block();
}

std::string ChatMessage::getDecodedString(ndn::Block& block)
{
    block.parse();
    auto i = block.elements_begin();
    std::string msg((char*)i->value(), i->value_size());
    return msg;
}
