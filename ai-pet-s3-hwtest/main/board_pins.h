#pragma once

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"
#include "driver/uart.h"

/*
 * ESP32-S3-WROOM-2-N32R16V USB 摄像头大板
 * 原理图：hardware/SCH_USB摄像头大板_2026-08-25.pdf
 * 网表：  hardware/Netlist_USB摄像头大板_2026-08-25.tel
 * 眼睛：  hardware/XJ0.99TFT-12P/（XJ0.99TFT-12P / GC9107 / 128×115）
 * GPIO 以模组脚为准，不以丝印 IO4_TOUCH4 等名称为准。
 */

#define HW_I2C_PORT              I2C_NUM_0
#define HW_I2C_SDA_GPIO          GPIO_NUM_1
#define HW_I2C_SCL_GPIO          GPIO_NUM_2
#define HW_ES8311_ADDR           0x18
#define HW_ES7210_ADDR           0x41

#define HW_I2S_MCLK_GPIO         GPIO_NUM_38
#define HW_I2S_WS_GPIO           GPIO_NUM_13
#define HW_I2S_BCLK_GPIO         GPIO_NUM_14
#define HW_I2S_DIN_GPIO          GPIO_NUM_12
#define HW_I2S_DOUT_GPIO         GPIO_NUM_45

#define HW_PA_EN_SCH_GPIO        GPIO_NUM_46
#define HW_PA_REWORK_GPIO9       GPIO_NUM_9
#define HW_PA_REWORK_GPIO21      GPIO_NUM_21

#define HW_BOOT_BTN_GPIO         GPIO_NUM_0
#define HW_USER_BTN_GPIO         GPIO_NUM_4

#define HW_TOUCH1_GPIO           GPIO_NUM_5
#define HW_TOUCH2_GPIO           GPIO_NUM_6
#define HW_TOUCH3_GPIO           GPIO_NUM_7

#define HW_EYE_SCLK_GPIO         GPIO_NUM_41
#define HW_EYE_MOSI_GPIO         GPIO_NUM_40
#define HW_EYE_DC_GPIO           GPIO_NUM_39
#define HW_EYE_CS1_GPIO          GPIO_NUM_17
#define HW_EYE_CS2_GPIO          GPIO_NUM_16
#define HW_EYE_BL_GPIO           GPIO_NUM_42
#define HW_EYE_SPI_HOST          SPI2_HOST
/* XJ0.99TFT-12P / GC9107：128×115，不是 GC9A01 240×240 */
#define HW_EYE_W                 128
#define HW_EYE_H                 115

#define HW_WS2812_DIN_GPIO       GPIO_NUM_3
#define HW_WS2812_EN_GPIO        GPIO_NUM_15
#define HW_WS2812_COUNT          2

#define HW_SERVO_PWM_GPIO        GPIO_NUM_8

#define HW_K230_UART_NUM         UART_NUM_1
#define HW_K230_TX_GPIO          GPIO_NUM_10
#define HW_K230_RX_GPIO          GPIO_NUM_11

#define HW_ML307_UART_NUM        UART_NUM_2
#define HW_ML307_TX_GPIO         GPIO_NUM_48
#define HW_ML307_RX_GPIO         GPIO_NUM_47
#define HW_ML307_PWR_GPIO        GPIO_NUM_18

/* U4（A1252WV-04P）：S3 原生 USB Host，接 USB 摄像头。勿当普通 GPIO。 */
#define HW_USB_DM_GPIO           GPIO_NUM_19
#define HW_USB_DP_GPIO           GPIO_NUM_20
