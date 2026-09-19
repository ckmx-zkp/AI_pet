# P1 眼睛、RGB 与对话联动

目标：主人说一句带情绪的话，**同一轮**眼睛和灯一起变，而不是只眨得快一点。

## 现状缺口

| 机制 | 现在 | 问题 |
|------|------|------|
| `self.eye.set_emotion` | 换 C1 状态图 | 不改灯 |
| `self.led.set_emotion` | 换两颗 WS2812 色相 | LLM 经常忘了调 |
| `llm.emotion` JSON | 给对话屏 | S3 `NoDisplay`，双眼忽略 |
| `kDeviceStateSpeaking` | 加快自动眨眼 | 不读文本、不改表情 |

`led_mood_controller.h` 注释写「眼睛 MCP 也会调，跟着变色」，`ApplyEmotion` 实际只被灯自己的 MCP 调用。

## 固件要改

2026-09-20 已在 `esp32-s3-usb-cam` 落地（未烧录）。记忆仍在云端：不注册 `self.memory.*`。

1. `EyeController::set_emotion` 成功后调用 `LedMoodController::ApplyEmotion`（同一份 canonical：neutral/happy/angry/sad/joy）。✅
2. `GetDisplay()` 继续 `NoDisplay`，不要把聊天 UI 画到眼珠上。✅ 未改
3. `self.eye.set_blink_profile(interval_ms 800–8000)` 写入 `DualEyeDisplay`；说话时仍按间隔 ×0.45 加快。`duration_ms` 未用。✅
4. listening：左右轻漂视线；speaking 停漂；MCP `look` 后约 8s 不抢。✅
5. idle 约 45s 眼睛回到 neutral（灯暂不跟回，等下一次 `set_emotion`）；休息继续走 `self.eye.close`。✅ 眼 / ⚠ 灯

云端 `persona_pack.default_emotion` 已映射 `self_eye_set_emotion`，烧录后会同时改脸和灯。`blink_profile` 要等小智真调新工具。

不要在固件里做中文分词或「听到开心就变脸」。语义判断留给 LLM。

## 小智侧要改（可不改契约）

- Prompt / 工具说明：换情必须 **同轮** 调 `self.eye.set_emotion` 和 `self.led.set_emotion`。
- b12 已支持批量 `function_calls`，优先走批量，避免一眼一灯分两轮。
- 连接建立后应用 `persona_pack.default_emotion`（b4 起就有映射，在 S3 大板上核一次是否真的打到双眼和灯）。

## 表情与灯色对照（已在固件里）

| 表情 | 灯的大致色相 |
|------|----------------|
| neutral | 冷青 |
| happy | 暖黄 |
| angry | 红 |
| sad | 蓝紫 |
| joy | 品红 |

星座色走 `self.led.set_zodiac`，和情绪互斥（现有实现会清掉另一方）。

## 验收句

- 「你看起来开心一点」→ 双眼 happy + 灯偏黄，同一次回复内完成。✅ 2026-09-20 COM28（话术为「做一个开心一点的表情」；难过/生气同样同轮）
- 只说「闭眼休息」→ 闭眼，会话按服务端 S2 断开（依赖 `self.eye.close` 成功）。✅ 日志「好了，休息吧」→ `closed=1` +「晚安，先休息啦。」+ WS 断开；再唤醒未自动睁眼
- 说话过程中眨眼明显比静默勤，灯不闪成 listening 状态灯（氛围灯继续不接 `GetLed()`）。未单独立项
