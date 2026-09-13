# AGENTS.md — ai-pet-s3-hwtest

独立 ESP-IDF 硬件测试工程，不是量产固件。先读 `D:/Home_Work/AGENTS.md`。

- 目标板：USB 摄像头大板（2026-08-25 原理图）
- 引脚真源：`main/board_pins.h`
- 不要把本工程的改动混进 `ESP32_XIAOZHI/xiaozhi-esp32/`
- 构建：IDF 5.5.2，`idf.py set-target esp32s3 && idf.py build`
- 4G 电源默认关闭；`4gon` 会拉高 `GPIO18`
- USB 摄像头走 U4 / GPIO19/20，命令 `cam`

代码改动后更新 `work_dashboard/AI-Pet固件联调看板.md` 进度日志。
