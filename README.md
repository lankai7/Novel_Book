# 📖 Novel Book — Qt 小说在线阅读器

![License](https://img.shields.io/badge/License-MIT-green.svg)
![Qt](https://img.shields.io/badge/Qt-5.14.2-blue.svg)
![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-512BD4.svg)
![Build](https://img.shields.io/badge/Build-qmake-brightgreen.svg)

一个基于 **Qt Widgets（Qt 5.14.2 + MinGW）** 开发的  
**轻量级小说在线阅读器 / 本地阅读器**，界面简洁、功能实用、易于扩展。

---

## 🖼️ 项目预览

### 🏠 主界面
![小说](https://github.com/lankai7/Novel_Book/blob/main/res/home.png)

---

### 📚 小说阅读界面
![小说](https://github.com/lankai7/Novel_Book/blob/main/res/novel.png)

---

## ✨ 功能特性

- 🌐 **在线小说阅读（API 获取）**
- 📜 **自动滚动阅读（支持快捷键）**
- 💾 **阅读进度自动保存**
- 🎨 **字体 / 字号 / 行距 / 背景可调**
- 📊 **阅读进度显示**
- ⚡ **启动快、无多余依赖**
- 🪟 **窗口状态与布局自动记忆**

---

## ⌨️ 快捷键说明

| 快捷键 | 功能 |
|------|------|
| **F5** | 自动滚动 开 / 关 |
| **Ctrl + ↑** | 增大字体 |
| **Ctrl + ↓** | 减小字体 |
| **PageUp** | 上一页 |
| **PageDown** | 下一页 |
| **Esc** | 退出阅读 |

---

## 🚀 使用方式

1. 启动程序
2. 选择 **在线小说** 或 **本地 TXT 文件**
3. 进入阅读界面后：
   - 可使用快捷键调整阅读体验
   - 可开启自动滚动解放双手
4. 程序会自动保存：
   - 阅读位置
   - 字体与界面设置
   - 上次阅读内容

---

## 🛠️ 技术栈

- **Qt 5.14.2 (MinGW 7.3.0)**
- **Qt Widgets**
- **QNetworkAccessManager（在线请求）**
- **QTextBrowser**
- **QSettings（配置保存）**
- **C++11**
- **qmake 项目结构**

---

## 📦 Windows 打包

使用 Qt 自带工具：

```bash
windeployqt --release Novel_Book.exe
确保生成以下目录：

复制代码
platforms/
└── qwindows.dll
否则程序无法启动。

📄 License
本项目采用 MIT License，仅用于学习与交流。

🧠 项目说明
该项目是一个 纯 Qt Widgets 实现的小说阅读器，
未依赖第三方 UI 框架，结构清晰，适合：

Qt 初学者进阶

桌面阅读工具开发

网络 + 本地内容整合练习

后续可扩展方向包括：

章节目录解析

EPUB 支持

夜间模式

书架管理

云端同步
