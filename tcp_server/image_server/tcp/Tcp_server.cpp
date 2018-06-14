//------------------------------------------------------------------------------

#include "Tcp_server.hpp"

//------------------------------------------------------------------------------

namespace Receiver {

//------------------------------------------------------------------------------

Tcp_server::Tcp_server(QObject* parent) : QTcpServer(parent)
{
    qDebug() <<  this << "created on" << QThread::currentThread();
}

//------------------------------------------------------------------------------

Tcp_server::~Tcp_server()
{
    qDebug() <<  this << "destroyed";
}

//------------------------------------------------------------------------------

bool Tcp_server::listen(const QHostAddress& address, quint16 port)
{
    if(!QTcpServer::listen(address, port)) return false;

    thread = new QThread(this);
    connections = new Tcp_connections();

    connect(thread, &QThread::started,
            connections, &Tcp_connections::start_cb, Qt::QueuedConnection);
    connect(this, &Tcp_server::accepting,
            connections, &Tcp_connections::accept_cb, Qt::QueuedConnection);
    connect(this, &Tcp_server::finished,
            connections, &Tcp_connections::quit_cb, Qt::QueuedConnection);
    connect(connections, &Tcp_connections::finished,
            this, &Tcp_server::complete_cb, Qt::QueuedConnection);

    connections->moveToThread(thread);

    thread->start();

    return true;
}

//------------------------------------------------------------------------------

void Tcp_server::close()
{
    qDebug() << this << "closing server";
    emit finished();
    QTcpServer::close();
}

//------------------------------------------------------------------------------

qint64 Tcp_server::port()
{
    if(isListening())
        return this->serverPort();
    return 1000;
}

//------------------------------------------------------------------------------

void Tcp_server::incomingConnection(qintptr descriptor)
{
    qDebug() << this << "attempting to accept connection" << descriptor;
    accept(descriptor);
}

//------------------------------------------------------------------------------

void Tcp_server::accept(qintptr descriptor)
{
    qDebug() << this << "accepting the connection" << descriptor;
    emit accepting(descriptor);
}

//------------------------------------------------------------------------------

void Tcp_server::complete_cb()
{
    if(!thread) {
        qWarning() << this << "exiting complete_cb there was no thread!";
        return;
    }

    qDebug() << this << "complete_cb called, destroying thread";
    delete connections;

    qDebug() << this << "quitting thread";
    thread->quit();
    thread->wait();

    delete thread;

    qDebug() << this << "complete_cb";
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------
