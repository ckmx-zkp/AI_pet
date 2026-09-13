#include "hwtest.h"
#include "board_pins.h"

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static bool setup_uart(uart_port_t port, int tx, int rx)
{
    uart_config_t cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    if (uart_driver_install(port, 1024, 1024, 0, NULL, 0) != ESP_OK) {
        uart_driver_delete(port);
        if (uart_driver_install(port, 1024, 1024, 0, NULL, 0) != ESP_OK) {
            return false;
        }
    }
    if (uart_param_config(port, &cfg) != ESP_OK) {
        return false;
    }
    return uart_set_pin(port, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) == ESP_OK;
}

static int dump_rx(uart_port_t port, int ms)
{
    uint8_t buf[128];
    int total = 0;
    int elapsed = 0;
    while (elapsed < ms) {
        int n = uart_read_bytes(port, buf, sizeof(buf) - 1, pdMS_TO_TICKS(100));
        if (n > 0) {
            buf[n] = 0;
            printf("  RX %dB: %s\n", n, (char *)buf);
            total += n;
        }
        elapsed += 100;
    }
    return total;
}

void test_k230_uart(void)
{
    if (!setup_uart(HW_K230_UART_NUM, HW_K230_TX_GPIO, HW_K230_RX_GPIO)) {
        hw_report("K230 UART", false, "UART1 配置失败");
        return;
    }
    const char *msg = "AI_PET_HWTEST_PING\n";
    uart_write_bytes(HW_K230_UART_NUM, msg, strlen(msg));
    printf("已向 K230 发送 PING（GPIO10 TX / GPIO11 RX），等待 2 秒...\n");
    int n = dump_rx(HW_K230_UART_NUM, 2000);
    hw_report("K230 UART1", true, n > 0 ? "对端有回包" : "已发送，无回包（K230 未上电也属正常）");
}

void test_ml307_uart(bool power_on)
{
    if (power_on) {
        printf("警告：4G UART 原理图标 1.8V?，S3 为 3.3V。即将拉高 4G_PWR=GPIO18。\n");
        gpio_set_level(HW_ML307_PWR_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(800));
    } else {
        printf("4G 电源保持关闭，只测 UART2 引脚收发。\n");
    }

    if (!setup_uart(HW_ML307_UART_NUM, HW_ML307_TX_GPIO, HW_ML307_RX_GPIO)) {
        hw_report("ML307 UART", false, "UART2 配置失败");
        return;
    }
    const char *at = "AT\r\n";
    uart_write_bytes(HW_ML307_UART_NUM, at, strlen(at));
    int n = dump_rx(HW_ML307_UART_NUM, 1500);
    hw_report("ML307 UART2", true, n > 0 ? "收到数据" : "无回包（电源关闭或电平未匹配时正常）");
    if (power_on) {
        printf("测试结束，拉低 4G_PWR。\n");
        gpio_set_level(HW_ML307_PWR_GPIO, 0);
    }
}
