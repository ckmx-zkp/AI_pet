#include "hwtest.h"
#include "board_pins.h"

#include "led_strip.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static led_strip_handle_t s_strip;

void test_led(void)
{
    if (!s_strip) {
        led_strip_config_t strip_config = {
            .strip_gpio_num = HW_WS2812_DIN_GPIO,
            .max_leds = HW_WS2812_COUNT,
        };
        led_strip_rmt_config_t rmt_config = {
            .resolution_hz = 10 * 1000 * 1000,
            .flags = {
                .with_dma = false,
            },
        };
        if (led_strip_new_rmt_device(&strip_config, &rmt_config, &s_strip) != ESP_OK) {
            hw_report("WS2812", false, "RMT 设备创建失败");
            return;
        }
    }

    led_strip_set_pixel(s_strip, 0, 32, 0, 0);
    led_strip_set_pixel(s_strip, 1, 0, 32, 0);
    if (led_strip_refresh(s_strip) != ESP_OK) {
        hw_report("WS2812", false, "刷新失败");
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(400));
    led_strip_set_pixel(s_strip, 0, 0, 0, 32);
    led_strip_set_pixel(s_strip, 1, 32, 32, 0);
    led_strip_refresh(s_strip);
    hw_report("WS2812", true, "U18 红/蓝，U19 绿/黄；确认 LED_EN=GPIO15 已拉高");
}
