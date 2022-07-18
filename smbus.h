#ifndef SMBUS_H
#define SMBUS_H

#include "cp2112.h"

#include <QObject>



class SMBus : public QObject
{
    Q_OBJECT
public:
    explicit SMBus(QObject *parent = nullptr);

    bool addressReadRequest(CP2112 *dev, quint8 reg, QByteArray *dataArray, quint8 dataLen);
    bool readWord(CP2112 *dev, quint8 reg, quint16 *data);
    bool readBlock(CP2112 *dev, quint8 reg, QByteArray *blockData, quint8 *blockLen);
    bool isConnected (CP2112 *dev);
    bool writeRequest(CP2112 *dev, QByteArray *dataArray);
    bool writeWord(CP2112 *dev, quint8 reg, quint16 data);
    bool writeBlock(CP2112 *dev, quint8 reg, QByteArray *dataArray, bool pec);




signals:
private:
    quint8 getPEC(QByteArray *data);
};

#endif // SMBUS_H
