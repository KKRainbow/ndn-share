#include "chatroom.h"
#include <QStringList>

std::shared_ptr<ChatroomBackend> Chatroom::m_backend;

Chatroom::Chatroom(QString chatroomName,
                   QObject *parent):
    QObject(parent),
    m_chatroomName(chatroomName),
    m_msgQueueSema()
{
}

void Chatroom::sendMessage(std::string& msg)
{
    emit sendMessageSignal(m_chatroomName, QString::fromStdString(msg));
}

void Chatroom::emitAddChatroomSignal()
{
    emit addChatroomSignal(m_chatroomName);
}

std::shared_ptr<Chatroom> Chatroom::getChatroom(QString chatroomName,
                                                       ndn::Name routePrefix,
                                                       ndn::Name broadcastPrefix)
{
    if (!m_backend)
    {
        m_backend = std::make_shared<ChatroomBackend>(routePrefix, broadcastPrefix);
        m_backend->start();
    }
    auto frontend = std::make_shared<Chatroom>(chatroomName);

    connect(m_backend.get(), SIGNAL(fetchMessage(QString,QString)),
            frontend.get(), SLOT(fetchMessageSlot(QString,QString)));

    connect(frontend.get(), SIGNAL(sendMessageSignal(QString,QString)),
            m_backend.get(), SLOT(sendMessage(QString,QString)));
    connect(frontend.get(), SIGNAL(addChatroomSignal(QString)),
            m_backend.get(), SLOT(addChatroom(QString)));

    frontend->emitAddChatroomSignal();

    return frontend;
}

void Chatroom::fetchMessageSlot(QString chatroomName, QString msg)
{
    if (chatroomName != m_chatroomName)
    {
        //not this room
        return;
    }
    m_msgQueueMutex.lock();
    m_messageQueue.push(msg);
    if (m_messageQueue.size() == 1)
    {
        m_msgQueueSema.release();
    }
    std::cerr << "get msg from " << chatroomName.toStdString() << msg.toStdString() <<std::endl;
    m_msgQueueMutex.unlock();
}
QString Chatroom::getOneMessage()
{
    m_msgQueueMutex.lock();
    if (m_messageQueue.empty())
    {
        m_msgQueueMutex.unlock();
        m_msgQueueSema.acquire();
    }
    else
    {
        m_msgQueueMutex.unlock();
    }
}

QStringList Chatroom::getMessages(int num)
{
    return QStringList();
}
