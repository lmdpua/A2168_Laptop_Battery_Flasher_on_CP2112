#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include "A2168_interface.h"

void MainWindow::setTableBitFlags (QTableView *tablePlace, QStandardItemModel *&tableName, const struct BITS *bits)
{
    const   quint8      rows        = 2;
    const   quint8      columns     = 8;
    const   quint8      height      = 20;
    const   quint8      width       = 48;
    const   QSize       table_size  = QSize(width*columns+2, height*rows+2);
            QModelIndex index;

    tableName = new QStandardItemModel(rows,columns,this);    //Создадим модель таблицы
    tablePlace->setModel(tableName);                          //Установим модель
    tablePlace->verticalHeader()->setMinimumSectionSize(5);   //Минимальная высота строки, без этого не работает изменение
//    tablePlace->horizontalHeader()->setMinimumSectionSize(5); //Минимальная ширина строки, без этого не работает изменение
    tablePlace->verticalHeader()->setDefaultSectionSize(height);  //Высота строки по умолчанию
    tablePlace->horizontalHeader()->setDefaultSectionSize(width); //Ширина строки по умолчанию
    tablePlace->verticalHeader()->hide();
    tablePlace->horizontalHeader()->hide();
    tablePlace->setMaximumSize(table_size);

    for(quint8 row=0; row<rows; row++){
        for (quint8 column=0; column<columns; column++){
            index = tableName->index(1-row,7-column); //Найдем индекс нужной ячейки
            tableName->setData(index, bits[columns*row + column].name);//Установим название
            tableName->setData(index, bits[columns*row + column].tooltip, Qt::ToolTipRole);//Установим подсказку
//            QBrush br(Qt::red);
//            tableName->setData(index, br, Qt::BackgroundRole);//Установим подсказку
            tableName->item(1-row,7-column)->setTextAlignment(Qt::AlignCenter); //Выровняем по центру
        }
    }
}

void MainWindow::on_button_ReadOnceBits_clicked()
{
    while(device.isInTransferMode()){} //Ждем пока закончится передача
    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        ui->lineEdit_Result->setText("No Device!");
        return;
    }

    if(battery.isConnected(&device)){ //Если батарея подключена
        if(!setDataTableBits(tableFETControl, 0x46)){
            device.close();
            device.setTransfer(false);
        }
        if(!setDataTableBits(tableStateOfHealth, 0x4f)){
            device.close();
            device.setTransfer(false);
        }
        if(!setDataTableBits(tableSafetyAlarm1, 0x51)){
            device.close();
            device.setTransfer(false);
        }
        if(!setDataTableBits(tablePFAlarm1, 0x53)){
            device.close();
            device.setTransfer(false);
        }
        if(!setDataTableBits(tableOperationStatus, 0x54)){
            device.close();
            device.setTransfer(false);
        }
        if(!setDataTableBits(tableChargingStatus, 0x55)){
            device.close();
            device.setTransfer(false);
        }
        if(!setDataTableBits(tableSafetyAlarm2, 0x69)){
            device.close();
            device.setTransfer(false);
        }
        if(!setDataTableBits(tablePFAlarm2, 0x6b)){
            device.close();
            device.setTransfer(false);
        }
        if(!setDataTableBits(tableTempRange, 0x72)){
            device.close();
            device.setTransfer(false);
        }
    }



    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи

}

void MainWindow::on_button_ReadAllTheTimeBits_clicked()
{

}

bool MainWindow::setDataTableBits(QStandardItemModel *&tableName, quint16 command)
{
    const   quint8      rows        = 2;
    const   quint8      columns     = 8;
            QModelIndex index;
            quint16     data        = 0;
            bool        error       = false;
            bool        bitState;
            quint8      bitNumber;
            QBrush      brush;


//    if(!battery.writeWord(&device, 0x00, command))return false;//Отправляем команду
    if(!battery.readWord(&device, command, &data)){
        error=true;
    }

    if(command == 0x4f){
        ui->lineEdit_StateOFHealth->setText(QString::number((quint8)data));
    }

    for(quint8 row=0; row<rows; row++){
        for (quint8 column=0; column<columns; column++){
            index = tableName->index(1-row,7-column); //Найдем индекс нужной ячейки
            if(error){
                brush = QBrush(Qt::gray);//Серый фон ячейки
            }else{
                bitNumber = columns*row + column;
                bitState = (data >> bitNumber) & 1U;
                if(bitState){
                    brush = QBrush(Qt::red);//Красный фон ячейки
                }else{
                    brush = QBrush(Qt::green);//Зеленый фон ячейки
                }
            }
            tableName->setData(index, brush, Qt::BackgroundRole);
        }
    }
    return true;
}


