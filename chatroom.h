#ifndef CHATROOM_H
#define CHATROOM_H

#include <QObject>
#include "common.h"
#include <ChronoSync/socket.hpp>
#include <memory>
#include "chatroombackend.h"
#include <QSemaphore>
#include <QMutex>

class Chatroom : public QObject
{
    Q_OBJECT
    static std::shared_ptr<ChatroomBackend> m_backend;
public:
    static std::shared_ptr<Chatroom> getChatroom(QString chatroomName,
            QString nickname,
            ndn::Name routePrefix,
            ndn::Name broadcastPrefix);

    QString getOneMessage(bool blocked);
    QStringList getMessages(int num);
    void sendMessage(std::string& msg);
    Chatroom(QString chatroomName,
             QString nickname,
             QObject *parent = 0);
    QString getNickname();
private:
    void emitAddChatroomSignal();
private:
    QString m_chatroomName;
    std::queue<QString> m_messageQueue;
    QSemaphore m_msgQueueSema;
    QMutex m_msgQueueMutex;
    QString m_nick;

signals:
    void sendMessageSignal(QString chatroomName, QByteArray msg);
    void addChatroomSignal(QString chatroomName);
    void newMessageSignal(QString chatroomName, QString nick, qint64 timestamp, QString msg);

public slots:
    void fetchMessageSlot(QString chatroomName, QByteArray msg);
};

#endif // CHATROOM_H
