#include "Tcp_connection.hpp"

//------------------------------------------------------------------------------

Tcp_connecton::Tcp_connecton(qintptr handle, QObject *parent)
    : QObject(parent),
      up_socket(std::make_unique<QTcpSocket>())
{
    if(!up_socket->setSocketDescriptor(handle))
        throw Bad_tcp_connection();

    qDebug() << this << "Created";
}

//------------------------------------------------------------------------------

Tcp_connecton::~Tcp_connecton()
{
    qDebug() << this << "Destroyed";
}

//------------------------------------------------------------------------------

void Tcp_connecton::set_socket(QTcpSocket* socket)
{
    up_socket = socket;
    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_0);
    connect(up_socket,&QTcpSocket::connected_cb, this, &Tcp_connecton::connected_cb);
    connect(up_socket,&QTcpSocket::disconnected_cb, this, &Tcp_connecton::disconnected_cb);
    connect(up_socket,&QTcpSocket::readyRead, this, &Tcp_connecton::ready_read_cb);
    connect(up_socket,&QTcpSocket::bytesWritten, this, &Tcp_connecton::bytes_written_cb);
    connect(up_socket,&QTcpSocket::stateChanged, this, &Tcp_connecton::state_changed_cb);
    connect(up_socket,static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this,&Tcp_connecton::error);
}

//------------------------------------------------------------------------------

QTcpSocket* Tcp_connecton::get_socket()
{
    if(!sender()) return 0;
    return static_cast<QTcpSocket*>(sender());
}

//------------------------------------------------------------------------------

void Tcp_connecton::connected_cb()
{
    if(!sender()) return;
    qDebug() << this << " connected_cb " << sender();
}

//------------------------------------------------------------------------------

void Tcp_connecton::disconnected_cb()
{
    if(!sender()) return;
    qDebug() << this << " disconnected_cb " << getSocket();
}

//------------------------------------------------------------------------------

void Tcp_connecton::ready_read_cb()
{
    if(!sender()) return;
    QTcpSocket* socket = getSocket();
    if(!socket) return;

    qDebug() << this << " readyRead " << socket;

    in.startTransaction();

    Byte stx;
    in >> stx;

    if (stx == manage_codes[to_int(Mng_code::stx)]) {
        Code_comm cmd_code;
        in >> cmd_code;

        switch (cmd_code) {
        case Code_comm::image_exch:
        {
            QString name;
            QByteArray buffer;
            in >> name >> buffer;

            QImage image;
            image.loadFromData(std::move(buffer));
            if (image.isNull()) {
                socket->close();
                qDebug("The image is null. Something failed.");
            }
            else {

                image.save(name, "JPG", 100);
            }
        }
        default:
            if (!in.commitTransaction())
                return;
            socket->close();
            break;
        }
    }
    if (!in.commitTransaction())
        return;
}

//------------------------------------------------------------------------------

void Tcp_connecton::bytesWritten(qint64 bytes)
{
    if(!sender()) return;
    qDebug() << this << " bytesWritten " << getSocket() << " number of bytes = " << bytes;
}

//------------------------------------------------------------------------------

void Tcp_connecton::state_changed_cb(QAbstractSocket::SocketState socketState)
{
    if(!sender()) return;
    qDebug() << this << " stateChanged " << getSocket() << " state = " << socketState;
}

//------------------------------------------------------------------------------

void Tcp_connecton::error(QAbstractSocket::SocketError socketError)
{
    if(!sender()) return;
    qDebug() << this << " error " << getSocket() << " error = " << socketError;
}

//------------------------------------------------------------------------------
