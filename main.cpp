#include <QCoreApplication>
#include "chatroom.h"
#include "sendrandommsg.h"
#include "nfdc.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ndn::Name r = ndn::Name("/ndn/edu");
    ndn::Name b = ndn::Name("/ndn/broadcast/CHAT");
    auto cr = Chatroom::getChatroom("abc",r,b);
    QString msg = "fkldsajlkfdjsa";
    if (argc == 2)
    {
        msg = argv[1];
    }
    SendRandomMsg* sr = new SendRandomMsg(cr.get(), 3, msg);

    return a.exec();
}
