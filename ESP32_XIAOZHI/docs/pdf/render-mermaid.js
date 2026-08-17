// 供 md-to-pdf --config-file 备用；正式导出请用 export-versioned-pdfs.mjs（会 wait）。
(() => {
  const MERMAID_CDN =
    'https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.min.js';

  const loadMermaid = () =>
    new Promise((resolve, reject) => {
      if (window.mermaid) {
        resolve(window.mermaid);
        return;
      }
      const script = document.createElement('script');
      script.src = MERMAID_CDN;
      script.async = true;
      script.onload = () => resolve(window.mermaid);
      script.onerror = () =>
        reject(new Error(`无法从 CDN 加载 Mermaid：${MERMAID_CDN}`));
      document.head.appendChild(script);
    });

  window.renderMermaidForPdf = async () => {
    const mermaid = await loadMermaid();
    mermaid.initialize({
      startOnLoad: false,
      securityLevel: 'loose',
      theme: 'default',
      flowchart: { htmlLabels: true, useMaxWidth: true },
    });
    const blocks = [...document.querySelectorAll('code.language-mermaid')];
    for (const code of blocks) {
      const source = code.textContent.trim();
      const container = code.closest('pre') || code;
      const target = document.createElement('div');
      target.className = 'mermaid';
      target.textContent = source;
      container.replaceWith(target);
    }
    await mermaid.run({ querySelector: '.mermaid' });
  };

  // 尽力触发（CLI 可能在完成前截 PDF）
  window.renderMermaidForPdf().catch((e) => {
    console.error(e);
  });
})();
