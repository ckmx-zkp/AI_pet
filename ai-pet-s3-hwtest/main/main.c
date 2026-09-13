#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "hwtest.h"
#include "board_pins.h"

static const char *TAG = "hwtest";

static void banner(void)
{
    printf("\n");
    printf("============================================================\n");
    printf(" AI Pet S3 USB 摄像头大板 — 硬件测试台\n");
    printf(" 模组 ESP32-S3-WROOM-2-N32R16V\n");
    printf(" 原理图 hardware/SCH_USB摄像头大板_2026-08-25.pdf\n");
    printf("============================================================\n");
    printf(" 命令:\n");
    printf("   help   本说明\n");
    printf("   all    安全套件（不开 4G 电源，不含按键等待）\n");
    printf("   i2c    扫描 ES8311/ES7210\n");
    printf("   audio  读 codec + I2S 正弦（无功放改版可能无声）\n");
    printf("   eye    左红右蓝（屏为 GC9107 128x115；当前仍走 GC9A01 初始化，未点亮）\n");
    printf("   led    两颗 WS2812\n");
    printf("   btn    3 秒检测 BOOT / KEY1\n");
    printf("   touch  读 CN1 三路触摸\n");
    printf("   servo  CN2 舵机摆动\n");
    printf("   k230   UART1 发 PING\n");
    printf("   4g     仅 UART2，不供电\n");
    printf("   4gon   拉高 4G_PWR 后发 AT（电平未确认，慎用）\n");
    printf("   wifi   扫描周围 AP\n");
    printf("   pa     打印功放门禁\n");
    printf("   cam    U4 USB Host：枚举 + UVC 取 3 帧\n");
    printf(" USB 摄像头插 U4（GPIO19/20）。勿把 19/20 当普通 GPIO。\n");
    printf("============================================================\n");
}

static void handle_line(char *line)
{
    char *nl = strpbrk(line, "\r\n");
    if (nl) {
        *nl = 0;
    }
    if (line[0] == 0) {
        return;
    }
    if (!strcmp(line, "help") || !strcmp(line, "?")) {
        banner();
    } else if (!strcmp(line, "all")) {
        test_all_safe();
    } else if (!strcmp(line, "i2c")) {
        test_i2c();
    } else if (!strcmp(line, "audio")) {
        test_audio();
    } else if (!strcmp(line, "eye")) {
        test_display();
    } else if (!strcmp(line, "led")) {
        test_led();
    } else if (!strcmp(line, "btn")) {
        test_buttons();
    } else if (!strcmp(line, "touch")) {
        test_touch();
    } else if (!strcmp(line, "servo")) {
        test_servo();
    } else if (!strcmp(line, "k230")) {
        test_k230_uart();
    } else if (!strcmp(line, "4g")) {
        test_ml307_uart(false);
    } else if (!strcmp(line, "4gon")) {
        test_ml307_uart(true);
    } else if (!strcmp(line, "wifi")) {
        test_wifi_scan();
    } else if (!strcmp(line, "pa")) {
        test_pa_pin();
    } else if (!strcmp(line, "cam")) {
        test_usb_cam();
    } else {
        printf("未知命令: %s  （输入 help）\n", line);
    }
}

static void console_loop(void)
{
    uint8_t buf[64];
    size_t n = 0;
    printf("hwtest> ");
    fflush(stdout);
    while (1) {
        uint8_t ch;
        int r = uart_read_bytes(UART_NUM_0, &ch, 1, pdMS_TO_TICKS(50));
        if (r <= 0) {
            continue;
        }
        if (ch == '\r' || ch == '\n') {
            uart_write_bytes(UART_NUM_0, "\r\n", 2);
            buf[n] = 0;
            handle_line((char *)buf);
            n = 0;
            printf("hwtest> ");
            fflush(stdout);
            continue;
        }
        if (ch == 0x08 || ch == 0x7F) {
            if (n > 0) {
                n--;
                uart_write_bytes(UART_NUM_0, "\b \b", 3);
            }
            continue;
        }
        if (n + 1 < sizeof(buf) && ch >= 32) {
            buf[n++] = ch;
            uart_write_bytes(UART_NUM_0, &ch, 1);
        }
    }
}

void app_main(void)
{
    hw_init_safe_rails();
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    uart_vfs_dev_use_driver(UART_NUM_0);
    ESP_LOGI(TAG, "reset reason=%d  free=%lu", (int)esp_reset_reason(),
             (unsigned long)esp_get_free_heap_size());
    banner();
    console_loop();
}
