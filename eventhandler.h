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
#include "chatroom.h"
#include <QThread>
#include <tuple>
#include <QFile>
#include "resourcefetcher.h"
#include <QTimer>
#include "resourceregister.h"

class EventHandler : public QThread
{
    Q_OBJECT
public:
    explicit EventHandler(Chatroom* chatroom, QObject *parent = 0);

private:
    void possessHandler(QString resource, QString address, QString nickname);
    void releaseHandler(QString resource, QString nickname);
    void joinHandler(QString nickname);
    void leaveHandler(QString nickname);
    bool randomBool(double poosibility);

protected:
    Chatroom* m_chatroom;
    //address, availablity, host nickname
    ndn::Name& getAddressFromResourceNode(ResourceNode& node) {return std::get<0>(node);}
    bool& getAvailabilityFromResourceNode(ResourceNode& node) {return std::get<1>(node);}
    QString& getNicknameFromResourceNode(ResourceNode& node) {return std::get<2>(node);}
    typedef std::tuple<ndn::Name, bool, QString> ResourceNode;
    typedef std::vector<ResourceNode> ResourceNodes;
    typedef std::map<QString, ResourceNodes> IndexTable;
    IndexTable m_indexTable;
    std::pair<int, int> m_redundentValue;
    std::set<QString> m_onlineHost;
    ResourceFetcher m_resourceFetcher;
    ResourceRegister m_resourceRegister;

private:
    QTimer m_timerIncRedundance, m_timerDecRedundance;

signals:
    void fetchResource(QString resource, QString address);
    void registerResource(QString resourceName, QFile file);
    void unregisterResource(QString resourceName);

public slots:
    void newMessageSlot(QString chatroom, QString nick, qint64 timestamp, QString msg);
    void possessResourceSlot(QString resource, QFile file);
    void registerResourceResultSlot(QString resource, bool res, QString msg);
    void unregisterResourceResultSlot(QString resource, bool res, QString msg);
    void fetchResourceResultSlot(QString resource, QString address, QByteArray data);

    void increseRedundanceHandler();
    void decreseRedundanceHandler();
};

#endif // EVENTHANDLER_H
