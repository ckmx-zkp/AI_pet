#include "hwtest.h"
#include "board_pins.h"

#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "usb/usb_host.h"
#include "usb/uvc_host.h"

static const char *TAG = "hwtest.cam";

#define USB_HOST_TASK_PRIO  15
#define FRAME_WAIT_MS       5000
#define OPEN_WAIT_MS        8000
#define WANT_FRAMES         3

static QueueHandle_t s_frame_q;
static bool s_usb_ready;
static volatile bool s_disconnected;

static bool frame_callback(const uvc_host_frame_t *frame, void *user_ctx)
{
    QueueHandle_t q = *((QueueHandle_t *)user_ctx);
    if (xQueueSendToBack(q, &frame, 0) != pdPASS) {
        return true;
    }
    return false;
}

static void stream_callback(const uvc_host_stream_event_data_t *event, void *user_ctx)
{
    (void)user_ctx;
    switch (event->type) {
    case UVC_HOST_TRANSFER_ERROR:
        ESP_LOGE(TAG, "USB 传输错误 err=%d", event->transfer_error.error);
        break;
    case UVC_HOST_DEVICE_DISCONNECTED:
        s_disconnected = true;
        ESP_LOGW(TAG, "UVC 设备断开");
        uvc_host_stream_close(event->device_disconnected.stream_hdl);
        break;
    case UVC_HOST_FRAME_BUFFER_OVERFLOW:
        ESP_LOGW(TAG, "帧缓冲溢出");
        break;
    case UVC_HOST_FRAME_BUFFER_UNDERFLOW:
        ESP_LOGW(TAG, "帧缓冲不足");
        break;
    default:
        break;
    }
}

static void usb_lib_task(void *arg)
{
    (void)arg;
    while (1) {
        uint32_t flags = 0;
        usb_host_lib_handle_events(portMAX_DELAY, &flags);
        if (flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            usb_host_device_free_all();
        }
    }
}

static bool usb_stack_init(void)
{
    if (s_usb_ready) {
        return true;
    }

    const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .root_port_unpowered = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    esp_err_t err = usb_host_install(&host_config);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "usb_host_install: %s", esp_err_to_name(err));
        return false;
    }

    if (xTaskCreatePinnedToCore(usb_lib_task, "usb_lib", 4096, NULL,
                                USB_HOST_TASK_PRIO, NULL, tskNO_AFFINITY) != pdPASS) {
        ESP_LOGE(TAG, "usb_lib 任务创建失败");
        return false;
    }

    const uvc_host_driver_config_t uvc_cfg = {
        .driver_task_stack_size = 4 * 1024,
        .driver_task_priority = USB_HOST_TASK_PRIO + 1,
        .xCoreID = tskNO_AFFINITY,
        .create_background_task = true,
    };
    err = uvc_host_install(&uvc_cfg);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "uvc_host_install: %s", esp_err_to_name(err));
        return false;
    }

    s_frame_q = xQueueCreate(3, sizeof(uvc_host_frame_t *));
    if (!s_frame_q) {
        return false;
    }

    s_usb_ready = true;
    return true;
}

static int wait_device_count(int timeout_ms)
{
    int waited = 0;
    usb_host_lib_info_t info = {0};
    while (waited <= timeout_ms) {
        if (usb_host_lib_info(&info) == ESP_OK && info.num_devices > 0) {
            return info.num_devices;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
        waited += 100;
    }
    usb_host_lib_info(&info);
    return info.num_devices;
}

static void drain_frames(uvc_host_stream_hdl_t stream)
{
    uvc_host_frame_t *frame = NULL;
    while (xQueueReceive(s_frame_q, &frame, 0) == pdPASS) {
        if (stream) {
            uvc_host_frame_return(stream, frame);
        }
    }
}

static int capture_frames(uvc_host_stream_hdl_t stream, int want)
{
    int got = 0;
    s_disconnected = false;
    if (uvc_host_stream_start(stream) != ESP_OK) {
        return 0;
    }

    while (got < want && !s_disconnected) {
        uvc_host_frame_t *frame = NULL;
        if (xQueueReceive(s_frame_q, &frame, pdMS_TO_TICKS(FRAME_WAIT_MS)) != pdPASS) {
            break;
        }
        got++;
        printf("  第 %d 帧  %u 字节\n", got, (unsigned)frame->data_len);
        uvc_host_frame_return(stream, frame);
    }

    if (!s_disconnected) {
        uvc_host_stream_stop(stream);
    }
    drain_frames(s_disconnected ? NULL : stream);
    return got;
}

static bool try_open_and_capture(uint16_t w, uint16_t h, float fps,
                                 enum uvc_host_stream_format fmt, int *got_out)
{
    uvc_host_stream_config_t cfg = {
        .event_cb = stream_callback,
        .frame_cb = frame_callback,
        .user_ctx = &s_frame_q,
        .usb = {
            .vid = UVC_HOST_ANY_VID,
            .pid = UVC_HOST_ANY_PID,
            .uvc_stream_index = 0,
        },
        .vs_format = {
            .h_res = w,
            .v_res = h,
            .fps = fps,
            .format = fmt,
        },
        .advanced = {
            .frame_size = 0,
            .number_of_frame_buffers = 3,
            .number_of_urbs = 3,
            .urb_size = 10 * 1024,
            .frame_heap_caps = MALLOC_CAP_SPIRAM,
        },
    };

    const char *fmt_name = (fmt == UVC_VS_FORMAT_DEFAULT) ? "DEFAULT" : "MJPEG";
    printf("  尝试 UVC %s %ux%u @ %.0f fps ...\n", fmt_name, w, h, fps);
    uvc_host_stream_hdl_t stream = NULL;
    esp_err_t err = uvc_host_stream_open(&cfg, pdMS_TO_TICKS(OPEN_WAIT_MS), &stream);
    if (err != ESP_OK) {
        printf("  打开失败: %s\n", esp_err_to_name(err));
        return false;
    }

    *got_out = capture_frames(stream, WANT_FRAMES);
    if (!s_disconnected && stream) {
        uvc_host_stream_close(stream);
    }
    return *got_out > 0;
}

void test_usb_cam(void)
{
    printf("USB 摄像头：U4  D−=GPIO%d  D+=GPIO%d（S3 原生 USB Host）\n",
           (int)HW_USB_DM_GPIO, (int)HW_USB_DP_GPIO);
    printf("请把 UVC 摄像头插在 U4，不要插 K230 Type-C。\n");

    if (!usb_stack_init()) {
        hw_report("USB Host", false, "协议栈初始化失败");
        return;
    }
    hw_report("USB Host", true, "已安装，PHY=内部 GPIO19/20");

    int n = wait_device_count(3000);
    char bus[48];
    snprintf(bus, sizeof(bus), "枚举到 %d 个设备", n);
    hw_report("USB 总线", n > 0, bus);
    if (n <= 0) {
        hw_report("UVC 取流", false, "U4 上没有 USB 设备");
        return;
    }

    static const struct {
        uint16_t w;
        uint16_t h;
        float fps;
        enum uvc_host_stream_format fmt;
    } formats[] = {
        {320, 240, 0,  UVC_VS_FORMAT_DEFAULT},
        {320, 240, 15, UVC_VS_FORMAT_MJPEG},
        {320, 240, 30, UVC_VS_FORMAT_MJPEG},
        {640, 480, 15, UVC_VS_FORMAT_MJPEG},
        {160, 120, 15, UVC_VS_FORMAT_MJPEG},
    };

    int got = 0;
    bool ok = false;
    for (size_t i = 0; i < sizeof(formats) / sizeof(formats[0]); i++) {
        if (try_open_and_capture(formats[i].w, formats[i].h, formats[i].fps, formats[i].fmt, &got)) {
            char detail[64];
            snprintf(detail, sizeof(detail), "%ux%u 收到 %d 帧",
                     formats[i].w, formats[i].h, got);
            hw_report("UVC 取流", true, detail);
            ok = true;
            break;
        }
    }
    if (!ok) {
        hw_report("UVC 取流", false, "设备在线但无法开 UVC 流（看描述符）");
    }
}
