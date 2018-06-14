#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "Tcp_server.hpp"

using namespace Receiver;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_btnStart_clicked();

    void on_btnStop_clicked();

private:
    Ui::Dialog *ui;
    Tcp_server m_server;
    void setStarted(bool started);
};

#endif // DIALOG_H
