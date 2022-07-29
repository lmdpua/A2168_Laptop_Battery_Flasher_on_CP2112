#pragma once
#ifndef A2168_INTERFACE_H
#define A2168_INTERFACE_H

#include "types.h"
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
    SECTOR_1,
    SECTOR_2,
    SECTOR_3
};

enum TYPE
{
    VOLTAGE,
    CURRENT,
    TEMPERATURE,
    VERIFICATION,
    CHARGE_TEMP_CFG,
    CHARGE_CFG,
    TERMINATION_CFG,
    CB_CFG,
    CHARGING_FAULTS,
    REGISTERS,
    HARDWARE,
    POWER,
    DATA,
    CONFIGURATION,
    MANUFACTURER_INFO,
    LIFETIME_DATA,
    LIFETIME_TEMP_SAMPLES,
    PF_RECORD,
    GG_CFG,
    CURRENT_THRESHOLDS,
    TEMP_MODEL,
    LED_CFG,
};

enum DATA_TYPE
{
    I1,
    I2,
    I4,
    U1,
    U2,
    H1,
    H2,
    S5,
    S20,
    S21,
    S32
};

struct FUNCTION
{
    SECTOR_ID   id;         //Номер блока параметров
    TYPE        type;       //"Напряжение", "Температура",...
    QVariant    offset;     //Сдвиг параметра, относительно начала блока
    QString     name;       //Имя параметра
    DATA_TYPE   data_type;  //Тип данных - uint8_t, int16_t...
    QVariant    min_val;    //Минимальное значение. Переменная может быть как числом так и строкой
    QVariant    max_val;    //Максимальное значение. Переменная может быть как числом так и строкой
    QVariant    def_val;    //Значение по умолчанию (из даташита). Переменная может быть как числом так и строкой
    QString     unit;       //Единица измерения
}; //Тип переменной QVariant в некоторых членах, чтобы QT не ругался на выравнивание структуры

const FUNCTION list_First_Level_Protection [38] =
{
    {SECTOR_0,  VOLTAGE,     0,  "LT COV Threshold",        I2,	3700,   5000,   4390,   "mV"    },
    {SECTOR_0,  VOLTAGE,     2,  "LT COV Recovery",         I2,	0,      4400,   3900,   "mV"    },
    {SECTOR_0,  VOLTAGE,     4,  "ST COV Threshold",        I2,	3700,   5000,   4500,   "mV"    },
    {SECTOR_0,  VOLTAGE,     6,  "STL COV Recovery",        I2,	0,      4400,   4100,   "mV"    },
    {SECTOR_0,  VOLTAGE,     8,  "HT COV Threshold",        I2,	3700,   5000,   4400,   "mV"    },
    {SECTOR_0,  VOLTAGE,     10, "HT COV Recovery",         I2,	0,      4400,   4000,   "mV"    },
    {SECTOR_0,  VOLTAGE,     12, "COV Time",                U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  VOLTAGE,     13, "CUV Threshold",           I2,	0,      3500,   2200,   "mV"    },
    {SECTOR_0,  VOLTAGE,     15, "CUV Time",                U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  VOLTAGE,     16, "CUV Recovery",            I2,	0,      3600,   3000,   "mV"    },
    {SECTOR_0,  CURRENT,     18, "OC1 Chg",                 I2,	0,      20000,  6000,   "mA"    },
    {SECTOR_0,  CURRENT,     20, "OC1 Chg Time",            U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  CURRENT,     21, "OC1 Chg Recovery",        I2,	0,      500,    200,    "mA"    },
    {SECTOR_0,  CURRENT,     23, "OC1 Dsg",                 I2,	0,      20000,  6000,   "mA"    },
    {SECTOR_0,  CURRENT,     25, "OC1 Chg Time",            U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  CURRENT,     26, "OC1 Chg Recovery",        I2,	0,      500,    200,    "mA"    },
    {SECTOR_0,  CURRENT,     28, "OC2 Chg",                 I2,	0,      20000,  6000,   "mA"    },
    {SECTOR_0,  CURRENT,     30, "OC2 Chg Time",            U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  CURRENT,     31, "OC2 Dsg",                 I2,	0,      20000,  6000,   "mA"    },
    {SECTOR_0,  CURRENT,     33, "OC2 Chg Time",            U1,	0,      255,    1,      "sec"   },
    {SECTOR_0,  CURRENT,     34, "Current Recovery Time",   U1,	0,      240,    8,      "sec"   },
    {SECTOR_0,  CURRENT,     35, "AFE OC Dsg Vol",          H1,	0x0,    0xff,   0x12,   "-"     },
    {SECTOR_0,  CURRENT,     36, "AFE OC Dsg Time",         H1,	0x0,    0xff,   0xf,    "-"     },

    {SECTOR_0,  CURRENT,     39, "AFE SC Chg Cfg",          H1,	0x0,    0xff,   0x77,   "-"     },
    {SECTOR_0,  CURRENT,     40, "AFE SC Dsg Cfg",          H1,	0x0,    0xff,   0x77,   "-"     },
    {SECTOR_0,  TEMPERATURE, 41, "OT1 Chg Threshold",       I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  TEMPERATURE, 43, "OT1 Chg Time",            U1,	0,      240,    2,      "sec"   },
    {SECTOR_0,  TEMPERATURE, 44, "OT1 Chg Recovery",        I2,	0,      2550,   500,    "0.1°C" },
    {SECTOR_0,  TEMPERATURE, 46, "OT2 Chg Threshold",       I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  TEMPERATURE, 48, "OT2 Chg Time",            U1,	0,      240,    2,      "sec"   },
    {SECTOR_0,  TEMPERATURE, 49, "OT2 Chg Recovery",        I2,	0,      2550,   500,    "0.1°C" },
    {SECTOR_0,  TEMPERATURE, 51, "OT1 Dsg Threshold",       I2,	0,      2550,   600,    "0.1°C" },
    {SECTOR_0,  TEMPERATURE, 53, "OT1 Dsg Time",            U1,	0,      240,    2,      "sec"   },
    {SECTOR_0,  TEMPERATURE, 54, "OT1 Dsg Recovery",        I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  TEMPERATURE, 56, "OT2 Dsg Threshold",       I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  TEMPERATURE, 58, "OT2 Dsg Time",            U1,	0,      240,    2,      "sec"   },
    {SECTOR_0,  TEMPERATURE, 59, "OT2 Dsg Recovery",        I2,	0,      2550,   550,    "0.1°C" },
    {SECTOR_0,  TEMPERATURE, 61, "Hi Dsg Start Temp",       I2,	0,      1200,   600,    "0.1°C" },
};

const FUNCTION list_Second_Level_Protection[31] =
{
    {SECTOR_0,	VOLTAGE,        63,     "LT POV Threshold",           I2,	0,      20000,	4400,	"mV"    },
    {SECTOR_0,	VOLTAGE,        65,     "ST POV Threshold",           I2,	0,      20000,	4600,	"mV"    },
    {SECTOR_0,	VOLTAGE,        67,     "HT POV Threshold",           I2,	0,      20000,	4500,	"mV"    },
    {SECTOR_0,	VOLTAGE,        69,     "POV Time",                   U1,	0,      240,	0,      "s"     },
    {SECTOR_0,	VOLTAGE,        70,     "PF POV Fuse Blow Delay",     U2,	0,      65535,	0,      "s"     },
    {SECTOR_0,	VOLTAGE,        72,     "PUV Threshold",              I1,	0,      20000,	2000,	"mV"    },
    {SECTOR_0,	VOLTAGE,        74,     "PUV Time",                   U1,	0,      240,	0,      "s"     },
    {SECTOR_0,	VOLTAGE,        75,     "Rest CIM Current",           U1,	0,      200,	5,      "mA"    },
    {SECTOR_0,	VOLTAGE,        76,     "Rest CIM Fail Voltage",      I2,	0,      5000,	1000,	"mV"    },
    {SECTOR_0,	VOLTAGE,        78,     "Rest CIM Time",              U1,	0,      240,	0,      "s"     },
    {SECTOR_0,	VOLTAGE,        79,     "CIM Battery Rest Time",      U2,	0,      65535,	1800,	"s"     },
    {SECTOR_0,	VOLTAGE,        81,     "Rest CIM Check Voltage",     U2,	0,      65535,	3000,	"mV"    },
    {SECTOR_0,	VOLTAGE,        83,     "Charge CIM Fail Voltage",    I2,	0,  	5000,	1000,	"mV"    },
    {SECTOR_0,	VOLTAGE,        85,     "Charge CIM Time",            U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	VOLTAGE,        86,     "Charge CIM Check Voltage",   I2,	0,  	32768,	3000,	"mV"    },
    {SECTOR_0,	VOLTAGE,        88,     "PFIN Detect Time",           U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	VOLTAGE,        89,     "PF Min Fuse Blow Voltage",   I2,	0,  	20000,	8000,	"mV"    },
    {SECTOR_0,	CURRENT,        91,     "POC Chg",                    I2,	0,  	30000,	10000,	"mA"    },
    {SECTOR_0,	CURRENT,        93,     "POC Chg Time",               U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	CURRENT,        94,     "POC Dsg",                    I2,	0,  	30000,	10000,	"mA"    },
    {SECTOR_0,	CURRENT,        96,     "POC Dsg Time",               U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	TEMPERATURE,    97,     "POT1 Chg Threshold",         I2,	0,  	2550,	550,	"0.1°C" },
    {SECTOR_0,	TEMPERATURE,    99,     "POT1 Chg Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	TEMPERATURE,    100,    "POT2 Chg Threshold",         I2,	0,  	2550,	550,	"0.1°C" },
    {SECTOR_0,	TEMPERATURE,    102,    "POT2 Chg Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	TEMPERATURE,    103,    "POT1 Dsg Threshold",         I2,	0,  	2550,	500,	"0.1°C" },
    {SECTOR_0,	TEMPERATURE,    105,    "POT1 Dsg Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	TEMPERATURE,    106,    "POT2 Dsg Threshold",         I2,	0,  	2550,	550,    "0.1°C" },
    {SECTOR_0,	TEMPERATURE,    108,    "POT2 Dsg Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	VERIFICATION,   111,    "FET Fail Time",              U1,	0,  	240,	0,      "s"     },
    {SECTOR_0,	VERIFICATION,   113,    "Data Flash Fail Limit",      U1,	0,  	240,	5,      "-"     },
};

const FUNCTION list_Charge_Control[38] =
{
    {SECTOR_0,	CHARGE_TEMP_CFG,	115,	"JT1",                          I2,	-400,	1200,	0,      "0.1°C" },
    {SECTOR_0,	CHARGE_TEMP_CFG,	117,	"JT2",                          I2,	-400,	1200,	120,	"0.1°C" },
    {SECTOR_0,	CHARGE_TEMP_CFG,	119,	"JT2a",                         I2,	-400,	1200,	300,	"0.1°C" },
    {SECTOR_0,	CHARGE_TEMP_CFG,	121,	"JT3",                          I2,	-400,	1200,	450,	"0.1°C" },
    {SECTOR_0,	CHARGE_TEMP_CFG,	123,	"JT4",                          I2,	-400,	1200,	550,	"0.1°C" },
    {SECTOR_0,	CHARGE_TEMP_CFG,	125,	"Temp Hys",                     I2,	0,  	100,	10, 	"0.1°C" },
    {SECTOR_0,	CHARGE_TEMP_CFG,	127,	"Pre-chg Voltage Threshold",	I2,	0,  	20000,	3000,	"mV"    },
    {SECTOR_0,	CHARGE_TEMP_CFG,	129,	"Pre-chg Recovery Voltage",     I2,	0,  	20000,	3100,	"mV"    },
    {SECTOR_0,	CHARGE_TEMP_CFG,	131,	"Pre-chg Current",              I2,	0,  	2000,	250,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	133,	"LT Chg Voltage",               I2,	0,  	20000,	12000,	"mV"    },
    {SECTOR_0,	CHARGE_CFG,     	135,	"LT Chg Current1",              I2,	0,  	20000,	2000,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	137,	"LT Chg Current2",              I2,	0,  	20000,	2000,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	139,	"LT Chg Current3",              I2,	0,  	20000,	2000,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	141,	"ST1 Chg Voltage",              I2,	0,  	20000,	16800,	"mV"    },
    {SECTOR_0,	CHARGE_CFG,     	143,	"ST1 Chg Current1",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	145,	"ST1 Chg Current2",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	147,	"ST1 Chg Current3",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	149,	"ST2 Chg Voltage",              I2,	0,      20000,	16800,	"mV"    },
    {SECTOR_0,	CHARGE_CFG,     	151,	"ST2 Chg Current1",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	153,	"ST2 Chg Current2",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	155,	"ST2 Chg Current3",             I2,	0,      20000,	4000,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	157,	"HT Chg Voltage",               I2,	0,      20000,	16760,	"mV"    },
    {SECTOR_0,	CHARGE_CFG,     	159,	"HT Chg Current1",              I2,	0,      20000,	3800,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	161,	"HT Chg Current2",              I2,	0,      20000,	3800,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	163,	"HT Chg Current3",              I2,	0,      20000,	3800,	"mA"    },
    {SECTOR_0,	CHARGE_CFG,     	165,	"Cell Voltage Threshold1",      I2,	0,      5000,	3900,	"mV"    },
    {SECTOR_0,	CHARGE_CFG,     	167,	"Cell Voltage Threshold2",      I2,	0,      5000,	4000,	"mV"    },
    {SECTOR_0,	CHARGE_CFG,     	169,	"Cell Voltage Thresh Hys",      I2,	0,      1000,	10,     "mV"    },
    {SECTOR_0,	TERMINATION_CFG,	173,	"Taper Current",                I2,	0,      1000,	250,	"mA"    },
    {SECTOR_0,	TERMINATION_CFG,	175,	"Taper Voltage",                I2,	0,      1000,	75,     "mV"    },
    {SECTOR_0,	TERMINATION_CFG,	179,	"TCA Clear %",                  I1,	-1,     100,	95,     "%"     },
    {SECTOR_0,	TERMINATION_CFG,	181,	"FC Clear %",                   I1,	-1,     100,	98,     "%"     },
    {SECTOR_0,	CB_CFG,             184,	"CB Threshold",                 U2,	0,      5000,	3900,	"mV"    },
    {SECTOR_0,	CB_CFG,             186,	"CB ON Voltage",                U2,	0,      5000,	100,	"mV"    },
    {SECTOR_0,	CB_CFG,             188,	"CB OFF Voltage",               U2,	0,      5000,	0,      "mV"    },
    {SECTOR_0,	CB_CFG,             189,	"CB CHK Interval",              U1,	0,      255,	0,      "s"     },
    {SECTOR_0,	CHARGING_FAULTS,	191,	"Over Charge Capacity",         I2,	0,      4000,	300,	"mAh"   },
    {SECTOR_0,	CHARGING_FAULTS,	199,	"Charge Fault Cfg",             H1,	0x0,    0xffff,	0x0,	"-"     },
};

const FUNCTION list_System_Setting[14] =
{
    {SECTOR_0,	REGISTERS,	200,	"OP Cfg A",                 H2,	0x0,	0xffff,	0x0329,	"-"   },
    {SECTOR_0,	REGISTERS,	202,	"OP Cfg B",                 H2,	0x0,	0xffff,	0x2440,	"-"   },
    {SECTOR_0,	REGISTERS,	204,	"OP Cfg C",                 H2,	0x0,	0xffff,	0x0130,	"-"   },
    {SECTOR_0,	REGISTERS,	206,	"PF Cfg",                   H2,	0x0,	0xffff,	0x0,	"-"   },
    {SECTOR_0,	REGISTERS,	208,	"PF Cfg 2",                 H2,	0x0,	0xffff,	0x0,	"-"   },
    {SECTOR_0,	REGISTERS,	210,	"NR Cfg",                   H2,	0x0,	0xffff,	0x0,	"-"   },
    {SECTOR_0,	HARDWARE,   212,	"HW cfg",                   H1,	0x0,	0xff,	0x0,	"-"   },
    {SECTOR_0,	POWER,      213,	"Flash Update OK Voltage",	I2,	6000,	20000,	6000,	"mV"  },
    {SECTOR_0,	POWER,      215,	"Shutdown Voltage",         I2,	5000,	20000,	5000,	"mV"  },
    {SECTOR_0,	POWER,      218,	"Cell Shutdown Voltage",	I2,	0,      5000,	1750,	"mV"  },
    {SECTOR_0,	POWER,      221,	"Charger Present",          I2,	0,      23000,	3000,	"mV"  },
    {SECTOR_0,	POWER,      223,	"Sleep Current",            I2,	0,      100,	10,     "mA"  },
    {SECTOR_0,	POWER,      232,	"Wake Current Reg",         H1,	0x0,	0xff,	0x0,	"-"   },
    {SECTOR_0,	POWER,      233,	"Sealed Ship Delay",        U1,	0,      255,	10, 	"s"   },
};

const FUNCTION list_SBS_Setting []=
{
    {SECTOR_0,	DATA,           254,	"Rem Cap Alarm",                I2,     0,      700,	300,        "mAh"   },
    {SECTOR_1,	DATA,           0,      "Rem Energy Alarm",             I2,     0,      1000,	432,        "mWh"   },
    {SECTOR_1,	DATA,           2,      "Rem Time Alarm",               U2,     0,      30,     10,         "min"   },
    {SECTOR_1,	DATA,           4,      "Init Battery Mode",            H2,     0x0,	0xffff,	0x81,       "-"     },
    {SECTOR_1,	DATA,           6,      "Design Voltage",               I2,     7000,	18000,	14400,      "mV"    },
    {SECTOR_1,	DATA,           8,      "Spec Info",                    H2,     0x0,	0xffff,	0x31,       "-"     },
    {SECTOR_1,	DATA,           10,     "Manuf Date",                   U2,     0,      65535,	0,          "Day + Mo*32 +(Yr-1980)*256 (date)"   },
    {SECTOR_1,	DATA,           12,     "Ser. Num.",                    H2,     0x0000,	0xffff,	0x1,        "-"     },
    {SECTOR_1,	DATA,           16,     "CC Threshold",                 I2,     100,	32767,	4400,       "mAh"   },
    {SECTOR_3,	DATA,           18,     "Cycle Count",                  U2,     0,      65535,	0,          "-"     },
    {SECTOR_1,	DATA,           19,     "CF MaxError Limit",            U1,     0,      100,	100,        "%"     },
    {SECTOR_1,	DATA,           20,     "Design Capacity",              I2,     0,      32767,	4400,       "mAh"   },
    {SECTOR_1,	DATA,           22,     "Design Energy",                I2,     0,      32767,	6336,       "10mWh" },
    {SECTOR_1,	DATA,           24,     "Manuf Name",                   S21,	"",     "",     "",         "-"     },
    {SECTOR_1,	DATA,           45,     "Device Name",                  S21,	"",     "",     "A2168",    "-"     },
    {SECTOR_1,	DATA,           66,     "Device Chemistry",             S5,     "",     "",     "LION",     "-"     },
    {SECTOR_1,	DATA,           71,     "Deterioration Warn Limit",     U1,     0,      100,	50,         "%"     },
    {SECTOR_1,	DATA,           72,     "Deterioration Fault Limit",	U1,     0,      100,	30,         "%"     },
    {SECTOR_1,	DATA,           73,     "Cell Life Limit",              U1,     0,      100,	20,         "%"     },
    {SECTOR_1,	CONFIGURATION,	74,     "TDA Set %",                    I1,     -1,     100,	6,          "%"     },
    {SECTOR_1,	CONFIGURATION,	75,     "TDA Clear %",                  I1,     -1,     100,	8,          "%"     },
    {SECTOR_1,	CONFIGURATION,	76,     "FD Set %",                     I1,     -1,     100,	2,          "%"     },
    {SECTOR_1,	CONFIGURATION,	77,     "FD Clear %",                   I1,     -1,     100,	5,          "%"     },
    {SECTOR_1,	CONFIGURATION,	78,     "TDA Set Volt Threshold",       I2,     0,      16800,	5000,       "mV"    },
    {SECTOR_1,	CONFIGURATION,	80,     "TDA Set Volt Time",            U1,     0,      240,	5,          "s"     },
    {SECTOR_1,	CONFIGURATION,	81,     "TDA Clear Volt",               I2,     0,      16800,	5500,       "mV"    },
};

const FUNCTION list_System_Data[]=
{
    {SECTOR_1,	MANUFACTURER_INFO,      83,     "Manuf. Info 0",            S32,	"", 	"",         "0123456789ABCDEF0123456789ABCDE",	"-"     },
    {SECTOR_1,	MANUFACTURER_INFO,      115,	"Manuf. Info 1",            S20,	"", 	"",         "0123456789ABCDEF012",              "-"     },
    {SECTOR_1,	MANUFACTURER_INFO,      136,	"Manuf. Info 2",            S20,	"", 	"",         "0123456789ABCDEF012",              "-"     },
    {SECTOR_1,	MANUFACTURER_INFO,      157,	"Manuf. Info 3",            S20,	"", 	"",         "0123456789ABCDEF012",              "-"     },
    {SECTOR_1,	MANUFACTURER_INFO,      178,	"Manuf. Info 4",            S20,	"", 	"",         "0123456789ABCDEF012",              "-"     },
    {SECTOR_2,	LIFETIME_DATA,          244,	"Life Max Temp",            I2, 	0,  	1400,       300,                                "0.1°C" },
    {SECTOR_2,	LIFETIME_DATA,          246,	"Life Min Temp",            I2,     -600,	1400,       200,                                "0.1°C" },
    {SECTOR_2,	LIFETIME_DATA,          248,	"Life Max Cell Voltage",	I2,     0,  	32767,      3500,                               "mV"    },
    {SECTOR_2,	LIFETIME_DATA,          250,	"Life Min Cell Voltage",	I2,     0,  	32767,      3200,                               "mV"    },
    {SECTOR_2,	LIFETIME_DATA,          252,	"Life Max Pack Voltage",	I2,     0,  	32767,      14000,                              "mV"    },
    {SECTOR_2,	LIFETIME_DATA,          254,	"Life Min Pack Voltage",    I2,     0,  	32767,      12800,                              "mV"    },
    {SECTOR_3,	LIFETIME_DATA,          0,      "Life Max Chg Current",     I2,     -32767,	32767,      1500,                               "mA"    },
    {SECTOR_3,	LIFETIME_DATA,          2,      "Life Max Dsg Current",     I2,     -32767,	32767,      -3000,                              "mA"    },
    {SECTOR_3,	LIFETIME_DATA,          4,      "Life Max Chg Power",    	I2,     -32767,	32767,      1500,                               "10mW"  },
    {SECTOR_3,	LIFETIME_DATA,          6,      "Life Max Dsg Power",    	I2,     -32767,	32767,      -1500,                              "10mW"  },
    {SECTOR_3,	LIFETIME_DATA,          8,      "Life Max AvgDsg Cur",   	I2,     -32767,	32767,      -1000,                              "mA"    },
    {SECTOR_3,	LIFETIME_DATA,          10,     "Life Max AvgDsg Pow",  	I2,     -32767,	32767,      -1500,                              "10mW"  },
    {SECTOR_3,	LIFETIME_DATA,          12,     "Life Avg Temp",            I2,     0,  	1400,       250,                                "0.1°C" },
    {SECTOR_3,	LIFETIME_TEMP_SAMPLES,	14,     "Avg Temp Samples",         I4,     0,  	1.40E+08,	0,                                  "-"     },
    {SECTOR_3,	LIFETIME_DATA,          26,     "Life Max Temp 2",          I2,     0,  	1400,       300,                                "0.1°C" },
    {SECTOR_3,	LIFETIME_DATA,          28,     "Life Min Temp 2",          I2,     -600,   1400,       200,                                "0.1°C" },
    {SECTOR_3,	LIFETIME_DATA,          30,     "Life Max Cell 2 Voltage",	I2,     0,  	32767,      3500,                               "mV"    },
    {SECTOR_3,	LIFETIME_DATA,          32,     "Life Max Cell 3 Voltage",	I2,     0,  	32767,      3500,                               "mV"    },
    {SECTOR_3,	LIFETIME_DATA,          34,     "Life Max Cell 4 Voltage",	I2,     0,  	32767,      3500,                               "mV"    },
    {SECTOR_3,	LIFETIME_DATA,          36,     "Life Min Cell 2 Voltage",	I2,     0,  	32767,      3200,                               "mV"    },
    {SECTOR_3,	LIFETIME_DATA,          38,     "Life Min Cell 3 Voltage",	I2,     0,  	32767,      3200,                               "mV"    },
    {SECTOR_3,	LIFETIME_DATA,          40,     "Life Min Cell 4 Voltage",	I2,     0,  	32767,      3200,                               "mV"    },
    {SECTOR_2,	PF_RECORD,              236,	"Saved PF Flags 1",         H2,     0x0,    0xffff,     0x0,                                "HEX"   },
    {SECTOR_2,	PF_RECORD,              238,	"Saved PF Flags 2",         H2,     0x0,	0xffff,     0x0,                                "HEX"   },
    {SECTOR_2,	PF_RECORD,              240,    "Saved 1st PF Flags 1",     H2,     0x0,	0xffff,     0x0,                                "HEX"   },
    {SECTOR_2,	PF_RECORD,              242,	"Saved 1st PF Flags 2",     H2,     0x0,	0xffff,     0x0,                                "HEX"   },
};

const FUNCTION list_Gas_Gauge[]=
{
    {SECTOR_2,	GG_CFG,                 60,     "Load Mode",            	U1, 	0,  	255,	0,  	"-"     },
    {SECTOR_2,	GG_CFG,                 61,     "Term Voltage",         	I2, 	-32768,	32767,	12000,	"mV"    },
    {SECTOR_2,	CURRENT_THRESHOLDS,     71,     "Dsg Current Threshold",	I2, 	0,  	2000,	100,	"mA"    },
    {SECTOR_2,	CURRENT_THRESHOLDS,     73,     "Chg Current Threshold",	I2, 	0,  	2000,	50, 	"mA"    },
    {SECTOR_2,	CURRENT_THRESHOLDS,     75,     "Quit Current",         	I2, 	0,  	1000,	10, 	"mA"    },
};

const FUNCTION list_Calibration[]=
{
    {SECTOR_2,	TEMP_MODEL, 	32, 	"Ext Coef 1",	I2, 	-32768, 	32767,  	-28285, 	"-"     },
    {SECTOR_2,	TEMP_MODEL, 	34, 	"Ext Coef 2",	I2, 	-32768, 	32767,  	20848,  	"-"     },
    {SECTOR_2,	TEMP_MODEL, 	36, 	"Ext Coef 3",	I2, 	-32768, 	32767,  	-7537,  	"-"     },
    {SECTOR_2,	TEMP_MODEL, 	38, 	"Ext Coef 4",	I2, 	-32768, 	32767,  	4012,   	"-"     },
    {SECTOR_2,	CURRENT,    	56, 	"Filter",   	U1, 	0,          255,    	239,    	"-"     },
    {SECTOR_2,	CURRENT,    	57, 	"Deadband", 	U1, 	0,          255,    	3,      	"-"     },
};

const FUNCTION list_LED_Support[]=
{
    {SECTOR_0,	LED_CFG,	234,	"LED Flash Rate",	U2, 	0,      65535,	512,	"500μSec"   },
    {SECTOR_0,	LED_CFG,	236,	"LED Blink Rate",	U2, 	0,      65535,	1024,	"500μSec"   },
    {SECTOR_0,	LED_CFG,	238,	"LED Delay",        U2, 	0,      65535,	100,	"500μSec"   },
    {SECTOR_0,	LED_CFG,	240,	"LED Hold Time",	U1, 	0,      255,	4,  	"Sec"       },
    {SECTOR_0,	LED_CFG,	241,	"CHG Flash Alarm",	I1, 	-1,     101,	10, 	"%"         },
    {SECTOR_0,	LED_CFG,	242,	"CHG Thresh 1",     I1, 	-1,     101,	0,  	"%"         },
    {SECTOR_0,	LED_CFG,	243,	"CHG Thresh 2",     I1, 	-1,     101,	20,     "%"         },
    {SECTOR_0,	LED_CFG,	244,	"CHG Thresh 3",     I1, 	-1,     101,	40,     "%"         },
    {SECTOR_0,	LED_CFG,	245,	"CHG Thresh 4",     I1, 	-1,     101,	60,     "%"         },
    {SECTOR_0,	LED_CFG,	246,	"CHG Thresh 5",     I1, 	-1,     101,	80,     "%"         },
    {SECTOR_0,	LED_CFG,	247,	"DSG Flash Alarm",	I1, 	-1,     101,	10,     "%"         },
    {SECTOR_0,	LED_CFG,	248,	"DSG Thresh 1",     I1, 	-1,     101,	0,      "%"         },
    {SECTOR_0,	LED_CFG,	249,	"DSG Thresh 2",     I1, 	-1,     101,	20,     "%"         },
    {SECTOR_0,	LED_CFG,	250,	"DSG Thresh 3",     I1, 	-1,     101,	40,     "%"         },
    {SECTOR_0,	LED_CFG,	251,	"DSG Thresh 4",     I1, 	-1,     101,	60,     "%"         },
    {SECTOR_0,	LED_CFG,	252,	"DSG Thresh 5",     I1, 	-1, 	101,	80,     "%"         },
    {SECTOR_0,	LED_CFG,	253,	"Sink Current",     U1, 	0x00,	0x03,	0x03,	"-"         },
};

/***********************BITS************************/

struct BITS
{
    QString name;
    QString tooltip;
};

//(0x46)
const BITS FETControl [16]=
{
    {"RSVD",    ""},
    {"DSG",     "Discharge FET Control\n0 = Turn OFF DSG FET. DSG FET does not turn off in charge mode to protect the FET body diode\n1 = Turn ON DSG FET"},
    {"CHG",     "Charge FET Control\n0 = Turn OFF CHG FET. CHG FET does not turn off in discharge mode to protect the FET body diode\n1 = Turn ON CHG FET"},
    {"PCHG",    "Pre-charge FET control\n0 = Turn OFF pre-charge FET\n1 = Turn ON pre-charge FET"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
};

//(0x4f)
const BITS StateOfHealth [16]=
{
    {"StOH",    "State of Health %"},
    {"StOH",    "State of Health %"},
    {"StOH",    "State of Health %"},
    {"StOH",    "State of Health %"},
    {"StOH",    "State of Health %"},
    {"StOH",    "State of Health %"},
    {"StOH",    "State of Health %"},
    {"StOH",    "State of Health %"},
    {"DetW",    "Deterioration Warning\n1 = Capacity of the pack fallen below Deterioration Warn Limit threshold"},
    {"DetF",    "Deterioration Fault\n1 = Capacity of the pack fallen below Deterioration Fault Limit Threshold"},
    {"CLL",     "Cell Life Limit\n1 = Capacity of the pack fallen below Cell Life Limit threshold"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""}
};

//0x51
const BITS SafetyAlarm1 [16]=
{
    {"SCD",     "1=Discharge short-circuit condition"},
    {"SCC",     "1=Charge short-circuit condition"},
    {"AOCD",    "1=Abnormal discharge overcurrent condition"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"FBO",     "1=Fuse blowout condition"},
    {"COV",     "1=Cell overvoltage condition"},
    {"CUV",     "1=Cell undervoltage condition"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"OCC",     "1=Charge overcurrent condition"},
    {"OCD",     "1=Discharge overcurrent condition"},
    {"OT1C",    "1=Charge overtemperature on THM1 condition"},
    {"OT1D",    "1=Discharge overtemperature on THM1 condition"}
};

//0x53
const BITS PFAlarm1 [16]=
{
    {"PFIN",    "1=External Input Indication of permanent failure"},
    {"POV",     "1=PF Overvoltage permanent failure"},
    {"POT1C",   "1=Charge PF Overtemperature on T1 permanent failure"},
    {"POT1D",   "1=Discharge PF Overtemperature on T1 permanent failure"},
    {"CIM_R",   "1=Cell-Imbalance (At Rest method) permanent failure"},
    {"CFETF",   "1=Charge-FET-Failure permanent failure"},
    {"DFETF",   "1=Discharge-FET-Failure permanent failure"},
    {"FMF",     "1=Flash memory failure"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"POCC",    "1=Charge PF Overcurrent permanent failure"},
    {"POCD",    "1=Discharge PF Overcurrent permanent failure"},
    {"RSVD",    ""},
    {"PUV",     "1=PF Undervoltage permanent failure"},
    {"RSVD",    ""},
    {"RSVD",    ""}
};

//0x54
const BITS OperationStatus [16]=
{
    {"GGEN",    "1=Gas gauge function is enabled"},
    {"VOK",     ""},
    {"R_DIS",   ""},
    {"DSGIN",   "1=Discharge inhibited due to a high temperature issue"},
    {"XDSGI",   "1=Discharge disabled due to a current issue"},
    {"XDSG",    "1=Discharge fault"},
    {"DSG",     "Replica of the SBS:BatteryStatus(0x16)[DSG] flag"},
    {"WAKE",    "1=A2168 in WAKE mode"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"LDMD",    "1=Load mode, 0 = constant current, 1 = constant power"},
    {"RSVD",    ""},
    {"CSV",     ""},
    {"SS",      "1=Sealed security mode"},
    {"FAS",     "0=Full access security mode"},
    {"PRES",    "1=PRS is low, indicating that the system is present (battery inserted)."}
};

//0x55
const BITS ChargingStatus [16]=
{
    {"RSVD",    ""},
    {"OC",      "1=Overcharge fault"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"CB",      "1=Cell balancing in progress"},
    {"RSVD",    ""},
    {"HTCHG",   "High temperature charging"},
    {"ST2CHG",  "1=Standard temperature charging 2"},
    {"ST1CHG",  "1=Standard temperature charging 1"},
    {"LTCHG",   "1=Low temperature charging"},
    {"MCHG",    "1=Maintenance charging"},
    {"PCHG",    "1=Precharging"},
    {"CHSUSP",  "1=Charging suspended"},
    {"XCHG",    "1=Charging disabled"},
};

//0x69
const BITS SafetyAlarm2 [16]=
{
    {"OT2C",    "1=Charge overtermperature condition on THM2"},
    {"OT2D",    "1=Discharge overtemperature condition on THM2"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
};

//0x6b
const BITS PFAlarm2 [16]=
{
    {"CIM_C",    "1=Cell-Imbalance (Charge method) permanent failure"},
    {"POT2C",    "1=Charge PF Overtemperature in THM2 permanent failure"},
    {"POT2D",    "1=Discharge PF Overtemperature on THM2 permanent failure"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
};

//0x72
const BITS TempRange [16]=
{
    {"TR1",     "1=temperature range 1: Temperature < JT1"},
    {"TR2",     "1=temperature range 2: JT1 ≦ Temperature < JT2"},
    {"TR2A",    "1=temperature range 3: JT2 ≦ Temperature < JT2a"},
    {"TR3",     "1=temperature range 4: JT2a ≦ Temperature < JT3"},
    {"TR4",     "1=temperature range 5: JT3 ≦ Temperature < JT4"},
    {"TR5",     "1=temperature range 6: JT4 ≦ Temperature"},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
    {"RSVD",    ""},
};


    bool readFlash(CP2112 *dev, SMBus *batt, QByteArray *flashArray);
    bool readSector(CP2112 *dev, SMBus *batt, quint8 sectorID, QByteArray *sectorArray);

    bool writeFlash(CP2112 *dev, SMBus *batt, QByteArray *flashArray, bool pec);
    bool writeSector(CP2112 *dev, SMBus *batt, qint8 sector, QByteArray *sectorArray, bool pec);


#endif // A2168_INTERFACE_H
