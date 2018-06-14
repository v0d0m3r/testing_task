//------------------------------------------------------------------------------

#ifndef TCP_SERVER_DIALOG_HPP
#define TCP_SERVER_DIALOG_HPP

//------------------------------------------------------------------------------
// Qt
#include <QDialog>

//------------------------------------------------------------------------------
// Local
#include "Tcp_server.hpp"

//------------------------------------------------------------------------------

namespace Ui {
class Tcp_server_dialog;
}

//------------------------------------------------------------------------------

namespace Receiver {

//------------------------------------------------------------------------------

class Tcp_server_dialog : public QDialog {
    Q_OBJECT
public:
    explicit Tcp_server_dialog(QWidget* parent = 0);
    ~Tcp_server_dialog();

private slots:
    void start_cb();
    void stop_cb();

private:
    Tcp_server image_server;
    Ui::Tcp_server_dialog* ui;

    void set_started(bool started);
};

//------------------------------------------------------------------------------

}   // Receiver

//------------------------------------------------------------------------------

#endif // TCP_SERVER_DIALOG_HPP

//------------------------------------------------------------------------------
