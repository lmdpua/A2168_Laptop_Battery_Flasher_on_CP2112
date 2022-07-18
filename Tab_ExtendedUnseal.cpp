#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include "A2168_interface.h"


void MainWindow::setTableExtendedUnseal (QTableView *tablePlace, QStandardItemModel *&tableName, BYTE start, BYTE end)
{
    BYTE row    = end - start;
    BYTE column = 4;

    tableName = new QStandardItemModel(row,column,this);    //Создадим модель таблицы
    tablePlace->setModel(tableName);                          //Установим модель
    tablePlace->verticalHeader()->setMinimumSectionSize(5);   //Минимальная высота строки, без этого не работает изменение
    tablePlace->horizontalHeader()->setMinimumSectionSize(5); //Минимальная ширина строки, без этого не работает изменение
    tablePlace->verticalHeader()->setDefaultSectionSize(20);  //Высота строки по умолчанию
//    tablePlace->horizontalHeader()->setDefaultSectionSize(23); //Ширина строки по умолчанию

    tablePlace->setColumnWidth(0, 45);           //Ширина первого(нулевого) столбца
    tablePlace->setColumnWidth(1, 120);
    tablePlace->setColumnWidth(2, 135);
    tablePlace->setColumnWidth(3, 45);

    tableName->setHeaderData(0, Qt::Horizontal, "Access"); //Проименуем столбцы
    tableName->setHeaderData(1, Qt::Horizontal, "Name");   //Проименуем столбцы
    tableName->setHeaderData(2, Qt::Horizontal, "Data");   //Проименуем столбцы
    tableName->setHeaderData(3, Qt::Horizontal, "Unit");   //Проименуем столбцы

    QStringList RowLabels; //Создадим список для имен строк
    for (BYTE i=start; i<end; i++)
    {
        QString str = QString("%1")                 //Строка, в которой будет один аргумент
                            .arg(Extended_Commands[i].addr, //А вот и аргумент
                                 2,                 //Четыре символа
                                 16,                //HEX
                                 QLatin1Char('0'))  //C лидирующими нулями
                            .toUpper();             //Все символы в верхнем регистре
        str = str.prepend("0x");                    //Добавим перед строкой
        RowLabels <<str; //Добавим значения в список

        //Небольшой костыль, чтобы увеличить высоту некоторых ячеек
        if(Extended_Commands[i].addr==0x70){
            tablePlace->setRowHeight(i-start,40);
        }
        if(Extended_Commands[i].addr==0x73){
            tablePlace->setRowHeight(i-start,80);
        }
        if(Extended_Commands[i].addr==0x74){
            tablePlace->setRowHeight(i-start,80);
        }
    }
    tableName->setVerticalHeaderLabels(RowLabels); //Проименуем строки значениями из списка

    tablePlace->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#e1e1e1;}");  //Цвет фона заголовков столбцов
    tablePlace->verticalHeader()->setStyleSheet("QHeaderView::section { background-color:#e1e1e1;}");   //Цвет заголовков строк
    tablePlace->horizontalHeader()->setStretchLastSection(true);  // Устанавливаем адаптивный размер последнего столбца

    for(BYTE i=0; i<row; i++)
    {
        QModelIndex indexAccess, indexName, indexUnit; //Объявим переменную для индексации ячеек таблицы
        indexAccess = tableName->index(i,0); //Найдем индекс нужной ячейки
        switch (Extended_Commands[i+start].access) {
        case R:
            tableName->setData(indexAccess, "R");
            break;
        case RW:
            tableName->setData(indexAccess, "R/W");
            break;
        }
        tableName->item(i,0)->setTextAlignment(Qt::AlignCenter); //Выровняем по центру

        indexName = tableName->index(i,1); //Найдем индекс нужной ячейки
        tableName->setData(indexName, Extended_Commands[i+start].name); //Отобразим параметр в ячейке index

        indexUnit = tableName->index(i,3); //Найдем индекс нужной ячейки
        tableName->setData(indexUnit, Extended_Commands[i+start].unit); //Отобразим параметр в ячейке index
        tableName->item(i,3)->setTextAlignment(Qt::AlignCenter); //Выровняем по центру
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

void MainWindow::setDataTableExtendedUnseal  (QStandardItemModel *&tableName, quint8 start, quint8 end, CP2112 *dev, SMBus *batt)
{
    if(!batt->isConnected(dev))return; //Если батарея не подключена, выходим

    QModelIndex indexData;          //Переменная для индексации ячеек таблицы
    quint16     data        = 0;    //Переменная для приема данных
    QString     str;                //Строка для вывода данных
    QByteArray  array;              //Массив для приема данных
    quint8      len         = 0;    //Длина данных

    for(quint8 i=start; i<end; i++){

        indexData = tableName->index(i-start,2); //Найдем индекс нужной ячейки

        switch ((quint8)Extended_Commands[i].format){

        case HEX2:
            batt->readWord(dev, Extended_Commands[i].addr, &data);

            str = QString("%1")                 //Строка, в которой будет один аргумент
                    .arg(data,              //А вот и аргумент
                         4,                 //Четыре символа
                         16,                //HEX
                         QLatin1Char('0'))  //C лидирующими нулями
                    .toUpper();             //Все символы в верхнем регистре
            str = str.prepend("0x");                    //Добавим перед строкой

            tableName->setData(indexData, str);
            break;

        case UINT2:
            batt->readWord(dev, Extended_Commands[i].addr, &data);
            tableName->setData(indexData, (quint16)data);
            break;

        case UINT1:
            batt->readWord(dev, Extended_Commands[i].addr, &data);
            tableName->setData(indexData, (quint8)data);
            break;

        case INT2:
            batt->readWord(dev, Extended_Commands[i].addr, &data);
            tableName->setData(indexData, (qint16)data);
            break;

        case HEX4:
            batt->readBlock(dev, Extended_Commands[i].addr,&array,&len);
            str.clear();
            str = array.toHex().toUpper();
            str.prepend("0x");
            tableName->setData(indexData, str);
            break;

        case STR20:
        case STR31:
        case STR32:
            if (Extended_Commands[i].addr == 0x73 || Extended_Commands[i].addr == 0x74){
                batt->readBlock(dev, Extended_Commands[i].addr,&array,&len);
                str.clear();
                str = array.toHex(' ').toUpper();
            }else{
                batt->readBlock(dev, Extended_Commands[i].addr,&array,&len);
                str.clear();
                str.append(array);
            }
            tableName->setData(indexData, str);
            break;
        }
        tableName->itemFromIndex(indexData)->setTextAlignment(Qt::AlignCenter);
    }
}

void MainWindow::on_button_ExtendedUnseal_ReadOnce_clicked()
{
    quint8 index = ui->tabWidget->indexOf(ui->tab_ExtendedUnseal);
    if(index != ui->tabWidget->currentIndex()){
        timerStandartSBSReading->stop();
    }
    while(device.isInTransferMode()){} //Ждем пока закончится передача

    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        return;
    }

    setDataTableExtendedUnseal(tableExtendedUnseal_Left, 0, 19, &device, &battery);
    setDataTableExtendedUnseal(tableExtendedUnseal_Right, 19, 31, &device, &battery);

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
}

