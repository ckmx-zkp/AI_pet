# 在另一台电脑初始化或更新整个 Home_Work 工作区。
# 用法：在仓库根目录执行  .\scripts\sync-workspace.ps1

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

if (-not (Test-Path (Join-Path $root ".git"))) {
    Write-Error "当前目录不是 git 仓库。请先: git clone --recurse-submodules https://github.com/ckmx-zkp/AI_pet.git"
}

Write-Host "== 更新总仓 =="
git pull --ff-only

Write-Host "== 拉取子模块（各独立仓） =="
git submodule update --init --recursive
git submodule foreach --recursive "git checkout main; git pull --ff-only"

Write-Host ""
Write-Host "完成。接下来在各子仓按 AGENTS.md 安装依赖（npm install / venv）。"
Write-Host "改代码请进入子仓提交并 push 到该仓自己的 origin，不要改 origin 指向 AI_pet。"
