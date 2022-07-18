#include "cp2112.h"
#include <QFile>
#include <QMessageBox>

//CP2112::CP2112(QObject *parent)
//    : QObject{parent}
CP2112::CP2112():m_hidSmbus(0),m_transfer(false)
{

}

//Инициализация cp2112. Если успешно, возвращает true
bool CP2112::init()
{
    HID_SMBUS_STATUS    status;
    DWORD               numDevices  = -1;   //Количество найденных устройств
    INT                 devNumber   = 0;    //Пока нет реализации выбора из нескольких устройств,
    //считаем, что устройство одно, под номером 0
    BOOL                opened;             //Открылось ли устройство

    setConnected(false);//Статус по умолчанию - не подключено

    status = HidSmbus_GetNumDevices(&numDevices, VID, PID);
    if (status !=HID_SMBUS_SUCCESS)
    {
        initFail(status);
        return false;
    }

    if (numDevices==0)
    {
        return false;
    }

    status = HidSmbus_Open(&m_hidSmbus, devNumber, VID, PID);
    if (status != HID_SMBUS_SUCCESS)
    {
        initFail(status);
        return false;
    }

    status = HidSmbus_IsOpened(m_hidSmbus, &opened);
    if (status != HID_SMBUS_SUCCESS || !opened)
    {
        initFail(status);
        return false;
    }


    //Передаем user конфиг на устройство
    status =  HidSmbus_SetSmbusConfig(m_hidSmbus,
                                      //                                          BITRATE_HZ,
                                      //                                          ACK_ADDRESS,
                                      //                                          AUTO_RESPOND,
                                      //                                          WRITE_TIMEOUT_MS,
                                      //                                          READ_TIMEOUT_MS,
                                      //                                          SCL_LOW_TIMEOUT,
                                      //                                          TRANSFER_RETRIES
                                      cp2112config[Bitrate].m_userData.toUInt(),
                                      cp2112config[ACK_Address].m_userData.toUInt(),
                                      cp2112config[Auto_Respond].m_userData.toUInt(),
                                      cp2112config[Write_Timeout].m_userData.toUInt(),
                                      cp2112config[Read_Timeout].m_userData.toUInt(),
                                      cp2112config[SCL_Low_Timeout].m_userData.toUInt(),
                                      cp2112config[Transfer_Retries].m_userData.toUInt()
                                      );

    if (status != HID_SMBUS_SUCCESS)
    {
        initFail(status);
        return false;
    }

    //Теперь читаем конфиг с устройства, чтобы убедится, что он записался
    DWORD   bitRate;              //Для этого нужно объявить кучу переменных
    BYTE    address;
    BOOL    autoReadRespond;
    WORD    writeTimeout;
    WORD    readTimeout;
    BOOL    sclLowTimeout;
    WORD    transferRetries;
    //И вот только теперь читаем конфиг
    status =  HidSmbus_GetSmbusConfig(m_hidSmbus,
                                      &bitRate,
                                      &address,
                                      &autoReadRespond,
                                      &writeTimeout,
                                      &readTimeout,
                                      &sclLowTimeout,
                                      &transferRetries);
    //Если конфиг передался с ошибкой
    if (status          != HID_SMBUS_SUCCESS                                ||
            bitRate         != cp2112config[Bitrate].m_userData.toUInt()        ||
            address         != cp2112config[ACK_Address].m_userData.toUInt()    ||
            autoReadRespond != cp2112config[Auto_Respond].m_userData.toInt()    ||
            writeTimeout    != cp2112config[Write_Timeout].m_userData.toUInt()  ||
            readTimeout     != cp2112config[Read_Timeout].m_userData.toUInt()   ||
            sclLowTimeout   != cp2112config[SCL_Low_Timeout].m_userData.toInt() ||
            transferRetries != cp2112config[Transfer_Retries].m_userData.toUInt())
    {
        initFail(status);
        return false;
    }

    //Передаем таймаут конфиг на устройство
    status = HidSmbus_SetTimeouts(m_hidSmbus, cp2112config[Response_Timeout].m_userData.toUInt());
    if (status != HID_SMBUS_SUCCESS)
    {
        initFail(status);
        return false;
    }
    //Проверяем запись
    DWORD response_timeout_ms;

    status = HidSmbus_GetTimeouts(m_hidSmbus, &response_timeout_ms);
    if (status              != HID_SMBUS_SUCCESS    ||
            response_timeout_ms != cp2112config[Response_Timeout].m_userData.toUInt())
    {
        initFail(status);
        return false;
    }


    //Подготовим конфигурацию GPIO
    BYTE    direction   = 0x00; //is a bitmask that specifies each GPIO pin’s direction.
    BYTE    mode        = 0x00; //is a bitmask that specifies each GPIO pin’s mode.
    BYTE    function    = 0x00; //is a bitmask that specifies the special behavior of GPIO.0, GPIO.1, and GPIO.7
    BYTE    m_clkDiv    = 0x00; //is the clock output divider value used for GPIO.7 when configured in clock output mode.

    //Если направление GPIO - выход, то включаем бит по маске
    if (GPIO_0)     direction |= HID_SMBUS_MASK_GPIO_0;
    if (GPIO_1)     direction |= HID_SMBUS_MASK_GPIO_1;
    if (GPIO_2)     direction |= HID_SMBUS_MASK_GPIO_2;
    if (GPIO_3)     direction |= HID_SMBUS_MASK_GPIO_3;
    if (GPIO_4)     direction |= HID_SMBUS_MASK_GPIO_4;
    if (GPIO_5)     direction |= HID_SMBUS_MASK_GPIO_5;
    if (GPIO_6)     direction |= HID_SMBUS_MASK_GPIO_6;
    if (GPIO_7)     direction |= HID_SMBUS_MASK_GPIO_7;

    //Если режим GPIO - push-pull, то включаем бит по маске
    if (GPIO_0_MODE)	mode |= HID_SMBUS_MASK_GPIO_0;
    if (GPIO_1_MODE)	mode |= HID_SMBUS_MASK_GPIO_1;
    if (GPIO_2_MODE)	mode |= HID_SMBUS_MASK_GPIO_2;
    if (GPIO_3_MODE)	mode |= HID_SMBUS_MASK_GPIO_3;
    if (GPIO_4_MODE)	mode |= HID_SMBUS_MASK_GPIO_4;
    if (GPIO_5_MODE)	mode |= HID_SMBUS_MASK_GPIO_5;
    if (GPIO_6_MODE)	mode |= HID_SMBUS_MASK_GPIO_6;
    if (GPIO_7_MODE)	mode |= HID_SMBUS_MASK_GPIO_7;

    //Если используется специальный режим GPIO, то включаем бит по маске
    //В расчет берутся только GPIO_0, GPIO_1, GPIO_7.
    //Для остальных GPIO специальный режим отсутствует
    if(cp2112config[RX_TX_Leds_On].m_userData==true)
    {
        function |= HID_SMBUS_MASK_FUNCTION_GPIO_0_TXT;
        function |= HID_SMBUS_MASK_FUNCTION_GPIO_1_RXT;
    }

    status = HidSmbus_SetGpioConfig(m_hidSmbus, direction, mode, function, m_clkDiv);
    if (status != HID_SMBUS_SUCCESS)
    {
        initFail(status);
        return false;
    }

    //Проверим как записался конфиг GPIO

    BYTE    _direction   = 0x00; //is a bitmask that specifies each GPIO pin’s direction.
    BYTE    _mode        = 0x00; //is a bitmask that specifies each GPIO pin’s mode.
    BYTE    _function    = 0x00; //is a bitmask that specifies the special behavior of GPIO.0, GPIO.1, and GPIO.7
    BYTE    _m_clkDiv    = 0x00; //is the clock output divider value used for GPIO.7 when configured in clock output mode.

    status = HidSmbus_GetGpioConfig(m_hidSmbus, &_direction, &_mode, &_function, &_m_clkDiv);
    //Если конфиг передался с ошибкой
    if (status      != HID_SMBUS_SUCCESS    ||
            _direction  != direction            ||
            _mode       != mode                 ||
            _function   != function             ||
            _m_clkDiv   != m_clkDiv             )
    {
        initFail(status);
        return false;
    }

    //Прочитаем спецификацию CP2112
    HID_SMBUS_DEVICE_STR serial={0};
    HID_SMBUS_DEVICE_STR manufacturer={0};
    HID_SMBUS_DEVICE_STR product={0};
    status = HidSmbus_GetOpenedString(m_hidSmbus,serial,HID_SMBUS_GET_SERIAL_STR);
    if (status      != HID_SMBUS_SUCCESS)
    {
        initFail(status);
        return false;
    }
    setSerial(serial);
    status = HidSmbus_GetOpenedString(m_hidSmbus,manufacturer,HID_SMBUS_GET_MANUFACTURER_STR);
    if (status      != HID_SMBUS_SUCCESS)
    {
        initFail(status);
        return false;
    }
    setManufacturer(manufacturer);
    status = HidSmbus_GetOpenedString(m_hidSmbus,product,HID_SMBUS_GET_PRODUCT_STR);
    if (status      != HID_SMBUS_SUCCESS)
    {
        initFail(status);
        return false;
    }
    setProduct(product);

    setStatus(status);
    setConnected(true);//Статус подключено
    return true;
}

void CP2112::initFail(HID_SMBUS_STATUS status)
{
    setStatus(status);
    char str[] = "";
    setSerial(str);
    setManufacturer(str);
    setProduct(str);
}

void CP2112::close()
{
    HidSmbus_Close(m_hidSmbus);
}

void CP2112::reset()
{
    HidSmbus_CancelTransfer(m_hidSmbus);
    HidSmbus_CancelIo(m_hidSmbus);
    HidSmbus_Reset(m_hidSmbus);
}

QString & CP2112::getStatus()
{
    m_str_status = "";

    switch (m_status) {
    case HID_SMBUS_SUCCESS:
        m_str_status = "HID_SMBUS_SUCCESS";
        break;
    case HID_SMBUS_DEVICE_NOT_FOUND:
        m_str_status = "HID_SMBUS_DEVICE_NOT_FOUND";
        break;
    case HID_SMBUS_INVALID_HANDLE:
        m_str_status = "HID_SMBUS_INVALID_HANDLE";
        break;
    case HID_SMBUS_INVALID_DEVICE_OBJECT:
        m_str_status = "HID_SMBUS_INVALID_DEVICE_OBJECT";
        break;
    case HID_SMBUS_INVALID_PARAMETER:
        m_str_status = "HID_SMBUS_INVALID_PARAMETER";
        break;
    case HID_SMBUS_INVALID_REQUEST_LENGTH:
        m_str_status = "HID_SMBUS_INVALID_REQUEST_LENGTH";
        break;
    case HID_SMBUS_READ_ERROR:
        m_str_status = "HID_SMBUS_READ_ERROR";
        break;
    case HID_SMBUS_WRITE_ERROR:
        m_str_status = "HID_SMBUS_WRITE_ERROR";
        break;
    case HID_SMBUS_READ_TIMED_OUT:
        m_str_status = "HID_SMBUS_READ_TIMED_OUT";
        break;
    case HID_SMBUS_WRITE_TIMED_OUT:
        m_str_status = "HID_SMBUS_WRITE_TIMED_OUT";
        break;
    case HID_SMBUS_DEVICE_IO_FAILED:
        m_str_status = "HID_SMBUS_DEVICE_IO_FAILED";
        break;
    case HID_SMBUS_DEVICE_ACCESS_ERROR:
        m_str_status = "HID_SMBUS_DEVICE_ACCESS_ERROR";
        break;
    case HID_SMBUS_DEVICE_NOT_SUPPORTED:
        m_str_status = "HID_SMBUS_DEVICE_NOT_SUPPORTED";
        break;
    case HID_SMBUS_UNKNOWN_ERROR:
        m_str_status = "HID_SMBUS_UNKNOWN_ERROR";
        break;
    }
    return m_str_status;
}

QString & CP2112::getStatus_0()
{
    m_str_status_0 = "";

    switch(m_status_0){
    case HID_SMBUS_S0_IDLE:
        m_str_status_0 = "HID_SMBUS_S0_IDLE";
        break;
    case HID_SMBUS_S0_BUSY:
        m_str_status_0 = "HID_SMBUS_S0_BUSY";
        break;
    case HID_SMBUS_S0_COMPLETE:
        m_str_status_0 = "HID_SMBUS_S0_COMPLETE";
        break;
    case HID_SMBUS_S0_ERROR:
        m_str_status_0 = "HID_SMBUS_S0_ERROR";
        break;
    }
    return m_str_status_0;
}

QString & CP2112::getStatus_1()
{
    m_str_status_1 = "";

    if(m_status_0 == HID_SMBUS_S0_BUSY){
        switch(m_status_1){
        case HID_SMBUS_S1_BUSY_ADDRESS_ACKED:
            m_str_status_1 = "HID_SMBUS_S1_BUSY_ADDRESS_ACKED";
            break;
        case HID_SMBUS_S1_BUSY_ADDRESS_NACKED:
            m_str_status_1 = "HID_SMBUS_S1_BUSY_ADDRESS_NACKED";
            break;
        case HID_SMBUS_S1_BUSY_READING:
            m_str_status_1 = "HID_SMBUS_S1_BUSY_READING";
            break;
        case HID_SMBUS_S1_BUSY_WRITING:
            m_str_status_1 = "HID_SMBUS_S1_BUSY_WRITING";
            break;
        }
    }else if(m_status_0 == HID_SMBUS_S0_ERROR){
        switch(m_status_1){
        case HID_SMBUS_S1_ERROR_TIMEOUT_NACK:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_TIMEOUT_NACK";
            break;
        case HID_SMBUS_S1_ERROR_TIMEOUT_BUS_NOT_FREE:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_TIMEOUT_BUS_NOT_FREE";
            break;
        case HID_SMBUS_S1_ERROR_ARB_LOST:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_ARB_LOST";
            break;
        case HID_SMBUS_S1_ERROR_READ_INCOMPLETE:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_READ_INCOMPLETE";
            break;
        case HID_SMBUS_S1_ERROR_WRITE_INCOMPLETE:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_WRITE_INCOMPLETE";
            break;
        case HID_SMBUS_S1_ERROR_SUCCESS_AFTER_RETRY:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_SUCCESS_AFTER_RETRY";
            break;
        }
    }
    return m_str_status_1;
}

QString &  CP2112::getConnected()
{
    return m_str_connected;
}

quint16 CP2112::getSpeed()
{
    return cp2112config[0].m_userData.toUInt();
}

void CP2112::setStatus(HID_SMBUS_STATUS status)
{

    if (status != m_status) {
        m_status = status;
    }
    QString str = getStatus();
    emit status_Changed(str);
}

void CP2112::setStatus_0(HID_SMBUS_S0 status_0)
{
    if (status_0 != m_status_0) {
        m_status_0 = status_0;
    }
    QString str = getStatus_0();
    emit status_0_Changed(str);
}

void CP2112::setStatus_1(HID_SMBUS_S1 status_1)
{
    if (status_1 != m_status_1) {
        m_status_1 = status_1;
    }
    QString str = getStatus_1();
    emit status_1_Changed(str);
}

void CP2112::setConnected(bool connected)
{
    QString str="";
    if(connected){
        str = "CP2112 connected";
        m_str_connected = str;
    }else{
        str = "CP2112 not connected";
        m_str_connected = str;
        str.clear();
        m_serial = str;
        emit serial_Changed(str);
        m_manufacturer = str;
        emit manufacturer_Changed(str);
        m_product = str;
        emit product_Changed(str);
    }

    emit connected_Changed(str);
}

void CP2112::setSerial(HID_SMBUS_DEVICE_STR serial)
{
    QString str="";
    m_serial=serial;
    str = getSerial();
    emit serial_Changed(str);
}

void CP2112::setManufacturer(HID_SMBUS_DEVICE_STR manufacturer)
{
    if (manufacturer != m_manufacturer) {
        m_manufacturer = manufacturer;
    }
    QString str = getManufacturer();
    emit manufacturer_Changed(str);
}

void CP2112::setProduct(HID_SMBUS_DEVICE_STR product)
{
    if (product != m_product) {
        m_product = product;
    }
    QString str = getProduct();
    emit product_Changed(str);
}

QString & CP2112::getSerial()
{
    return m_serial;
}

QString & CP2112::getManufacturer()
{
    return m_manufacturer;
}

QString & CP2112::getProduct()
{
    return m_product;
}

void CP2112::readINI ()
{
    QString end = "\r\n";

    QFile file;
    file.setFileName("config.ini");
    if(!file.exists())//Если файл не существует
    {
        file.open(QIODevice::NewOnly); //Создадим новый файл
        QByteArray array = {0};
        QString str;

        str = cp2112config[Bitrate].m_name + QString("%1").arg(BITRATE_HZ) + end;
        array.append(str.toUtf8());
        str = cp2112config[ACK_Address].m_name + QString("0x%1").arg(ACK_ADDRESS,2,16,QLatin1Char('0')) + end;
        array.append(str.toUtf8());
        str = cp2112config[Auto_Respond].m_name + QString(AUTO_RESPOND ? "true" : "false") + end;
        array.append(str.toUtf8());
        str = cp2112config[Write_Timeout].m_name + QString("%1").arg(WRITE_TIMEOUT_MS) + end;
        array.append(str.toUtf8());
        str = cp2112config[Read_Timeout].m_name + QString("%1").arg(READ_TIMEOUT_MS) + end;
        array.append(str.toUtf8());
        str = cp2112config[Transfer_Retries].m_name + QString("%1").arg(TRANSFER_RETRIES) + end;
        array.append(str.toUtf8());
        str = cp2112config[SCL_Low_Timeout].m_name + QString(SCL_LOW_TIMEOUT ? "true" : "false") + end;
        array.append(str.toUtf8());
        str = cp2112config[Response_Timeout].m_name + QString("%1").arg(RESPONSE_TIMEOUT_MS) + end;
        array.append(str.toUtf8());
        str = cp2112config[RX_TX_Leds_On].m_name + QString(RX_TX_LEDS_ON ? "true" : "false") + end;
        array.append(str.toUtf8());

        if(file.write(array)==-1){//Сохраняем файл. Если не можем сохранить, применяем настройки по умолчанию
            QMessageBox::critical(nullptr,"Error","Can't write ini file\nThe default settings will be applied.");
        }
        file.close();//Закрываем файл

        for(quint8 i=0; i<configsize; i++){//Применяем настройки по умолчанию
            cp2112config[i].m_userData = cp2112config[i].m_defaultData;
        }
        QMessageBox::information(nullptr,"Info","First run of the program\nThe default parameters are written to a file config.ini");
        return;//Новый конфиг записан, все данные по умолчанию, выходим

    }else if (!file.open(QFile::ReadOnly)){ //проверяем открывается ли для чтения
        QMessageBox::critical(nullptr,"Error","Can't read ini file\nThe default settings will be applied.");
        for(quint8 i=0; i<configsize; i++){//Применяем настройки по умолчанию
            cp2112config[i].m_userData = cp2112config[i].m_defaultData;
        }
        return;//Файл есть, но не открывается для чтения, выходим

    }else{
        while (!file.atEnd()) {//Читаем конфиг из файла
            bool ok=false;
            QString line = file.readLine();
            for(quint8 i=0; i<configsize; i++){
                if(line.contains(cp2112config[i].m_name, Qt::CaseSensitive)){
                    cp2112config[i].m_userData = getDataFromString (line, &ok);
                    if(!ok) cp2112config[i].m_userData = cp2112config[i].m_defaultData;
                }
            }
        }
        file.close();                   //Закрываем файл
        return;
    }
}

QVariant CP2112::getDataFromString (QString str, bool *ok)
{
    str = str.section('=', 1,1);//Отделим все до знака "="
    str = str.section('\r',0,0); //Оставим только то, что перед \r
    quint8 strLen = str.size();
    if(strLen == 0)
    {
        *ok=false; //Если строка пустая, выходим
        return 0;
    }
    if (str == "true") return true;
    if (str == "false") return false;
    quint32 temp = str.toUInt(ok);
    return temp;
}

QVariant CP2112::getConfig(quint8 num)
{
    return cp2112config[num].m_userData;
}

void CP2112::setConfig(quint8 num, QVariant data)
{
    cp2112config[num].m_userData = data;
}

void CP2112::writeINI ()
{
    QString end = "\r\n";

    QFile file;
    file.setFileName("config.ini");
    if(!file.exists())return;//Если файл не существует

    file.open(QIODevice::WriteOnly); //Если файл доступен для записи
    QByteArray array = {0};
    QString str;

    str = cp2112config[Bitrate].m_name + QString("%1").arg(cp2112config[Bitrate].m_userData.toInt())+ end;
    array.append(str.toUtf8());
    str = cp2112config[ACK_Address].m_name + QString("0x%1").arg(cp2112config[ACK_Address].m_userData.toInt(),2,16,QLatin1Char('0'))+ end;
    array.append(str.toUtf8());
    str = cp2112config[Auto_Respond].m_name + QString(cp2112config[ACK_Address].m_userData.toBool() ? "true" : "false") + end;
    array.append(str.toUtf8());
    str = cp2112config[Write_Timeout].m_name + QString("%1").arg(cp2112config[Write_Timeout].m_userData.toInt())+ end;
    array.append(str.toUtf8());
    str = cp2112config[Read_Timeout].m_name + QString("%1").arg(cp2112config[Read_Timeout].m_userData.toInt())+ end;
    array.append(str.toUtf8());
    str = cp2112config[Transfer_Retries].m_name + QString("%1").arg(cp2112config[Transfer_Retries].m_userData.toInt())+ end;
    array.append(str.toUtf8());
    str = cp2112config[SCL_Low_Timeout].m_name + QString(cp2112config[SCL_Low_Timeout].m_userData.toBool() ? "true" : "false") + end;
    array.append(str.toUtf8());
    str = cp2112config[Response_Timeout].m_name + QString("%1").arg(cp2112config[Response_Timeout].m_userData.toInt())+ end;
    array.append(str.toUtf8());
    str = cp2112config[RX_TX_Leds_On].m_name + QString(cp2112config[RX_TX_Leds_On].m_userData.toBool() ? "true" : "false") + end;
    array.append(str.toUtf8());

    if(file.write(array)==-1){//Сохраняем файл. Если не можем сохранить, применяем настройки по умолчанию
        QMessageBox::critical(nullptr,"Error","Can't write ini file\nThe default settings will be applied.");
    }
    file.close();
}

void CP2112::setTransfer(bool transfer)
{
    transfer ? m_transfer=true : m_transfer=false;
}
bool CP2112::isInTransferMode()
{
    if (m_transfer){return true;}
    return false;
}

