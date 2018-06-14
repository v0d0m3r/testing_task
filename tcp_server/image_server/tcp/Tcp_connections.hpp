//------------------------------------------------------------------------------

#ifndef TCP_CONNECTIONS_HPP
#define TCP_CONNECTIONS_HPP

//------------------------------------------------------------------------------
// Qt
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTcpSocket>
#include <QMap>
#include <QReadWriteLock>

//------------------------------------------------------------------------------
// Local
#include "Tcp_connection.hpp"

//------------------------------------------------------------------------------

namespace Receiver {

//------------------------------------------------------------------------------

using Connection_tb = std::map<QTcpSocket*, std::unique_ptr<Tcp_connecton>>;

//------------------------------------------------------------------------------

class Tcp_connections : public QObject {
    Q_OBJECT
public:
    explicit Tcp_connections(QObject* parent = nullptr);
    virtual ~Tcp_connections();

    virtual int count() const;

protected:
    Connection_tb connections;
    void remove_socket(QTcpSocket* socket);

signals:
    void quitting();
    void finished();

protected slots:
    void disconnected_cb();
    void error_cb(QAbstractSocket::SocketError socket_error);

public slots:
    void start_cb();
    void quit_cb();
    void accept_cb(qintptr handle);
};

//------------------------------------------------------------------------------

}   // Receiver

//------------------------------------------------------------------------------

#endif // TCP_CONNECTIONS_HPP

//------------------------------------------------------------------------------
