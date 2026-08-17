# 产品文档 PDF 导出（含 Mermaid）

## 导出最新带版本号 PDF

```powershell
cd D:\Home_Work\ESP32_XIAOZHI\docs\pdf
npm install
node export-versioned-pdfs.mjs
```

输出到仓库根目录，命名规则：

`{文档名}_v{版本}.pdf`

版本逻辑：

1. 读取 Markdown 页首 `版本：vX.Y`（改文档时请先自增此版本号）
2. 生成 `{文档名}_vX.Y.pdf`（同版本覆盖）
3. 同时尝试覆盖无后缀的 `{文档名}.pdf` 作为「最新」；若文件被占用则写 `{文档名}_最新.pdf`

## 说明

- 标准 `npx md-to-pdf` **不会等待** Mermaid 异步渲染，请用本目录脚本。
- 需要能访问 `cdn.jsdelivr.net` 加载 Mermaid 10。
