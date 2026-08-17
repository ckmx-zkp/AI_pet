# AGENTS.md — ai-pet-ops（服务器运维与状态检测）

> 本仓负责 AI Pet 服务器的**只读状态采集、健康判定和告警编排**。进入本仓前先读 `D:/Home_Work/AGENTS.md`，并按根规则安全检查/同步 `work_dashboard`；再读本文件和 `docs/00*`。

## 职责与边界

- 可做：主机资源、Docker 容器、HTTP 健康、备份年龄、证书到期、部署版本的采集与展示；状态变化告警。
- 禁止：自动重启容器、自动执行部署/迁移、读取或输出 `.env`、密钥、数据库业务数据、对话内容。
- 采集器只调用固定白名单命令和固定 loopback HTTP 地址；不得让 LLM 或外部输入拼接 shell 命令。
- 本仓不替代业务后端的 agent-worker，也不承担人设、记忆或内容生产任务。

## 目录

- `collector/collect_status.py`：零第三方依赖的服务器状态采集器。
- `config/services.example.json`：容器与 HTTP 探测的脱敏示例配置。
- `deploy/`：systemd service/timer 模板，只部署采集器，不开放端口。
- `docs/`：边界、指标和部署手册。

## 验证

```powershell
python -m py_compile collector/collect_status.py
python collector/collect_status.py --config config/services.example.json --output work/status.json
```

本地没有 Docker 或对应 loopback 服务时，HTTP/Docker 探测失败是预期结果；验证重点是 JSON 能生成、失败项可解释且不泄露敏感信息。服务器部署后再以 systemd 定时结果验收。

## 收工义务

- 采集项、阈值或告警语义变化先更新 `docs/01-监测指标与告警规范.md`，再改代码。
- 涉及服务器部署时先更新 `docs/02-部署与回滚手册.md`；不得直接修改现有业务服务。
- 正式部署或跨仓状态变化完成后，按根规则更新协作看板。
