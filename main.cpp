#include <QCoreApplication>
#include "chatroom.h"
#include "sendrandommsg.h"
#include "consoleserver.h"

//args: host prefix
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ndn::Name r = ndn::Name("/ndn/edu");
    ndn::Name b = ndn::Name("/ndn/broadcast/CHAT");

    if (argc == 2)
    {
        r.append(argv[1]);
    }

    auto cr = Chatroom::getChatroom("abc", argv[1], r,b);

    ConsoleServer s(cr);
    s.start();

    /*
    QString msg = argv[2];
    auto cr = Chatroom::getChatroom("abc",r,b);
    SendRandomMsg* sr = new SendRandomMsg(cr.get(), 3, msg);
    */

    return a.exec();
}

