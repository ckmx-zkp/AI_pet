# AI Pet 工作区总仓

GitHub：https://github.com/ckmx-zkp/AI_pet

本仓是 `D:\Home_Work` 的多设备入口。各子项目仍是**独立 Git 仓库**，CI 和服务器部署继续拉各自的远端，不会因为推这个总仓而改变。

| 本仓里的路径 | 远端 | 说明 |
|---|---|---|
| `ai-pet-backend/` | `ckmx-zkp/ai-pet-backend` | 业务后端；CI 跑测试 |
| `xiaozhi-server/` | `ckmx-zkp/aipet-xiaozhi-server-` | 实时语音后台 |
| `ai-pet-admin/` | `ckmx-zkp/ai-pet-admin` | 管理台 |
| `ai-pet-app/` | `ckmx-zkp/ai-pet-app-` | 用户端 |
| `work_dashboard/` | `ckmx-zkp/work_dashboard` | 协作看板 |
| `prototype/` | `ckmx-zkp/AIPET_prototype` | 原型 |
| `ESP32_XIAOZHI/xiaozhi-esp32/` | `ckmx-zkp/Tboy_P4_xiaozhi` | 固件；CI 编板 |
| 其余目录（`docs/`、`hardware/`、`ai-pet-ops/`、母文档等） | 只在本仓 | 没有独立远端 |

## 另一台电脑第一次拿代码

```bash
git clone --recurse-submodules https://github.com/ckmx-zkp/AI_pet.git Home_Work
cd Home_Work
```

如果克隆时忘了 `--recurse-submodules`：

```bash
git submodule update --init --recursive
```

Windows 也可以直接跑 `.\scripts\sync-workspace.ps1`。

依赖不进 Git。各仓按自己的 `AGENTS.md` 装：

- 前端：`cd ai-pet-admin` 或 `ai-pet-app`，再 `npm install`
- 后端：`cd ai-pet-backend`，再建 venv、装依赖
- 密钥：各仓复制 `.env.example` 为本地 `.env`，或按看板「部署环境」在服务器上看位置。不要把生产密钥拷进仓库。

## 日常怎么改、怎么推

代码改在**子仓库**里，推到**那个子仓库自己的 origin**。这样另一台电脑、CI、ECS 部署才会看到。

```bash
cd ai-pet-backend          # 或 admin / app / xiaozhi-server / 固件
git pull --ff-only
# ... 改代码 ...
git add ...
git commit -m "..."
git push origin main
```

总仓只记「现在各子仓指到哪一次提交」，以及没有独立远端的资料。子仓 push 之后，如需让总仓指针跟上：

```bash
cd Home_Work
git add ai-pet-backend     # 换成你刚推过的子目录
git commit -m "chore: bump ai-pet-backend"
git push origin main
```

不要把某个子仓的 `origin` 改成 `AI_pet`，也不要在总仓里对子仓做 force-push。

开工仍先读根目录 `AGENTS.md` 和 `work_dashboard/` 里的看板。
