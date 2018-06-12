//------------------------------------------------------------------------------

#ifndef TRANSMIT_IMAGE_DIALOG_HPP
#define TRANSMIT_IMAGE_DIALOG_HPP

//------------------------------------------------------------------------------
// Qt
#include <QDataStream>
#include <QTcpSocket>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkSession>

//------------------------------------------------------------------------------
// std
#include "Common_facilities.hpp"

//------------------------------------------------------------------------------

namespace Imager {

//------------------------------------------------------------------------------

class Transmit_image_dialog : public QDialog {
    Q_OBJECT
public:
    Transmit_image_dialog(const QImage& image, int compress_lvl,
                          QWidget* parent = nullptr);
private slots:
    void send_image_cb();
    void read_answer_cb();
    void display_error_cb(QAbstractSocket::SocketError socket_error);
    void enable_send_image_button_cb();
    void session_opened_cb();

private:
    const QImage& img;
    int compression_level;

    Ref_unique_ptr<QComboBox> host_combo;
    Ref_unique_ptr<QLineEdit> port_ledit;
    Ref_unique_ptr<QLabel> status_label;
    Ref_unique_ptr<QPushButton> send_image_button;

    Ref_unique_ptr<QTcpSocket> tcp_socket;
    Ref_unique_ptr<QNetworkSession> network_session;
};

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // TRANSMIT_IMAGE_DIALOG_HPP

//------------------------------------------------------------------------------
