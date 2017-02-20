#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <ui_dialog.h>
//#include "masterthread.h"
#include <QTimer>
#include "qcustomplot.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
namespace Ui {
//class Dialog;
class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QComboBox;
}

class Dialog : public QDialog
{
    Q_OBJECT
    QSerialPort serial;

public:
    explicit Dialog(QWidget *parent = 0);
    friend class MasterThread;
    QString data;

private slots:
    void transaction();
    void handleReadyRead();
    void receivedData(double val);
    void clearGraph();

private:
    void setControlsEnabled(bool enable);
    QByteArray serialBuffer;
    volatile double lastSerialValue;

signals:
    void newData(double data);

private:
    QString portName;
    QTimer dataTimer;
    QComboBox *serialPortComboBox;
    QPushButton *runButton;
    QPushButton *clearButton;
    QCustomPlot *customPlot;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit2;
    bool connected = false;
};

#endif // DIALOG_H
