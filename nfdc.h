#ifndef NFDC_H
#define NFDC_H

#include <QObject>
#include "common.h"

class Nfdc : public QObject
{
    Q_OBJECT
public:
    explicit Nfdc(QObject *parent = 0);

    //Make Name Broadcast
    static bool makeNameBroadcast(const ndn::Name& name, std::string& msg);

private:
    static bool setStrategy(const ndn::Name& name, const std::string strategy);
    static bool addNextHop(const ndn::Name& name, const int faceId);
    static std::vector<int> getNonLocalFaceIds();

signals:

public slots:
};

#endif // NFDC_H
