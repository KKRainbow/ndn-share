#include "consoleserver.h"

 ConsoleServer::ConsoleServer(std::shared_ptr<Chatroom> chatroom, QObject *parent):
    QThread(parent),
    m_inputStream(stdin),
    m_outputStream(stdout),
    m_chatroom(chatroom)
{
}

void ConsoleServer::run()
{
    while(true)
    {
        QString line = m_inputStream.readLine();
        m_outputStream << execute(line) << '\n';
        m_outputStream.flush();
    }

}

QString ConsoleServer::resultFormatter(bool isSuccessful, QString msg)
{
    return "";
}

QString ConsoleServer::execute(QString commandLine)
{
    QTextStream s(&commandLine);
    QString com, chatroomName;
    s >> com;

    std::shared_ptr<Chatroom> room = m_chatroom;

    if (com == "fetch")
    {
        QString blockStr;
        s >> blockStr;
        bool block = blockStr == "block";
        QString msg = room->getOneMessage(block);
        return msg;
    }
    else if (com == "send")
    {
        auto msg = s.readAll().toStdString();
        room->sendMessage(msg);
        return "";
    }
    return "Usage: fetch, send MSG";
}
