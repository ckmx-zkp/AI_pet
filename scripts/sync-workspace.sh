#!/usr/bin/env bash
# 在另一台电脑初始化或更新整个 Home_Work 工作区。
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"

if [[ ! -d .git ]]; then
  echo "当前目录不是 git 仓库。请先: git clone --recurse-submodules https://github.com/ckmx-zkp/AI_pet.git" >&2
  exit 1
fi

echo "== 更新总仓 =="
git pull --ff-only

echo "== 拉取子模块（各独立仓） =="
git submodule update --init --recursive
git submodule foreach --recursive 'git checkout main && git pull --ff-only'

echo
echo "完成。接下来在各子仓按 AGENTS.md 安装依赖（npm install / venv）。"
echo "改代码请进入子仓提交并 push 到该仓自己的 origin，不要改 origin 指向 AI_pet。"
