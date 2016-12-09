#ifndef RESOURCEREGISTER_H
#define RESOURCEREGISTER_H

#include <QObject>
#include <QThread>
#include <QFile>
#include "common.h"

class ResourceRegister : public QThread
{
    Q_OBJECT
public:
    explicit ResourceRegister(ndn::Name prefix, QObject *parent = 0);

private:
    void registerPrefixFailed(std::string& failInfo);
    void onInterest(const ndn::Name& prefix, const ndn::Interest& interest);

private:
    ndn::Face m_face;
    std::map<QString, QFile> m_resourceFileMap;
    std::map<QString, int> m_resourceCounterMap;
    ndn::Name m_prefix;

signals:
    void registerResourceResult(QString resource, bool res, QString msg);
    void unregisterResourceResult(QString resource, bool res, QString msg);

public slots:
    void registerResource(QString resourceName, QFile file);
    void unregisterResource(QString resourceName);
};

#endif // RESOURCEREGISTER_H
