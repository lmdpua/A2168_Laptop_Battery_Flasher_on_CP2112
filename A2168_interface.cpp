#include "A2168_interface.h"
#include <QThread>
#include <QApplication>

Progress flashWrite;
Progress flashRead;

bool readFlash(CP2112 *dev, SMBus *batt, QByteArray *flashArray)
{
    QByteArray sectorArray;//Массив для данных из сектора
    flashRead.setProgress(0);

    for (quint8 SectorID=0; SectorID<SECTOR_AMOUNT ; SectorID++) {//Читаем все сектора по-порядку
        if(!readSector(dev, batt, SectorID, &sectorArray))return false;
        flashArray->append(sectorArray);//Данные из секторов заносим в массив
    }
    flashRead.setProgress(65);
    return true;
}

bool readSector(CP2112 *dev, SMBus *batt, quint8 sectorID, QByteArray *sectorArray)
{
    quint16 temp = 0;
    quint8  len  = 0;
    QByteArray pageArray;//Массив для данных из страницы

    if(!batt->writeWord(dev,0x77,sectorID))return false;//Запишем номер сектора в 0x77;
    QThread::msleep(10);
    if(!batt->readWord(dev,0x77,&temp))return false;//Прочитаем номер сектора из 0x77;
    QThread::msleep(10);
    if(temp!=sectorID)return false;//Если значения не совпадают, выходим с ошибкой

    sectorArray->clear(); //Очистим массив, иначе при повторном вызове функции запись будет дальше чем 256 байт
    for(quint8 page = 0x78; page<(0x78+PAGE_IN_SECTOR); page++){//0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f Адреса страниц
        if(!batt->readBlock(dev,page,&pageArray,&len))return false; //Читаем страницу
        sectorArray->append(pageArray);
        QThread::msleep(50);
        quint8 val = flashRead.getProgress();
        val++;
        flashRead.setProgress(val);
        qApp->processEvents();//Дадим выполниться другим событиям
    }
    return true;
}

bool writeFlash(CP2112 *dev, SMBus *batt, QByteArray *flashArray, bool pec)
{
    quint16 offset = 0;     //Смещение относительно начала массива флеш
    QByteArray sectorArray; //Создадим массив данных сектора
    flashWrite.setProgress(0);

    for(qint8 sector=0; sector<SECTOR_AMOUNT; sector++){
        offset = sector*SECTOR_SIZE;
        sectorArray=QByteArray::fromRawData(flashArray->data()+offset, SECTOR_SIZE);
        if(!writeSector(dev, batt, sector, &sectorArray, pec))return false;
    }

    QThread::msleep(300);
    batt->writeWord(dev,0x0021,0x00); //Gas Gauge Enable

    QThread::msleep(1500);
    batt->writeWord(dev,0x0041,0x00);//Reset

    quint8 val = flashWrite.getProgress();
    val++;
    flashWrite.setProgress(val);

    return true;
}

bool writeSector(CP2112 *dev, SMBus *batt, qint8 sector, QByteArray *sectorArray, bool pec)
{
    quint16 temp    = 0;
    quint8 offset   = 0;    //Смещение относительно начала сектора
    QByteArray pageArray;   //Создадим массив данных страницы

    const quint8 sectorAddress  = 0x77; //По этому адресу пишется адрес сектора
    const quint8 startAddress   = 0x78; //Первый адрес страниц памяти

    if(!batt->writeWord(dev,sectorAddress, sector))return false;//Запишем номер сектора
    QThread::msleep(10);
    if(!batt->readWord(dev,sectorAddress, &temp))return false;//Прочитаем номер сектора
    QThread::msleep(10);
    if(sector!=temp)return false;//Проверим корректно ли записалось

    for(quint8 page = 0; page<PAGE_IN_SECTOR; page++)
    {
        offset = page*FLASH_PAGE_SIZE;
        pageArray = QByteArray::fromRawData(sectorArray->data()+offset, FLASH_PAGE_SIZE);
        if(!batt->writeBlock(dev, page+startAddress, &pageArray, pec))return false;
        QThread::msleep(200);
        quint8 val = flashWrite.getProgress();
        val++;
        flashWrite.setProgress(val);
        qApp->processEvents();//Дадим выполниться другим событиям
    }
    return true;
}
