# 05-快速 Demo 原型设计与需求分配（PRD）

> 角色：产品经理。本文回答三个问题：①快速 Demo 演示什么；②哪些功能必须（Must）、哪些预留（Reserve）；③需求如何分配到 backend / admin / xiaozhi-server / 固件 / App 五个项目。
> 依据：`00-AI宠物市场与竞品调研.md` 的六条启示、`01~04` 各项目缺口文档、`work_dashboard/AI-Pet协作看板.md` 当前进度。
> 版本：v1.0（2026-08-01）

## 一、Demo 的定位：验证什么

快速 Demo 不是功能展销，是**用 5 分钟证明"这不是语音助手，是一只有人格、有记忆的宠物"**。市场告诉我们：Demo 打动人的从来不是功能数量，而是"它记得我"的那一瞬间（芙崽记者实测原文：深夜被它安慰"完成比完美更重要"时产生养成情绪）。

**Demo 剧本（验收标准即剧本跑通）：**

| 幕 | 动作 | 要证明的 |
|----|------|---------|
| 1. 认领 | 用户注册 → 扫码/输码绑定设备 | 30 秒完成，无卡点 |
| 2. 赋格 | 选星座（双鱼）+ MBTI 简问（4 题）→ 保存 → **当场看到编译后的人设卡预览** | "我在塑造它"，而非"我在填表" |
| 3. 初聊 | 对设备说话 → 宠物用双鱼 INFP 口吻回应，眼睛情绪联动 | 口吻可辨，盲测区别于通用助手 |
| 4. 埋点 | 用户说"我下周要考试，有点紧张" → 挂断 | 语料落库（脱敏） |
| 5. 成忆 | （worker 夜间或手动触发）管理台/App 出现记忆候选 → 用户一键"通过" | 记忆资产可见、可审 |
| 6. 召回 | **下一次开场，宠物主动说"考试准备得怎么样啦"** | 全 Demo 的高潮：它记得我 |
| 7. 资产 | 用户在 App/管理台看到：对话历史、记忆列表、人设卡、设备状态 | 数据是我的，可查可删 |

第 6 幕是整个 Demo 的灵魂，也是当前系统最大的缺口（记忆只存不召回，见 01-G3、03-G6）。**Demo 范围就按让这 7 幕跑通来切，多一个功能都不要。**

## 二、Must / Reserve / Won't 总表

### Must（Demo 必须，没有就演不下去）

| # | 需求 | 落在哪个项目 |
|---|------|-------------|
| M1 | persona 最小链：KB 种子（水相+双鱼+4 个 MBTI 即可）+ PersonaCompiler + persona_pack API | backend |
| M2 | 人设编译预览（dry-run）：保存前看到编译结果 | backend 出端点 + admin 展示 |
| M3 | persona_pack 运行时拉取 + 缓存降级 + 开场 default_emotion | xiaozhi-server |
| M4 | 旁路转写落库（`POST /internal/chat/events`，脱敏） | xiaozhi-server → backend |
| M5 | 记忆最小环：worker 从对话产候选 → memories approve API → Memory MCP `search/add`（HTTP 传输） | backend |
| M6 | **记忆主动召回 hook（最小版）**：开场时把该设备最近 3 条已审记忆注入 prompt | xiaozhi-server（拉 pack 时捎带） |
| M7 | 管理台 M2 人设页（含预览）+ M3 记忆审核页（列表+通过/驳回） | admin |
| M8 | 体验债清偿：S1 关开场白 / S2 休息即挂断 / 误唤醒阈值 | xiaozhi-server + 固件 |
| M9 | messages 查询 API（历史浏览，最简分页） | backend |
| M10 | pet_state 最小字段：`last_interaction_at`、`bond_level`（只加列，不做状态机） | backend |

### Reserve（预留：Demo 不做，但设计必须留口，避免返工）

| # | 预留项 | 预留方式 |
|---|--------|---------|
| R1 | 人格状态机（饥饿/心情/精力） | pet_state 表按 JSONB 或预留列设计，Demo 只写两个字段 |
| R2 | 主动会话通道（宠物先开口） | xiaozhi-server 侧确认 WS 下推可行性并写备忘录，不实现 |
| R3 | 触摸传感器 | 固件结构未冻结，M3 前留 GPIO 与结构空间（BOM 预留） |
| R4 | 养成/演化（性格随相处变化） | persona 数据模型保留 `growth` 扩展字段位 |
| R5 | 共同经历数据模型 | messages 表加 `event_type` 列（chat/game/ritual），Demo 只写 chat |
| R6 | 声纹分人记忆 | memories 表保留 `speaker_id` 可空列 |
| R7 | 多版本灰度 | 已有 follow_latest/钉扎，UI 不做 |
| R8 | 导出格式 | E8 契约先行定 schema，Demo 不做导出 |
| R9 | App 全部页面 | 见下节"App 的取舍" |
| R10 | 右眼/灯带/舵机/K230 | 固件 M2~M4 排期不变，不进 Demo |

### Won't（明确不做，防范围膨胀）

- 语音助手能力（天气/问答强化）——防助手化是产品红线
- 社交、排行榜、Feed、成长曲线 UI、NFC、IP 联名
- 每条消息 embedding / 重型星历 / 客户端算星盘
- 端侧 LLM、视频流

## 三、App 的取舍：Demo 用 admin 顶替，App 定义需求但后置

现实约束：App 仓零代码，唯一阻塞项是 backend API；admin 已公网上线且 M2/M3 与 App P3/P4 页面高度同源（同一套用户 API）。

**决策：**
- **Demo 的"用户视角"由 admin 承担**（admin 本就是 user+admin 双角色，user 侧页面即未来 App 的桌面版雏形）。
- **App 本阶段只交付两份产物，不写代码**：①V0.2 页面原型（P0~P8 的字段级线框，直接在 `ai-pet-app/docs/` 已有规格上补齐交互与空态）；②技术验证一条：Vue PWA 在 iOS/Android 添加到主屏的可行性 demo（半天工作量，决定技术栈三段式是否成立）。
- App 开工触发条件：backend 的 persona/memories/messages 三组 API 转 200。届时 admin 的 user 侧页面组件可直接迁移（同为 Vue3+Element Plus，设计时已要求组件与后端契约对齐）。

**风险声明**：这个取舍意味着内测用户初期没有手机端。"5 分钟注册→绑设备→定人设"的成功标准在桌面浏览器上验收，手机端 PWA 可用性由技术验证兜底。

## 四、分项目需求清单（给各 AI agent 的任务书摘要）

### 4.1 ai-pet-backend（会话 A）—— Demo 的绝对关键路径

按依赖序，E2/E3 合并为"Demo 冲刺"：

1. **B1/B3/B4 最小人设链**（对应 M1）：KB 种子=水相元素+双鱼差分+4 个 MBTI（INFP/ENFP/INTJ/ESTJ，覆盖盲测对比）；PersonaCompiler 出 7 字段 persona_pack；`GET /internal/devices/{uid}/persona_pack` 转 200。
2. **编译预览端点**（M2）：`POST /api/devices/{id}/persona/preview`，输入未保存的人设草稿，返回编译结果——admin 预览与"盲测口吻"验收都靠它。
3. **C1 旁路写入**（M4）：`POST /internal/chat/events` 落库+脱敏，**最高优先**（每晚一天多丢一天语料）。
4. **C2/E5 记忆最小环**（M5）：worker 候选生成（规则版即可，不上重模型）→ approve/reject → Memory MCP 改 HTTP 传输（会上已倾向，需先更新 docs/05 契约）。
5. **召回数据支持**（M6）：persona_pack 响应里增加 `recent_memories` 字段（最近 3 条 approved），让小智侧一次请求拿全——避免小智多一次 MCP 往返，降低联调面。
6. **E4 messages 最简查询**（M9）：按设备分页，不做搜索。
7. **预留落地**：pet_state 建表（只写 last_interaction_at/bond_level，R1）、messages 加 event_type（R5）、memories 加 speaker_id（R6）。

### 4.2 xiaozhi-server（会话 B）

1. **Epic A+（插队最前，M8）**：S1 关开场白、S2 休息即挂断（提示词/工具编排）、S4 采样率对齐。误唤醒期的体感修复比任何新功能都保"宠物感"。
2. **Epic B（M3+M6）**：会话前拉 persona_pack（含 recent_memories）→ 注入 system prompt；缓存降级（拉失败用上次）；开场按 default_emotion 驱动眼睛。**召回 hook 不做复杂时机策略，Demo 只做"开场注入"一种**。
3. **Epic C 前半（M4）**：旁路写入，失败本地重试队列；不阻断 TTS。
4. **R2 备忘录**：调研上游 WS 是否支持服务端主动下推（`server→device` 主动发起会话），输出半页结论，不实现。
5. 防漂移最小招（配合 Demo 第 3 幕盲测）：在 pack 注入层固定追加 style_constraints 与 taboo，不依赖智控台手工配置。

### 4.3 ai-pet-admin（会话 C / Codex）

1. **M2 人设页（M7）**：星座网格 + MBTI 四维选择 + 忌口 + 钉扎开关；**保存前必须调 preview 端点展示编译结果**（这是 M2 的验收条件，见 02-G1）。
2. **M3 记忆审核页（M7）**：候选列表 + 通过/驳回 + 已通过记忆列表。不做搜索筛选增强。
3. **M3 历史页最简版（M9 支撑）**：按设备分页浏览 + 删除。
4. 设备页从 501 空态转实数（backend E1 已点亮，顺手完成）。
5. 不做：KB 运营界面（M4）、分析卡片、A/B、BI 看板——Demo 后评估。

### 4.4 ESP32 固件（固件会话）

Demo 期间**不排新功能**，只做体验清偿与预留：
1. WakeNet 误唤醒阈值调优（M8，与 S1 配合）。
2. `self.eye.close` 后挂断会话的可选实现（与 S2 双保险，待拍板项——本 PRD 建议做）。
3. R3 预留：结构/E 原理图上为触摸传感器（1~2 路触摸电极）留位，不焊接不实现。
4. R10：右眼/灯带/舵机按原 M2~M4 排期，与 Demo 并行但不进 Demo 验收。
5. OTA 分区减压（资产迁 assets 分区）——这是工程定时炸弹，建议插队。

### 4.5 ai-pet-app（暂不分配开发会话）

1. 补齐 V0.2 页面原型的字段级规格（P0 登录 / P1 首页宠物卡 / P2 绑定 / P3 人设 / P4 记忆 / P5 历史），直接在已有 docs 上修订。
2. PWA 技术验证（可安装性、iOS 推送限制确认），输出 Go/No-Go 结论。
3. 开工触发器：backend persona/memories/messages 转 200。

## 五、验收与度量

**Demo 验收** = 第七节剧本 7 幕一次性跑通 + 第 3 幕盲测（3 个人听两段对话，分辨"星仔"与裸 GLM，正确率 ≥2/3）。

**Demo 后立即开始埋点**（为北极星指标）：每次会话计 `interaction`、每条 approved 记忆计 `recallable`——后端随 E6 worker 顺带做，这是判断"情感陪伴是否成立"的唯一量化依据（对照市场第二月留存断崖）。

**里程碑映射**：V0.2 完成定义修订为 = 本 PRD 的 Must 全部交付 + Demo 剧本验收通过。原 V0.2 中未完成项（KB 补全、D1 日运、admin M4）顺延 V0.3，与 V0.3 的主动触达/养成/共同经历合并规划。

## 六、待拍板事项（需要用户/PM 决策）

| # | 事项 | 建议 |
|---|------|------|
| 1 | 固件 `eye.close` 后挂断（与 S2 双保险） | 做 |
| 2 | Memory MCP 传输改 HTTP | 同意，先改 docs/05 契约 |
| 3 | Demo 用户视角用 admin 顶替 App | 同意（本文第三节） |
| 4 | 触摸传感器进入 M3 硬件计划 | 同意预留位，Demo 后再评估实装 |
| 5 | 域名+ICP 备案启动时点 | Demo 验收后立即启动（上 TLS/WSS、MQTT 前提） |
