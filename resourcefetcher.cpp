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
    emit fetchResourceResultSignal(resource,
                                   QString::fromStdString(interest.getName().toUri()),
                                   bytes);
}

void ResourceFetcher::fetchResource(QString resource, QString address)
{
    ndn::Name name = address.toStdString();
    ndn::Interest interest;
    interest.setName(name);
    interest.setMustBeFresh(true);
    interest.setInterestLifetime(ndn::time::milliseconds(60000));
    interest.getNonce();
    m_face.expressInterest(interest,
                           std::bind(&ResourceFetcher::onData, this, std::placeholders::_1, std::placeholders::_2, resource),
                           std::bind(&ResourceFetcher::onInterestTimeout, this, std::placeholders::_1));
}

void ResourceFetcher::onInterestTimeout(const ndn::Interest& interest)
{
    std::cerr << "can not get interest: " << interest.getName().toUri() << std::endl;
}

void ResourceFetcher::run()
{
    m_face.getIoService().run();
    std::cerr << "Fetcher exit!!!" << std::endl;
}
