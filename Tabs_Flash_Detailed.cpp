#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include "A2168_interface.h"

enum flashDetailedcolumnName {h_Type, h_Sector_ID, h_Offset, h_Name, h_Value, h_Unit};

void MainWindow::setTableFlashDetailed (QTableView *tablePlace, QStandardItemModel *&tableName, const struct FUNCTION *fun, quint8 row)
{
//    quint8 row      = end - start;
    quint8 column   = 6;
    quint8 rowHeigh = 20;    

    tableName = new QStandardItemModel(row,column,this);    //Создадим модель таблицы
    tablePlace->setModel(tableName);                          //Установим модель
    tablePlace->verticalHeader()->setMinimumSectionSize(5);   //Минимальная высота строки, без этого не работает изменение
    tablePlace->horizontalHeader()->setMinimumSectionSize(5); //Минимальная ширина строки, без этого не работает изменение
    tablePlace->verticalHeader()->setDefaultSectionSize(rowHeigh);  //Высота строки по умолчанию
    tablePlace->verticalHeader()->hide(); //Скроем заголовки строк

    tablePlace->setColumnWidth(0, 140);           //Ширина первого(нулевого) столбца
    tablePlace->setColumnWidth(1, 50);
    tablePlace->setColumnWidth(2, 50);
    tablePlace->setColumnWidth(3, 150);
    tablePlace->setColumnWidth(4, 200);
    tablePlace->setColumnWidth(5, 50);

    tableName->setHeaderData(0, Qt::Horizontal, "Type");        //Проименуем столбцы
    tableName->setHeaderData(1, Qt::Horizontal, "Sector\nID");  //Проименуем столбцы
    tableName->setHeaderData(2, Qt::Horizontal, "Offset");      //Проименуем столбцы
    tableName->setHeaderData(3, Qt::Horizontal, "Name");        //Проименуем столбцы
    tableName->setHeaderData(4, Qt::Horizontal, "Value");       //Проименуем столбцы
    tableName->setHeaderData(5, Qt::Horizontal, "Unit");        //Проименуем столбцы

    tablePlace->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#e1e1e1;}");  //Цвет фона заголовков столбцов
    tablePlace->horizontalHeader()->setStretchLastSection(true);  // Устанавливаем адаптивный размер последнего столбца

    QMap<quint8, QString> map={
        {VOLTAGE,               "Voltage"               },
        {CURRENT,               "Current"               },
        {TEMPERATURE,           "Temperature"           },
        {VERIFICATION,          "Verification"          },
        {CHARGE_TEMP_CFG,       "Charge Temp Cfg"       },
        {CHARGE_CFG,            "Charge Cfg"            },
        {TERMINATION_CFG,       "Termination Cfg"       },
        {CB_CFG,                "Cell Balancing Cfg"    },
        {CHARGING_FAULTS,       "Charging Faults"       },
        {REGISTERS,             "Registers"             },
        {HARDWARE,              "Hardware"              },
        {POWER,                 "Power"                 },
        {DATA,                  "Data"                  },
        {CONFIGURATION,         "Configuration"         },
        {MANUFACTURER_INFO,     "Manufacturer Info"     },
        {LIFETIME_DATA,         "Lifetime Data"         },
        {LIFETIME_TEMP_SAMPLES, "Lifetime Temp Samples" },
        {PF_RECORD,             "Permanent Fail Record" },
        {GG_CFG,                "Gas Gauge Cfg"         },
        {CURRENT_THRESHOLDS,    "Current Thresholds"    },
        {TEMP_MODEL,            "Temp Model"            },
        {LED_CFG,               "Led Cfg"               }
    };

    for(quint8 i=0; i<row; i++){
        QModelIndex indexType, indexSector, indexOffset, indexName, indexUnit; //Объявим переменные для индексации ячеек таблицы
        quint8 type = fun[i].type;      //Найдем тип данных
        QString str = map.value(type);  //Найдем соответствующую строку

        indexType   = tableName->index(i, h_Type);         //Найдем индекс нужной ячейки
        indexSector = tableName->index(i, h_Sector_ID);
        indexOffset = tableName->index(i, h_Offset);
        indexName   = tableName->index(i, h_Name);
        indexUnit   = tableName->index(i, h_Unit);

        tableName->setData(indexType,   str);             //Вставим строку Type
        tableName->setData(indexSector, fun[i].id);       //Вставим строку Sector ID
        tableName->setData(indexOffset, fun[i].offset);   //Вставим строку Offset
        tableName->setData(indexName,   fun[i].name);     //Вставим строку Name
        tableName->setData(indexUnit,   fun[i].unit);     //Вставим строку Unit

        tableName->item(i,h_Type)->setTextAlignment(Qt::AlignCenter); //Выровняем по центру
        tableName->item(i,h_Sector_ID)->setTextAlignment(Qt::AlignCenter);
        tableName->item(i,h_Offset)->setTextAlignment(Qt::AlignCenter);
//        tableName->item(i,h_Value)->setTextAlignment(Qt::AlignCenter);
        tableName->item(i,h_Unit)->setTextAlignment(Qt::AlignCenter);
    }

    //Определим высоту таблицы
    quint16 commonHeigh = 0;
    for(quint8 i=0; i<row; i++){//Сумма высот всех строк
        commonHeigh+=tablePlace->rowHeight(i);
    }
    commonHeigh+=tablePlace->horizontalHeader()->height();//Плюс высота заголовка
    commonHeigh+=2;
    tablePlace->setMaximumHeight(commonHeigh);//Установим максимальную высоту
}

void MainWindow::on_button_InLayoutReadFlash_clicked()
{
    connect(&flashRead, SIGNAL(progressChanged(int)),ui->progressBarFlashDelailed, SLOT(setValue(int)));
    while(device.isInTransferMode()){} //Ждем пока закончится передача

    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        return;
    }
    if(!battery.isConnected(&device)){
        QMessageBox::critical(this,"Error","Battery not found");
        return;
    }

    flashData->clear();//Очистим массив

    if(readFlash(&device,&battery,flashData)){
        quint8 row = 0;

        row = sizeof(list_First_Level_Protection)/sizeof(list_First_Level_Protection[0]);
        setDataTableFlashDetailed(table1stLevelProtection, list_First_Level_Protection, row);

        row = sizeof(list_Second_Level_Protection)/sizeof(list_Second_Level_Protection[0]);
        setDataTableFlashDetailed(table2ndLevelProtection, list_Second_Level_Protection, row);

        row = sizeof(list_Charge_Control)/sizeof(list_Charge_Control[0]);
        setDataTableFlashDetailed(tableChargeControl, list_Charge_Control, row);

        row = sizeof(list_System_Setting)/sizeof(list_System_Setting[0]);
        setDataTableFlashDetailed(tableSystemSetting, list_System_Setting, row);

        row = sizeof(list_SBS_Setting)/sizeof(list_SBS_Setting[0]);
        setDataTableFlashDetailed(tableSBS_Setting, list_SBS_Setting, row);

        row = sizeof(list_System_Data)/sizeof(list_System_Data[0]);
        setDataTableFlashDetailed(tableSystemData, list_System_Data, row);

        row = sizeof(list_Gas_Gauge)/sizeof(list_Gas_Gauge[0]);
        setDataTableFlashDetailed(tableGasGauge, list_Gas_Gauge, row);

        row = sizeof(list_Calibration)/sizeof(list_Calibration[0]);
        setDataTableFlashDetailed(tableCalibration, list_Calibration, row);

        row = sizeof(list_LED_Support)/sizeof(list_LED_Support[0]);
        setDataTableFlashDetailed(tableLEDSupport, list_LED_Support, row);

        QMessageBox::information(this,"Success","Flash read successfully");
        flashArrayIsFull = true;

    }else{
        QMessageBox::critical(this,"Error","Flash read fault");
        flashArrayIsFull = false;
    }

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
//    connect(ui->tableViewFlashMemory->model(),
//            SIGNAL(dataChanged(QModelIndex, QModelIndex)), this,
//            SLOT(onTableFlashMemoryCellChanded(QModelIndex, QModelIndex)));

//    connect(ui->lineEdit_Data, SIGNAL(returnPressed()), this, SLOT(flashMemorySaveToCell()));
    disconnect(&flashRead, SIGNAL(progressChanged(int)),ui->progressBarFlashDelailed, SLOT(setValue(int)));
}

void MainWindow::setDataTableFlashDetailed(QStandardItemModel *&tableName, const struct FUNCTION *fun, quint8 row)
{
    QModelIndex indexValue;
    quint8      sector      = 0;
    quint16     offset      = 0;
    quint8      type        = 0;
    qint64      data        = 0;
    QString     str;

    for(quint8 i=0; i<row; i++){
        indexValue = tableName->index(i,h_Value);
        sector = fun[i].id;
        offset = sector*SECTOR_SIZE + fun[i].offset.toUInt();
        type = fun[i].data_type;
        switch(type){
        case I1:
            data = flashData->at(offset);
            tableName->setData(indexValue, data);     //Вставим строку Value
            break;
        case I2:
            data = (qint16)((quint8)flashData->at(offset) << 8) | (quint8)(flashData->at(offset + 1));
            tableName->setData(indexValue, data);     //Вставим строку Value
            break;
        case I4:
            data =  ((quint8)flashData->at(offset)      << 24) |
                    ((quint8)flashData->at(offset + 1 ) << 16) |
                    ((quint8)flashData->at(offset + 1 ) << 8 ) |
                    ((quint8)flashData->at(offset + 1 ));
            tableName->setData(indexValue, (qint64)data);     //Вставим строку Value
            break;
        case U1:
            data = (quint8)flashData->at(offset);
            tableName->setData(indexValue, data);     //Вставим строку Value
            break;
        case U2:
            data = ((quint8)flashData->at(offset) << 8) | (quint8)(flashData->at(offset + 1));
            tableName->setData(indexValue, data);     //Вставим строку Value
            break;
        case H1:
            data = (quint8)flashData->at(offset);
            str = QString("%1").arg(data,2,16,QLatin1Char ('0')).toUpper();
            str.prepend("0x");
            tableName->setData(indexValue, str);     //Вставим строку Value
            break;
        case H2:
            data = ((quint8)flashData->at(offset) << 8) | (quint8)(flashData->at(offset + 1));
            str = QString("%1").arg(data,4,16,QLatin1Char ('0')).toUpper();
            str.prepend("0x");
            tableName->setData(indexValue, str);     //Вставим строку Value
            break;
        case S5:
        case S20:
        case S21:
        case S32:
            str = QString::fromLocal8Bit(flashData->data()+offset + 1,flashData->at(offset));
            tableName->setData(indexValue, str);     //Вставим строку Value
            break;
        }
        tableName->item(i,h_Value)->setTextAlignment(Qt::AlignCenter);
    }
}
