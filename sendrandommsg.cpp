#include "sendrandommsg.h"

SendRandomMsg::SendRandomMsg(Chatroom* cd, int second, QString msg, QObject* parent):
    QObject(parent),
    m_msg(msg),
    m_chatroom(cd)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()),this,SLOT(onMyTimer()));
    m_timer->start(second * 1000);
}
SendRandomMsg::~SendRandomMsg()
{
    m_timer->stop();
    delete m_timer;
}

void SendRandomMsg::onMyTimer()
{
    uint32_t r = ndn::random::generateWord32();
    std::stringstream ss;
    {
        using namespace CryptoPP;
        StringSource(reinterpret_cast<uint8_t*>(&r), 4, true,
                     new HexEncoder(new FileSink(ss), false));

    }
    if (m_msg.length() == 0)
    {
        std::string msg = ss.str();
        m_chatroom->sendMessage(msg);
    }
    else
    {
        std::string msg = m_msg.toStdString();
        m_chatroom->sendMessage(msg);
    }
}
