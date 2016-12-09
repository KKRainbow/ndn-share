#include <QCoreApplication>
#include "sendrandommsg.h"
#include "eventhandler.h"
#include "consoleserver.h"

//args: host dir
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ndn::Name r = ndn::Name("/ndn/edu");
    ndn::Name b = ndn::Name("/ndn/broadcast/CHAT");

    r.append(argv[1]);
    QDir dir(argv[2]);

    auto cr = Chatroom::getChatroom("abc", argv[1], r, b);
    auto eventHandler = std::make_shared<EventHandler>(cr, dir, r);

    ConsoleServer s(eventHandler);
    s.start();

    /*
    QString msg = argv[2];
    auto cr = Chatroom::getChatroom("abc",r,b);
    SendRandomMsg* sr = new SendRandomMsg(cr.get(), 3, msg);
    */

    return a.exec();
}

