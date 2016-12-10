#include "resourceregister.h"
#include "chatmessage.h"
#include <ndn-cxx/encoding/tlv.hpp>

ResourceRegister::ResourceRegister(ndn::Name prefix, QObject *parent):
    QThread(parent),
    m_prefix(prefix)
{
    m_prefix.append("SHARE");
    std::cerr << "Register prefix " << m_prefix.toUri() << std::endl;
    m_face.setInterestFilter(m_prefix,
                             std::bind(&ResourceRegister::onInterest, this, std::placeholders::_1, std::placeholders::_2),
                             std::bind(&ResourceRegister::registerPrefixFailed, this, std::placeholders::_1, std::placeholders::_2));
}

void ResourceRegister::registerPrefixFailed(const ndn::Name& prefix, const std::string& failInfo)
{
    std::cerr << "Resource register start failed: " << failInfo << std::endl;
}

void ResourceRegister::onInterest(const ndn::Name& prefix, const ndn::Interest& interest)
{
    ndn::Name name = interest.getName();
    if (name.size() != prefix.size() + 1)
    {
        std::cerr << "Unknown name" << std::endl;
    }
    std::string resourceNameStr = name.at(name.size() - 1).toUri();
    QString resourceName = QString::fromStdString(resourceNameStr);

    try
    {
        QFile file(m_resourceFileMap.at(resourceName));
        file.open(QFile::ReadOnly);
        QByteArray bytes = file.readAll();
        file.close();
        auto blk = ChatMessage::getEncodedBlock(bytes);
        std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>();
        data->setName(name);
        //data->setContent(ndn::makeBinaryBlock(ndn::tlv::Content, bytes.data(), bytes.length()));
        data->setContent(blk);
        data->setFreshnessPeriod(ndn::time::milliseconds(60000));
        m_keyChain.sign(*data, ndn::security::signingWithSha256());
        m_face.put(*data);
        m_resourceCounterMap[resourceName]++;
    }
    catch(std::exception& e)
    {
        std::cerr << "Interest of unkonwn resource: " << resourceNameStr <<
                  "reason: " << e.what() << std::endl;
    }
}

void ResourceRegister::registerResource(QString resourceName, QString filename)
{
    QFile file(filename);
    if (!file.exists())
    {
        emit registerResourceResult(resourceName, false, "file doesn't exists");
    }
    else
    {
        this->m_resourceFileMap[resourceName] = filename;
        ndn::Name tmp = m_prefix;
        tmp.append(resourceName.toStdString());
        emit registerResourceResult(resourceName, true, QString::fromStdString(tmp.toUri()));
    }
}

void ResourceRegister::unregisterResource(QString resourceName)
{
    int i = this->m_resourceFileMap.erase(resourceName);
    emit unregisterResourceResult(resourceName, i > 0, i > 0 ? "": " no such resource");
}

void ResourceRegister::run()
{
    m_face.processEvents(ndn::time::milliseconds::zero(), true);
    std::cerr << "Register exit!!!" << std::endl;
}
