#include "eventhandler.h"
#include <QTextStream>
#include <algorithm>
#include <QTime>
#include <QFileInfo>

EventHandler::EventHandler(std::shared_ptr<Chatroom> chatroom, QDir dir, ndn::Name prefix, quint32 min, quint32 max, QObject *parent) :
    QThread(parent),
    m_chatroom(chatroom),
    m_dir(dir),
    m_resourceRegister(prefix),
    m_redundentValue(min, max)
{
    if (!m_dir.exists())
    {
        m_dir.mkpath(m_dir.absolutePath());
    }
    connect(chatroom.get(),
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
            SIGNAL(registerResource(QString,QString)),
            &m_resourceRegister,
            SLOT(registerResource(QString,QString)));
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
    QTime time = QTime::currentTime();
    qsrand(time.msec());

}

void EventHandler::newMessageSlot(QString chatroom, QString nick, qint64 timestamp, QString msg)
{
    QTextStream ts(&msg);
    QString com;
    ts >> com;
    std::cerr << "New message in "<< chatroom.toStdString() <<
              " from " << nick.toStdString() <<
              " time: " << timestamp <<
              ", Conetent: " << msg.toStdString() << std::endl;
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
            std::cerr << "Corresponsibal resource found, changing availibility" << std::endl;
            getAvailabilityFromResourceNode(node) = true;
            return;
        }
    }
    ResourceNode newNode = std::make_tuple(ndn::Name(address.toStdString()), true, nickname);
    nodes.push_back(newNode);
    std::cerr << "Corresponsibal resource not found, adding to tail" << ", new size" <<
              nodes.size() << std::endl;
}

void EventHandler::releaseHandler(QString resource, QString nickname)
{
    try
    {
        ResourceNodes& nodes = m_indexTable.at(resource);
        auto iter = findResourceNodeByNickname(nodes, nickname);
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

void EventHandler::registerResourceResultSlot(QString resource, bool res, QString msg)
{
    std::cerr << "Register resource: " << resource.toStdString() << " result: " << res << " msg: "
              <<msg.toStdString() << std::endl;
    if (res)
    {
        ResourceNodes& nodes = m_indexTable[resource];
        ndn::Name name = msg.toStdString();
        sendPosses(resource, name);
        possessHandler(resource, msg, m_chatroom->getNickname());
    }
}


void EventHandler::increseRedundanceHandler()
{
    for (auto& nodesPair : m_indexTable)
    {
        if (nodesPair.second.size() < m_redundentValue.first &&
                nodesPair.second.size() > 0)
        {
            auto iter = findResourceNodeByNickname(nodesPair.second, m_chatroom->getNickname());
            if (iter != nodesPair.second.end())
            {
                //don't get my file
                continue;
            }
            int r = nodesPair.second.size();
            int x = m_onlineHost.size();
            int t = m_redundentValue.first;
            bool flag = randomBool((double)(t - r) / (double)x);
            if (flag)
            {
                emit fetchResource(nodesPair.first,
                                   QString::fromStdString(getAddressFromResourceNode(nodesPair.second[qrand() % r]).toUri()));
            }
        }
    }
}

void EventHandler::unregisterResourceResultSlot(QString resource, bool res, QString msg)
{
    std::cerr << "Unregister resource: " << resource.toStdString() << " result: " << res << " msg: "
              << msg.toStdString() << std::endl;
    if (res)
    {
        try
        {
            ResourceNodes& nodes = m_indexTable.at(resource);
            auto iter =  findResourceNodeByNickname(nodes, m_chatroom->getNickname());
            if (iter != nodes.end())
            {
                nodes.erase(iter);
            }
            sendRelease(resource);
        }
        catch(...)
        {
            std::cerr << "No such resource found" << std::endl;
        }
    }
}

void EventHandler::decreseRedundanceHandler()
{
    for (auto& nodesPair : m_indexTable)
    {
        if (nodesPair.second.size() > m_redundentValue.second)
        {
            auto iter = findResourceNodeByNickname(nodesPair.second, m_chatroom->getNickname());
            if (iter == nodesPair.second.end())
            {
                //I don't have this file!
                continue;
            }
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
    QFile file(m_dir.filePath(resource));
    file.open(QFile::ReadWrite);
    file.write(data);
    file.close();
    QFileInfo info(file);
    emit registerResource(resource, info.absoluteFilePath());
}

bool EventHandler::randomBool(double possibility)
{
    int n = qrand() % 1000;
    return (double)n < possibility * 1000.0;
}

EventHandler::ResourceNodes::iterator EventHandler::findResourceNodeByNickname(ResourceNodes& nodes, QString nickname)
{
    auto iter = std::find_if(nodes.begin(),
                             nodes.end(),
    [this, &nickname](ResourceNodes::value_type& v) {
        return this->getNicknameFromResourceNode(v) == nickname;
    });
    return iter;
}

void EventHandler::sendJoin()
{
    std::string join = "JOIN";
    m_chatroom->sendMessage(join);
}

void EventHandler::sendLeave()
{
    std::string leave = "LEAVE";
    m_chatroom->sendMessage(leave);
}

void EventHandler::sendPosses(QString resource, ndn::Name address)
{
    QString str = QString("POSSES %1 %2").arg(resource).arg(QString::fromStdString(address.toUri()));
    auto stdstr = str.toStdString();
    m_chatroom->sendMessage(stdstr);
}

void EventHandler::sendRelease(QString resource)
{
    QString str = QString("RELEASE %1").arg(resource);
    auto stdstr = str.toStdString();
    m_chatroom->sendMessage(stdstr);
}

void EventHandler::publishResource(QString resource, QString filename)
{
    emit registerResource(resource, filename);
}


void EventHandler::shutdown()
{
    sendLeave();
    leaveHandler(m_chatroom->getNickname());
    this->exit();
}

void EventHandler::run()
{
    m_timerIncRedundance.start(6000);
    m_timerDecRedundance.start(6000);
    m_resourceFetcher.start();
    m_resourceRegister.start();
    sendJoin();
    joinHandler(m_chatroom->getNickname());
    exec();
}

void EventHandler::getResourcesList()
{
    QStringList list;
    for (auto& nodesPair : m_indexTable)
    {
        list << nodesPair.first;
    }
    emit resourcesListReady(list);
}

void EventHandler::getNodesList(QString resource)
{
    QStringList list;
    try
    {
        auto& nodes = m_indexTable.at(resource);
        QString str;
        for (auto& node : nodes)
        {
            str = QString("%1 %2 %3").arg(QString::fromStdString(std::get<0>(node).toUri()))
                  .arg(std::get<1>(node))
                  .arg(std::get<2>(node));
            list << str;
        }
    }
    catch(...)
    {
    }
    emit nodesListReady(list);
}

void EventHandler::getOnlineList()
{
    QStringList list;
    for (auto& n : m_onlineHost)
    {
        list << n;
    }
    emit onlineListReady(list);
}
