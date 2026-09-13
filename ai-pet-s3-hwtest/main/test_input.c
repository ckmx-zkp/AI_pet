#include "hwtest.h"
#include "board_pins.h"

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/touch_sensor_legacy.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void test_buttons(void)
{
    printf("3 秒内请按 BOOT(GPIO0) 和 KEY1(GPIO4)...\n");
    bool boot = false;
    bool key = false;
    for (int i = 0; i < 30; i++) {
        if (gpio_get_level(HW_BOOT_BTN_GPIO) == 0) {
            boot = true;
        }
        if (gpio_get_level(HW_USER_BTN_GPIO) == 0) {
            key = true;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    hw_report("BOOT 键 GPIO0", boot, boot ? "已检测到按下" : "未按下（可再跑 btn）");
    hw_report("KEY1 GPIO4", key, key ? "已检测到按下" : "未按下（可再跑 btn）");
}

void test_touch(void)
{
    esp_err_t err = touch_pad_init();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        hw_report("触摸外设", false, esp_err_to_name(err));
        return;
    }
    touch_pad_config(TOUCH_PAD_NUM5);
    touch_pad_config(TOUCH_PAD_NUM6);
    touch_pad_config(TOUCH_PAD_NUM7);

    uint32_t v5 = 0, v6 = 0, v7 = 0;
    touch_pad_read_raw_data(TOUCH_PAD_NUM5, &v5);
    touch_pad_read_raw_data(TOUCH_PAD_NUM6, &v6);
    touch_pad_read_raw_data(TOUCH_PAD_NUM7, &v7);
    printf("触摸原始值 GPIO5=%lu GPIO6=%lu GPIO7=%lu（手指靠近应变化）\n",
           (unsigned long)v5, (unsigned long)v6, (unsigned long)v7);

    bool alive = (v5 + v6 + v7) > 0;
    hw_report("触摸 CN1", alive, alive ? "外设可读，请对照数值变化" : "读数为 0");
}
