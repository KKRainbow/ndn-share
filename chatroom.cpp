#include "chatroom.h"
#include <QStringList>
#include <QDataStream>
#include <QDateTime>

std::shared_ptr<ChatroomBackend> Chatroom::m_backend;

Chatroom::Chatroom(QString chatroomName,
                   QString nickname,
                   QObject *parent):
    QObject(parent),
    m_nick(nickname),
    m_chatroomName(chatroomName)
{
}

void Chatroom::sendMessage(std::string& msg)
{
    QByteArray byteArr;
    QDataStream s(&byteArr, QIODevice::WriteOnly);
    qint64 timestamp = (qint64)QDateTime::currentDateTime().toTime_t();
    s << m_nick << timestamp << QString::fromStdString(msg);

    emit sendMessageSignal(m_chatroomName, byteArr);
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

    connect(m_backend.get(), SIGNAL(fetchMessage(QString,QByteArray)),
            frontend.get(), SLOT(fetchMessageSlot(QString,QByteArray)));

    connect(frontend.get(), SIGNAL(sendMessageSignal(QString,QByteArray)),
            m_backend.get(), SLOT(sendMessage(QString,QByteArray)));
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
    QDataStream s(msg);
    QString nick;
    qint64 timestamp;
    QString content;
    s >> nick >> timestamp >> content;
    emit newMessageSignal(chatroomName, nick, timestamp, content);
}

QString Chatroom::getNickname()
{
    return m_nick;
}
