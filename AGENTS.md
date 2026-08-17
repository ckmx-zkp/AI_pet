# AGENTS.md — AI Pet 工作区总入口（D:/Home_Work）

> 本文件是**所有 AI 编程会话**（Kimi CLI / Codex / 其他 agent）进入本工作区的第一入口。
> 本工作区采用多会话 vibe coding 协作：不同 AI 会话分别负责不同仓库，**通过文档同步状态，而不是通过口头约定**。

## 开工三步（每次会话必做）

1. 先安全同步看板仓：`cd work_dashboard && git status --short`。**仅工作树干净时**执行 `git pull --ff-only`；若有未提交改动，不得 pull、不得用自己的提交混入他人改动，先阅读当前内容并协调。随后读 `work_dashboard/AI-Pet项目全景与进度.md`（**第一信息源**：经代码核实的各仓真实进度、文档地图、出入提示）；再读 `work_dashboard/AI-Pet协作看板.md`（日常状态流水）；任务涉及固件时，再读 `work_dashboard/AI-Pet固件联调看板.md`。
2. 读你负责仓库根目录的 `AGENTS.md`，以及该仓 `docs/00*`（协作边界文档）。
3. 确认任务的上下游状态（看板"集成点状态""待决事项"），避免与并行会话冲突或重复劳动。

## 收工义务（每次会话必做）

- 当代码、接口、部署、正式需求决策或跨仓依赖状态发生变化时，**立即**更新看板中对应状态，并在看板文末"进度日志"追加一行（日期 + 仓库/侧 + 事项）。提交前先复查 `git status`，只暂存目标看板文件；确认没有混入他人改动后再 `git commit && push`（远端 `github.com/ckmx-zkp/work_dashboard`，远程同事在网页上看和改）。纯分析、调研或未确认的视觉试验不写看板。
- **契约先行**：改接口先改契约文档，再改代码，并在进度日志注明"契约已变更"。接口唯一真源：
  - `ai-pet-backend/docs/06-HTTP-API规范.md`（用户/内部 HTTP API）
  - `xiaozhi-server/docs/05-与业务后端集成接口.md`（backend ↔ xiaozhi-server）
- 看板只记**状态与事实**；设计细节、接口 schema 一律进各仓 docs，不进看板。
- **密钥、密码不进任何文档和 git 仓库**，只记位置（如"见服务器 .env"）。

## 仓库地图

| 仓库 | 路径 | 职责 | 栈 | 状态 | 协作文件 |
|------|------|------|-----|------|----------|
| ai-pet-backend | `ai-pet-backend/` | 业务后端：用户/设备/KB/persona/记忆/MCP/worker | Python 3.11+ / FastAPI / PG16 | 已上线能力与迭代状态见项目全景/看板 | 本仓 `AGENTS.md`（已有） |
| xiaozhi-server | `xiaozhi-server/` | 自建小智兼容后台：OTA/激活/实时语音/MCP 路由 | Python + Java + Vue2（上游快照） | v0.9.6 已上线，V0.2 集成中 | 本仓 `AGENTS.md` |
| ai-pet-admin | `ai-pet-admin/` | Web 管理台（开发者/运营） | Vue3 + Vite + TS + Element Plus | 已上线能力与迭代状态见项目全景/看板 | 本仓 `AGENTS.md` |
| ai-pet-app | `ai-pet-app/` | 用户端（手机 PWA + 桌面） | Vue3 + Vite + TS | 已上线能力与迭代状态见项目全景/看板 | 本仓 `AGENTS.md` |
| ai-pet-ops | `ai-pet-ops/` | 服务器运维与状态检测 | Python 标准库 + systemd | V0 骨架 | 本仓 `AGENTS.md` |
| ESP32_XIAOZHI | `ESP32_XIAOZHI/xiaozhi-esp32/` | 固件 Git 仓（外层 `ESP32_XIAOZHI/` 为母文档与资料目录） | C/C++ ESP-IDF | 已上线能力与迭代状态见项目全景/看板 | 外层与固件仓 `AGENTS.md` |

除 `ESP32_XIAOZHI/` 外层母目录外，各项目均为**独立 git 仓库**（远端在 GitHub `ckmx-zkp/` 下）；固件实际仓为其下 `xiaozhi-esp32/`。跨仓改动须分别提交。

工作区总仓是 `ckmx-zkp/AI_pet`（本目录）。它用 git submodule 挂上表内各仓，方便另一台电脑一次克隆整个 `Home_Work`。**日常提交仍进各子仓自己的 origin**；服务器部署和各仓 CI 也只拉那些独立远端。不要把子仓的 `origin` 改成 `AI_pet`。另一台电脑用法见根目录 `README.md`。

## 部署环境

唯一真源在 `work_dashboard/AI-Pet协作看板.md` 的"部署环境"节（阿里云 ECS `39.107.143.71`，端口分配、部署目录、密钥位置），此处不复制、不另建副本。

## 通用约定

- 文档、注释、看板一律用**中文**；代码标识符用英文。
- 最小改动：不做与任务无关的重构/清理；改动只落在任务涉及的仓库和模块。
- 每个仓库的构建/测试命令见各自 `AGENTS.md`；提交前跑该仓的质量闸（有的话）。
- 没有测试的仓库（xiaozhi-server / ai-pet-admin / ai-pet-app / 固件）以构建通过 + 真机或端到端验证为准，并在看板写清验证结果。
