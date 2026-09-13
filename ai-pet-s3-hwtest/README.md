# AI Pet S3 硬件测试工程

独立 ESP-IDF 工程，用来在 **USB 摄像头大板** 上逐项确认外设，不是量产小智固件。

- 板卡：ESP32-S3-WROOM-2-N32R16V
- 原理图：`../hardware/SCH_USB摄像头大板_2026-08-25.pdf`
- 眼睛屏：`../hardware/XJ0.99TFT-12P/`（GC9107，128×115，12P FPC）
- 引脚真源：`main/board_pins.h`

量产语音固件仍在 `ESP32_XIAOZHI/xiaozhi-esp32/`。

## 编译 / 烧录

本机 IDF 5.5.2：`d:\v5.5.2\esp-idf`

本机 `export.ps1` 会找错 Python 3.14 环境，请直接用 5.5 的 3.11：

```powershell
$env:IDF_PATH = "d:\v5.5.2\esp-idf"
$env:IDF_TOOLS_PATH = "D:\Espressif"
$env:IDF_PYTHON_ENV_PATH = "D:\Espressif\python_env\idf5.5_py3.11_env"
$env:PATH = "D:\Espressif\tools\cmake\3.30.2\bin;D:\Espressif\tools\ninja\1.12.1;D:\Espressif\tools\idf-exe\1.0.3;D:\Espressif\python_env\idf5.5_py3.11_env\Scripts;" + $env:PATH
cd D:\Home_Work\ai-pet-s3-hwtest
python $env:IDF_PATH\tools\idf.py set-target esp32s3
python $env:IDF_PATH\tools\idf.py build
python $env:IDF_PATH\tools\idf.py -p COMx flash monitor
```

2026-09-05 已在 IDF 5.5.2 下编译通过（含 USB Host / UVC）。

串口 115200。出现 `hwtest>` 后输入命令。

## 命令

| 命令 | 测什么 |
|------|--------|
| `all` | 安全套件（不开 4G 电源，不等待按键） |
| `i2c` | ES8311 `0x18`、ES7210 `0x41` |
| `audio` | 读 codec + I2S 正弦 |
| `eye` | 左眼红、右眼蓝。屏是 XJ0.99TFT-12P / GC9107 128×115，当前代码仍用 GC9A01 初始化，不能当点亮验收 |
| `led` | 两颗 WS2812 |
| `btn` | BOOT / KEY1 |
| `touch` | CN1 三路触摸原始值 |
| `servo` | CN2 舵机 |
| `k230` | UART1 PING |
| `4g` / `4gon` | UART2；`4gon` 会拉高 4G 电源 |
| `wifi` | 扫描 AP |
| `pa` | 功放 GPIO46 门禁 |
| `cam` | U4 USB Host：枚举设备并取 3 帧 MJPEG |

USB 摄像头插 **U4**（S3 原生 USB，GPIO19 D− / GPIO20 D+），不要插 K230 Type-C。`all` 会跑 `cam`。

## 已知硬件门禁

1. `PA_EN` 接到 GPIO46（仅输入），喇叭可能无声，需飞线/改版到 GPIO9 或 GPIO21。
2. 4G UART 原理图仍标 `1.8V?`，默认不供电。
