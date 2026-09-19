# AI Pet 平台访问与本地运行操作指南

本文档汇总了 **APP（用户端）**、**智控台（小智硬件/语音控制台）**、**后端管理平台（业务管理台）** 三大平台的职责定位、**线上直接访问地址** 以及 **本地开发启动方式**。

---

## 平台速查一览表

| 平台名称 | 对应项目目录 | 服务对象与核心功能 | 线上即开即用地址（推荐） | 本地开发启动命令 | 默认端口 | 登录与账号说明 |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **APP (用户端)** | `ai-pet-app/` | **普通用户 / 主人**<br>• 设备绑定认领（基于 Binding ID）<br>• 人设设置（星座×MBTI、星仔档案）<br>• 记忆管理与候选审核<br>• 日运 / 服务端小记<br>• 27 类结构化相处关系<br>• 对话脱敏历史浏览<br>• 眼睛外设状态查看 | [http://39.107.143.71:8081/](http://39.107.143.71:8081/) | `cd ai-pet-app`<br>`npm run dev` | 5173 | 页面自带「注册 / 登录」切换；支持自主注册账号直接使用 |
| **智控台 (小智管理控制台)** | `xiaozhi-server/` (`manager-web`) | **硬件与基础设施运维人员**<br>• ESP32 硬件设备注册与连接管理<br>• OTA 固件更新与版本下发<br>• ASR / LLM / TTS 模型参数与 API 密钥管理<br>• 智能体 Prompt 运行时管理 | [http://39.107.143.71:8002/](http://39.107.143.71:8002/) | `cd xiaozhi-server/xiaozhi-esp32-server/main/manager-web`<br>`npm run serve` | 8001 / 8002 | 默认管理员账号密码通常为 `admin` / `123456`（页面需输入图形验证码） |
| **后端管理平台 (业务管理台)** | `ai-pet-admin/` | **开发者 / 业务运营人员**<br>• 设备资产清册与诊断（MAC/SN 查 Binding ID）<br>• 绑定码轮换管理<br>• 记忆候选人审<br>• 知识库（KB）分级版本运营与发布<br>• 运营指标监控<br>• 顶栏外链直通小智智控台 | [http://39.107.143.71:8080/](http://39.107.143.71:8080/) | `cd ai-pet-admin`<br>`npm run dev` | 5173 / 5174 | 页面自带「注册 / 登录」；普通用户注册后可查看自身设备，管理员可查看 KB 与运营指标 |

---

## 方式一：线上即开即用（最快捷，无需本地配置依赖）

阿里云 ECS 服务器（`39.107.143.71`）上已部署全套容器化服务，**无需在本地安装 Node.js 或启动后端，直接用浏览器打开对应网址即可使用**：

### 1. 打开用户端 APP
- **访问地址**：[http://39.107.143.71:8081/](http://39.107.143.71:8081/)
- **手机安装为 PWA（推荐）**：
  - **iOS (Safari)**：打开网址后，点击底部分享按钮 -> 选择「添加到主屏幕」。
  - **Android (Chrome / 常见浏览器)**：打开网址后，点击右上角菜单 -> 选择「安装应用」或「添加到主屏幕」。
  - 安装后可在手机主屏以全屏无地址栏的形式体验，体验媲美原生 App。
- **注册与登录**：
  - 首次打开若未登录会自动跳转至 `/login` 页面。
  - 点击「注册」，输入 3~64 位账号与 8 位以上密码即可注册并登录。
  - 登录后进入首页，可输入固件/设备生成的 `binding_id` 认领绑定设备。

### 2. 打开后端管理平台（ai-pet-admin）
- **访问地址**：[http://39.107.143.71:8080/](http://39.107.143.71:8080/)
- **使用说明**：
  - 首次进入同样展示登录页，可直接点击「注册」新建运营/开发者账号，也可以用已有管理员账号登录。
  - 登录成功后进入管理台，左侧导航栏提供：**设备管理、人设设置、对话历史、记忆管理、分析、外设状态、运势核对、知识库、运营指标**。
  - 顶栏右上角提供了 **「小智智控台」** 的快捷超链接，点击即可一键新窗口打开智控台。

### 3. 打开智控台（xiaozhi-server 控制台）
- **访问地址**：[http://39.107.143.71:8002/](http://39.107.143.71:8002/)
- **使用说明**：
  - 小智的设备管理与固件 OTA 管理系统。
  - 登录凭据：默认管理员用户名为 `admin`，密码为 `123456`（若线上已修改密码，请参考服务器配置记录）。
  - 在智控台中可进行：模型供应器配置（豆包、MiniMax、火山 TTS 等）、固件包版本上传、设备在线列表查看等。

---

## 方式二：在本地开发环境下启动与运行

如果需要进行前端页面的修改、二次开发与本地调试，请按以下步骤在本地启动前端服务。

### 环境准备要求
- **Node.js**：推荐 v18.x 或 v20.x
- **包管理器**：`npm` 或 `pnpm`
- （可选）**SSH 客户端**：用于本地调通线上后端的 8010 接口

---

### 1. 本地启动用户端 APP (`ai-pet-app`)

1. 打开终端（PowerShell 或 CMD），进入项目目录：
   ```powershell
   cd d:\Home_Work\ai-pet-app
   ```
2. 安装依赖：
   ```powershell
   npm install
   ```
3. 启动开发服务器：
   ```powershell
   npm run dev
   ```
4. 终端会输出本地预览地址，通常为：
   ```text
   ➜  Local:   http://localhost:5173/
   ```
5. **本地开发时连接后端 API 说明**：
   - 本地前端调用的 API 基准路径由 `VITE_API_BASE` 决定（缺省为 `/api`）。
   - **方式 A（通过 SSH 隧道直连线上后端，最推荐）**：
     在另一个终端窗口开启 SSH 端口转发：
     ```powershell
     ssh -L 8010:127.0.0.1:8010 aliyun-aipet
     ```
     并在 `ai-pet-app/vite.config.ts` 的 `defineConfig` 中添加本地代理规则：
     ```ts
     server: {
       proxy: {
         '/api': {
           target: 'http://localhost:8010',
           changeOrigin: true
         }
       }
     }
     ```
   - **方式 B（直接打到线上内测反代入口）**：
     在 `ai-pet-app` 根目录新建 `.env.development`：
     ```properties
     VITE_API_BASE=http://39.107.143.71:8081/api
     ```

---

### 2. 本地启动后端管理平台 (`ai-pet-admin`)

1. 进入管理台目录：
   ```powershell
   cd d:\Home_Work\ai-pet-admin
   ```
2. 安装依赖：
   ```powershell
   npm install
   ```
3. 启动本地开发服务：
   ```powershell
   npm run dev
   ```
4. 访问地址：
   - 默认端口也是 5173；若 `ai-pet-app` 已占用，Vite 会自动切换到 `http://localhost:5174/`。
5. **后端代理说明**：
   - `ai-pet-admin/vite.config.ts` 中**已经内置**了如下反向代理：
     ```ts
     server: {
       proxy: {
         '/api': {
           target: 'http://localhost:8010',
           changeOrigin: true,
         },
       },
     }
     ```
   - 只需运行以下 SSH 隧道命令，即可让本地管理台直接读写线上真实环境数据：
     ```powershell
     ssh -L 8010:127.0.0.1:8010 aliyun-aipet
     ```

---

### 3. 本地启动小智智控台前端 (`manager-web`)

小智智控台的前端工程位于 `xiaozhi-server` 子目录中：

1. 进入 `manager-web` 目录：
   ```powershell
   cd d:\Home_Work\xiaozhi-server\xiaozhi-esp32-server\main\manager-web
   ```
2. 安装依赖：
   ```powershell
   npm install
   ```
3. 启动本地服务：
   ```powershell
   npm run serve
   ```
4. 启动完成后，在浏览器中打开：
   ```text
   http://localhost:8001/
   ```
> **提示**：小智智控台完整工作依赖 `manager-api` (Java Spring Boot 8002 端口)、MySQL 8.0 以及 Redis。因此**日常查看和配置强烈建议直接访问线上智控台**（`http://39.107.143.71:8002/`），省去本地配置 Java 与数据库环境的繁琐工作。

---

## 常见操作疑问解答（FAQ）

### Q1：为什么在 APP 里不能直接用设备 MAC 地址绑定？
**答**：系统实施了严格的「设备身份分离安全红线」：
- 设备的 MAC 地址 / SN 只在底层硬件与小智后台之间传输。
- 用户在 APP 端绑定设备时，输入的是设备激活首见时动态生成的 **`binding_id`**（绑定码），从而避免设备 MAC 被他人恶意扫码占有。
- 运营/开发者如果需要排查某台硬件的绑定码，可以在 **后端管理平台（8080）** 的「设备管理」中通过 MAC/SN 进行检索诊断。

### Q2：修改前端代码后，如何部署到线上？
依据各前端仓库的 `AGENTS.md` 规范：
- **用户端 APP**：
  ```powershell
  cd ai-pet-app
  npm run build
  # 构建产物位于 dist/，通过 SSH 同步至 ECS /opt/ai-pet/ai-pet-app/dist/
  ```
- **管理平台 Admin**：
  ```powershell
  cd ai-pet-admin
  npm run build
  # 构建产物位于 dist/client/，同步至 ECS /opt/ai-pet/ai-pet-admin/dist/
  ```
