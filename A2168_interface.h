#pragma once
#ifndef A2168_INTERFACE_H
#define A2168_INTERFACE_H

#include "types.h"
#include "qstring.h"
#include "QVariant"
#include "cp2112.h"
#include "smbus.h"
#include "progress.h"

#define FLASH_PAGE_SIZE 32  //32 байта в странице
#define PAGE_IN_SECTOR  8   //8 страниц в секторе
#define SECTOR_AMOUNT   8   //Всего 8 секторов
#define SECTOR_SIZE     256 //Всего в секторе 256 байт PAGE_IN_SECTOR*FLASH_PAGE_SIZE

extern Progress flashWrite;
extern Progress flashRead;

enum SystemCommand
{
    DEVICE_PART_NUMBER = 0x0001,
    FIRMWARE_VERSION   = 0x0002,
    HARDWARE_VERSION   = 0x0003,
    CHEMISTRY_ID       = 0x0008,
    SHUTDOWN           = 0x0010,
    SLEEP              = 0x0011,
    SEAL_DEVICE        = 0x0020,
    GAS_GAUGE_ENABLE   = 0x0021,
    FUSO_ACTIVATION    = 0x0030,
    FUSO_CLEAR         = 0x0031,
    LEDS_ON            = 0x0032,
    LEDS_OFF           = 0x0033,
    DISPLAY_ON         = 0x0034,
    CALIBRATION_MODE   = 0x0040,
    RESET              = 0x0041,
    BOOT_ROM           = 0x0f00,
};

enum Security
{
    UNSEAL,
    FULLACCESS,
    PFCLEAR
};

enum DATA_FORMAT
{
    HEX2,     //HEX 2 байта
    HEX4,     //HEX 4 байта
    UINT1,    //uint8_t
    UINT2,    //uint16_t
    INT2,     //int16_t
    STR4,     //string
    STR14,    //string
    STR20,    //string
    STR31,     //string
    STR32     //string
};

enum ACCESS
{
    R,
    RW
};

struct SBS_COMMANDS
{
    INT          addr;          //Адрес команды
    ACCESS       access;        //Запись/Чтение
    DATA_FORMAT  format;        //Тип данных - uint8_t, int16_t, string...
    QString      name;          //Имя параметра
    QString      unit;          //Единица измерения
};

const SBS_COMMANDS Standard_Commands [38] =
{
    {   0x00,   RW, HEX2,   "ManufacturerAccess",       "-"     },
    {   0x01,   RW, UINT2,  "RemainingCapacityAlarm",   "mAh"   },
    {   0x02,   RW, UINT2,  "RemainingTimeAlarm",       "min"   },
    {   0x03,   RW, HEX2,   "BatteryMode",              "-"     },
    {   0x04,   RW, INT2,   "AtRate",                   "mA"    },
    {   0x05,   R,  UINT2,  "AtRateTimeToFull",         "min"   },
    {   0x06,   R,  UINT2,  "AtRateTimeToEmpty",        "min"   },
    {   0x07,   R,  UINT2,  "AtRateOK",                 "-"     },
    {   0x08,   R,  UINT2,  "Temperature",              "0.1°K" },
    {   0x09,   R,  UINT2,  "Voltage",                  "mV"    },
    {   0x0a,   R,  INT2,   "Current",                  "mA"    },
    {   0x0b,   R,  INT2,   "AverageCurrent",           "mA"    },
    {   0x0c,   R,  UINT1,  "MaxError",                 "%"     },
    {   0x0d,   R,  UINT1,  "RelativeStateOfCharge",    "%"     },
    {   0x0e,   R,  UINT1,  "AbsoluteStateOfCharge",    "%"     },
    {   0x0f,   RW, UINT2,  "RemainingCapacity",        "mAh"   },
    {   0x10,   R,  UINT2,  "FullChargeCapacity",       "mAh"   },
    {   0x11,   R,  UINT2,  "RunTimeToEmpty",           "min"   },
    {   0x12,   R,  UINT2,  "AverageTimeToEmpty",       "min"   },
    {   0x13,   R,  UINT2,  "AverageTimeToFull",        "min"   },
    {   0x14,   R,  UINT2,  "ChargingCurrent",          "mA"    },
    {   0x15,   R,  UINT2,  "ChargingVoltage",          "mV"    },
    {   0x16,   R,  HEX2,   "BatteryStatus",            "-"     },
    {   0x17,   RW, UINT2,  "CycleCount",               "-"     },
    {   0x18,   RW, UINT2,  "DesignCapacity",           "mAh"   },
    {   0x19,   RW, UINT2,  "DesignVoltage",            "mV"    },
    {   0x1a,   RW, HEX2,   "SpecificationInfo",        "-"     },
    {   0x1b,   RW, UINT2,  "ManufactureDate",          "d-m-y" },
    {   0x1c,   RW, HEX2,   "SerialNumber",             "-"     },
    {   0x20,   RW, STR20,  "ManufacturerName",         "-"     },
    {   0x21,   RW, STR20,  "DeviceName",               "-"     },
    {   0x22,   RW, STR4,   "DeviceChemistry",          "-"     },
    {   0x23,   RW, STR14,  "ManufacturerData",         "-"     },
    {   0x2f,   RW, STR20,  "Authenticate",             "-"     },
    {   0x3c,   R,  UINT2,  "CellVoltage4",             "mV"    },
    {   0x3d,   R,  UINT2,  "CellVoltage3",             "mV"    },
    {   0x3e,   R,  UINT2,  "CellVoltage2",             "mV"    },
    {   0x3f,   R,  UINT2,  "Cellvoltage1",             "mV"    }
};

const SBS_COMMANDS Extended_Commands [31] =
{
    {   0x46,   RW, HEX2,   "FETControl",               "-"     },
    {   0x4f,   R,  HEX2,   "StateOfHealth",            "-"     },
    {   0x51,   R,  HEX2,   "SafetyAlarm1",             "-"     },
    {   0x53,   R,  HEX2,   "PFAlarm1",                 "-"     },
    {   0x54,   R,  HEX2,   "OperationStatus",          "-"     },
    {   0x55,   R,  HEX2,   "ChargingStatus",           "-"     },
    {   0x57,   R,  HEX2,   "ResetCount",               "-"     },
    {   0x58,   R,  UINT2,  "WDResetCount",             "-"     },
    {   0x5a,   R,  UINT2,  "PackVoltage",              "mV"    },
    {   0x5d,   R,  UINT2,  "AverageVoltage",           "mV"    },
    {   0x5e,   R,  INT2,   "Temperature1",             "0.1°C" },
    {   0x5f,   R,  INT2,   "Temperature2",             "0.1°C" },
    {   0x60,   RW, HEX4,   "UnSealKey",                "-"     },
    {   0x61,   RW, HEX4,   "FullAccessKey",            "-"     },
    {   0x62,   RW, HEX4,   "PFKey",                    "-"     },
    {   0x63,   RW, HEX4,   "AuthenKey3",               "-"     },
    {   0x64,   RW, HEX4,   "AuthenKey2",               "-"     },
    {   0x65,   RW, HEX4,   "AuthenKey1",               "-"     },
    {   0x66,   RW, HEX4,   "AuthenKey0",               "-"     },
    {   0x69,   R,  HEX2,   "SafetyAlarm2",             "-"     },
    {   0x6a,   R,  INT2,   "InternalTemperature",      "0.1°C" },
    {   0x6b,   R,  HEX2,   "PFAlarm2",                 "-"     },
    {   0x6c,   RW, STR20,  "ManufBlock1",              "-"     },
    {   0x6d,   RW, STR20,  "ManufBlock2",              "-"     },
    {   0x6e,   RW, STR20,  "ManufBlock3",              "-"     },
    {   0x6f,   RW, STR20,  "ManufBlock4",              "-"     },
    {   0x70,   RW, STR31,  "ManufacturerInfo",         "-"     },
    {   0x71,   RW, UINT2,  "SenseResistor",            "μΩ"    },
    {   0x72,   R,  HEX2,   "TempRange",                "-"     },
    {   0x73,   R,  STR32,  "LifeData1",                "-"     },
    {   0x74,   R,  STR32,  "LifeData2",                "-"     },
};

enum SECTOR_ID
{
    SECTOR_0,
    SECTOR_1
};

enum TYPE
{
    VOLTAGE,
    CURRENT,
    TEMPERATURE,
    VERIFICATION,
    CHARGE_TEMP_CFG,
    CHARGE_CFG,

    CAPAСITY,
    FET,
    AFE,
    PRE_CHARGE_CFG,
    TERMINATION_CFG,
    CELL_BALANCING_CFG,
    CHARGING_FAULTS,
    DATA,
    CONFIGURATION,
    MANUFACTURER_INFO,
    REGISTERS,
    POWER,
    FG_CONFIG,
    CURRENT_THRESHOLDS,
    STATE,
    DEVICE_STATUS_DATA,

};

enum DATA_TYPE
{
    I1,
    I2,
    U1,
    U2,
    H1,
    H2
};

struct FUNCTION
{
    QVariant    id;         //Номер блока параметров
    QString     type;       //"Напряжение", "Температура",...
    QVariant    offset;     //Сдвиг параметра, относительно начала блока
    QString     name;       //Имя параметра
    DATA_TYPE   data_type;  //Тип данных - uint8_t, int16_t...
//    qint64    min_val;    //Минимальное значение. Переменная может быть как числом так и строкой
//    qint64    max_val;    //Максимальное значение. Переменная может быть как числом так и строкой
//    qint64    def_val;    //Значение по умолчанию (из даташита). Переменная может быть как числом так и строкой
    QVariant    min_val;    //Минимальное значение. Переменная может быть как числом так и строкой
    QVariant    max_val;    //Максимальное значение. Переменная может быть как числом так и строкой
    QVariant    def_val;    //Значение по умолчанию (из даташита). Переменная может быть как числом так и строкой
    QString     unit;       //Единица измерения
}; //Размер QVariant в некоторых членах, чтобы QT не ругался на выравнивание структуры

const FUNCTION list_First_Level_Protection [38] =
{
    {SECTOR_0,  "Voltage",     0,  "LT COV Threshold",        I2,	3700,   5000,   4390,   "mV"    },
    {SECTOR_0,  "Voltage",     2,  "LT COV Recovery",         I2,	0,      4400,   3900,   "mV"    },
    {SECTOR_0,  "Voltage",     4,  "ST COV Threshold",        I2,	3700,   5000,   4500,   "mV"    },
    {SECTOR_0,  "Voltage",     6,  "STL COV Recovery",        I2,	0,      4400,   4100,   "mV"    },
    {SECTOR_0,  "Voltage",     8,  "HT COV Threshold",        I2,	3700,   5000,   4400,   "mV"    },
    {SECTOR_0,  "Voltage",     10, "HT COV Recovery",         I2,	0,      4400,   4000,   "mV"    },
    {SECTOR_0,  "Voltage",     12, "COV Time",                U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  "Voltage",     13, "CUV Threshold",           I2,	0,      3500,   2200,   "mV"    },
    {SECTOR_0,  "Voltage",     15, "CUV Time",                U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  "Voltage",     16, "CUV Recovery",            I2,	0,      3600,   3000,   "mV"    },
    {SECTOR_0,  "Current",     18, "OC1 Chg",                 I2,	0,      20000,  6000,   "mA"    },
    {SECTOR_0,  "Current",     20, "OC1 Chg Time",            U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  "Current",     21, "OC1 Chg Recovery",        I2,	0,      500,    200,    "mA"    },
    {SECTOR_0,  "Current",     23, "OC1 Dsg",                 I2,	0,      20000,  6000,   "mA"    },
    {SECTOR_0,  "Current",     25, "OC1 Chg Time",            U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  "Current",     26, "OC1 Chg Recovery",        I2,	0,      500,    200,    "mA"    },
    {SECTOR_0,  "Current",     28, "OC2 Chg",                 I2,	0,      20000,  6000,   "mA"    },
    {SECTOR_0,  "Current",     30, "OC2 Chg Time",            U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  "Current",     31, "OC2 Dsg",                 I2,	0,      20000,  6000,   "mA"    },
    {SECTOR_0,  "Current",     33, "OC2 Chg Time",            U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  "Current",     34, "Current Recovery Time",   U1,	0,      240,    8,      "sec"   },
    {SECTOR_0,  "Current",     35, "AFE OC Dsg Vol",          H1,	0x0,    0xff,   0x12,   "-"     },
    {SECTOR_0,  "Current",     36, "AFE OC Dsg Time",         H1,	0x0,    0xff,   0xf,    "-"     },

    {SECTOR_0,  "Current",     39, "AFE SC Chg Cfg",          H1,	0x0,    0xff,   0x77,   "-"     },
    {SECTOR_0,  "Current",     40, "AFE SC Dsg Cfg",          H1,	0x0,    0xff,   0x77,   "-"     },
    {SECTOR_0,  "Temperature", 41, "OT1 Chg Threshold",       I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  "Temperature", 43, "OT1 Chg Time",            U1,	0,      240,    2,      "sec"   },
    {SECTOR_0,  "Temperature", 44, "OT1 Chg Recovery",        I2,	0,      2550,   500,    "0.1°C" },
    {SECTOR_0,  "Temperature", 46, "OT2 Chg Threshold",       I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  "Temperature", 48, "OT2 Chg Time",            U1,	0,      240,    2,      "sec"   },
    {SECTOR_0,  "Temperature", 49, "OT2 Chg Recovery",        I2,	0,      2550,   500,    "0.1°C" },
    {SECTOR_0,  "Temperature", 51, "OT1 Dsg Threshold",       I2,	0,      2550,   600,    "0.1°C" },
    {SECTOR_0,  "Temperature", 53, "OT1 Dsg Time",            U1,	0,      240,    2,      "sec"   },
    {SECTOR_0,  "Temperature", 54, "OT1 Dsg Recovery",        I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  "Temperature", 56, "OT2 Dsg Threshold",       I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  "Temperature", 58, "OT2 Dsg Time",            U1,	0,      240,    2,      "sec"   },
    {SECTOR_0,  "Temperature", 59, "OT2 Dsg Recovery",        I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  "Temperature", 61, "Hi Dsg Start Temp",       I2,	0,      1200,   600,    "0.1°C" },
};

const FUNCTION list_Second_Level_Protection[31] =
{
    {SECTOR_0,	"Voltage",        63,     "LT POV Threshold",           I2,	0,      20000,	4400,	"mV"    },
    {SECTOR_0,	"Voltage",        65,     "ST POV Threshold",           I2,	0,      20000,	4600,	"mV"    },
    {SECTOR_0,	"Voltage",        67,     "HT POV Threshold",           I2,	0,      20000,	4500,	"mV"    },
    {SECTOR_0,	"Voltage",        69,     "POV Time",                   U1,	0,      240,	0,      "s"     },
    {SECTOR_0,	"Voltage",        70,     "PF POV Fuse Blow Delay",     U2,	0,      65535,	0,      "s"     },
    {SECTOR_0,	"Voltage",        72,     "PUV Threshold",              I1,	0,      20000,	2000,	"mV"    },
    {SECTOR_0,	"Voltage",        74,     "PUV Time",                   U1,	0,      240,	0,      "s"     },
    {SECTOR_0,	"Voltage",        75,     "Rest CIM Current",           U1,	0,      200,	5,      "mA"    },
    {SECTOR_0,	"Voltage",        76,     "Rest CIM Fail Voltage",      I2,	0,      5000,	1000,	"mV"    },
    {SECTOR_0,	"Voltage",        78,     "Rest CIM Time",              U1,	0,      240,	0,      "s"     },
    {SECTOR_0,	"Voltage",        79,     "CIM Battery Rest Time",      U2,	0,      65535,	1800,	"s"     },
    {SECTOR_0,	"Voltage",        81,     "Rest CIM Check Voltage",     U2,	0,      65535,	3000,	"mV"    },
    {SECTOR_0,	"Voltage",        83,     "Charge CIM Fail Voltage",	I2,	0,  	5000,	1000,	"mV"    },
    {SECTOR_0,	"Voltage",        85,     "Charge CIM Time",            U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	"Voltage",        86,     "Charge CIM Check Voltage",   I2,	0,  	32768,	3000,	"mV"    },
    {SECTOR_0,	"Voltage",        88,     "PFIN Detect Time",           U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	"Voltage",        89,     "PF Min Fuse Blow Voltage",	I2,	0,  	20000,	8000,	"mV"    },
    {SECTOR_0,	"Current",        91,     "POC Chg",                    I2,	0,  	30000,	10000,	"mA"    },
    {SECTOR_0,	"Current",        93,     "POC Chg Time",               U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	"Current",        94,     "POC Dsg",                    I2,	0,  	30000,	10000,	"mA"    },
    {SECTOR_0,	"Current",        96,     "POC Dsg Time",               U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	"Temperature",    97,     "POT1 Chg Threshold",         I2,	0,  	2550,	550,	"0.1°C" },
    {SECTOR_0,	"Temperature",    99,     "POT1 Chg Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	"Temperature",    100,    "POT2 Chg Threshold",         I2,	0,  	2550,	550,	"0.1°C" },
    {SECTOR_0,	"Temperature",    102,    "POT2 Chg Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	"Temperature",    103,    "POT1 Dsg Threshold",         I2,	0,  	2550,	500,	"0.1°C" },
    {SECTOR_0,	"Temperature",    105,    "POT1 Dsg Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	"Temperature",    106,    "POT2 Dsg Threshold",         I2,	0,  	2550,	550,    "0.1°C" },
    {SECTOR_0,	"Temperature",    108,    "POT2 Dsg Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	"Verification",   111,    "FET Fail Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	"Verification",   113,    "Data Flash Fail Limit",      U1,	0,  	240,	5,      "-"     },
};

const FUNCTION list_Charge_Control[38] =
{
    {SECTOR_0,	"Charge temp Cfg",	115,	"JT1",                          I2,	-400,	1200,	0,      "0.1°C" },
    {SECTOR_0,	"Charge temp Cfg",	117,	"JT2",                          I2,	-400,	1200,	120,	"0.1°C" },
    {SECTOR_0,	"Charge temp Cfg",	119,	"JT2a",                         I2,	-400,	1200,	300,	"0.1°C" },
    {SECTOR_0,	"Charge temp Cfg",	121,	"JT3",                          I2,	-400,	1200,	450,	"0.1°C" },
    {SECTOR_0,	"Charge temp Cfg",	123,	"JT4",                          I2,	-400,	1200,	550,	"0.1°C" },
    {SECTOR_0,	"Charge temp Cfg",	125,	"Temp Hys",                     I2,	0,  	100,	10, 	"0.1°C" },
    {SECTOR_0,	"Charge temp Cfg",	127,	"Pre-chg Voltage Threshold",	I2,	0,  	20000,	3000,	"mV"    },
    {SECTOR_0,	"Charge temp Cfg",	129,	"Pre-chg Recovery Voltage",     I2,	0,  	20000,	3100,	"mV"    },
    {SECTOR_0,	"Charge temp Cfg",	131,	"Pre-chg Current",              I2,	0,  	2000,	250,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	133,	"LT Chg Voltage",               I2,	0,  	20000,	12000,	"mV"    },
    {SECTOR_0,	"Charge Cfg",     	135,	"LT Chg Current1",              I2,	0,  	20000,	2000,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	137,	"LT Chg Current2",              I2,	0,  	20000,	2000,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	139,	"LT Chg Current3",              I2,	0,  	20000,	2000,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	141,	"ST1 Chg Voltage",              I2,	0,  	20000,	16800,	"mV"    },
    {SECTOR_0,	"Charge Cfg",     	143,	"ST1 Chg Current1",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	145,	"ST1 Chg Current2",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	147,	"ST1 Chg Current3",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	149,	"ST2 Chg Voltage",              I2,	0,      20000,	16800,	"mV"    },
    {SECTOR_0,	"Charge Cfg",     	151,	"ST2 Chg Current1",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	153,	"ST2 Chg Current2",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	155,	"ST2 Chg Current3",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	157,	"HT Chg Voltage",               I2,	0,      20000,	16760,	"mV"    },
    {SECTOR_0,	"Charge Cfg",     	159,	"HT Chg Current1",              I2,	0,      20000,	3800,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	161,	"HT Chg Current2",              I2,	0,      20000,	3800,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	163,	"HT Chg Current3",              I2,	0,      20000,	3800,	"mA"    },
    {SECTOR_0,	"Charge Cfg",     	165,	"Cell Voltage Threshold1",      I2,	0,      5000,	3900,	"mV"    },
    {SECTOR_0,	"Charge Cfg",     	167,	"Cell Voltage Threshold2",      I2,	0,      5000,	4000,	"mV"    },
    {SECTOR_0,	"Charge Cfg",     	169,	"Cell Voltage Thresh Hys",      I2,	0,      1000,	10,     "mV"    },
    {SECTOR_0,	"Termination Cfg",	173,	"Taper Current",                I2,	0,      1000,	250,	"mA"    },
    {SECTOR_0,	"Termination Cfg",	175,	"Taper Voltage",                I2,	0,      1000,	75,     "mV"    },
    {SECTOR_0,	"Termination Cfg",	179,	"TCA Clear %",                  I1,	-1,     100,	95,     "%"     },
    {SECTOR_0,	"Termination Cfg",	181,	"FC Clear %",                   I1,	-1,     100,	98,     "%"     },
    {SECTOR_0,	"CB Cfg",           184,	"CB Threshold",                 U2,	0,      5000,	3900,	"mV"    },
    {SECTOR_0,	"CB Cfg",           186,	"CB ON Voltage",                U2,	0,      5000,	100,	"mV"    },
    {SECTOR_0,	"CB Cfg",           188,	"CB OFF Voltage",               U2,	0,      5000,	0,      "mV"    },
    {SECTOR_0,	"CB Cfg",           189,	"CB CHK Interval",              U1,	0,      255,	0,      "s"     },
    {SECTOR_0,	"Charging Faults",	191,	"Over Charge Capacity",         I2,	0,      4000,	300,	"mAh"   },
    {SECTOR_0,	"Charging Faults",	199,	"Charge Fault Cfg",             H1,	0x0,    0xffff,	0x0,	"-"     },
};

const FUNCTION list_System_Setting[14] =
{
    {SECTOR_0,	"Registers",	200,	"OP Cfg A",                 H2,	0x0,	0xffff,	0x0329,	"-"   },
    {SECTOR_0,	"Registers",	202,	"OP Cfg B",                 H2,	0x0,	0xffff,	0x2440,	"-"   },
    {SECTOR_0,	"Registers",	204,	"OP Cfg C",                 H2,	0x0,	0xffff,	0x0130,	"-"   },
    {SECTOR_0,	"Registers",	206,	"PF Cfg",                   H2,	0x0,	0xffff,	0x0,	"-"   },
    {SECTOR_0,	"Registers",	208,	"PF Cfg 2",                 H2,	0x0,	0xffff,	0x0,	"-"   },
    {SECTOR_0,	"Registers",	210,	"NR Cfg",                   H2,	0x0,	0xffff,	0x0,	"-"   },
    {SECTOR_0,	"Hardware",     212,	"HW cfg",                   H1,	0x0,	0xff,	0x0,	"-"   },
    {SECTOR_0,	"Power",        213,	"Flash Update OK Voltage",	I2,	6000,	20000,	6000,	"mV"  },
    {SECTOR_0,	"Power",        215,	"Shutdown Voltage",         I2,	5000,	20000,	5000,	"mV"  },
    {SECTOR_0,	"Power",        218,	"Cell Shutdown Voltage",	I2,	0,      5000,	1750,	"mV"  },
    {SECTOR_0,	"Power",        221,	"Charger Present",          I2,	0,      23000,	3000,	"mV"  },
    {SECTOR_0,	"Power",        223,	"Sleep Current",            I2,	0,      100,	10,     "mA"  },
    {SECTOR_0,	"Power",        232,	"Wake Current Reg",         H1,	0x0,	0xff,	0x0,	"-"   },
    {SECTOR_0,	"Power",        233,	"Sealed Ship Delay",        U1,	0,      255,	10, 	"s"   },
};


    bool readFlash(CP2112 *dev, SMBus *batt, QByteArray *flashArray);
    bool readSector(CP2112 *dev, SMBus *batt, quint8 sectorID, QByteArray *sectorArray);

    bool writeFlash(CP2112 *dev, SMBus *batt, QByteArray *flashArray, bool pec);
    bool writeSector(CP2112 *dev, SMBus *batt, qint8 sector, QByteArray *sectorArray, bool pec);


#endif // A2168_INTERFACE_H