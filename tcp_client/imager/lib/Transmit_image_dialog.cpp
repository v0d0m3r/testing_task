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
// local
#include "Transmit_image_dialog.hpp"

//------------------------------------------------------------------------------

namespace Imager {

Transmit_image_dialog::Transmit_image_dialog(const QImage& image,
                                             int compress_lvl, QWidget* parent)
    : QDialog(parent),
      img(image),
      compression_level(compress_lvl),
      host_combo(new QComboBox()),
      port_ledit(new QLineEdit()),
      status_label(new QLabel()),
      send_image_button(new QPushButton(tr("Send"))),
      tcp_socket(new QTcpSocket()),
      network_session(new QNetworkSession())
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    host_combo->setEditable(true);
    const auto& name = QHostInfo::localHostName();
    if (!name.isEmpty()) {
        host_combo->addItem(name);
        const auto& domain = QHostInfo::localDomainName();
        if (!domain.isEmpty())
            host_combo->addItem(name + QChar('.') + domain);
    }
    if (name != QLatin1String("localHost"))
        host_combo->addItem(QString("localhost"));
    const auto& ip_addr_list = QNetworkInterface::allAddresses();
    for (const auto& ip : ip_addr_list)
        if (ip.isLoopback())
            host_combo->addItem(ip.toString());

    port_ledit->setValidator(new QIntValidator(1, 65535, this));

    auto host_label = new QLabel(tr("&Server name:"));
    host_label->setBuddy(host_combo);

    auto port_label = new QLabel(tr("S&erver port"));
    port_label->setBuddy(port_ledit);

    status_label = new QLabel(tr("This examples requires that you run the "
                                 "Fortune Server example as well."));

    send_image_button->setDefault(true);
    send_image_button->setEnabled(false);

    auto quit_button = new QPushButton(tr("Quit"));
    auto button_box = new QDialogButtonBox;
    button_box->addButton(send_image_button, QDialogButtonBox::ActionRole);
    button_box->addButton(quit_button, QDialogButtonBox::RejectRole);

    in.setDevice(tcp_socket);
    in.setVersion(QDataStream::Qt_5_10);

    connect(host_combo, &QComboBox::editTextChanged,
            this, &Transmit_image_dialog::enable_send_image_button_cb);
    connect(port_ledit, &QLineEdit::textChanged,
            this, &Transmit_image_dialog::enable_send_image_button_cb);
    connect(send_image_button, &QAbstractButton::clicked,
            this, &Transmit_image_dialog::send_image_cb);
    connect(quit_button, &QAbstractButton::clicked,
            this, &QWidget::close);
    connect(tcp_socket, &QIODevice::readyRead, this, &Transmit_image_dialog::read_answer_cb);
    connect(tcp_socket,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &Transmit_image_dialog::display_error_cb);

    QGridLayout* main_layout;
    if (       QGuiApplication::styleHints()->showIsFullScreen()
            || QGuiApplication::styleHints()->showIsMaximized()) {
        auto outer_vertical_layout = new QVBoxLayout(this);
        outer_vertical_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        auto outer_horizontal_layout = new QHBoxLayout;
        outer_horizontal_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        auto groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        main_layout = new QGridLayout(groupBox);
        outer_horizontal_layout->addWidget(groupBox);
        outer_horizontal_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        outer_vertical_layout->addLayout(outer_horizontal_layout);
        outer_vertical_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    }
    else
        main_layout = new QGridLayout(this);

    main_layout->addWidget(host_label, 0, 0);
    main_layout->addWidget(host_combo, 0, 1);
    main_layout->addWidget(port_label, 1, 0);
    main_layout->addWidget(port_ledit, 1, 1);
    main_layout->addWidget(status_label, 2, 0, 1, 2);
    main_layout->addWidget(button_box, 3, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    port_ledit->setFocus();

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

        network_session = new QNetworkSession(config, this);
        connect(network_session, &QNetworkSession::opened,
                this, &Transmit_image_dialog::session_opened_cb);

        send_image_button->setEnabled(false);
        status_label->setText(tr("Opening network session."));
        network_session->open();
    }
}

//------------------------------------------------------------------------------

void Transmit_image_dialog::send_image_cb()
{
    send_image_button->setEnabled(false);
    tcp_socket->abort();
    tcp_socket->connectToHost(host_combo->currentText(),
                              port_ledit->text().toInt());
    QImage img_obj;
    Q_ASSERT(img_obj.load("./test2.png"));

    QByteArray block;
    QBuffer buff(&block);
    img_obj.save(&buff, "PNG");

    QDataStream out(tcp_socket);
    out.setVersion(QDataStream::Qt_4_0);

    QString file_name = QLatin1String("./test5.png");

    out << file_name;

    /*if (tcp_socket->write(ba) < block.size())
        qWarning("transmit error!");*/
}

//------------------------------------------------------------------------------

void Transmit_image_dialog::read_answer_cb()
{
    QImage img_obj;
    in >> img_obj;

    if (img_obj.isNull())
        qDebug("Received Image is empty");

    Q_ASSERT(img_obj.save("./test2.png"));

    /*current_fortune = next_fortune;
    status_label->setText(current_fortune);*/
    send_image_button->setEnabled(true);
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
                                 .arg(tcp_socket->errorString()));
    }

    send_image_button->setEnabled(true);
}

//------------------------------------------------------------------------------

void Transmit_image_dialog::enable_send_image_button_cb()
{
    send_image_button->setEnabled(
                (!network_session || network_session->isOpen()) &&
                !host_combo->currentText().isEmpty() &&
                !port_ledit->text().isEmpty());
}

//------------------------------------------------------------------------------

void Transmit_image_dialog::session_opened_cb()
{
    // Save the used configuration
    QNetworkConfiguration config = network_session->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = network_session->sessionProperty(
                    QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    status_label->setText(tr("This examples requires that you run the "
                            "Fortune Server example as well."));

    enable_send_image_button_cb();
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------
