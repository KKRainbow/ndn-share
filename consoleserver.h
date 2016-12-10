#ifndef CONSOLESERVER_H
#define CONSOLESERVER_H

#include <QObject>
#include <QTextStream>
#include <QThread>
#include "chatroom.h"
#include "eventhandler.h"

class ConsoleServer : public QThread
{
    Q_OBJECT
public:
    explicit ConsoleServer(std::shared_ptr<EventHandler> eventHandler, QObject *parent = 0);
    void run() override;

protected:
    QTextStream m_inputStream, m_outputStream;
    QString execute(QString commandLine);
    std::shared_ptr<EventHandler> m_eventHandler;

private:
    ResourceFetcher m_resourceFetcher;

private:
    bool running = false;

signals:
    void publishResourceSignal(QString resource, QString filename);
    void fetchResource(QString resource, QString address);
    void getResourcesList();
    void getNodesList(QString resource);

private slots:
    void fetchResourceResultSlot(QString resource, QString address, QByteArray data);
    void onResourcesListReady(QStringList stringList);
    void onNodesListReady(QStringList stringList);
    void onOnlineListReady(QStringList stringList);

public slots:
};

#endif // CONSOLESERVER_H
