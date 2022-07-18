#ifndef CP2112_H
#define CP2112_H

#include <QObject>
#include <QVariant>
#include "Library\Windows\SLABCP2112.h"
#include "cp2112_config.h"

enum
{
    Bitrate,
    ACK_Address,
    Auto_Respond,
    Write_Timeout,
    Read_Timeout,
    Transfer_Retries,
    SCL_Low_Timeout,
    Response_Timeout,
    RX_TX_Leds_On,
};

class CP2112 : public QObject
{
    Q_OBJECT

public:
//    explicit CP2112(QObject *parent = nullptr);
    CP2112();
    QString & getStatus();
    QString & getStatus_0();
    QString & getStatus_1();
    QString & getConnected();
    QString & getSerial();
    QString & getManufacturer();
    QString & getProduct();
    quint16   getSpeed();
    void      readINI();
    void      writeINI();
    QVariant  getConfig(quint8 num);
    bool      isInTransferMode();
    friend class SMBus;

public slots:
    bool init();
    void close();
    void reset();
    void setStatus(HID_SMBUS_STATUS status);
    void setStatus_0(HID_SMBUS_S0 status_0);
    void setStatus_1(HID_SMBUS_S1 status_1);
    void setConnected(bool connected);
    void setSerial(HID_SMBUS_DEVICE_STR serial);
    void setManufacturer(HID_SMBUS_DEVICE_STR manufacturer);
    void setProduct(HID_SMBUS_DEVICE_STR product);
    void setConfig(quint8 num, QVariant data);
    void setTransfer(bool transfer);

signals:
    void status_Changed(const QString &);
    void status_0_Changed (const QString &);
    void status_1_Changed (const QString &);
    void connected_Changed (const QString &);
    void serial_Changed(const QString &);
    void manufacturer_Changed (const QString &);
    void product_Changed (const QString &);

private:

    HID_SMBUS_DEVICE    m_hidSmbus;
    HID_SMBUS_STATUS    m_status;
    HID_SMBUS_S0        m_status_0;
    HID_SMBUS_S1        m_status_1;
    bool                m_connected;
    QString             m_str_status;
    QString             m_str_status_0;
    QString             m_str_status_1;
    QString             m_str_connected;
    QString             m_serial;
    QString             m_manufacturer;
    QString             m_product;
    bool                m_transfer;

    struct config
    {
        QString     m_name;
        QVariant    m_defaultData;
        QVariant    m_userData;
    };

    const static quint8 configsize = 9;

    config cp2112config[configsize]=
    {
        {"BITRATE_HZ=",             BITRATE_HZ,            0},
        {"ACK_ADDRESS=",            ACK_ADDRESS,           0},
        {"AUTO_RESPOND=",           AUTO_RESPOND,          0},
        {"WRITE_TIMEOUT_MS=",       WRITE_TIMEOUT_MS,      0},
        {"READ_TIMEOUT_MS=",        READ_TIMEOUT_MS,       0},
        {"TRANSFER_RETRIES=",       TRANSFER_RETRIES,      0},
        {"SCL_LOW_TIMEOUT=",        SCL_LOW_TIMEOUT,       0},
        {"RESPONSE_TIMEOUT_MS=",    RESPONSE_TIMEOUT_MS,   0},
        {"RX_TX_LEDS_ON=",          RX_TX_LEDS_ON,         0},
    };

    QVariant getDataFromString (QString str, bool *ok);
    void initFail(HID_SMBUS_STATUS status);

};

#endif // CP2112_H
