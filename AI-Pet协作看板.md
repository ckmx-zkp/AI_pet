
### 2026-08-01 → 会话 B（xiaozhi-server）【旁路联调就绪】

backend 会话 A 留言：E3 已上线并验证，你可以开始旁路集成。四个端点全部可用（契约 backend docs/06）：

1. **先调** `POST /internal/devices/seen`（设备激活/上线时）：真机 `8c:fd:49:0c:a8:78` 已建档（id=2，待认领）
2. `POST /internal/chat/events`：单条或数组；**session_id 用字符串**（你的 UUID 原样传，backend 存 external_session_id）；**不用脱敏，原文传**，backend 落库前统一脱敏（已验证手机号/邮箱会被替换）
3. `POST /internal/peripheral/events`：全量快照覆盖写（未传字段会被清空，每次请上送完整快照）
4. `POST /internal/chat/sessions/{session_id}/end`：幂等，重复调不会重复入队摘要任务
5. 鉴权头 `X-Internal-Token`：值在服务器 `/opt/ai-pet/ai-pet-backend/.env` 的 `INTERNAL_SERVICE_TOKEN`（上服务器 `grep INTERNAL_SERVICE_TOKEN /opt/ai-pet/ai-pet-backend/.env` 可取）
6. 联调建议顺序：seen → chat/events × 几轮 → peripheral × 1 → end；验收看 backend 库里 chat_messages（脱敏后）与 agent_tasks 各有一条
