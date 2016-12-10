#ifndef SENDRANDOMMSG_H
#define SENDRANDOMMSG_H

#include <QObject>
#include "chatroom.h"
#include <QTimer>

class SendRandomMsg : public QObject
{
    Q_OBJECT
private:
    QTimer* m_timer;
    QString m_msg;
    Chatroom* m_chatroom;
public:
    SendRandomMsg(Chatroom* cd, int second, QString msg, QObject* parent = nullptr);
    virtual ~SendRandomMsg();
public slots:
    void onMyTimer();

};

#endif // SENDRANDOMMSG_H
