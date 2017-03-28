#include "dialog.h"



#include <QElapsedTimer>

#include <QLabel>

#include <QLineEdit>

#include <QComboBox>

#include <QSpinBox>

#include <QPushButton>

#include <QGridLayout>

#include <ui_dialog.h>

#include <QtSerialPort/QSerialPortInfo>

#include <QtSerialPort/QSerialPort>

#include <QTime>

#include <QTextStream>

#include <QCoreApplication>

QT_USE_NAMESPACE



Dialog::Dialog(QWidget *parent)

    : QDialog(parent)

    , serialPortComboBox(new QComboBox())

    , runButton(new QPushButton(tr("Connect")))

    , clearButton(new QPushButton(tr("Clear")))

    , customPlot(new QCustomPlot())

    , lineEdit(new QLineEdit())

    ,lineEdit2(new QLineEdit())

    ,lineEdit3(new QLineEdit())

    ,lineEdit4(new QLineEdit())

{

    const auto infos = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : infos){

        serialPortComboBox->addItem(info.portName());

    }



    auto mainLayout = new QGridLayout;

    mainLayout->setRowMinimumHeight(65, 65);

    mainLayout->setColumnMinimumWidth(38, 38);

    mainLayout->addWidget(serialPortComboBox, 0, 1);

    mainLayout->addWidget(runButton, 0, 2, 1, 1);

    mainLayout->addWidget(clearButton, 0, 3, 1, 1);

    mainLayout->addWidget(customPlot, 2, 1, 65, 38);

    mainLayout->addWidget(lineEdit, 0, 4, 1, 1);

    mainLayout->addWidget(lineEdit2, 0, 5, 1, 1);

    mainLayout->addWidget(lineEdit3, 0, 6, 1, 1);

    mainLayout->addWidget(lineEdit4, 0, 7, 1, 1);

    setLayout(mainLayout);



    setWindowTitle(tr("Photogate"));

    serialPortComboBox->setFocus();

    lineEdit->setText("Airtime:");

    lineEdit2->setText("Jump Height:");

    lineEdit3->setText("Number of steps: ");

    lineEdit4->setText("Average height: ");

    connect(runButton, &QPushButton::clicked, this, &Dialog::transaction);

    connect(clearButton, &QPushButton::clicked, this, &Dialog::clearGraph);



    customPlot->addGraph();

    customPlot->graph(0)->setPen(QPen(QColor(255, 110, 40)));   //this line sets the color of the graph



    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);

    timeTicker->setTimeFormat("%h:%m:%s");

    customPlot->xAxis->setTicker(timeTicker);

    customPlot->axisRect()->setupFullAxesBox();

    customPlot->yAxis->setRange(0, 1023);

    customPlot->yAxis->setLabel("Jump Plate");

    customPlot->xAxis->setLabel("Time (Sec)");

    // make left and bottom axes transfer their ranges to right and top axes:

    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));

    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

}



void Dialog::clearGraph(){

    customPlot->graph(0)->data()->clear();

    lineEdit->setText("Airtime:");

    lineEdit2->setText("Jump Height:");

    lineEdit3->setText("Number of steps: ");

    lineEdit4->setText("Average height: ");

    totalJumpHeight = 0;

    numSteps = 0;
}



void Dialog::transaction(){

    if(connected == false){

        setControlsEnabled(false);

        serial.setPortName(serialPortComboBox->currentText());

        connect(&serial, &QSerialPort::readyRead, this, &Dialog::handleReadyRead, Qt::UniqueConnection);

        connect(this, &Dialog::newData, this, &Dialog::receivedData, Qt::UniqueConnection);

        if (!serial.open(QIODevice::ReadOnly)) {

            return;

        }

        serialBuffer.clear();

    }

    else if(connected == true){

        runButton->setText("Connect");

        serialPortComboBox->setEnabled(true);

        serial.close();

        connected = false;

    }

}





void Dialog::setControlsEnabled(bool enable){

    runButton->setText("Disconnect");



    connected = true;

    serialPortComboBox->setEnabled(enable);

}





void Dialog::receivedData(double val, double oldData){


    static QTime time(QTime::currentTime());

    double key = time.elapsed()/1000.0;

    static double lastPointKey = 0;

    //QTextStream(stdout) << val - oldData << endl;



    if (key-lastPointKey > 0.002) // at most add point every 2 ms

    {

        customPlot->graph(0)->addData(key, val);

        lastPointKey = key;

        customPlot->xAxis->setRange(key, 8, Qt::AlignRight);

        customPlot->replot();

        static double lastFpsKey;

        static int frameCount;

        ++frameCount;

        if (key-lastFpsKey > 2) // average fps over 2 seconds

        {

            lastFpsKey = key;

            frameCount = 0;

        }

    }

    if(val - oldData > 100){

        dataTimer.start();

    }

    if(oldData - val > 80){


        duration = dataTimer.elapsed();

        double height = 0.5 * 9.8 * qPow((duration/1000), 2);

        lineEdit->setText("Airtime: " + QString::number(duration/1000));

        lineEdit2->setText("Jump Height: " + QString::number(height));

        totalJumpHeight += height;

        numSteps++;

        lineEdit3->setText("Number of Steps: " + QString::number(numSteps/3));

        lineEdit4->setText("Average height: " + QString::number(totalJumpHeight/(numSteps)));

    }

}



void Dialog::handleReadyRead(){

    QString temp;

    temp = serial.readAll();

    serialBuffer.append(temp);

    bool ok = false;

    int serPos = serialBuffer.indexOf('\n');

    double currentValue = QString::fromLatin1(serialBuffer.left(serPos)).toDouble(&ok);

    while ((serPos = serialBuffer.indexOf('\n')) >= 0)

    {



        currentValue = QString::fromLatin1(serialBuffer.left(serPos)).toDouble(&ok);

        if (ok){

            emit newData(currentValue, previousValue);

        }

        previousValue = currentValue;

        serialBuffer = serialBuffer.mid(serPos+1);

        ok = false;

    }

}
