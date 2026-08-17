/**
 * 备用配置：标准 md-to-pdf CLI 不会等待异步 Mermaid。
 * 请优先使用：node docs/pdf/export-versioned-pdfs.mjs
 */
const path = require('node:path');

module.exports = {
  pdf_options: {
    format: 'A4',
    margin: {
      top: '16mm',
      right: '14mm',
      bottom: '16mm',
      left: '14mm',
    },
    printBackground: true,
  },
  script: [path.join(__dirname, 'render-mermaid.js')],
};
