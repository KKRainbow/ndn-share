#include "nfdc.h"
#include <QProcess>
#include <QTextCodec>

Nfdc::Nfdc(QObject *parent) :
    QObject(parent)
{
}

bool Nfdc::makeNameBroadcast(const ndn::Name& name, std::string& msg)
{
    std::string s = "/localhost/nfd/strategy/multicast";
    auto faceIds = getNonLocalFaceIds();
    bool suc1 = setStrategy(name, s);

    if (!suc1)
    {
        msg = "Set strategy failed";
        return false;
    }

    bool suc2 = false;
    for (auto &faceId : faceIds)
    {
        suc2 = addNextHop(name, faceId);
        if (!suc2)
        {
            msg = QString("Add nexthop %1 failed").arg(faceId).toStdString();
            return false;
        }
        else
        {
            std::cerr << "Add nexthop " << faceId << ' ' << name << std::endl;
        }
    }
    return true;
}

bool Nfdc::setStrategy(const ndn::Name& name, const std::string strategy)
{
    QProcess process;
    process.setProgram("nfdc");
    QStringList args;
    QString uri = QString::fromStdString(name.toUri());
    args << "set-strategy" << uri << QString::fromStdString(strategy);
    process.setArguments(args);

    QString tmp = args.join(' ');

    process.start();
    process.waitForFinished();
    QString str(process.readAll());
    return str.indexOf("Successfully") >= 0;
}

bool Nfdc::addNextHop(const ndn::Name& name, const int faceId)
{
    QProcess process;
    process.setProgram("nfdc");
    QStringList args;
    QString uri = QString::fromStdString(name.toUri());
    args << "add-nexthop" << uri << QString::number(faceId);
    process.setArguments(args);
    process.start();
    process.waitForFinished();
    QString str(process.readAll());
    return str.indexOf("succeeded") >= 0;
}

std::vector<int> Nfdc::getNonLocalFaceIds()
{
    QProcess process;
    process.setProgram("nfd-status");
    process.start();
    process.waitForFinished();

    std::vector<int> resVec;
    QByteArray byteArr = process.readAllStandardOutput();
    QString str(byteArr);
    QStringList lines = str.split("\n", QString::SkipEmptyParts);
    for (QString& str : lines)
    {
        QString tmp = "faceid";
        QString udp = "udp4://";
        QString trimmed = str.trimmed();
        if (trimmed.startsWith(tmp) && trimmed.indexOf(udp) >= 0)
        {
            QRegExp re("faceid=(\\d+)");
            int pos = trimmed.indexOf(re);
            BOOST_ASSERT(pos >= 0 && re.captureCount() == 1);
            auto faceidStr = re.capturedTexts()[1];
            resVec.push_back(faceidStr.toInt());
        }
    }
    return resVec;
}
