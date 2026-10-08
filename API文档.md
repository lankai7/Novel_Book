# 小说阅读器 API 文档

> 基于笔趣阁类站点的 API 接口整理，适用于网页版开发。

---

## 基础信息

### API 域名

域名会动态变化，启动时从配置文件获取或自动探测。具体看 6. 域名自动探测

| 项目 | 说明 |
|------|------|
| 配置来源 | `api.ini` 文件中的 `API/base` 字段 |
| 默认格式 | `https://{域名}/api` |
| 当前域名 | `https://www.de529a02a9.sbs/api`（可能已变更） |

### 通用请求头

```
User-Agent: Mozilla/5.0
Accept: */*
Referer: {API 域名}/
```

### 封面图片 URL 规则

```
{API_BASE}/bookimg/{folder}/{bookId}.jpg
```

- `folder` = bookId 前缀数字（bookId 长度 > 3 时取前 N-3 位，否则为 0）
- 例如 bookId=1155 → folder=1 → `{API_BASE}/bookimg/1/1155.jpg`

---

## 接口列表

### 1. 搜索小说

**请求**

```
GET {API_BASE}/search?q={关键词}
```

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| q | string | 是 | 搜索关键词，URL 编码 |

**响应**

```json
{
  "data": [
    {
      "id": "1155",
      "title": "修罗武神",
      "author": "善良的蜜蜂",
      "intro": "简介内容..."
    }
  ]
}
```

> 兼容旧格式：`data` 字段可能为 `list`

---

### 2. 获取书籍信息

**请求**

```
GET {API_BASE}/book?id={bookId}
```

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| id | string | 是 | 书籍 ID |

**响应**

```json
{
  "id": "1155",
  "title": "修罗武神",
  "author": "善良的蜜蜂",
  "intro": "简介内容...",
  "lastupdate": "2024-01-01",
  "lastchapter": "第100章 xxx",
  "full": "连载中",
  "sortname": "玄幻",
  "dirid": "1155"
}
```

| 字段 | 说明 |
|------|------|
| dirid | 目录 ID，用于获取章节列表 |
| sortname | 书籍分类 |
| full | 连载状态 |

---

### 3. 获取章节列表

**请求**

```
GET {API_BASE}/booklist?id={dirid}
```

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| id | string | 是 | 目录 ID（来自 book 接口的 dirid 字段） |

**响应**

```json
{
  "list": [
    "1 第一章 外门弟子",
    "2 第二章 xxx",
    "第三章 xxx"
  ]
}
```

> 兼容旧格式：`list` 字段可能为 `data`

**解析规则**：
- 格式 `数字 标题` → 解析出 id 和 title
- 纯文本 → id 按顺序递增

---

### 4. 获取章节内容 ⭐ 需要加密

**请求**

```
GET https://apibi.cc/api/chapter?token={加密Token}
```

> ⚠️ 此接口固定使用 `apibi.cc`，不走通用 API 域名

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| token | string | 是 | AES 加密后的参数（见下方加密算法） |

**请求头（特殊）**

```
Referer: https://www.bqg551.cc/
```

**响应**

```json
{
  "id": 1155,
  "chapterid": 1,
  "dirid": 1155,
  "title": "修罗武神",
  "author": "善良的蜜蜂",
  "chaptername": "第一章 外门弟子",
  "cs": 6746,
  "ck": null,
  "txt": "正文内容...",
  "time": 1701153839,
  "md5": "c63a7b4a6b58208d4136bdd2602810fa"
}
```

---

### 5. 排行榜 / 首页推荐

**请求**

```
GET {API_BASE}/index?sort=index    # 首页推荐
GET {API_BASE}/sort?sort={type}    # 分类排行
```

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| sort | string | 是 | 类型：`index`（首页）/ `xuanhuan` / `wuxia` / `dushi` 等 |

**响应**

```json
{
  "toplist": [
    { "id": "1155", "title": "修罗武神", "author": "善良的蜜蜂" }
  ],
  "addlist": [
    { "id": "2233", "title": "xxx", "author": "xxx" }
  ],
  "data": [
    { "id": "3344", "title": "xxx", "author": "xxx" }
  ]
}
```

> 三个字段都可能有数据，全部解析合并

---

### 6. 域名自动探测

**请求**

```
GET https://www.bqg78.com/js/compc.js
```

**响应（JS 文件）**

```javascript
var array = ["domain1.com", "domain2.com", "domain3.com"];
```

**解析**：提取数组第一个元素，拼接为 `https://{域名}/api`

---

## Token 加密算法（章节内容接口专用）

### 流程

```
1. 构造 JSON 参数
2. MD5 哈希密钥源字符串
3. 提取 IV 和 Key
4. AES-128-CBC 加密
5. Base64 编码
6. URL 编码（+ → %2B, / → %2F, = → %3D）
```

### 详细步骤

**Step 1：构造参数**

```json
{
  "id": 1155,
  "chapterid": 1
}
```

> `id` 为书籍 ID（整数），`chapterid` 为章节 ID（整数）

**Step 2：生成密钥**

```javascript
// 密钥源（固定字符串）
keySource = "book@token.html"

// MD5 哈希并转为十六进制字符串
md5Hex = MD5(keySource).toHex()
// 结果示例："5961323e94f0e0b5f25f4fdc768e3895"（32个十六进制字符）
```

**Step 3：提取 IV 和 Key**

```javascript
iv   = md5Hex.substring(0, 16)   // 前16个字符的 UTF-8 字节
key  = md5Hex.substring(16, 32)  // 后16个字符的 UTF-8 字节
```

> ⚠️ 注意：是将十六进制字符串**当作 UTF-8 文本**解析为字节，不是将十六进制解码为字节

**Step 4：AES 加密**

```
算法：AES-128-CBC
填充：PKCS7
输入：JSON 参数的紧凑字符串
```

**Step 5：Base64 编码**

加密后的字节数组直接 Base64 编码。

**Step 6：URL 编码**

最终 token 需要 URL 编码后拼接到 URL 中：

```
https://apibi.cc/api/chapter?token={urlEncode(base64Token)}
```

### JavaScript 实现参考

```javascript
function generateToken(bookId, chapterId) {
    const data = JSON.stringify({ id: bookId, chapterid: chapterId });

    // MD5 密钥源
    const md5Hex = CryptoJS.MD5("book@token.html").toString();

    // IV 和 Key：十六进制字符串当作 UTF-8 字节
    const iv  = CryptoJS.enc.Utf8.parse(md5Hex.substring(0, 16));
    const key = CryptoJS.enc.Utf8.parse(md5Hex.substring(16));

    // AES CBC 加密
    const encrypted = CryptoJS.AES.encrypt(data, key, {
        iv: iv,
        mode: CryptoJS.mode.CBC,
        padding: CryptoJS.pad.Pkcs7
    });

    // 返回 Base64
    return encrypted.toString();
}
```

### Python 实现参考

```python
import hashlib
import json
from base64 import b64encode
from urllib.parse import quote
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

def generate_token(book_id, chapter_id):
    data = json.dumps({"id": book_id, "chapterid": chapter_id}, separators=(',', ':'))

    # MD5 密钥源
    md5_hex = hashlib.md5("book@token.html".encode()).hexdigest()

    # IV 和 Key：十六进制字符串当作 UTF-8 字节
    iv  = md5_hex[:16].encode('utf-8')
    key = md5_hex[16:].encode('utf-8')

    # AES CBC 加密
    cipher = AES.new(key, AES.MODE_CBC, iv)
    encrypted = cipher.encrypt(pad(data.encode(), AES.block_size))

    # Base64 编码
    token = b64encode(encrypted).decode()

    return quote(token, safe='')
```

---

## 数据结构

### NovelSearchItem（搜索结果）

| 字段 | 类型 | 说明 |
|------|------|------|
| id | string | 书籍 ID |
| title | string | 书名 |
| author | string | 作者 |
| intro | string | 简介 |

### NovelBookInfo（书籍信息）

| 字段 | 类型 | 说明 |
|------|------|------|
| id | string | 书籍 ID |
| dirid | string | 目录 ID |
| title | string | 书名 |
| author | string | 作者 |
| coverUrl | string | 封面 URL |
| intro | string | 简介 |
| full | string | 连载状态 |
| lastchapter | string | 最新章节名 |
| lastupdate | string | 最后更新时间 |
| type | string | 分类 |

### NovelChapterItem（章节列表项）

| 字段 | 类型 | 说明 |
|------|------|------|
| id | int | 章节 ID |
| title | string | 章节标题 |

### NovelChapter（章节内容）

| 字段 | 类型 | 说明 |
|------|------|------|
| chapterId | int | 章节 ID |
| chapterName | string | 章节标题 |
| text | string | 正文内容 |

---

## 注意事项

1. **章节内容接口必须用 HTTPS**，服务器会将 HTTP 重定向为 HTTPS
2. **Token 中的 `+` 号**在 URL 中会被解释为空格，必须编码为 `%2B`
3. **Referer 必须匹配**，章节接口需要 `https://www.bqg551.cc/`
4. **SSL 证书**：该站点使用 Cloudflare，可能需要忽略证书验证
5. **域名会变**：API 域名不固定，需要定期从 `compc.js` 探测更新
6. **章节列表格式不统一**：有的带数字编号，有的纯标题，需要兼容解析
