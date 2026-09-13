#include "hwtest.h"
#include "board_pins.h"

#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static bool s_servo_ready;

static void servo_us(int us)
{
    const uint32_t max_duty = (1 << 14) - 1;
    uint32_t duty = (uint32_t)((uint64_t)us * max_duty / 20000);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void test_servo(void)
{
    if (!s_servo_ready) {
        ledc_timer_config_t timer = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_num = LEDC_TIMER_0,
            .duty_resolution = LEDC_TIMER_14_BIT,
            .freq_hz = 50,
            .clk_cfg = LEDC_AUTO_CLK,
        };
        ledc_channel_config_t ch = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = LEDC_CHANNEL_0,
            .timer_sel = LEDC_TIMER_0,
            .intr_type = LEDC_INTR_DISABLE,
            .gpio_num = HW_SERVO_PWM_GPIO,
            .duty = 0,
            .hpoint = 0,
        };
        if (ledc_timer_config(&timer) != ESP_OK || ledc_channel_config(&ch) != ESP_OK) {
            hw_report("舵机 PWM", false, "LEDC 配置失败");
            return;
        }
        s_servo_ready = true;
    }

    const int steps[] = {1000, 1500, 2000, 1500};
    for (int i = 0; i < 4; i++) {
        servo_us(steps[i]);
        vTaskDelay(pdMS_TO_TICKS(400));
    }
    hw_report("舵机 GPIO8", true, "已输出 1.0/1.5/2.0ms 脉宽，目视 CN2 舵机");
}
