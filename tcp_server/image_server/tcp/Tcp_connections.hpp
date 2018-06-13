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
// local
#include "Tcp_connection.hpp"

//------------------------------------------------------------------------------

class Tcp_connections : public QObject
{
    Q_OBJECT
public:
    explicit Tcp_connections(QObject* parent = nullptr);
    virtual ~Tcp_connections();

    virtual int count() const;

protected:
    QMap<qintptr, Tcp_connecton> connections;
    void removeSocket(QTcpSocket *socket);

signals:
    void quitting();
    void finished();

protected slots:
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);

public slots:
    void start();
    void quit();
    void accept(qintptr handle, Tcp_connecton *connection);
};

//------------------------------------------------------------------------------

#endif // TCP_CONNECTIONS_HPP

//------------------------------------------------------------------------------
