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
#include <QHostInfo>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QtGui>
#include <QtCore>
#include <QNetworkConfigurationManager>
#include <QGroupBox>
#include <QMessageBox>

//------------------------------------------------------------------------------
// local
#include "Transmit_image_dialog.hpp"
#include "../../common_lib/Protocol.hpp"

//------------------------------------------------------------------------------

namespace Imager {

//------------------------------------------------------------------------------

Transmit_image_widget::Transmit_image_widget(QWidget* parent)
    : QWidget(parent),
      compress_spin_box(new QSpinBox()),
      host_combo(new QComboBox()),
      port_ledit(new QLineEdit()),
      status_label(new QLabel(tr("This programs requires that you run the "
                                 "Server as well."))),
      send_image_button(new QPushButton(tr("Send"))),
      quit_button(new QPushButton(tr("Quit")))
{
    host_combo.ref().setEditable(true);
    port_ledit.ref().setValidator(new QIntValidator(1, 65535, this));

    send_image_button.ref().setDefault(true);
    send_image_button.ref().setEnabled(false);

    compress_spin_box.ref().setRange(-1, 9);
    compress_spin_box.ref().setValue(-1);
    auto compress_label = std::make_unique<QLabel>(tr("&Compression level:"));
    compress_label->setBuddy(compress_spin_box.uptr().get());

    auto host_label = std::make_unique<QLabel>(tr("&Server name:"));
    host_label->setBuddy(host_combo.uptr().get());

    auto port_label = std::make_unique<QLabel>(tr("S&erver port:"));
    port_label->setBuddy(port_ledit.uptr().get());

    auto button_box = std::make_unique<QDialogButtonBox>();
    button_box->addButton(do_release(send_image_button), QDialogButtonBox::ActionRole);
    button_box->addButton(do_release(quit_button), QDialogButtonBox::RejectRole);

    auto main_layout = std::make_unique<QGridLayout>();

    main_layout->addWidget(compress_label.release(), 0, 0);
    main_layout->addWidget(do_release(compress_spin_box), 0, 1);
    main_layout->addWidget(host_label.release(), 1, 0);
    main_layout->addWidget(do_release(host_combo), 1, 1);
    main_layout->addWidget(port_label.release(), 2, 0);
    main_layout->addWidget(do_release(port_ledit), 2, 1);
    main_layout->addWidget(do_release(status_label), 3, 0, 1, 2);
    main_layout->addWidget(button_box.release(), 4, 0, 1, 2);

    setLayout(main_layout.release());
}

//------------------------------------------------------------------------------

Transmit_image_dialog::Transmit_image_dialog(const QImage& image,
                                             QWidget* parent)
    : QDialog(parent),
      img(image),
      content(new Transmit_image_widget()),
      tcp_socket(new QTcpSocket()),
      network_session(nullptr)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    const auto& name = QHostInfo::localHostName();
    if (!name.isEmpty()) {
        content.ref().host_combo.ref().addItem(name);
        const auto& domain = QHostInfo::localDomainName();
        if (!domain.isEmpty())
            content.ref().host_combo.ref().addItem(name + QChar('.') + domain);
    }
    if (name != QLatin1String("localHost"))
        content.ref().host_combo.ref().addItem(QString("localhost"));
    const auto& ip_addr_list = QNetworkInterface::allAddresses();
    for (const auto& ip : ip_addr_list)
        if (ip.isLoopback())
            content.ref().host_combo.ref().addItem(ip.toString());

    connect(&content.ref().host_combo.ref(), &QComboBox::editTextChanged,
            this, &Transmit_image_dialog::enable_send_image_button_cb);
    connect(&content.ref().port_ledit.ref(), &QLineEdit::textChanged,
            this, &Transmit_image_dialog::enable_send_image_button_cb);
    connect(&content.ref().send_image_button.ref(), &QAbstractButton::clicked,
            this, &Transmit_image_dialog::send_image_cb);
    connect(&content.ref().quit_button.ref(), &QAbstractButton::clicked,
            this, &QWidget::close);
    connect(&tcp_socket.ref(), &QIODevice::readyRead, this, &Transmit_image_dialog::read_answer_cb);
    connect(&tcp_socket.ref(), QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &Transmit_image_dialog::display_error_cb);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    content.ref().port_ledit.ref().setFocus();

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered)
            config = manager.defaultConfiguration();

        network_session.uptr().reset(new QNetworkSession(config));
        connect(&network_session.ref(), &QNetworkSession::opened,
                this, &Transmit_image_dialog::session_opened_cb);

        content.ref().send_image_button.ref().setEnabled(false);
        content.ref().status_label.ref().setText(tr("Opening network session."));
        network_session.ref().open();
    }

    auto main_layout = new QVBoxLayout();
    main_layout->addWidget(do_release(content));
    setLayout(main_layout);
}

//------------------------------------------------------------------------------

void Transmit_image_dialog::send_image_cb()
{
    content.ref().send_image_button.ref().setEnabled(false);
    //tcp_socket.ref().abort();
    tcp_socket.ref().connectToHost(content.ref().host_combo.ref().currentText(),
                                   content.ref().port_ledit.ref().text().toInt());

    QByteArray block;
    QBuffer buff(&block);
    buff.open(QIODevice::WriteOnly);
    img.save(&buff, "png");

    QDataStream out(&tcp_socket.ref());
    out.setVersion(QDataStream::Qt_4_0);

    qDebug() << block.size();

    out << manage_codes[to_int(Mng_code::stx)]
        << imager_comand_codes[to_int(Imager_codes::send_image)]
        << qCompress(std::move(block), compression_level());

    content.ref().status_label.ref().setText(tr("Process sending..."));
}

//------------------------------------------------------------------------------

void Transmit_image_dialog::read_answer_cb()
{
    Byte answer = 0;
    QDataStream in;
    in.setDevice(&tcp_socket.ref());
    in >> answer;

    switch (answer) {
    case manage_codes[to_int(Mng_code::ack)]:
        QMessageBox::information(this, tr("Transmit image dialog"),
                                 tr("Sending successful!"));
        break;
    case manage_codes[to_int(Mng_code::nak)]:
        QMessageBox::information(this, tr("Transmit image dialog"),
                                 tr("Bad data of sending!"));
        break;
    default:
        QMessageBox::information(this, tr("Transmit image dialog"),
                                 tr("Recived no correct command!"));
        break;
    }
    close();
}

//------------------------------------------------------------------------------

void Transmit_image_dialog::display_error_cb(QAbstractSocket::SocketError socket_error)
{
    switch (socket_error) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Transmit_image_dialog"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Transmit_image_dialog"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Transmit_image_dialog"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcp_socket.ref().errorString()));
    }

    content.ref().send_image_button.ref().setEnabled(true);
}

//------------------------------------------------------------------------------

void Transmit_image_dialog::enable_send_image_button_cb()
{
    content.ref().send_image_button.ref().setEnabled(
                (!network_session.uptr().get() || network_session.ref().isOpen()) &&
                !content.ref().host_combo.ref().currentText().isEmpty() &&
                !content.ref().port_ledit.ref().text().isEmpty());
}

//------------------------------------------------------------------------------

void Transmit_image_dialog::session_opened_cb()
{
    // Save the used configuration
    QNetworkConfiguration config = network_session.ref().configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = network_session.ref().sessionProperty(
                    QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    content.ref().status_label.ref().setText(tr("This programs requires that you run the "
                                                "Server as well."));

    enable_send_image_button_cb();
}

//------------------------------------------------------------------------------

}   // Imager

//------------------------------------------------------------------------------
