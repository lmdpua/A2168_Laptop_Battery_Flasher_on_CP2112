#ifndef CP2112_CONFIG_H
#define CP2112_CONFIG_H

#define VID 0x10C4
#define PID 0xEA90

// Режим работы шины SMBUS
#define BITRATE_HZ                  70000
#define ACK_ADDRESS                 0x02
#define AUTO_RESPOND                false
#define WRITE_TIMEOUT_MS            100
#define READ_TIMEOUT_MS             100
#define TRANSFER_RETRIES            5
#define SCL_LOW_TIMEOUT             true
#define RESPONSE_TIMEOUT_MS         100

//GPIO конфиг
#define GPIO_0         HID_SMBUS_DIRECTION_OUTPUT
#define GPIO_1         HID_SMBUS_DIRECTION_OUTPUT
#define GPIO_2         HID_SMBUS_DIRECTION_INPUT
#define GPIO_3         HID_SMBUS_DIRECTION_INPUT
#define GPIO_4         HID_SMBUS_DIRECTION_INPUT
#define GPIO_5         HID_SMBUS_DIRECTION_INPUT
#define GPIO_6         HID_SMBUS_DIRECTION_INPUT
#define GPIO_7         HID_SMBUS_DIRECTION_INPUT

#define GPIO_0_MODE    HID_SMBUS_MODE_PUSH_PULL
#define GPIO_1_MODE    HID_SMBUS_MODE_PUSH_PULL
#define GPIO_2_MODE    HID_SMBUS_MODE_OPEN_DRAIN
#define GPIO_3_MODE    HID_SMBUS_MODE_OPEN_DRAIN
#define GPIO_4_MODE    HID_SMBUS_MODE_OPEN_DRAIN
#define GPIO_5_MODE    HID_SMBUS_MODE_OPEN_DRAIN
#define GPIO_6_MODE    HID_SMBUS_MODE_OPEN_DRAIN
#define GPIO_7_MODE    HID_SMBUS_MODE_OPEN_DRAIN

#define TX             true
#define RX             true
#define RX_TX_LEDS_ON  true

//Конфиг батареи
#define BATTERY_ADDRESS             0x0B
#define MAX_SMBUS_BLOCK_SIZE        32

#endif // CP2112_CONFIG_H

