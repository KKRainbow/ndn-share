#ifndef CHATROOMBACKEND_H
#define CHATROOMBACKEND_H

#include <QObject>
#include <common.h>
#include <ChronoSync/socket.hpp>
#include <QThread>
#include <QTimer>

class ChatroomBackend : public QThread
{
    Q_OBJECT
public:
    explicit ChatroomBackend(ndn::Name routePrefix,
                             ndn::Name broadcastPrefix,
                             QObject *parent = 0);
    void run() override;
    void removeChatroom(QString chatroomName);
private:
    void processFetchedData(const std::shared_ptr<const ndn::Data>& data);
    void processSyncUpdate(const std::vector<chronosync::MissingDataInfo>& updates);

    void initChronoSync();

private:
    ndn::Face m_face;
    ndn::Name m_routePrefix, m_broadcastPrefix;
    QString m_chatroomName;
    std::shared_ptr<chronosync::Socket> m_socket;
    std::set<QString> m_chatroomList;
    ndn::Name getNodePrefix(QString chatroomName);

    QTimer m_timerNfdc;

signals:
    void fetchMessage(QString chatroomName, QString msg);

public slots:
    void sendMessage(QString chatroomName, QString msg);
    void addChatroom(QString chatroomName);
    void nfdcMakeBroadcast();
};

#endif // CHATROOMBACKEND_H
