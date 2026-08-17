# AGENTS.md — ESP32_XIAOZHI（固件 + 产品母文档）

> AI 会话进本目录前先读 `D:/Home_Work/AGENTS.md`，并按其中的安全规则检查/同步 `work_dashboard`；再读项目全景、本文件和 `D:/Home_Work/work_dashboard/AI-Pet固件联调看板.md`（固件↔服务端联调状态唯一同步点）。看板工作树不干净时不得擅自 pull 或把他人改动提交出去。

## 定位与布局

本目录 = **固件工程 + 产品/服务器/赛道母文档库**，不是一个 git 仓库：

- `xiaozhi-esp32/`：**真正的固件 git 仓库**（上游 `78/xiaozhi-esp32` v2.2.6 fork，远端 `ckmx-zkp/Tboy_P4_xiaozhi`），目标硬件 Waveshare ESP32-P4-WIFI6-Touch-LCD-7B。所有代码改动都在这里。
- 根目录 `*.md` 母文档（`AI_Pet完整业务设计文档.md` 等）：**只读追溯用，勿删**。
- `开发仓库目录索引.md`：本目录与四个兄弟仓的分工索引。
- `模块-7针蓝板模块/`、`舵机控制例程/`、`outputs/`：硬件参考资料。

## 固件仓（xiaozhi-esp32/）关键文件

- `AI_PET_PROGRESS_zh.md`：**进度真源，每次改动后必须同步更新**（状态/引脚/路径/下一步）。
- `AI_PET_DEV_PLAN_zh.md`：设计总纲；`AI_PET_VISION_REALTIME_PLAN_zh.md`：视觉规划。
- 自研板型：`main/boards/waveshare/esp32-p4-ai-pet/`（含 `pet_eye_display.*`、`eye_controller.h`）；新功能目录 `main/pet/`。

## 技术栈与命令

- C/C++ + ESP-IDF + CMake；Python 脚本做资产生成（`scripts/gen_placeholder_eyes.py` 等）。
- 构建/烧录：`idf.py build flash`；监视：`idf.py monitor`。
- **约定默认增量编译，非必要不 clean**（全量编译很慢）。
- **无自动化测试**：验证 = 烧录 + 串口日志 + 真机目视/语音验证，结果写进固件联调看板。

## 红线与约定

- 中文注释与文档；新板型/新功能**派生目录，不动旧 7B 板型代码**。
- MCP 工具仿 `LampController` 模式注册（参考 `eye_controller.h` 的 `self.eye.*`）；服务端只路由，协议细节以 `xiaozhi-esp32/docs/` 上游协议文档为准。
- 眼睛渲染走"整帧状态图切换"（C1 架构），资产 RGB565 小端、刷屏字节交换——改动前先看 `AI_PET_EYE_*` 系列文档。
- 接入信息（OTA 地址、WS 地址、本机 MAC/UUID）以 `D:/Home_Work/work_dashboard/AI-Pet固件联调看板.md` 为准，不硬编码进文档副本。

## 收工义务

1. 更新 `xiaozhi-esp32/AI_PET_PROGRESS_zh.md`（代码改动必做）。
2. 更新 `D:/Home_Work/work_dashboard/AI-Pet固件联调看板.md` 对应集成点状态与进度日志。
