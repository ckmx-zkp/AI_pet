#include "hwtest.h"
#include "board_pins.h"

#include <stdio.h>
#include "driver/gpio.h"

void test_pa_pin(void)
{
    int level = gpio_get_level(HW_PA_EN_SCH_GPIO);
    printf("原理图 PA_EN=GPIO46（仅输入）当前电平=%d\n", level);
    printf("R40 下拉，NS4150B 默认关闭。软件无法把 GPIO46 拉高。\n");
    printf("改版/飞线请改到 GPIO9 或 GPIO21。\n");
    hw_report("PA_EN 脚位", false, "GPIO46 仅输入，功放软件不可控（设计门禁）");
}

void test_all_safe(void)
{
    hw_reset_counts();
    test_pa_pin();
    test_i2c();
    test_audio();
    test_display();
    test_led();
    test_touch();
    test_servo();
    test_k230_uart();
    test_ml307_uart(false);
    test_wifi_scan();
    test_usb_cam();
    printf("\n安全套件结束：PASS=%d  FAIL=%d（btn / 4gon 需手工另跑）\n",
           hw_pass_count(), hw_fail_count());
}
