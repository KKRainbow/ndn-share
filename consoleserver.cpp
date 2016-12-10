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
    connect(this,
            SIGNAL(getOnlineList()),
            m_eventHandler.get(),
            SLOT(getOnlineList()));
    connect(m_eventHandler.get(),
            SIGNAL(resourcesListReady(QStringList)),
            this,
            SLOT(onResourcesListReady(QStringList)));
    connect(m_eventHandler.get(),
            SIGNAL(nodesListReady(QStringList)),
            this,
            SLOT(onNodesListReady(QStringList)));
    connect(m_eventHandler.get(),
            SIGNAL(onlineListReady(QStringList)),
            this,
            SLOT(onOnlineListReady(QStringList)));
}

void ConsoleServer::run()
{
    m_eventHandler->start();
    m_resourceFetcher.start();
    while(true)
    {
        QString line = m_inputStream.readLine();
        execute(line);
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
        m_outputStream << "Running previous command, please wait" << '\n';
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
    else if (com == "listonline")
    {
        running = true;
        emit getOnlineList();
    }
    return "";
}

void ConsoleServer::fetchResourceResultSlot(QString resource, QString address, QByteArray data)
{
    m_outputStream << resource << ' ' << address << ' ' << QString(data) << '\n';
    m_outputStream.flush();
    running = false;
}

void ConsoleServer::onResourcesListReady(QStringList stringList)
{
    for (auto& str : stringList)
    {
        m_outputStream << str << '\n';
    }
    m_outputStream.flush();
    running = false;
}

void ConsoleServer::onNodesListReady(QStringList stringList)
{
    for (auto& str : stringList)
    {
        m_outputStream << str << '\n';
    }
    m_outputStream.flush();
    running = false;
}
void ConsoleServer::onOnlineListReady(QStringList stringList)
{
    for (auto& str : stringList)
    {
        m_outputStream << str << '\n';
    }
    m_outputStream.flush();
    running = false;
}
