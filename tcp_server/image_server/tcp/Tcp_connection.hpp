//------------------------------------------------------------------------------

#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

//------------------------------------------------------------------------------

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <memory>
#include <QtCore>
#include <QBuffer>
#include <QImage>

//------------------------------------------------------------------------------

class Bad_tcp_connection {};

//------------------------------------------------------------------------------

class Tcp_connecton : public QObject {
    Q_OBJECT
public:
    explicit Tcp_connecton(qintptr handle,
                           QObject* parent = nullptr);

    virtual ~Tcp_connecton();

protected:
    QTcpSocket* get_socket() const;

public slots:
    virtual void connected_cb();
    virtual void disconnected_cb();
    virtual void ready_read_cb();
    virtual void bytes_written_cb(qint64 bytes);
    virtual void state_changed_cb(QAbstractSocket::SocketState socketState);
    virtual void error_cb(QAbstractSocket::SocketError socketError);

private:
    QDataStream in;
    std::unique_ptr<QTcpSocket> up_socket;
};

//------------------------------------------------------------------------------

#endif // TCP_CONNECTION_HPP

//------------------------------------------------------------------------------
