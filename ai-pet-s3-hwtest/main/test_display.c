#include "hwtest.h"
#include "board_pins.h"

#include <stdlib.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_gc9a01.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

static const char *TAG = "eye";
static bool s_spi_ready;
static esp_lcd_panel_handle_t s_left;
static esp_lcd_panel_handle_t s_right;

static uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    return (uint16_t)((c >> 8) | (c << 8));
}

static bool fill_panel(esp_lcd_panel_handle_t panel, uint16_t color)
{
    const int band_h = 20;
    uint16_t *buf = heap_caps_malloc((size_t)HW_EYE_W * band_h * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (!buf) {
        return false;
    }
    for (int i = 0; i < HW_EYE_W * band_h; i++) {
        buf[i] = color;
    }
    bool ok = true;
    for (int y = 0; y < HW_EYE_H; y += band_h) {
        if (esp_lcd_panel_draw_bitmap(panel, 0, y, HW_EYE_W, y + band_h, buf) != ESP_OK) {
            ok = false;
            break;
        }
    }
    free(buf);
    return ok;
}

static esp_lcd_panel_handle_t make_eye(gpio_num_t cs)
{
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_io_spi_config_t io_cfg = {
        .cs_gpio_num = cs,
        .dc_gpio_num = HW_EYE_DC_GPIO,
        .spi_mode = 0,
        .pclk_hz = 40 * 1000 * 1000,
        .trans_queue_depth = 8,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    if (esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)HW_EYE_SPI_HOST, &io_cfg, &io) != ESP_OK) {
        return NULL;
    }

    esp_lcd_panel_handle_t panel = NULL;
    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    if (esp_lcd_new_panel_gc9a01(io, &panel_cfg, &panel) != ESP_OK) {
        return NULL;
    }
    esp_lcd_panel_reset(panel);
    if (esp_lcd_panel_init(panel) != ESP_OK) {
        return NULL;
    }
    esp_lcd_panel_invert_color(panel, true);
    esp_lcd_panel_disp_on_off(panel, true);
    return panel;
}

static bool ensure_spi(void)
{
    if (s_spi_ready) {
        return true;
    }
    spi_bus_config_t buscfg = {
        .sclk_io_num = HW_EYE_SCLK_GPIO,
        .mosi_io_num = HW_EYE_MOSI_GPIO,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = HW_EYE_W * 20 * sizeof(uint16_t),
    };
    esp_err_t err = spi_bus_initialize(HW_EYE_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "SPI 初始化失败: %s", esp_err_to_name(err));
        return false;
    }
    s_spi_ready = true;
    return true;
}

void test_display(void)
{
    ESP_LOGW(TAG, "本板眼睛是 XJ0.99TFT-12P/GC9107 128x115，不是 GC9A01；厂商初始化尚未移植");
    if (!ensure_spi()) {
        hw_report("眼睛 SPI", false, "总线失败");
        return;
    }
    gpio_set_level(HW_EYE_BL_GPIO, 1);
    if (!s_left) {
        s_left = make_eye(HW_EYE_CS1_GPIO);
    }
    if (!s_right) {
        s_right = make_eye(HW_EYE_CS2_GPIO);
    }

    bool left_ok = s_left && fill_panel(s_left, rgb565(255, 0, 0));
    bool right_ok = s_right && fill_panel(s_right, rgb565(0, 80, 255));
    hw_report("左眼 CS1=GPIO17", left_ok, left_ok ? "应显示红色" : "初始化或刷屏失败");
    hw_report("右眼 CS2=GPIO16", right_ok, right_ok ? "应显示蓝色" : "初始化或刷屏失败");
}
