#include "hwtest.h"
#include "board_pins.h"

#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "hwtest";
static i2c_master_bus_handle_t s_i2c;
static int s_pass;
static int s_fail;

void hw_init_safe_rails(void)
{
    gpio_config_t out = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << HW_WS2812_EN_GPIO) | (1ULL << HW_ML307_PWR_GPIO) | (1ULL << HW_EYE_BL_GPIO),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&out);
    gpio_set_level(HW_WS2812_EN_GPIO, 1);
    gpio_set_level(HW_ML307_PWR_GPIO, 0);
    gpio_set_level(HW_EYE_BL_GPIO, 0);

    gpio_config_t in = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << HW_BOOT_BTN_GPIO) | (1ULL << HW_USER_BTN_GPIO) | (1ULL << HW_PA_EN_SCH_GPIO),
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&in);

    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = HW_I2C_PORT,
        .sda_io_num = HW_I2C_SDA_GPIO,
        .scl_io_num = HW_I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            .enable_internal_pullup = true,
        },
    };
    esp_err_t err = i2c_new_master_bus(&bus_cfg, &s_i2c);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C 总线创建失败: %s", esp_err_to_name(err));
        s_i2c = NULL;
    }
}

i2c_master_bus_handle_t hw_i2c_bus(void)
{
    return s_i2c;
}

void hw_report(const char *name, bool ok, const char *detail)
{
    if (ok) {
        s_pass++;
        ESP_LOGI(TAG, "[PASS] %s%s%s", name, detail ? " — " : "", detail ? detail : "");
    } else {
        s_fail++;
        ESP_LOGE(TAG, "[FAIL] %s%s%s", name, detail ? " — " : "", detail ? detail : "");
    }
}

int hw_pass_count(void) { return s_pass; }
int hw_fail_count(void) { return s_fail; }

void hw_reset_counts(void)
{
    s_pass = 0;
    s_fail = 0;
}
