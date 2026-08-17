# ai-pet-ops

AI Pet 阿里云服务器的轻量运维与状态检测项目。V0 是一个由 systemd 定时运行的只读采集器：它写出脱敏 JSON 快照，供后续状态页或告警通道消费。

## 为什么独立建仓

它跨越 backend、admin、app、xiaozhi-server 与看板，但不属于任一业务仓。独立仓可让运维规则、部署清单和告警演进独立审查，避免把 Docker 主机权限混入业务应用。

## V0 能力

- 主机：负载、内存、根分区磁盘。
- Docker：预期容器是否运行、重启次数、健康状态、启动时间、OOM 标记。
- HTTP：仅探测配置中指定的 `127.0.0.1` 健康地址。
- 输出：原子写入 JSON 快照；任何失败均记录为状态项，不输出环境变量或响应正文。

详细约定见 [docs/01-监测指标与告警规范.md](docs/01-监测指标与告警规范.md)，服务器安装步骤见 [docs/02-部署与回滚手册.md](docs/02-部署与回滚手册.md)。

## 本地验证

```powershell
python -m py_compile collector/collect_status.py
python collector/collect_status.py --config config/services.example.json --output work/status.json
```

## 部署原则

- 不开放新公网端口。
- 不挂载 Docker Socket 给 Web 容器；采集动作仅由受控 systemd 单元运行。
- 不自动修复。V0 只检测、留痕和告警，任何重启或部署由人工确认执行。
