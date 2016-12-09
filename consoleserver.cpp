#include "consoleserver.h"

 ConsoleServer::ConsoleServer(std::shared_ptr<EventHandler> eventHandler, QObject *parent):
    QThread(parent),
    m_inputStream(stdin),
    m_outputStream(stdout),
    m_eventHandler(eventHandler)
{
    connect(&m_resourceFetcher,
            SIGNAL(fetchResourceResultSignal(QString,QString,QByteArray)),
            this,
            SLOT(fetchResourceResultSlot(QString,QString,QByteArray)));
    connect(this,
            SIGNAL(fetchResource(QString,QString)),
            &m_resourceFetcher,
            SLOT(fetchResource(QString,QString)));

    connect(this,
            SIGNAL(publishResourceSignal(QString,QString)),
            m_eventHandler.get(),
            SLOT(publishResource(QString,QString)));
    connect(this,
            SIGNAL(getNodesList(QString)),
            m_eventHandler.get(),
            SLOT(getNodesList(QString)));
    connect(this,
            SIGNAL(getResourcesList()),
            m_eventHandler.get(),
            SLOT(getResourcesList()));
}

void ConsoleServer::run()
{
    m_eventHandler->start();
    while(true)
    {
        QString line = m_inputStream.readLine();
        m_outputStream << execute(line) << '\n';
        m_outputStream.flush();
    }

}

QString ConsoleServer::execute(QString commandLine)
{
    QTextStream s(&commandLine);
    QString com;
    s >> com;

    if (running)
    {
        std::cout << "Running previous command, please wait" << std::endl;
    }

    if (com == "pull")
    {
        QString resource, address;
        s >> resource >> address;
        running = true;
        emit fetchResource(resource, address);
    }
    else if (com == "push")
    {
        QString resource, filename;
        s >> resource >> filename;
        filename = QDir::current().filePath(filename);
        emit publishResourceSignal(resource, filename);
    }
    else if (com == "listres")
    {
        running = true;
        emit getResourcesList();
    }
    else if (com == "listnode")
    {
        QString resource;
        s >> resource;
        running = true;
        emit getNodesList(resource);
    }
    return "Usage: pull RES ADDR, push RES FILENAME";
}

void ConsoleServer::fetchResourceResultSlot(QString resource, QString address, QByteArray data)
{
    std::cout << resource.toStdString() << ' ' << address.toStdString() << ' ' << QString(data).toStdString() << std::endl;
    running = false;
}

void ConsoleServer::onResourcesListReady(QStringList stringList)
{
    for (auto& str : stringList)
    {
        std::cout << str.toStdString() << std::endl;
    }
    running = false;
}

void ConsoleServer::onNodesListReady(QStringList stringList)
{
    for (auto& str : stringList)
    {
        std::cout << str.toStdString() << std::endl;
    }
    running = false;
}
