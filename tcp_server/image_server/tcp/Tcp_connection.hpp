//------------------------------------------------------------------------------

#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

//------------------------------------------------------------------------------
// Qt
#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QtCore>

//------------------------------------------------------------------------------
// Local
#include "../../common_lib/Protocol.hpp"

//------------------------------------------------------------------------------

namespace Receiver {

//------------------------------------------------------------------------------

class Bad_tcp_connection {};

//------------------------------------------------------------------------------

class Tcp_connecton : public QObject {
    Q_OBJECT
public:
    explicit Tcp_connecton(qintptr handle,
                           QObject* parent = nullptr);

    virtual ~Tcp_connecton()
        { qDebug() << this << "Destroyed"; }

    const deleted_unique_ptr<QTcpSocket>& get_socket() const
        { return psocket; }

public slots:
    virtual void connected_cb();
    virtual void disconnected_cb();
    virtual void ready_read_cb();
    virtual void bytes_written_cb(qint64 bytes);
    virtual void state_changed_cb(QAbstractSocket::SocketState socket_state);
    virtual void error_cb(QAbstractSocket::SocketError socket_error);

private:
    deleted_unique_ptr<QTcpSocket> psocket;
    QDataStream in;
};

//------------------------------------------------------------------------------

}   // Receiver

//------------------------------------------------------------------------------

#endif // TCP_CONNECTION_HPP

//------------------------------------------------------------------------------
