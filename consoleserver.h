#ifndef CONSOLESERVER_H
#define CONSOLESERVER_H

#include <QObject>
#include <QTextStream>
#include <QThread>
#include "chatroom.h"

class ConsoleServer : public QThread
{
    Q_OBJECT
public:
    explicit ConsoleServer(std::shared_ptr<Chatroom> chatroom, QObject *parent = 0);
    void run() override;

protected:
    QTextStream m_inputStream, m_outputStream;
    QString execute(QString commandLine);
    std::shared_ptr<Chatroom> m_chatroom;
    QString resultFormatter(bool isSuccessful, QString msg);
signals:

public slots:

};

#endif // CONSOLESERVER_H
