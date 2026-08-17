#!/usr/bin/env python3
"""AI Pet 服务器的只读状态采集器；只使用 Python 标准库。"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import tempfile
import urllib.error
import urllib.request
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


def run_command(arguments: list[str]) -> tuple[bool, str]:
    """执行固定命令；不使用 shell，避免外部输入变成命令。"""
    try:
        result = subprocess.run(
            arguments,
            check=False,
            capture_output=True,
            text=True,
            timeout=10,
        )
    except (OSError, subprocess.TimeoutExpired) as error:
        return False, type(error).__name__
    if result.returncode != 0:
        return False, f"exit_{result.returncode}"
    return True, result.stdout


def read_memory() -> dict[str, int] | None:
    try:
        values = {}
        for line in Path("/proc/meminfo").read_text(encoding="utf-8").splitlines():
            key, value = line.split(":", 1)
            values[key] = int(value.strip().split()[0]) * 1024
        return {
            "total_bytes": values["MemTotal"],
            "available_bytes": values["MemAvailable"],
            "swap_total_bytes": values.get("SwapTotal", 0),
            "swap_free_bytes": values.get("SwapFree", 0),
        }
    except (FileNotFoundError, KeyError, ValueError):
        return None


def collect_host(thresholds: dict[str, Any]) -> dict[str, Any]:
    usage = shutil.disk_usage("/")
    used_percent = round(usage.used / usage.total * 100, 2)
    level = "ok"
    if used_percent >= thresholds["root_disk_used_percent_critical"]:
        level = "critical"
    elif used_percent >= thresholds["root_disk_used_percent_warning"]:
        level = "warning"
    try:
        if not hasattr(os, "getloadavg"):
            raise OSError("load average is unavailable")
        load_1m, load_5m, load_15m = os.getloadavg()
        load = {"1m": load_1m, "5m": load_5m, "15m": load_15m}
    except OSError:
        load = None
    return {
        "status": level,
        "cpu_count": os.cpu_count(),
        "load": load,
        "memory": read_memory(),
        "root_disk": {
            "total_bytes": usage.total,
            "used_bytes": usage.used,
            "free_bytes": usage.free,
            "used_percent": used_percent,
        },
    }


def collect_containers(expected_names: list[str]) -> list[dict[str, Any]]:
    results = []
    for name in expected_names:
        ok, output = run_command(["docker", "inspect", "--format", "{{json .}}", name])
        if not ok:
            results.append({"name": name, "status": "critical", "reason": "not_found_or_unavailable"})
            continue
        try:
            inspection = json.loads(output)
            state = inspection["State"]
        except json.JSONDecodeError:
            results.append({"name": name, "status": "critical", "reason": "invalid_docker_state"})
            continue
        status = "ok" if state.get("Running") else "critical"
        if state.get("OOMKilled"):
            status = "critical"
        results.append(
            {
                "name": name,
                "status": status,
                "running": bool(state.get("Running")),
                "restarts": int(inspection.get("RestartCount") or 0),
                "started_at": state.get("StartedAt"),
                "exit_code": state.get("ExitCode"),
                "oom_killed": bool(state.get("OOMKilled")),
                "health": (state.get("Health") or {}).get("Status", "not_configured"),
            }
        )
    return results


def check_http(item: dict[str, Any], timeout: int) -> dict[str, Any]:
    request = urllib.request.Request(item["url"], method="GET")
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            actual = response.status
    except (urllib.error.URLError, TimeoutError, ValueError) as error:
        return {"name": item["name"], "status": "critical", "reason": type(error).__name__}
    expected = item["expected_status"]
    return {
        "name": item["name"],
        "status": "ok" if actual == expected else "critical",
        "expected_status": expected,
        "actual_status": actual,
    }


def write_json_atomically(output_path: Path, payload: dict[str, Any]) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(
        mode="w", encoding="utf-8", dir=output_path.parent, delete=False
    ) as temporary:
        json.dump(payload, temporary, ensure_ascii=False, indent=2, sort_keys=True)
        temporary.write("\n")
        temporary_path = Path(temporary.name)
    temporary_path.replace(output_path)


def main() -> int:
    parser = argparse.ArgumentParser(description="采集 AI Pet 服务器只读状态")
    parser.add_argument("--config", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    arguments = parser.parse_args()
    config = json.loads(arguments.config.read_text(encoding="utf-8"))
    thresholds = config["thresholds"]
    containers = collect_containers(config["expected_containers"])
    http_checks = [
        check_http(item, int(thresholds["http_timeout_seconds"]))
        for item in config["http_checks"]
    ]
    host = collect_host(thresholds)
    failures = [item for item in containers + http_checks if item["status"] == "critical"]
    payload = {
        "schema_version": 1,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "host": host,
        "containers": containers,
        "http_checks": http_checks,
        "summary": {"status": "critical" if failures else host["status"], "critical_count": len(failures)},
    }
    write_json_atomically(arguments.output, payload)
    return 2 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
