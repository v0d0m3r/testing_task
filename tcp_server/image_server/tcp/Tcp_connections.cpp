//------------------------------------------------------------------------------

#include "Tcp_connections.hpp"

//------------------------------------------------------------------------------

Tcp_connections::Tcp_connections(QObject* parent) : QObject(parent)
{
    qDebug() << this << "created";
}

//------------------------------------------------------------------------------

Tcp_connections::~Tcp_connections()
{
    qDebug() << this << "destroyed";
}

//------------------------------------------------------------------------------

int Tcp_connections::count()
{
    QReadWriteLock lock;
    lock.lockForRead();
    int value = connections.count();
    lock.unlock();

    return value;
}

//------------------------------------------------------------------------------

void Tcp_connections::removeSocket(QTcpSocket* socket)
{
    if(!socket) return;
    if(!connections.contains(socket)) return;

    qDebug() << this << "removing socket = " <<  socket;

    if(socket->isOpen())
    {
        qDebug() << this << "socket is open, attempting to close it " << socket;
        socket->disconnect();
        socket->close();
    }

    qDebug() << this << "deleting socket" << socket;
    connections.remove(socket);
    socket->deleteLater();

    qDebug() << this << "client count = " << connections.count();

}

//------------------------------------------------------------------------------

void Tcp_connections::disconnected()
{
    if(!sender()) return;
    qDebug() << this << "disconnecting socket"<< sender();

    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    removeSocket(socket);
}

//------------------------------------------------------------------------------

void Tcp_connections::error(QAbstractSocket::SocketError socketError)
{
    if(!sender()) return;
    qDebug() << this << "error in socket" << sender() << " error = " << socketError;

    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    removeSocket(socket);
}

//------------------------------------------------------------------------------

void Tcp_connections::start()
{
    qDebug() << this << "connections started on" << QThread::currentThread();
}

//------------------------------------------------------------------------------

void Tcp_connections::quit()
{
    if(!sender()) return;
    qDebug() << this << "connections quitting";

    foreach(QTcpSocket* socket, connections.keys())
    {
        qDebug() << this << "closing socket" << socket;
        removeSocket(socket);
    }

    qDebug() << this << "finishing";
    emit finished();
}

//------------------------------------------------------------------------------

void Tcp_connections::accept(qintptr handle)
try {
    qDebug() << "*** HEY WATCH THIS";

    Tcp_connecton connection(handle);

    connections.insert(handle, connection(handle));

    qDebug() << this << "clients = " << connections.count();

    connect(connections[handle].get_socket(), &QTcpSocket::disconnected,
            this, &Tcp_connections::disconnected);
    connect(connections[handle].get_socket(), static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this, &Tcp_connections::error);


    emit socket->connected();
}
catch(Bad_tcp_connection& b) {
    qWarning() << this << "could not accept connection" << handle;
}

//------------------------------------------------------------------------------
