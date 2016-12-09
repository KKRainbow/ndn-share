#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

/*
 * Message:
 * 	JOIN
 * 	LEAVE
 * 	POSSES resource_name ndn_name
 * 	RELEASE resource_name
 */

#include <QObject>
#include <QStringList>
#include "chatroom.h"
#include <QThread>
#include <tuple>
#include <QFile>
#include "resourcefetcher.h"
#include <QTimer>
#include <QDir>
#include "resourceregister.h"

class EventHandler : public QThread
{
    Q_OBJECT
public:
    typedef std::tuple<ndn::Name, bool, QString> ResourceNode;
    typedef std::vector<ResourceNode> ResourceNodes;
    typedef std::map<QString, ResourceNodes> IndexTable;
public:
    explicit EventHandler(std::shared_ptr<Chatroom> chatroom, QDir dir, ndn::Name prefix, QObject *parent = 0);
    void run() override;

protected:
    void possessHandler(QString resource, QString address, QString nickname);
    void releaseHandler(QString resource, QString nickname);
    void joinHandler(QString nickname);
    void leaveHandler(QString nickname);
    void sendJoin();
    void sendLeave();
    void sendPosses(QString resource, ndn::Name address);
    void sendRelease(QString resource);

private:
    bool randomBool(double poosibility);
    ResourceNodes::iterator findResourceNodeByNickname(ResourceNodes& nodes, QString nickname);

protected:
    std::shared_ptr<Chatroom> m_chatroom;
    //address, availablity, host nickname
    ndn::Name& getAddressFromResourceNode(ResourceNode& node) {return std::get<0>(node);}
    bool& getAvailabilityFromResourceNode(ResourceNode& node) {return std::get<1>(node);}
    QString& getNicknameFromResourceNode(ResourceNode& node) {return std::get<2>(node);}
    IndexTable m_indexTable;
    std::pair<int, int> m_redundentValue;
    std::set<QString> m_onlineHost;
    ResourceFetcher m_resourceFetcher;
    ResourceRegister m_resourceRegister;

private:
    QTimer m_timerIncRedundance, m_timerDecRedundance;
    QDir m_dir;

signals:
    void fetchResource(QString resource, QString address);
    void registerResource(QString resourceName, QString filename);
    void unregisterResource(QString resourceName);

    void resourcesListReady(QStringList stringList);
    void nodesListReady(QStringList stringList);

private slots:
    void newMessageSlot(QString chatroom, QString nick, qint64 timestamp, QString msg);

    void registerResourceResultSlot(QString resource, bool res, QString msg);
    void unregisterResourceResultSlot(QString resource, bool res, QString msg);
    void fetchResourceResultSlot(QString resource, QString address, QByteArray data);

    void increseRedundanceHandler();
    void decreseRedundanceHandler();

public slots:
    void publishResource(QString resource, QString filename);
    void getResourcesList();
    void getNodesList(QString resource);
    void shutdown();
};

#endif // EVENTHANDLER_H
