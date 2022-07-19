#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QBitArray>

enum QuickCommands{

    Device_Part_Number  = 0x0001,
    FW_Version          = 0x0002,
    HW_Version          = 0x0003,
    Chemistry_ID        = 0x0008,
    Shutdown            = 0x0010,
    Sleep               = 0x0011,
    Seal                = 0x0020,
    Gas_Gauge_Enable    = 0x0021,
    FUSO_Activation     = 0x0030,
    FUSO_Clear          = 0x0031,
    LEDs_On             = 0x0032,
    LEDs_Off            = 0x0033,
    Display_On          = 0x0034,
    Calibration_Mode    = 0x0040,
    Reset               = 0x0041,
    BootROM             = 0x0f00
};

void MainWindow::sendCommand(quint16 command, bool answer)
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

    if(!battery.isConnected(&device)){ //Если батарея не подключена
        ui->lineEdit_Result->setText("No Battery!");
    }else{
        if(battery.writeWord(&device, 0x00, command)){//Отправляем команду
            if(answer){ //Если нужет ответ
                quint16 data;
                if(battery.readWord(&device, 0x00, &data)){//Читаем ответ
                    ui->lineEdit_Result->setText(QString::number(data,16).toUpper());
                }else{//Если ответ не пришел
                    ui->lineEdit_Result->setText("No Response!");
                }
            }else{//Если ответ не нужен
                ui->lineEdit_Result->setText("Done!");
            }
        }
    }

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
}

void MainWindow::on_button_DevicePartNumber_clicked()
{
    sendCommand(Device_Part_Number, true);
}

void MainWindow::on_button_FW_Version_clicked()
{
    sendCommand(FW_Version, true);
}

void MainWindow::on_button_HW_Version_clicked()
{
    sendCommand(HW_Version, true);
}

void MainWindow::on_button_Chemistry_ID_clicked()
{
    sendCommand(Chemistry_ID, true);
}

void MainWindow::on_button_Shutdown_clicked()
{
    sendCommand(Shutdown, false);
}

void MainWindow::on_button_Sleep_clicked()
{
    sendCommand(Sleep, false);
}

void MainWindow::on_button_Seal_clicked()
{
    sendCommand(Seal, false);
}

void MainWindow::on_button_Gas_Gauge_Enable_clicked()
{
    sendCommand(Gas_Gauge_Enable, false);
}

void MainWindow::on_button_FUSO_Activation_clicked()
{
    sendCommand(FUSO_Activation, false);
}

void MainWindow::on_button_FUSO_Clear_clicked()
{
    sendCommand(FUSO_Clear, false);
}

void MainWindow::on_button_LEDs_On_clicked()
{
    sendCommand(LEDs_On, false);
}

void MainWindow::on_button_LEDs_Off_clicked()
{
    sendCommand(LEDs_Off, false);
}

void MainWindow::on_button_Display_On_clicked()
{
    sendCommand(Display_On, false);
}

void MainWindow::on_button_Calibration_Mode_clicked()
{
    sendCommand(Calibration_Mode, false);
}

void MainWindow::on_button_Reset_clicked()
{
    sendCommand(Reset, false);
}

void MainWindow::on_button_BootROM_clicked()
{
    QString str = "Are you sure you want to switch the battery to BootROM mode?";
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Warning", str, QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::No)return;
    sendCommand(BootROM, false);
}



enum FET_Control{

    Discharge_FET   = 0x1,
    Charge_FET      = 0x2,
    Precharge_FET   = 0x3
};

void MainWindow::FET(quint8 fet, bool on)
{
    while(device.isInTransferMode()){} //Ждем пока закончится передача
    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        ui->lineEdit_Result->setText("No Device!");
        return;
    }

    quint16 fet_data;
    if(!battery.readWord(&device, 0x46,&fet_data)){
        device.close();
        device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
    }

    if(on){
        fet_data |= 1UL << fet;//Установим бит
    }else{
        fet_data &= ~(1UL << fet);//Очистим бит
    }

    if(battery.writeWord(&device, 0x46, fet_data)){
        ui->lineEdit_FET_Result->setText("Done!");
    }else{
        ui->lineEdit_FET_Result->setText("Error!");
    }

    device.close();
    device.setTransfer(false);
}

void MainWindow::on_button_PFET_On_clicked()
{
    FET(Precharge_FET, true);
}

void MainWindow::on_button_PFET_Off_clicked()
{
    FET(Precharge_FET, false);
}

void MainWindow::on_button_CFET_On_clicked()
{
    FET(Charge_FET, true);
}

void MainWindow::on_button_CFET_Off_clicked()
{
    FET(Charge_FET, false);
}

void MainWindow::on_button_DFET_On_clicked()
{
    FET(Discharge_FET, true);
}

void MainWindow::on_button_DFET_Off_clicked()
{
    FET(Discharge_FET, false);
}
