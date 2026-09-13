#pragma once

#include <stdbool.h>
#include "driver/i2c_master.h"

void hw_init_safe_rails(void);
i2c_master_bus_handle_t hw_i2c_bus(void);

void hw_report(const char *name, bool ok, const char *detail);
int hw_pass_count(void);
int hw_fail_count(void);
void hw_reset_counts(void);

void test_i2c(void);
void test_display(void);
void test_led(void);
void test_buttons(void);
void test_touch(void);
void test_servo(void);
void test_k230_uart(void);
void test_ml307_uart(bool power_on);
void test_audio(void);
void test_wifi_scan(void);
void test_pa_pin(void);
void test_usb_cam(void);
void test_all_safe(void);
