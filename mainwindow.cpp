#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBoxCommand->addItem(tr("Read AI"), SixnetMessage::SixnetMessageReadAnalog);
    ui->comboBoxCommand->addItem(tr("Read DI"), SixnetMessage::SixnetMessageReadDigital);

    sixnetMessageHandler = new SixnetMessageDispatcher(
                QHostAddress(ui->lineEditIp->text()),
                ui->spinBoxPort->value(),
                1500,
                this);
    connect(sixnetMessageHandler, SIGNAL(debug(QString)), this, SLOT(debugSlot(QString)));

    connect(&m_queryTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_queryTimer.setSingleShot(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*void MainWindow::on_pushButtonBlink_toggled(bool checked)
{
    unsigned char data[16] = {
        0x7d, // Lead Character -> } fixed CRC
        0x0e, // size
        0xf1, // dst address (station number)
        0xf0, // src address
        0xc0, // session
        0x74, // seq num
        0xd0, // cmd
        0x01, // io type -> discrete output 1 bit
        0x00,
        0x00,
        0xb0, // SN H
        0x2c, // SN L
        0x00,
        0x00, // seems to be cmd (2 -> wink on. 0 -> wink off)
        0x1d, // checksum?
        0x0f  // checksum?
    };

    if (checked) {
        data[13] = 02;
    }
    udpSocket->write((const char*)data, 16);
}
*/

void MainWindow::on_pushButtonConnect_clicked()
{
    if (sixnetMessageHandler->connected())
        sixnetMessageHandler->connectToHost();
    else
        sixnetMessageHandler->disconnectFromHost();
}

void MainWindow::on_pushButtonQuery_clicked()
{
    SixnetMessage *msg = new SixnetMessage(
                            ui->spinBoxStationNumber->value(),
                            (SixnetMessage::SixnetMessageType)ui->comboBoxCommand->itemData(ui->comboBoxCommand->currentIndex()).toInt(),
                            ui->spinBoxSize->value(),
                            this);
    sixnetMessageHandler->sendMessage(msg);
    connect(msg, SIGNAL(dataReaded()), this, SLOT(messageDataReaded()));

    sentMessages.append(msg);
}

void MainWindow::on_lineEditIp_editingFinished()
{
    QHostAddress address;
    if (address.setAddress(ui->lineEditIp->text()))
        sixnetMessageHandler->setHostAddress(address);
    else
        QMessageBox::warning(this, tr("Invalid IP"), tr("The %1 is not a valid IP address").arg(ui->lineEditIp->text()));
}

void MainWindow::messageDataReaded()
{
    SixnetMessage *msg = NULL;
    for (int i = 0; i<sentMessages.size(); i++) {
        if (sentMessages.at(i) == this->sender()) {
            msg = sentMessages.takeAt(i);
            break;
        }
    }

    ui->textEditCommandsResults->clear();
    if (msg != NULL) {
        QString chPrefix = "";
        switch (msg->messageType()) {
        case SixnetMessage::SixnetMessageReadAnalog:
            chPrefix = tr("AI");
            break;
        case SixnetMessage::SixnetMessageReadDigital:
            chPrefix = tr("DI");
            break;
        }

        for (int i = 0; i<msg->channelCount(); i++) {
            ui->textEditCommandsResults->append(tr("%1%2: %3")
                                 .arg(chPrefix)
                                 .arg(i)
                                 .arg(msg->channelData(i)));
        }
    }

    msg->deleteLater();
    if (ui->checkBoxAIPeriodicQuery->isChecked())
        m_queryTimer.start();
}

void MainWindow::on_checkBoxAIPeriodicQuery_toggled(bool checked)
{
    if (checked)
        m_queryTimer.start();
    else
        m_queryTimer.stop();
}

void MainWindow::on_doubleSpinBoxAIQueryInterval_valueChanged(double arg1)
{
    m_queryTimer.setInterval(arg1 * 1000);
}

void MainWindow::timeout()
{
    on_pushButtonQuery_clicked();
}

void MainWindow::debugSlot(QString debugText)
{
    if (ui->checkBoxDebug->isChecked()) {
        ui->textEditDebug->append(debugText);
    }
}
