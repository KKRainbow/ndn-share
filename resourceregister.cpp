#include "resourceregister.h"
#include "chatmessage.h"

ResourceRegister::ResourceRegister(ndn::Name prefix, QObject *parent):
    m_prefix(prefix),
    QThread(parent)
{
    m_face.setInterestFilter(prefix,
                             std::bind(&ResourceRegister::onInterest, this, std::placeholders::_1, std::placeholders::_2),
                             std::bind(&ResourceRegister::registerPrefixFailed, this, std::placeholders::_1));
}

void ResourceRegister::registerPrefixFailed(std::string& failInfo)
{
    std::cerr << "Resource register start failed: " << failInfo << std::endl;
}

void ResourceRegister::onInterest(const ndn::Name& prefix, const ndn::Interest& interest)
{
   std::string name = interest.getName().toUri();
   if (name.size() != prefix.size() + 1)
   {
       std::cerr << "Unknown name" << std::endl;
   }
   std::string resourceNameStr = name.at(name.size() - 1).toUri();
   QString resourceName = QString::fromStdString(resourceNameStr);

   try
   {
       const QFile& file = m_resourceFileMap.at(resourceName);
       QByteArray bytes = file.readAll();
       auto blk = ChatMessage::getEncodedBlock(bytes);
       std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>();
       data->setName(name);
       data->setContent(blk);
       data->setFreshnessPeriod(ndn::time::milliseconds(60000));
       m_face.put(*data);
       m_resourceCounterMap[resourceName]++;
   }
   catch(...)
   {
       std::cerr << "Interest of unkonwn resource" << std::endl;
   }
}

void ResourceRegister::registerResource(QString resourceName, QFile file)
{
    if (!file.exists())
    {
        emit registerResourceResult(resourceName, false, "file doesn't exists");
    }
    else
    {
        this->m_resourceFileMap[resourceName] = file;
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
