#include "hwtest.h"
#include "board_pins.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static bool codec_write(uint8_t addr, uint8_t reg, uint8_t val)
{
    i2c_master_bus_handle_t bus = hw_i2c_bus();
    if (!bus) {
        return false;
    }
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000,
    };
    i2c_master_dev_handle_t dev = NULL;
    if (i2c_master_bus_add_device(bus, &dev_cfg, &dev) != ESP_OK) {
        return false;
    }
    uint8_t buf[2] = {reg, val};
    esp_err_t err = i2c_master_transmit(dev, buf, 2, 50);
    i2c_master_bus_rm_device(dev);
    return err == ESP_OK;
}

static bool codec_read(uint8_t addr, uint8_t reg, uint8_t *val)
{
    i2c_master_bus_handle_t bus = hw_i2c_bus();
    if (!bus) {
        return false;
    }
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000,
    };
    i2c_master_dev_handle_t dev = NULL;
    if (i2c_master_bus_add_device(bus, &dev_cfg, &dev) != ESP_OK) {
        return false;
    }
    esp_err_t err = i2c_master_transmit_receive(dev, &reg, 1, val, 1, 50);
    i2c_master_bus_rm_device(dev);
    return err == ESP_OK;
}

void test_audio(void)
{
    uint8_t id1 = 0, id2 = 0;
    bool es8311 = codec_read(HW_ES8311_ADDR, 0xFD, &id1) && codec_read(HW_ES8311_ADDR, 0xFE, &id2);
    if (es8311) {
        char buf[40];
        snprintf(buf, sizeof(buf), "chip 0x%02X%02X", id1, id2);
        hw_report("ES8311 ID", id1 == 0x83, buf);
    } else {
        hw_report("ES8311 ID", false, "读寄存器失败");
    }

    uint8_t es7210_id = 0;
    bool es7210 = codec_read(HW_ES7210_ADDR, 0x3D, &es7210_id) || codec_read(HW_ES7210_ADDR, 0x00, &es7210_id);
    hw_report("ES7210 读寄存器", es7210, es7210 ? "有应答" : "无应答");

    if (!es8311) {
        return;
    }

    codec_write(HW_ES8311_ADDR, 0x00, 0x1F);
    vTaskDelay(pdMS_TO_TICKS(10));
    codec_write(HW_ES8311_ADDR, 0x00, 0x80);
    codec_write(HW_ES8311_ADDR, 0x01, 0x30);
    codec_write(HW_ES8311_ADDR, 0x02, 0x10);
    codec_write(HW_ES8311_ADDR, 0x16, 0x24);
    codec_write(HW_ES8311_ADDR, 0x12, 0x00);
    codec_write(HW_ES8311_ADDR, 0x32, 0xBF);

    i2s_chan_handle_t tx = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    if (i2s_new_channel(&chan_cfg, &tx, NULL) != ESP_OK) {
        hw_report("I2S 通道", false, "创建失败");
        return;
    }
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = HW_I2S_MCLK_GPIO,
            .bclk = HW_I2S_BCLK_GPIO,
            .ws = HW_I2S_WS_GPIO,
            .dout = HW_I2S_DOUT_GPIO,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    if (i2s_channel_init_std_mode(tx, &std_cfg) != ESP_OK || i2s_channel_enable(tx) != ESP_OK) {
        hw_report("I2S 输出", false, "初始化失败");
        i2s_del_channel(tx);
        return;
    }

    const int samples = 256;
    int16_t *pcm = calloc((size_t)samples * 2, sizeof(int16_t));
    if (!pcm) {
        hw_report("I2S 正弦", false, "内存不足");
        i2s_del_channel(tx);
        return;
    }
    for (int i = 0; i < samples; i++) {
        int16_t s = (int16_t)(sinf(2.f * 3.1415926f * i / samples) * 8000);
        pcm[i * 2] = s;
        pcm[i * 2 + 1] = s;
    }
    size_t written = 0;
    for (int n = 0; n < 40; n++) {
        i2s_channel_write(tx, pcm, samples * 2 * sizeof(int16_t), &written, 100);
    }
    free(pcm);
    i2s_channel_disable(tx);
    i2s_del_channel(tx);
    hw_report("I2S 正弦", true, "已推流；无功放改版时喇叭可能无声");
}
