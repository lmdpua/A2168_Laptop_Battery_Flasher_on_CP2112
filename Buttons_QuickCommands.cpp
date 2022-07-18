#include "mainwindow.h"
#include "ui_mainwindow.h"

enum QuickCommands
{
    Device_Part_Number = 0x0001,
};

void MainWindow::sendCommand(quint16 command)
{
    QString str = QString("%1").arg(command, 4, 16, QLatin1Char('0')).toUpper();
    str.prepend("0x");
    ui->lineEdit_Command->setText(str);

    while(device.isInTransferMode()){} //Ждем пока закончится передача
    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        ui->lineEdit_Result->setText("No Device!");
        return;
    }

    if(!battery.isConnected(&device)){
        ui->lineEdit_Result->setText("No Battery!");
    }else{
        if(battery.writeWord(&device, 0x00, command)){
            quint16 data;
            battery.readWord(&device, 0x00, &data);
            ui->lineEdit_Result->setText(QString::number(data,16).toUpper());
        }else{
            ui->lineEdit_Result->setText("Error!");
        }
    }

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
}

void MainWindow::on_button_DevicePartNumber_clicked()
{
    sendCommand(Device_Part_Number);
}
