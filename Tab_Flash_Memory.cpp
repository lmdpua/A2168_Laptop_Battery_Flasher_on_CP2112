#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include "A2168_interface.h"
#include <QByteArrayView>


void MainWindow::setTableFlashMemory (QTableView *tablePlace, QStandardItemModel *&tableName)
{
    const quint8 sector = 8;
    const quint8 page = 8;
    const quint8 column = 33;

    tableName = new QStandardItemModel(sector*page,column,this); //Создадим модель таблицы
    tablePlace->setModel(tableName);                          //Установим модель
    tablePlace->setStyleSheet("padding: 0px; margin:0px");

    tablePlace->verticalHeader()->setMinimumSectionSize(5);    //Минимальная высота строки, без этого не работает изменение
    tablePlace->horizontalHeader()->setMinimumSectionSize(5);  //Минимальная ширина строки, без этого не работает изменение

    tablePlace->verticalHeader()->setDefaultSectionSize(20);   //Высота строки по умолчанию
    tablePlace->horizontalHeader()->setDefaultSectionSize(22); //Ширина строки по умолчанию

    tablePlace->setColumnWidth(0, 45);      //Ширина нулевого столбца

    tablePlace->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#e1e1e1;}");  //Цвет фона заголовков столбцов
    tablePlace->verticalHeader()->setStyleSheet("QHeaderView::section { background-color:#e1e1e1;}");   //Цвет заголовков строк

    tablePlace->horizontalHeader()->setStretchLastSection(true);  // Устанавливаем адаптивный размер последнего столбца

    tableName->setHeaderData(0, Qt::Horizontal, "Sector"); //Ярлык для нулевого столбца
//    tableName->setHeaderData(1, Qt::Horizontal, "Size");   //Ярлык для первого столбца

    for (BYTE i=0; i<column-1; i++)
    {
       tableName->setHeaderData(i+1, Qt::Horizontal, QString::number(i)); //Проименуем столбцы
    }
    QStringList VerticalHeaderLabels; //Создадим список
    for(quint8 i=0;i<sector;i++)
    {
        VerticalHeaderLabels <<"78"<<"79"<<"7A"<<"7B"<<"7C"<<"7D"<<"7E"<<"7F"; //Добавим значения в список
        tablePlace->setSpan(i*sector,0,sector,1);//Объединение ячеек нулевого столбца

        QStandardItem *itm = new QStandardItem(QString("%1").arg(i));//Создаем стандартный предмет типа строка
        itm->setFlags(Qt::ItemIsEnabled); //Делаем только видимым
        tableName->setItem(i*sector,0,itm); //Устанавливаем данные в нулевой столбец
        tableName->item(i*sector,0)->setTextAlignment(Qt::AlignCenter); //Выравниваем по центру
    }

    tableName->setVerticalHeaderLabels(VerticalHeaderLabels); //Проименуем строки значениями из списка
}

void MainWindow::setDataTableFlashMemory (void)
{
    quint8  rowNum      = 64;//SECTOR_AMOUNT*PAGE_IN_SECTOR;  //Количество строк в таблице
    quint8  column      = 0;
//    quint8  row         = 0;
    QModelIndex index;          //Переменная для индексации ячеек таблицы
    quint8 byte         = 0;

    for (quint8 row=0; row<rowNum; row++){
        for (quint8 n=0; n<FLASH_PAGE_SIZE; n++){ //Перебираем все значения массива
            column=n+1; //Номер ячейки для данных сдвинут на один байт, так как в таблице нулевая ячейка занята под номер сектора

            byte = flashData->at(row*(FLASH_PAGE_SIZE) + n);

            QString number = QString("%1").arg(byte, 2, 16, QLatin1Char('0')).toUpper(); //Два символа HEX в верхнем регистре

            index = tableFlashMemory->index(row,column);   //Найдем индекс нужной ячейки
            tableFlashMemory->setData(index, number);      //Отобразим параметр в ячейке index
            tableFlashMemory->item(row,column)->setTextAlignment(Qt::AlignCenter); //Выровняем по центру
        }
    }
}

void MainWindow::on_button_ReadFlash_clicked()
{
//    connect(&flashWrite, &Progress::progressChanged, [=](){ui->progressBarFlashMemory->setValue(flashRead.getProgress()); });

    connect(&flashRead, SIGNAL(progressChanged(int)),ui->progressBarFlashMemory, SLOT(setValue(int)));
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
        setDataTableFlashMemory();
        QMessageBox::information(this,"Success","Flash read successfully");
        flashArrayIsFull = true;

    }else{
        QMessageBox::critical(this,"Error","Flash read fault");
        flashArrayIsFull = false;
    }

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
    connect(ui->tableViewFlashMemory->model(),
            SIGNAL(dataChanged(QModelIndex, QModelIndex)), this,
            SLOT(onTableFlashMemoryCellChanded(QModelIndex, QModelIndex)));

    connect(ui->lineEdit_Data, SIGNAL(returnPressed()), this, SLOT(flashMemorySaveToCell()));
    disconnect(&flashRead, SIGNAL(progressChanged(int)),ui->progressBarFlashMemory, SLOT(setValue(int)));
}

void MainWindow::onTableFlashMemoryCellClicked(const QModelIndex &current,const QModelIndex &previous)
{
    Q_UNUSED(previous); //Фактически эта переменная не используются и ее можно заремачить

    quint8 row,column,sector,offset=0;

    row     = current.row();
    column  = current.column();

    if(column==0){//Если был клик по нулевой колонке, стираем, выходим
        ui->lineEdit_CurrentSector  ->clear();
        ui->lineEdit_Offset         ->clear();
        ui->lineEdit_Data           ->clear();
        return;
    }

    sector = row/8;
    offset = column-1+FLASH_PAGE_SIZE*row;

    ui->lineEdit_CurrentSector  ->setText(QString::number(sector));
    ui->lineEdit_Offset         ->setText(QString::number(offset));
    ui->lineEdit_Data           ->setText(current.data().toString());
}

void MainWindow::onTableFlashMemoryCellChanded(const QModelIndex &current,const QModelIndex &previous)
{
    quint8 byte,row,column,sector,offset=0;
    bool ok;

    row     = current.row();
    column  = current.column();
    byte    = current.data().toString().toInt(&ok,16);

    sector = row/8;
    offset = column-1+FLASH_PAGE_SIZE*row;

    if(!ok){
        QMessageBox::critical(this,"Error","The entered value is incorrect");
        return;
    }
    onTableFlashMemoryCellClicked(current,previous);
    quint16 it = sector*SECTOR_SIZE + offset;

//    *((flashData->data()) + it) = byte;
    flashData->operator[](it)=byte;
    //Более длинный, но понятный вариант
    //    char *ptr = flashData->data();
    //    ptr=ptr+it;
    //    *ptr=byte;
}

void MainWindow::flashMemorySaveToCell()
{
    quint8 data,temp,row,column,sector,offset=0;
    bool ok;
    QString str;
    QModelIndex index;

    str = ui->lineEdit_CurrentSector->text();
    sector = str.toInt(&ok);
    if(!ok || sector>7){
        QMessageBox::critical(this,"Error","The entered sector is incorrect");
        return;
    }

    str = ui->lineEdit_Offset->text();
    offset = str.toInt(&ok);
    if(!ok){
        QMessageBox::critical(this,"Error","The entered offset is incorrect");
        return;
    }

    str = ui->lineEdit_Data->text();
    data = str.toInt(&ok,16);
    if(!ok){
        QMessageBox::critical(this,"Error","The entered data is incorrect");
        return;
    }

    temp = offset/FLASH_PAGE_SIZE;
    row = sector*8+temp;
    column = (offset % FLASH_PAGE_SIZE)+1;

    index = tableFlashMemory->index(row,column);   //Найдем индекс нужной ячейки
    str = QString("%1").arg(data, 2, 16, QLatin1Char('0')).toUpper();
    tableFlashMemory->setData(index, str);      //Отобразим параметр в ячейке index

    if(column == 32){   //Если дошли до последней ячейки в строке,
        column=0;
        row++;          // переход на следующую строку
    }
    index = tableFlashMemory->index(row,column+1);      //Сдвинемся на одну ячейку вправо
    ui->tableViewFlashMemory->setCurrentIndex(index);   //Сделаем ее текущей

}

void MainWindow::on_button_WriteFlash_clicked()
{
    connect(&flashWrite, SIGNAL(progressChanged(int)),ui->progressBarFlashMemory, SLOT(setValue(int)));
    if(flashArrayIsFull == false){
        QMessageBox::critical(this,"Error","Data array not ready");
        return;
    }

    while(device.isInTransferMode()){} //Ждем пока закончится передача

    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        return;
    }

    if(writeFlash(&device, &battery, flashData, false)){
        QMessageBox::information(this,"Success","Flash write successfully");
    }else{
        QMessageBox::critical(this,"Erroe","Flash write fault");
    }

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
    disconnect(&flashWrite, SIGNAL(progressChanged(int)),ui->progressBarFlashMemory, SLOT(setValue(int)));
}

void MainWindow::on_button_SaveFlashDataToFile_clicked()
{
    if (!flashArrayIsFull){
        QMessageBox::critical(this,"Error","Flash must be read first!\nOr read data from a file");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                                    QDir::homePath(),
                                                    tr("Binary File(*.bin)") );

    if(fileName.isNull()){
        QMessageBox::information(this,"Error","File not selected");
    }else{
        if(!saveToFile(&fileName, flashData)){ //Сохраняем в файл
            QMessageBox::information(this,"Error","Can't write file");
        }
    }
}

bool MainWindow::saveToFile(QString *pathToFile, QByteArray *dataToSave)
{
    //Создаем или открываем файл
    //    QFile file(*pathToFile); //или по другому
    QFile file;
    file.setFileName(*pathToFile);

    if (!file.open(QFile::WriteOnly)){ //проверяем открыт ли для записи
        file.close();
        return false;
    }

    if(file.write(*dataToSave)!=dataToSave->size()){//Если записалось не все
        file.close();
        return false;
    }

    file.close();
    return true;
}

void MainWindow::on_button_LoadFlashDataFromFile_clicked()
{
    //    disconnect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(replaceData(int,int)));
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open file",
                                                    QDir::homePath(),
                                                    tr("Only binary files(*.bin)"));
    if(fileName.isNull()){
        QMessageBox::information(this,"Error","File not selected");
        return;
    }

    flashData->clear(); //Очистим массив
    flashArrayIsFull  = false;//Установим флаг, что массив пустой

    if(!readFromFile(&fileName, flashData)){
        QMessageBox::critical(this,"Error","File read error");
        return;
    }

    if(flashData->size()!=SECTOR_SIZE*SECTOR_AMOUNT){//Подходит ли файл по размеру
        QMessageBox::critical(this,"Error","File size does not fit");
        return;
    }

    flashArrayIsFull  = true; //Ставим флаг, что массив готов
    setDataTableFlashMemory();

    //    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(replaceData(int,int)));
}

bool MainWindow::readFromFile(QString *pathToFile, QByteArray *dataFromFile)
{
    //Создаем или открываем файл
//    QFile file(*pathToFile); //или по другому
    QFile file;
    file.setFileName(*pathToFile);

    if (!file.open(QFile::ReadOnly)) //проверяем открыт ли для чтения
    {
        return false;
    }
    else
    {
        *dataFromFile = file.readAll(); //Читаем из файла в массив
        file.close();                   //Закрываем файл

        return true;
    }
}

void MainWindow::on_button_Search_clicked()
{

    bool            ok              = false;           //Флаг успешного преобразования из текста в число
    QString         str;                            //Объявляем строку для приема текста
    quint8          strSize         = 0;
    quint8          block_length    = 0;
    QByteArray      array;
    QByteArrayView  search;
    static qint16         indexOfSearch   =0;
    static qint16          startIndex = 0;
    quint8          row,column      =0;

    str = ui->lineEdit_Search->text();
    QRegularExpression re("[^01234567890abcdefABCDEF]");
    str.remove(re);//Удалим неправильные символы
    strSize = str.size();

    if (strSize % 2 == 1){//Если количество символов нечетное
        QMessageBox::critical(this,"Error","Please enter an even number of characters");
        return;
    }

    block_length = strSize/2;

    if (block_length>MAX_SMBUS_BLOCK_SIZE){ //Если длина блока получится больше 32 байт
        QMessageBox::critical(this,"Error","The maximum number of characters is 64");
        return;
    }

    array.resize(block_length);

    for (BYTE i=0; i<strSize; i+=2){//Преобразуем строку в массив
        QString byteStr = str.mid(i,2);
        array[i/2]=byteStr.toUInt(&ok, 16);
        if (!ok){       //Если преобразование не успешное или число больше чем 0xff
            QMessageBox::critical(this,"Error","Interpretation error");
            return;
        }
    }

    search = QByteArrayView(array);

    indexOfSearch = flashData->indexOf(search, startIndex);
    if(indexOfSearch == -1){
        QMessageBox::information(this,"Done","The search has not given any results");
        startIndex=0;
        return;
    }
    startIndex=indexOfSearch+1;

    QModelIndex index;

    for (quint8 i=0; i<block_length; i++) {
        row = (indexOfSearch+i)/FLASH_PAGE_SIZE;
        column = indexOfSearch+i - row*FLASH_PAGE_SIZE +1;
        index = tableFlashMemory->index(row,column);
        if(i==0){
            ui->tableViewFlashMemory->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
            ui->tableViewFlashMemory->selectionModel()->currentChanged(index,index);
        }else{
            ui->tableViewFlashMemory->selectionModel()->select(index, QItemSelectionModel::Select);
        }
    }
}

void MainWindow::on_lineEdit_Search_editingFinished()
{
    on_button_Search_clicked();
}

