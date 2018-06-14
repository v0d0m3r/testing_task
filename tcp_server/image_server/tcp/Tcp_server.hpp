//------------------------------------------------------------------------------

#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

//------------------------------------------------------------------------------
// Qt
#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QThread>

//------------------------------------------------------------------------------
// Local
#include "Tcp_connections.hpp"
#include "Tcp_connection.hpp"

//------------------------------------------------------------------------------

namespace Receiver {

//------------------------------------------------------------------------------

class Tcp_server : public QTcpServer {
    Q_OBJECT
public:
    explicit Tcp_server(QObject* parent = nullptr);
    ~Tcp_server();

    virtual bool listen(const QHostAddress& address, quint16 port);
    virtual void close();
    virtual qint64 port();

protected:
    QThread* thread = nullptr;
    Tcp_connections* connections = nullptr;
    virtual void incomingConnection(qintptr descriptor) override;
    virtual void accept(qintptr descriptor);

signals:
    void accepting(qintptr handle);
    void finished();

public slots:
    void complete_cb();
};

//------------------------------------------------------------------------------

}   // Receiver

//------------------------------------------------------------------------------

#endif // TCP_SERVER_HPP
