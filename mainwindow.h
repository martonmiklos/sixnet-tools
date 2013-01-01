#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>

#include "sixnetmessagehandler.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButtonConnect_clicked();

    void on_pushButtonQuery_clicked();

    void on_lineEditIp_editingFinished();
    void messageDataReaded();
    void on_checkBoxAIPeriodicQuery_toggled(bool checked);
    void timeout();
    void debugSlot(QString debugText);
    void on_doubleSpinBoxAIQueryInterval_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
    SixnetMessageDispatcher *sixnetMessageHandler;
    QList<SixnetMessage*> sentMessages;
    QTimer m_queryTimer;
};

#endif // MAINWINDOW_H
