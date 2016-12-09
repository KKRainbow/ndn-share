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
                                            ndn::Name routePrefix,
                                            ndn::Name broadcastPrefix);

    QString getOneMessage();
    QStringList getMessages(int num);
    void sendMessage(std::string& msg);
    Chatroom(QString chatroomName,
             QObject *parent = 0);
private:
    void emitAddChatroomSignal();
private:
    QString m_chatroomName;
    std::queue<QString> m_messageQueue;
    QSemaphore m_msgQueueSema;
    QMutex m_msgQueueMutex;

signals:
    void sendMessageSignal(QString chatroomName, QString msg);
    void addChatroomSignal(QString chatroomName);

public slots:
    void fetchMessageSlot(QString chatroomName, QString msg);
};

#endif // CHATROOM_H
