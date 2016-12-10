#include "resourcefetcher.h"
#include <QString>
#include <chatmessage.h>
#include "ndn-cxx/face.hpp"

ResourceFetcher::ResourceFetcher(QObject *parent) :
    QThread(parent)
{
}

void ResourceFetcher::onData(const ndn::Interest& interest, ndn::Data& data, QString resource)
{
    auto blk = data.getContent();
    QByteArray bytes = ChatMessage::getDecodedString(blk);
    std::cerr << "On data" << std::endl;
    emit fetchResourceResultSignal(resource,
                                   QString::fromStdString(interest.getName().toUri()),
                                   bytes);
}

void ResourceFetcher::fetchResource(QString resource, QString address)
{
    ndn::Name name = address.trimmed().toStdString();
    ndn::Interest interest(name);
    m_face.expressInterest(interest,
                           std::bind(&ResourceFetcher::onData, this, std::placeholders::_1, std::placeholders::_2, resource),
                           std::bind(&ResourceFetcher::onInterestTimeout, this, std::placeholders::_1));
    std::cerr << "Express interest: " << address.toStdString() << std::endl;
}

void ResourceFetcher::onInterestTimeout(const ndn::Interest& interest)
{
    std::cerr << "can not get interest: " << interest.getName().toUri() << std::endl;
}

void ResourceFetcher::run()
{
    try
    {
        m_face.processEvents(ndn::time::milliseconds::zero(), true);
    }
    catch(std::exception& e)
    {
        std::cerr << "Fetcher exit!!!" << std::endl;
        std::cerr << e.what() << std::endl;
    }

}
