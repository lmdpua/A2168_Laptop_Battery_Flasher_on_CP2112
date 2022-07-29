#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QHeaderView>
#include "A2168_interface.h"

void MainWindow::setTableFlashDetailed (QTableView *tablePlace, QStandardItemModel *&tableName, const struct FUNCTION *fun, quint8 row)
{
//    quint8 row      = end - start;
    quint8 column   = 6;
    quint8 rowHeigh = 20;
    enum columnName {h_Type, h_Sector_ID, h_Offset, h_Name, h_Value, h_Unit};

    tableName = new QStandardItemModel(row,column,this);    //Создадим модель таблицы
    tablePlace->setModel(tableName);                          //Установим модель
    tablePlace->verticalHeader()->setMinimumSectionSize(5);   //Минимальная высота строки, без этого не работает изменение
    tablePlace->horizontalHeader()->setMinimumSectionSize(5); //Минимальная ширина строки, без этого не работает изменение
    tablePlace->verticalHeader()->setDefaultSectionSize(rowHeigh);  //Высота строки по умолчанию
    tablePlace->verticalHeader()->hide(); //Скроем заголовки строк

    tablePlace->setColumnWidth(0, 140);           //Ширина первого(нулевого) столбца
    tablePlace->setColumnWidth(1, 50);
    tablePlace->setColumnWidth(2, 50);
    tablePlace->setColumnWidth(3, 200);
    tablePlace->setColumnWidth(4, 110);
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
