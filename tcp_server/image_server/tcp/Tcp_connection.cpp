//------------------------------------------------------------------------------

#include "Tcp_connection.hpp"

//------------------------------------------------------------------------------

namespace Receiver {

//------------------------------------------------------------------------------

Tcp_connecton::Tcp_connecton(qintptr handle, QObject *parent)
    : QObject(parent),
      psocket(new QTcpSocket(), [] (QTcpSocket* p) { p->deleteLater(); })
{
    if(!psocket->setSocketDescriptor(handle))
        throw Bad_tcp_connection();

    in.setDevice(psocket.get());
    in.setVersion(QDataStream::Qt_4_0);

    connect(psocket.get(), &QTcpSocket::connected,
            this, &Tcp_connecton::connected_cb);
    connect(psocket.get(), &QTcpSocket::disconnected,
            this, &Tcp_connecton::disconnected_cb);
    connect(psocket.get(), &QTcpSocket::readyRead,
            this, &Tcp_connecton::ready_read_cb);
    connect(psocket.get(), &QTcpSocket::bytesWritten,
            this, &Tcp_connecton::bytes_written_cb);
    connect(psocket.get(), &QTcpSocket::stateChanged,
            this, &Tcp_connecton::state_changed_cb);
    connect(psocket.get(), static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this, &Tcp_connecton::error_cb);

    qDebug() << this << "Created";
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
    qDebug() << this << " disconnected_cb " << psocket.get();
}

//------------------------------------------------------------------------------

void Tcp_connecton::ready_read_cb()
{
    if(!sender()) return;
    qDebug() << this << " readyRead " << psocket.get();

    in.startTransaction();

    Byte stx;
    in >> stx;
    Byte answer = manage_codes[to_int(Mng_code::nak)];
    if (stx == manage_codes[to_int(Mng_code::stx)]) {
        Byte cmd_code;
        in >> cmd_code;
        switch (cmd_code) {
        case imager_comand_codes[to_int(Imager_codes::send_image)]:
        {
            QByteArray block;
            in >> block;

            if (!in.commitTransaction())
                return;

            qDebug() << block.size();

            QImage image;
            image.loadFromData(qUncompress(std::move(block)));
            if (image.isNull())
                qDebug("The image is null. Something failed.");
            else {
                QString path = "test.jpg";
                for  (int counter = 0; QFile::exists(path); ++counter)
                    path = QString("test%1.jpg").arg(QString::number(counter));

                image.save(path);
                answer = manage_codes[to_int(Mng_code::ack)];
            }
            break;
        }
        default:
            qDebug("This command is not exist.");
            break;
        }
    }
    if (!in.commitTransaction())
        return;

    QDataStream out(psocket.get());
    out << answer;
}

//------------------------------------------------------------------------------

void Tcp_connecton::bytes_written_cb(qint64 bytes)
{
    if(!sender()) return;
    qDebug() << this
             << " bytesWritten "      << psocket.get()
             << " number of bytes = " << bytes;
    psocket->close();
}

//------------------------------------------------------------------------------

void Tcp_connecton::state_changed_cb(QAbstractSocket::SocketState socket_state)
{
    if(!sender()) return;
    qDebug() << this
             << " stateChanged " << psocket.get()
             << " state = "      << socket_state;
}

//------------------------------------------------------------------------------

void Tcp_connecton::error_cb(QAbstractSocket::SocketError socket_error)
{
    if(!sender()) return;
    qDebug() << this
             << " error "   << psocket.get()
             << " error = " << socket_error;
}

//------------------------------------------------------------------------------

}   // Receiver

//------------------------------------------------------------------------------
