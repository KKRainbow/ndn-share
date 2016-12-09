#include "eventhandler.h"
#include <QTextStream>
#include <algorithm>
#include <QTime>

EventHandler::eventHandler(Chatroom* chatroom, QObject *parent) :
        m_chatroom(chatroom),
        QThread(parent)
{
    connect(chatroom,
            SIGNAL(newMessageSignal(QString,QString,qint64,QString)),
            this,
            SLOT(newMessageSlot(QString,QString,qint64,QString)));

    connect(&m_resourceRegister,
            SIGNAL(registerResourceResult(QString,bool,QString)),
            this,
            SLOT(registerResourceResultSlot(QString,bool,QString)));
    connect(&m_resourceRegister,
            SIGNAL(unregisterResourceResult(QString,bool,QString)),
            this,
            SLOT(unregisterResourceResultSlot(QString,bool,QString)));
    connect(this,
            SIGNAL(registerResource(QString,QFile)),
            &m_resourceRegister,
            SLOT(registerResource(QString,QFile)));
    connect(this,
            SIGNAL(unregisterResource(QString)),
            &m_resourceRegister,
            SLOT(unregisterResource(QString)));

    connect(&m_resourceFetcher,
            SIGNAL(fetchResourceResultSignal(QString,QString,QByteArray)),
            this,
            SLOT(fetchResourceResultSlot(QString,QString,QByteArray)));
    connect(this,
            SIGNAL(fetchResource(QString,QString)),
            &m_resourceFetcher,
            SLOT(fetchResource(QString,QString)));

    connect(&m_timerIncRedundance,
            SIGNAL(timeout()),
            this,
            SLOT(increseRedundanceHandler()));
    connect(&m_timerDecRedundance,
            SIGNAL(timeout()),
            this,
            SLOT(decreseRedundanceHandler()));
    m_timerIncRedundance.start();
    m_timerDecRedundance.start();
    QTime time = QTime::currentTime();
    qsrand(time.msec());
}

void EventHandler::newMessageSlot(QString chatroom, QString nick, qint64 timestamp, QString msg)
{
    QTextStream ts(&msg);
    QString com;
    ts >> com;
    if (com == "JOIN")
    {
        joinHandler(nick);
    }
    else if (com == "LEAVE")
    {
        leaveHandler(nick);
    }
    else if (com == "POSSES")
    {
        QString resource, name;
        ts >> resource >> name;
        possessHandler(resource, name, nick);
    }
    else if (com == "RELEASE")
    {
        QString resource;
        ts >> resource;
        releaseHandler(resource, nick);
    }
    else
    {
        std::cerr << "Receive an unknown message" << std::endl;
    }
}

void EventHandler::possessHandler(QString resource, QString address, QString nickname)
{
    ResourceNodes& nodes = m_indexTable[resource];
    for (auto& node : nodes)
    {
        if (getNicknameFromResourceNode(node) == nickname)
        {
            getAvailabilityFromResourceNode(node) = true;
            return;
        }
    }
    ResourceNode newNode = std::make_tuple(ndn::Name(address.toStdString()), true, nickname);
    nodes.push_back(newNode);
}

void EventHandler::releaseHandler(QString resource, QString nickname)
{
    try
    {
        ResourceNodes& nodes = m_indexTable.at(resource);
        auto iter = std::find_if(nodes.begin(),
                                 nodes.end(),
                                 [this, &nickname](ResourceNodes::value_type& v) {
            return this->getNicknameFromResourceNode(v) == nickname; });

        if (iter == nodes.end())
        {
            std::cerr << "release a resource it doesn't posses" << std::endl;
        }
        else
        {
            nodes.erase(iter);
        }
    }
    catch(...)
    {
        std::cerr << "release an unkown resource" << std::endl;
    }
}

void EventHandler::joinHandler(QString nickname)
{
    for (auto& nodesPair : m_indexTable)
    {
        for (auto& node : nodesPair.second)
        {
            if (getNicknameFromResourceNode(node) == nickname)
            {
                getAvailabilityFromResourceNode(node) = true;
            }
        }
    }
    m_onlineHost.insert(nickname);
}

void EventHandler::leaveHandler(QString nickname)
{
    for (auto& nodesPair : m_indexTable)
    {
        for (auto& node : nodesPair.second)
        {
            if (getNicknameFromResourceNode(node) == nickname)
            {
                getAvailabilityFromResourceNode(node) = false;
            }
        }
    }
    m_onlineHost.erase(nickname);
}

void EventHandler::possessResourceSlot(QString resource, QFile file)
{
    emit registerResource(resource, file);
}

void EventHandler::registerResourceResultSlot(QString resource, bool res, QString msg)
{
    if (res)
    {
        ResourceNodes& nodes = m_indexTable.at(resource);
        auto iter = std::find_if(nodes.begin(),
                                 nodes.end(),
                                 [this, &nickname](ResourceNodes::value_type& v) {
            return this->getNicknameFromResourceNode(v) == nickname; });
        if (iter != nodes.end())
        {
        this->m_indexTable[resource].push_back(std::make_tuple(msg, true, m_chatroom->getNickname()));
        }
    }
}


void EventHandler::increseRedundanceHandler()
{
    for (auto& nodesPair : m_indexTable)
    {
        if (nodesPair.second.size() < m_redundentValue.first &&
                nodesPair.second.size() > 0)
        {
            int r = nodesPair.second.size();
            int x = m_onlineHost.size();
            int t = m_redundentValue.first;
            bool flag = randomBool((double)(t - r) / (double)x);
            if (flag)
            {
                emit fetchResource(nodesPair.first,
                           getAddressFromResourceNode(nodesPair.second));
            }
        }
    }
}

void EventHandler::unregisterResourceResultSlot(QString resource, bool res, QString msg)
{

}

void EventHandler::decreseRedundanceHandler()
{
    for (auto& nodesPair : m_indexTable)
    {
        if (nodesPair.second.size() > m_redundentValue.second)
        {
            int r = nodesPair.second.size();
            int t = m_redundentValue.second;
            bool flag = randomBool((double)(r - t) / (double)r);
            if (flag)
            {
                emit unregisterResource(nodesPair.first);
            }
        }
    }
}

void EventHandler::fetchResourceResultSlot(QString resource, QString address, QByteArray data)
{

}

bool EventHandler::randomBool(double poosibility)
{
    int n = qrand() % 1000;
    return (double)n < possiblity * 1000.0;
}
