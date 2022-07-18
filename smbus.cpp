#include "smbus.h"
#include "cp2112.h"
#include "A2168_interface.h"

SMBus::SMBus(QObject *parent)
    : QObject{parent}
{

}

bool SMBus::addressReadRequest(CP2112 *dev, quint8 reg, QByteArray *dataArray, quint8 dataLen)
{

    HID_SMBUS_DEVICE    device = dev->m_hidSmbus;
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус
    HID_SMBUS_S1        status1;                //Расширенный статус
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead;              //Количество прочитанных байт
    BYTE                numBytesRead = 0;       //Количество прочитанных байт
    BYTE                totalNumBytesRead = 0;  //Сколько всего байт прочитано
    WORD                targetAddressSize = 1;  //Количество байт в адресе
    BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]={0}; //Приемный буфер
    BYTE                targetAddress[16]={0};  //Адрес для чтения

    targetAddress[0] = reg;

    status = HidSmbus_AddressReadRequest(device, BATTERY_ADDRESS << 1, dataLen, targetAddressSize, targetAddress);
    if (status != HID_SMBUS_SUCCESS){
        dev->setStatus(status);
        return false;
    }

    do{
    status = HidSmbus_TransferStatusRequest(device);
    if (status != HID_SMBUS_SUCCESS){
        dev->setStatus(status);
        return false;
    }

    status = HidSmbus_GetTransferStatusResponse(device, &status0, &status1, &numRetries, &bytesRead);
    dev->setStatus(status);
    dev->setStatus_0(status0);
    dev->setStatus_1(status1);

    if(status != HID_SMBUS_SUCCESS)return false;
//    if(status0 == HID_SMBUS_S0_ERROR)return false;
//    if(bytesRead!=dataLen)return false;
    }while(status0);

    status = HidSmbus_ForceReadResponse(device, dataLen);
    if (status != HID_SMBUS_SUCCESS){
        dev->setStatus(status);
        return false;
    }

    do
    {
        status = HidSmbus_GetReadResponse(device, &status0, buffer, HID_SMBUS_MAX_READ_RESPONSE_SIZE, &numBytesRead);
        if (status != HID_SMBUS_SUCCESS){
            dev->setStatus(status);
            return false;
        }
        dataArray->clear();//Очистим массив, иначе при повторном вызове функции запись будет дальше, чем 32байта
        dataArray->append((char *)buffer, numBytesRead);    //Копируем часть прочитанных данных
//        memcpy(&return_data[totalNumBytesRead], buffer, numBytesRead);  во временный
        //буфер, начиная с адреса totalNumBytesRead
        totalNumBytesRead += numBytesRead;                      //Увеличиваем адрес на величину прочитанных байт
    } while (totalNumBytesRead < dataLen);

    return true;
}

bool SMBus::readWord(CP2112 *dev, quint8 reg, quint16 *data)
{
    const quint8 dataLen = 2;
    QByteArray dataArray;
    if(!addressReadRequest(dev, reg, &dataArray, dataLen))return false;
    *data = ((quint8)dataArray.at(1) << 8) | (quint8)dataArray.at(0);
    return true;}

bool SMBus::readBlock(CP2112 *dev, quint8 reg, QByteArray *dataArray, quint8 *blockLen)
{
    const quint8 dataLen = 32+1;
    quint8 n;

    if(!addressReadRequest(dev, reg, dataArray, dataLen))return false;
    (quint8)dataArray->at(0)>32 ? n=32 : n = dataArray->at(0);
    *blockLen = n;
    *dataArray = dataArray->sliced(1,n); //Удалим первый байт, он содержит длину блока
    //и все после len байта

    return true;
}

bool SMBus::isConnected (CP2112 *dev)
{
    HID_SMBUS_DEVICE    device = dev->m_hidSmbus;
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead=0;            //Количество прочитанных байт HidSmbus_GetTransferStatusResponse
    const quint8        dataLen = 1;

    status = HidSmbus_ReadRequest(device, BATTERY_ADDRESS << 1, dataLen);
    if (status != HID_SMBUS_SUCCESS){
        dev->setStatus(status);
        return false;
    }
    status = HidSmbus_TransferStatusRequest(device);
    if (status != HID_SMBUS_SUCCESS){
        dev->setStatus(status);
        return false;
    }

    status = HidSmbus_GetTransferStatusResponse(device, &status0, &status1, &numRetries, &bytesRead);

    dev->setStatus(status);
    dev->setStatus_0(status0);
    dev->setStatus_1(status1);

    if(status != HID_SMBUS_SUCCESS)return false;
    if(status0 == HID_SMBUS_S0_ERROR)return false;
    if(bytesRead!=dataLen)return false;
    return true;
}

bool SMBus::writeRequest(CP2112 *dev, QByteArray *dataArray)
{
    HID_SMBUS_DEVICE    device = dev->m_hidSmbus;
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead;              //Количество прочитанных байт HidSmbus_GetTransferStatusResponse

    quint8 buffer[MAX_SMBUS_BLOCK_SIZE+1]={0};  //Исходящий буфер с максимальным размером 33 байта
    quint8 dataLen = dataArray->size();

    for(quint8 i=0; i<dataLen; i++){
        buffer[i]=dataArray->at(i);
    }

    status = HidSmbus_WriteRequest(device, BATTERY_ADDRESS << 1, buffer, dataLen);
    if (status!=HID_SMBUS_SUCCESS){
        dev->setStatus(status);
        return false;
    }

    do{
        status = HidSmbus_TransferStatusRequest(device);
        if (status!=HID_SMBUS_SUCCESS){
            dev->setStatus(status);
            return false;
        }

        status = HidSmbus_GetTransferStatusResponse(device, &status0, &status1, &numRetries, &bytesRead);

        dev->setStatus(status);
        dev->setStatus_0(status0);
        dev->setStatus_1(status1);

        if(status != HID_SMBUS_SUCCESS)return false;
//        if(status0 == HID_SMBUS_S0_ERROR)return false;

    }while (status0);

    return true;
}

bool SMBus::writeWord(CP2112 *dev, quint8 reg, quint16 data)
{
    QByteArray buffer;
    buffer.resize(3);
    buffer[0] = reg;
    buffer[1] = data & 0xff;
    buffer[2] = data >> 8;
    return writeRequest(dev, &buffer);
}

bool SMBus::writeBlock(CP2112 *dev, quint8 reg, QByteArray *dataArray, bool pec)
{

    quint8 size = dataArray->size();
    QByteArray buffer;

    buffer.resize(2+size);
    buffer[0] = reg;             //Нулевой элемент - адрес регистра
    buffer[1] = size;            //Первый элемент - размер блока
    for(quint8 i=0; i<size; i++){   //Копируем массив в буфер,
        buffer[i+2]=dataArray->at(i);//начиная со второго элемента
    }
    if(pec){
        buffer.prepend(BATTERY_ADDRESS<<1);
        quint8 PEC=getPEC(&buffer);
        buffer = buffer.sliced(1);
        buffer.append(PEC);
    }

    return writeRequest(dev, &buffer);
}

quint8 SMBus::getPEC(QByteArray *data)
{
    quint8 len = data->size();
    quint8 crc = 0x00;
    quint8 i, j;
    for (i = 0; i < len; i++) {
        crc ^= (quint8)data->at(i);
        for (j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0){
                crc = (quint8)((crc << 1) ^ 0x07);
            }else{
                crc <<= 1;
            }
        }
    }
    return crc;
}

