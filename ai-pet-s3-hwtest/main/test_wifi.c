#include "hwtest.h"

#include <stdio.h>
#include <string.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

static bool s_wifi_ready;

static bool ensure_wifi(void)
{
    if (s_wifi_ready) {
        return true;
    }
    if (nvs_flash_init() == ESP_ERR_NVS_NO_FREE_PAGES) {
        nvs_flash_erase();
        nvs_flash_init();
    }
    if (esp_netif_init() != ESP_OK) {
        return false;
    }
    esp_err_t ev = esp_event_loop_create_default();
    if (ev != ESP_OK && ev != ESP_ERR_INVALID_STATE) {
        return false;
    }
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (esp_wifi_init(&cfg) != ESP_OK) {
        return false;
    }
    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK || esp_wifi_start() != ESP_OK) {
        return false;
    }
    s_wifi_ready = true;
    return true;
}

void test_wifi_scan(void)
{
    if (!ensure_wifi()) {
        hw_report("WiFi", false, "初始化失败");
        return;
    }
    uint16_t number = 16;
    wifi_ap_record_t aps[16];
    memset(aps, 0, sizeof(aps));
    if (esp_wifi_scan_start(NULL, true) != ESP_OK) {
        hw_report("WiFi 扫描", false, "scan_start 失败");
        return;
    }
    if (esp_wifi_scan_get_ap_records(&number, aps) != ESP_OK) {
        hw_report("WiFi 扫描", false, "读记录失败");
        return;
    }
    printf("扫描到 %u 个 AP：\n", number);
    for (uint16_t i = 0; i < number; i++) {
        printf("  %s  rssi=%d\n", aps[i].ssid, aps[i].rssi);
    }
    hw_report("WiFi RF", number > 0, number > 0 ? "模组射频正常" : "未扫到 AP");
}
