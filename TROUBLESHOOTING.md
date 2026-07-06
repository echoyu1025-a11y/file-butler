# 故障排查 / Troubleshooting

[中文](#日志) | [English](#logs)

## 日志

文件管家会写三个滚动日志文件：

- `core.log`（核心：后端选择、模型加载、打包问题先看这个）
- `db.log`（数据库）
- `ui.log`（界面）

### 日志位置

- Windows：`%APPDATA%\AIFileSorter\logs`
- macOS / Linux：`~/.cache/AIFileSorter/logs`（若设置了 `$XDG_CACHE_HOME` 则在其下）

### 日志滚动

每个日志文件到 `5 MiB` 时滚动，保留 `3` 个历史文件——即每类日志最多占用约 `20 MiB`（1 个活动文件 + 3 个历史文件）。

排查后端选择、模型加载或打包问题时，请先看 `core.log`。

---

## Logs

File Butler writes three rotating log files:

- `core.log`
- `db.log`
- `ui.log`

### Log locations

- Windows: `%APPDATA%\AIFileSorter\logs`
- macOS / Linux: `~/.cache/AIFileSorter/logs` (or under `$XDG_CACHE_HOME` if set)

### Log rotation

Each log file rotates at `5 MiB` and keeps `3` rolled files — about `20 MiB` total per stream including the active file.

If you are troubleshooting backend selection, model loading, or packaging issues, start with `core.log`.
