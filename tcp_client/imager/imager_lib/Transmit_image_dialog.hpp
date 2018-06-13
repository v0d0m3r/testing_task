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
#include <QDialog>

//------------------------------------------------------------------------------
// std
#include "../../common_lib/Common_facilities.hpp"

//------------------------------------------------------------------------------

namespace Imager {

//------------------------------------------------------------------------------

struct Transmit_image_widget : public QWidget {
    Ref_unique_ptr<QSpinBox> compress_spin_box;
    Ref_unique_ptr<QComboBox> host_combo;
    Ref_unique_ptr<QLineEdit> port_ledit;
    Ref_unique_ptr<QLabel> status_label;
    Ref_unique_ptr<QPushButton> send_image_button;
    Ref_unique_ptr<QPushButton> quit_button;

    explicit Transmit_image_widget(QWidget* parent = nullptr);
};

//------------------------------------------------------------------------------

class Transmit_image_dialog : public QDialog {
    Q_OBJECT
public:
    Transmit_image_dialog(const QImage& image,
                          const QString& name_image,
                          QWidget* parent = nullptr);

    int compression_level() const
        { return content.ref().compress_spin_box.ref().value(); }

private slots:
    void send_image_cb();
    void read_answer_cb();
    void display_error_cb(QAbstractSocket::SocketError socket_error);
    void enable_send_image_button_cb();
    void session_opened_cb();

private:
    const QImage& img;
    const QString& name_img;

    Ref_unique_ptr<Transmit_image_widget> content;

    Ref_unique_ptr<QTcpSocket> tcp_socket;
    Ref_unique_ptr<QNetworkSession> network_session;
};

//------------------------------------------------------------------------------

}   // Imager

//------------------------------------------------------------------------------

#endif // TRANSMIT_IMAGE_DIALOG_HPP

//------------------------------------------------------------------------------
