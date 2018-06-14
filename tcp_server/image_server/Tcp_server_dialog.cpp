//------------------------------------------------------------------------------
// Ui
#include "ui_Tcp_server_dialog.h"

//------------------------------------------------------------------------------
// Local
#include "Tcp_server_dialog.hpp"

//------------------------------------------------------------------------------

namespace Receiver {

//------------------------------------------------------------------------------

Tcp_server_dialog::Tcp_server_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tcp_server_dialog)
{
    ui->setupUi(this);
    ui->stop_button->setEnabled(false);

    connect(ui->start_button, &QAbstractButton::clicked,
            this, &Tcp_server_dialog::start_cb);
    connect(ui->stop_button, &QAbstractButton::clicked,
            this, &Tcp_server_dialog::stop_cb);
}

//------------------------------------------------------------------------------

Tcp_server_dialog::~Tcp_server_dialog()
{
    delete ui;
}

//------------------------------------------------------------------------------

void Tcp_server_dialog::start_cb()
{
    if(image_server.listen(QHostAddress::Any, ui->port_spin_box->value())) {
        qDebug() << this << "server started!";
        set_started(true);
    }
    else {
        qCritical() << image_server.errorString();
        set_started(false);
    }
}

//------------------------------------------------------------------------------

void Tcp_server_dialog::stop_cb()
{
    image_server.close();
    set_started(false);
}

//------------------------------------------------------------------------------

void Tcp_server_dialog::set_started(bool started)
{
    if(started) {
        ui->start_button->setEnabled(false);
        ui->stop_button->setEnabled(true);
    }
    else {
        ui->start_button->setEnabled(true);
        ui->stop_button->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------
