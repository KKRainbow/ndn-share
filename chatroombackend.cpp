#include "chatroombackend.h"
#include <functional>
#include "chatmessage.h"
#include "nfdc.h"
#include <QRegExp>

ChatroomBackend::ChatroomBackend(ndn::Name routePrefix,
                                 ndn::Name broadcastPrefix,
                                 QObject *parent):
        QThread(parent),
        m_routePrefix(routePrefix),
        m_broadcastPrefix(broadcastPrefix)
{
    initChronoSync();
    connect(&m_timerNfdc, SIGNAL(timeout()), this, SLOT(nfdcMakeBroadcast()));
}
ndn::Name ChatroomBackend::getNodePrefix(QString chatroomName)
{
    ndn::Name nodeName = m_routePrefix;
    nodeName.append(chatroomName.toStdString()).append("node");
    return nodeName;
}

void ChatroomBackend::addChatroom(QString chatroomName)
{
    m_chatroomList.insert(chatroomName);
    m_socket->addSyncNode(getNodePrefix(chatroomName));
}

void ChatroomBackend::removeChatroom(QString chatroomName)
{
    m_chatroomList.erase(chatroomName);
    m_socket->removeSyncNode(getNodePrefix(chatroomName));
}

void ChatroomBackend::sendMessage(QString chatroomName, QByteArray msg)
{
    auto blk = ChatMessage::getEncodedBlock(msg);
    m_socket->publishData(blk.wire(), blk.size(), ndn::time::milliseconds(60000), getNodePrefix(chatroomName));
}

void ChatroomBackend::processFetchedData(const std::shared_ptr<const ndn::Data> &data)
{
    ndn::Block blk = data->getContent();
    auto msg = ChatMessage::getDecodedString(blk);
    auto name = data->getName();
    for (auto& room : m_chatroomList)
    {
        QString str = QString::fromStdString(name.toUri());
        if (str.indexOf(QRegExp(QString("/%1[/$]").arg(room))) >= 0)
        {
            emit fetchMessage(room, msg);
        }
    }
}

void ChatroomBackend::processSyncUpdate(const std::vector<chronosync::MissingDataInfo> &updates)
{
    for (const auto& update : updates)
    {
        m_socket->fetchData(update.session,
                            update.high,
                            std::bind(&ChatroomBackend::processFetchedData, this, std::placeholders::_1)
                            );
    }
}

void ChatroomBackend::initChronoSync()
{
    m_socket = std::make_shared<chronosync::Socket>(
                            m_broadcastPrefix,
                            m_routePrefix,
                            m_face,
                            std::bind(&ChatroomBackend::processSyncUpdate, this, std::placeholders::_1)
                            );
}

void ChatroomBackend::run()
{
    try
    {
        while (true)
        {
        m_timerNfdc.start(1000);
        m_face.getIoService().run();
        std::cerr << "chatroom backend nothing to do!" << std::endl;
        sleep(2);
        }
    }
    catch(...)
    {
    //should never be here
    std::cerr << "chatroom backend exit!!" << std::endl;
    }

}

void ChatroomBackend::nfdcMakeBroadcast()
{
    std::string resMsg;
    bool res = Nfdc::makeNameBroadcast(m_broadcastPrefix, resMsg);

    if (!res)
    {
        std::cerr << "Can not make name " << m_broadcastPrefix.toUri() << " broadcast" << std::endl;
    }
    else
    {
        std::cout << "succeeded to make name broadcast" << std::endl;
        m_timerNfdc.stop();
    }
}
