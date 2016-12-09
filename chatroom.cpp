#include "chatroom.h"
#include <QStringList>
#include <QDataStream>
#include <QDateTime>

std::shared_ptr<ChatroomBackend> Chatroom::m_backend;

Chatroom::Chatroom(QString chatroomName,
                   QString nickname,
                   QObject *parent):
    m_nick(nickname),
    QObject(parent),
    m_chatroomName(chatroomName),
    m_msgQueueSema()
{
}

void Chatroom::sendMessage(std::string& msg)
{
    QByteArray byteArr;
    QDataStream s(&byteaArr);
    qint64 timestamp = (qint64)QDateTime::currentDateTime().toTime_t();
    s << m_nick << timestamp << msg;

    emit sendMessageSignal(m_chatroomName, QString::fromStdString(msg));
}

void Chatroom::emitAddChatroomSignal()
{
    emit addChatroomSignal(m_chatroomName);
}

std::shared_ptr<Chatroom> Chatroom::getChatroom(QString chatroomName,
                                                QString nickname,
                                                ndn::Name routePrefix,
                                                ndn::Name broadcastPrefix)
{
    if (!m_backend)
    {
        m_backend = std::make_shared<ChatroomBackend>(routePrefix, broadcastPrefix);
        m_backend->start();
    }
    auto frontend = std::make_shared<Chatroom>(chatroomName, nickname);

    connect(m_backend.get(), SIGNAL(fetchMessage(QString,QString)),
            frontend.get(), SLOT(fetchMessageSlot(QString,QString)));

    connect(frontend.get(), SIGNAL(sendMessageSignal(QString,QString)),
            m_backend.get(), SLOT(sendMessage(QString,QString)));
    connect(frontend.get(), SIGNAL(addChatroomSignal(QString)),
            m_backend.get(), SLOT(addChatroom(QString)));

    frontend->emitAddChatroomSignal();

    return frontend;
}

void Chatroom::fetchMessageSlot(QString chatroomName, QByteArray msg)
{
    if (chatroomName != m_chatroomName)
    {
        //not this room
        return;
    }
    m_msgQueueMutex.lock();
    m_messageQueue.push(msg);
    m_msgQueueSema.release();
    std::cerr << "get msg from " << chatroomName.toStdString() << msg.toStdString() <<std::endl;
    m_msgQueueMutex.unlock();
    QDataStream s(&msg);
    QString nick;
    qint64 timestamp;
    QString content;
    s >> nick >> timestamp >> content;
    emit newMessageSignal(chatroomName, nick, timestamp, content);
}
QString Chatroom::getOneMessage(bool blocked)
{
    if (blocked)
    {
        m_msgQueueSema.acquire();
    }
    else
    {
        bool res = m_msgQueueSema.tryAcquire();
        if (!res)
        {
            return "";
        }
    }
    m_msgQueueMutex.lock();
    auto msg = m_messageQueue.front();
    m_messageQueue.pop();
    m_msgQueueMutex.unlock();
    return msg;
}

QStringList Chatroom::getMessages(int num)
{
    return QStringList();
}

void Chatroom::getNickname()
{
    return m_nick;
}
