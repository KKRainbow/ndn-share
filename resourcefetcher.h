#ifndef REDUNDANCEMANAGER_H
#define REDUNDANCEMANAGER_H

#include <QObject>
#include <QThread>
#include "common.h"

class ResourceFetcher : public QThread
{
    Q_OBJECT
public:
    explicit ResourceFetcher(QObject *parent = 0);
    void run() override;

private:
    ndn::Face m_face;
    void onData(const ndn::Interest& interest, ndn::Data& data, QString resource);
    void onInterestTimeout(const ndn::Interest& interest);
signals:
    void fetchResourceResultSignal(QString resource, QString address, QByteArray data);

public slots:
    void fetchResource(QString resource, QString address);
};

#endif // REDUNDANCEMANAGER_H
