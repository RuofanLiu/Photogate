
#ifndef DIALOG_H

#define DIALOG_H



#include <QDateTime>

#include <QElapsedTimer>

#include <QTime>

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

    void receivedData(double val, double oldData);

    void clearGraph();



private:

    void setControlsEnabled(bool enable);

    QByteArray serialBuffer;

    volatile double lastSerialValue;



signals:

    void newData(double data, double oldData);

    //void timeData(int data);



private:

    QString portName;

    QTime dataTimer;

    QComboBox *serialPortComboBox;

    QPushButton *runButton;

    QPushButton *clearButton;

    QCustomPlot *customPlot;

    QLineEdit *lineEdit;

    QLineEdit *lineEdit2;

    QLineEdit *lineEdit3;

    QLineEdit *lineEdit4;

    double previousValue = 0;

    double startTime = 0;

    double previousTime = 0;

    double currentTime = 0;

    double duration = 0;

    bool jumped = false;

    bool connected = false;

    double temp = 0;

    qint64 numSteps = 0;

    double totalJumpHeight = 0;
};



#endif // DIALOG_H
