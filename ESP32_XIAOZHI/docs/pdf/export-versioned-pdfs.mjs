/**
 * 将根目录产品 Markdown 导出为带版本号的 PDF，并渲染 Mermaid。
 * 用法（在 docs/pdf 目录）：node export-versioned-pdfs.mjs
 */
import fs from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { marked } from 'marked';
import puppeteer from 'puppeteer';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const ROOT = path.resolve(__dirname, '../..');

const DOCS = [
  'AI_Pet完整业务设计文档.md',
  'AI_Pet赛道定位与功能分层决策.md',
  'AI玩具市场业务逻辑与产品功能分析.md',
  '小智AI_Pet自控服务端-服务器需求分析.md',
];

function extractVersion(md) {
  const m =
    md.match(/>\s*版本：\s*v?([\d.]+)/) ||
    md.match(/版本：\s*v?([\d.]+)/);
  return m ? m[1] : '0.0';
}

/**
 * PDF 版本与 Markdown 页首版本对齐；同版本覆盖写入。
 * 需要新 PDF 版本时：先改 MD 页首版本号再导出（自增由文档维护）。
 */
function resolvePdfVersion(docVersion) {
  return docVersion;
}

function buildHtml(title, bodyHtml) {
  return `<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8" />
  <title>${title}</title>
  <style>
    body {
      font-family: "Segoe UI", "Microsoft YaHei", "PingFang SC", sans-serif;
      font-size: 11pt;
      line-height: 1.55;
      color: #1a1a1a;
      max-width: 900px;
      margin: 0 auto;
      padding: 12px 8px;
    }
    h1, h2, h3, h4 { page-break-after: avoid; }
    table { border-collapse: collapse; width: 100%; margin: 0.8em 0; font-size: 10pt; }
    th, td { border: 1px solid #ccc; padding: 6px 8px; vertical-align: top; }
    th { background: #f5f5f5; }
    code { font-family: Consolas, "Courier New", monospace; font-size: 0.92em; }
    pre { background: #f6f8fa; padding: 10px; overflow: auto; font-size: 9pt; }
    .mermaid { text-align: center; margin: 1em 0; page-break-inside: avoid; }
    .mermaid svg { max-width: 100%; height: auto; }
    blockquote { border-left: 3px solid #ddd; margin: 0.6em 0; padding: 0.2em 0.8em; color: #444; }
    a { color: #0969da; text-decoration: none; }
  </style>
  <script src="https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.min.js"></script>
</head>
<body>
${bodyHtml}
<script>
window.__MERMAID_DONE__ = false;
window.__MERMAID_ERROR__ = null;
(async () => {
  try {
    mermaid.initialize({
      startOnLoad: false,
      securityLevel: 'loose',
      theme: 'default',
      flowchart: { htmlLabels: true, useMaxWidth: true },
    });
    const blocks = [...document.querySelectorAll('code.language-mermaid')];
    for (const [i, code] of blocks.entries()) {
      const source = code.textContent.trim();
      const pre = code.closest('pre') || code;
      const div = document.createElement('div');
      div.className = 'mermaid';
      div.textContent = source;
      pre.replaceWith(div);
    }
    if (document.querySelectorAll('.mermaid').length) {
      await mermaid.run({ querySelector: '.mermaid' });
    }
    window.__MERMAID_DONE__ = true;
  } catch (e) {
    window.__MERMAID_ERROR__ = String(e && e.message ? e.message : e);
    window.__MERMAID_DONE__ = true;
  }
})();
</script>
</body>
</html>`;
}

async function exportOne(browser, fileName) {
  const mdPath = path.join(ROOT, fileName);
  const md = await fs.readFile(mdPath, 'utf8');
  const docVersion = extractVersion(md);
  const base = fileName.replace(/\.md$/i, '');
  const pdfVersion = resolvePdfVersion(docVersion);
  const outName = `${base}_v${pdfVersion}.pdf`;
  const outPath = path.join(ROOT, outName);

  const bodyHtml = marked.parse(md, { gfm: true, breaks: false });
  const html = buildHtml(base, bodyHtml);

  const page = await browser.newPage();
  page.setDefaultTimeout(120_000);
  await page.setContent(html, { waitUntil: 'networkidle0', timeout: 120_000 });
  await page.waitForFunction(() => window.__MERMAID_DONE__ === true, {
    timeout: 90_000,
  });
  const err = await page.evaluate(() => window.__MERMAID_ERROR__);
  if (err) {
    console.warn(`[warn] ${fileName} Mermaid: ${err}`);
  }
  const mermaidCount = await page.evaluate(
    () => document.querySelectorAll('.mermaid svg').length,
  );
  const blockCount = await page.evaluate(
    () => document.querySelectorAll('.mermaid').length,
  );
  if (blockCount && mermaidCount < blockCount) {
    console.warn(
      `[warn] ${fileName}: mermaid blocks=${blockCount}, svg ok=${mermaidCount}`,
    );
  }

  await page.pdf({
    path: outPath,
    format: 'A4',
    printBackground: true,
    margin: { top: '14mm', right: '12mm', bottom: '14mm', left: '12mm' },
  });
  await page.close();

  // 同步无版本后缀的「最新」副本（若未锁定）
  const latestPath = path.join(ROOT, `${base}.pdf`);
  try {
    await fs.copyFile(outPath, latestPath);
  } catch (e) {
    console.warn(`[warn] 无法覆盖 ${base}.pdf（可能被占用）: ${e.message}`);
    const alt = path.join(ROOT, `${base}_最新.pdf`);
    await fs.copyFile(outPath, alt);
    console.warn(`[info] 已写入 ${path.basename(alt)}`);
  }

  return { fileName, docVersion, pdfVersion, outName, mermaidCount };
}

async function main() {
  console.log('ROOT =', ROOT);
  const browser = await puppeteer.launch({
    headless: true,
    args: ['--no-sandbox', '--disable-setuid-sandbox'],
  });
  const results = [];
  try {
    for (const f of DOCS) {
      console.log('Exporting', f, '...');
      const r = await exportOne(browser, f);
      results.push(r);
      console.log(
        `  -> ${r.outName} (doc v${r.docVersion}, pdf v${r.pdfVersion}, mermaid svg=${r.mermaidCount})`,
      );
    }
  } finally {
    await browser.close();
  }
  console.log('\nDone:');
  for (const r of results) {
    console.log(`- ${r.outName}`);
  }
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});
