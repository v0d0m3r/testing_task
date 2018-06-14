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

int Tcp_connections::count() const
{
    QReadWriteLock lock;
    lock.lockForRead();
    int value = connections.size();
    lock.unlock();

    return value;
}

//------------------------------------------------------------------------------

void Tcp_connections::remove_socket(QTcpSocket* socket)
{
    if(!socket) return;
    auto p = connections.find(socket);
    if (p == connections.end()) return;

    qDebug() << this << "removing socket = " <<  socket;

    if(socket->isOpen()) {
        qDebug() << this << "socket is open, attempting to close it " << socket;
        socket->disconnect();
        socket->close();
    }

    qDebug() << this << "deleting socket" << socket;
    connections.erase(p);
    qDebug() << this << "client count = " << connections.size();

}

//------------------------------------------------------------------------------

void Tcp_connections::disconnected_cb()
{
    if(!sender()) return;
    qDebug() << this << "disconnecting socket"<< sender();

    auto* socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    remove_socket(socket);
}

//------------------------------------------------------------------------------

void Tcp_connections::error_cb(QAbstractSocket::SocketError socket_error)
{
    if(!sender()) return;
    qDebug() << this << "error in socket" << sender() << " error = " << socket_error;

    auto* socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    remove_socket(socket);
}

//------------------------------------------------------------------------------

void Tcp_connections::start_cb()
{
    qDebug() << this << "connections started on"
             << QThread::currentThread();
}

//------------------------------------------------------------------------------

void Tcp_connections::quit_cb()
{
    if(!sender()) return;
    qDebug() << this << "connections quitting";

    for (auto& elem : connections) {
        if (!elem.second) continue;
        const auto& psocket = elem.second->get_socket();
        if(psocket->isOpen()) {
            psocket->disconnect();
            psocket->close();
        }
    }
    connections.clear();
    qDebug() << this << "finishing";
    emit finished();
}

//------------------------------------------------------------------------------

void Tcp_connections::accept_cb(qintptr handle)
try {
    qDebug() << "*** HEY WATCH THIS";
    qDebug() << "handle " << handle;

    auto connection = std::make_unique<Tcp_connecton>(handle);
    auto socket = connection->get_socket().get();

    connect(socket, &QTcpSocket::disconnected,
            this, &Tcp_connections::disconnected_cb);
    connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this, &Tcp_connections::error_cb);

    connections.insert(std::make_pair(socket, std::move(connection)));

    qDebug() << this << "clients = " << connections.size();
    emit socket->connected();
}
catch(Bad_tcp_connection& b) {
    qWarning() << this << "could not accept connection" << handle;
}

//------------------------------------------------------------------------------
