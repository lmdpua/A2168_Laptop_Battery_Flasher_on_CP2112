#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "types.h"
#include "Library\Windows\SLABCP2112.h"
#include <QLabel>
#include <QMessageBox>

#include <QModelIndex>
#include <QAbstractItemView>
#include "A2168_interface.h"
#include <QFile>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    flashData = new QByteArray;//выделим память для массива из кучи
    flashArrayIsFull = false;

    timerStatusBar = new QTimer();
    connect(timerStatusBar, SIGNAL(timeout()), this, SLOT(slotTimerStatusBarAlarm()));
    timerStatusBar->start(2000); // Запустим таймер на 2 секунды

    timerStandartSBSReading = new QTimer();
    connect(timerStandartSBSReading, SIGNAL(timeout()), this, SLOT(on_button_StandartSBS_ReadOnce_clicked()));
    //Таймер запускается в другом месте, по кнопке

    statusbarTable = new QTableWidget;
    setTableStatusbar(statusbarTable);

    connect(&device, &CP2112::connected_Changed,    [=](){statusbarTable->item(0,0)->setText(device.getConnected())     ; });
    connect(&device, &CP2112::serial_Changed,       [=](){statusbarTable->item(1,0)->setText(device.getSerial())        ; });
    connect(&device, &CP2112::manufacturer_Changed, [=](){statusbarTable->item(0,1)->setText(device.getManufacturer())  ; });
    connect(&device, &CP2112::product_Changed,      [=](){statusbarTable->item(1,1)->setText(device.getProduct())       ; });
    connect(&device, &CP2112::status_Changed,       [=](){statusbarTable->item(0,2)->setText(device.getStatus())        ; });
    connect(&device, &CP2112::status_0_Changed,     [=](){statusbarTable->item(1,2)->setText(device.getStatus_0())      ; });
    connect(&device, &CP2112::status_1_Changed,     [=](){statusbarTable->item(1,3)->setText(device.getStatus_1())      ; });

    device.readINI();
    slotTimerStatusBarAlarm();
//    device.init();

//    ui->lineEdit_8->setText(QString ("%1").arg(device.getConfig(Bitrate).toInt()));
//    ui->lineEdit_9->setText(QString ("0x%1").arg(device.getConfig(ACK_Address).toInt(),2,16,QLatin1Char('0')));
//    ui->lineEdit_10->setText(QString ("%1").arg(device.getConfig(Auto_Respond).toBool()));
//    ui->lineEdit_11->setText(QString ("%1").arg(device.getConfig(Write_Timeout).toInt()));
//    ui->lineEdit_12->setText(QString ("%1").arg(device.getConfig(Read_Timeout).toInt()));
//    ui->lineEdit_13->setText(QString ("%1").arg(device.getConfig(Transfer_Retries).toInt()));
//    ui->lineEdit_14->setText(QString ("%1").arg(device.getConfig(SCL_Low_Timeout).toBool()));
//    ui->lineEdit_15->setText(QString ("%1").arg(device.getConfig(Response_Timeout).toInt()));
//    ui->lineEdit_16->setText(QString ("%1").arg(device.getConfig(RX_TX_Leds_On).toBool()));
    setTableStandardSBS (ui->tableViewStandartSBSLeft, tableStandartSBS_Left, 0, 21);
    setTableStandardSBS (ui->tableViewStandartSBSRight, tableStandartSBS_Right, 21, 38);

    setTableExtendedUnseal (ui->tableViewExtendedUnsealLeft, tableExtendedUnseal_Left, 0,19);
    setTableExtendedUnseal (ui->tableViewExtendedUnsealRight, tableExtendedUnseal_Right, 19,31);

    setTableFlashMemory(ui->tableViewFlashMemory, tableFlashMemory);

    //Реакция на клик по таблице Flash Memory
    connect(ui->tableViewFlashMemory->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)), this,
            SLOT(onTableFlashMemoryCellClicked(QModelIndex, QModelIndex)));

    setTableBitFlags(ui->tableViewFETControl, tableFETControl, FETControl);
    setTableBitFlags(ui->tableViewStateOfHealth, tableStateOfHealth, StateOfHealth);
    setTableBitFlags(ui->tableViewSafetyAlarm1, tableSafetyAlarm1, SafetyAlarm1);
    setTableBitFlags(ui->tableViewPFAlarm1, tablePFAlarm1, PFAlarm1);
    setTableBitFlags(ui->tableViewOperationStatus, tableOperationStatus, OperationStatus);
    setTableBitFlags(ui->tableViewChargingStatus, tableChargingStatus, ChargingStatus);
    setTableBitFlags(ui->tableViewSafetyAlarm2, tableSafetyAlarm2, SafetyAlarm2);
    setTableBitFlags(ui->tableViewPFAlarm2, tablePFAlarm2, PFAlarm2);
    setTableBitFlags(ui->tableViewTempRange, tableTempRange, TempRange);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotTimerStatusBarAlarm()
{
    if(device.isInTransferMode())return;//Если устройство в режиме передачи, выходим
    device.setTransfer(true);//Ставим влаг, что устройство в режиме передачи

    if(!device.init()){
        device.setTransfer(false); //Снимаем флаг, что устройство в режиме передачи
        return;
    }

    QString str = "Speed: " + QString::number(device.getSpeed()) + "Hz";
    statusbarTable->item(1,4)->setText(str);

    if(battery.isConnected(&device)){
        statusbarTable->item(0,3)->setText("Battery Connected");

        quint16 data;
        battery.readWord(&device,0x0d,&data);
        batteryLevel->setValue(data);

//        QByteArray array;
//        quint8 len=0;
//        battery.readBlock(&device,0x22,&array,&len);

//        QString str;
//        str.append(array);
//        statusbarTable->item(1,4)->setText(str);

    }else{
        statusbarTable->item(0,3)->setText("Battery Not Connected");
        batteryLevel->setValue(0);
    }
//    device.disconnect(); //Отключим все стигналы

    device.close();
    device.setTransfer(false); //Снимаем флаг, что устройство в режиме передачи
}




