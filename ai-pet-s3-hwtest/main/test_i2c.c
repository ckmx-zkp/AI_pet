#include "hwtest.h"
#include "board_pins.h"

#include <stdio.h>
#include "esp_err.h"

static bool probe(uint8_t addr)
{
    i2c_master_bus_handle_t bus = hw_i2c_bus();
    if (!bus) {
        return false;
    }
    return i2c_master_probe(bus, addr, 50) == ESP_OK;
}

void test_i2c(void)
{
    printf("I2C 扫描 GPIO1/SDA GPIO2/SCL ...\n");
    if (!hw_i2c_bus()) {
        hw_report("I2C 总线", false, "未初始化");
        return;
    }

    int found = 0;
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        if (probe(addr)) {
            printf("  发现 0x%02X\n", addr);
            found++;
        }
    }
    char buf[48];
    snprintf(buf, sizeof(buf), "共 %d 个地址", found);
    hw_report("I2C 扫描", found > 0, buf);
    hw_report("ES8311 0x18", probe(HW_ES8311_ADDR), probe(HW_ES8311_ADDR) ? "在线" : "无应答");
    hw_report("ES7210 0x41", probe(HW_ES7210_ADDR), probe(HW_ES7210_ADDR) ? "在线" : "无应答");
}
