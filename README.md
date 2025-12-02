🌙 Novel Book · 小说阅读器

轻量、干净、流畅的本地/在线小说阅读器，基于 Qt 5.14.2 (MinGW) 构建。

<p align="center"> <img src="https://img.shields.io/badge/Qt-5.14.2-41cd52.svg" /> <img src="https://img.shields.io/badge/C%2B%2B-17-00599C.svg"/> <img src="https://img.shields.io/badge/Platform-Windows-0078D6.svg"/> <img src="https://img.shields.io/badge/License-MIT-blue.svg"/> </p> <p align="center"> <b>专注本地阅读体验，也支持在线 API 自动更新域名。</b> </p>
📸 截图预览
<p align="center"> <img src="./images/main_window.png" width="70%" alt="主界面"/> </p> <p align="center"> <img src="./images/novel_window.png" width="70%" alt="小说阅读界面"/> </p>
✨ 功能亮点
🖥 主界面

加载本地 TXT 小说

自动解析章节目录

在线 API 域名自动检测（智能切换可用域名）

最近阅读记录

双击直接打开小说阅读页

📖 阅读器界面

自动保存阅读进度（精确到行）

字体大小 / 行距调节

白天 / 夜间模式

自动滚动阅读（F5 开关）

隐藏/显示阅读窗口（F4）

ESC 关闭阅读器

页面平滑滚动

🛠 技术实现

C++17

Qt Widgets（QMake 项目）

QTextBrowser 展示文本内容

QSettings 保存用户配置

QNetworkAccessManager 检测可用 API 域名

Qt 样式表（QSS）实现界面美化

📂 目录结构
Novel_Book/
├── main.cpp
├── mainwindow.h/.cpp
├── novelwindow.h/.cpp
├── VersionDetector.h/.cpp
├── images/
│   ├── main_window.png
│   └── novel_window.png
├── resources.qrc
├── settings.ini
├── api.ini
└── Novel_Book.pro

🚀 编译方式

使用 Qt Creator 打开项目

套件选择：Qt 5.14.2 MinGW 64-bit

编译并运行

📦 打包发布（Windows）

在 CMD 中执行：

"C:\MyTool\Qt\5.14.2\mingw73_64\bin\windeployqt.exe" --release "你的exe路径"


确保最终目录包含：

Novel_Book.exe

Qt5Core.dll / Qt5Gui.dll / Qt5Widgets.dll / Qt5Network.dll

platforms/qwindows.dll

资源文件（图片、配置文件）

⚙ 配置文件
settings.ini

存储字体、主题、滚动速度、窗口位置等。

api.ini
[API]
base=https://xxxxx/api


由 VersionDetector 自动更新。

📜 更新日志
v1.0.0

基本阅读器功能完成

自动域名检测

自动滚动

多主题切换

TXT 解析与章节跳转

⭐ 支持一下？

如果这个项目对你有用，可以点亮一个星星 ⭐，对开发者是很大的鼓励！
