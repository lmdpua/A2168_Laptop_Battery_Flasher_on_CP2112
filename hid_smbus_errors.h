#ifndef HID_SMBUS_ERRORS_H
#define HID_SMBUS_ERRORS_H

#include <QObject>
#include "Library\Windows\SLABCP2112.h"

//Дополнительные статусы чтобы стирать сообщения об ошибках
#define HID_SMBUS_STATUS_CLEAR  0x17 //Следующий статус после стандартных
#define HID_SMBUS_S0_CLEAR      0xFF

class HID_SMBUS_Errors : public QObject
{
    Q_OBJECT

public:
    HID_SMBUS_Errors(); //Конструктор по умолчанию
    QString & getStatus();
    QString & getStatus_0();
    QString & getStatus_1();

public slots:
    void setStatus(HID_SMBUS_STATUS status);
    void setStatus_0(HID_SMBUS_S0 status_0);
    void setStatus_1(HID_SMBUS_S1 status_1);

signals:
    void status_Changed(const QString &);
    void status_0_Changed (const QString &);
    void status_1_Changed (const QString &);

private:
    HID_SMBUS_STATUS m_status;
    HID_SMBUS_S0 m_status_0;
    HID_SMBUS_S1 m_status_1;
    QString m_str_status;
    QString m_str_status_0;
    QString m_str_status_1;
};

#endif // HID_SMBUS_ERRORS_H
