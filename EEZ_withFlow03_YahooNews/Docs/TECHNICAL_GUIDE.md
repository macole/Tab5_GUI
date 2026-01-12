# Technical Guide - Yahoo News Viewer

Yahoo News Viewer for M5Stack Tab5 の技術仕様とプログラム構造に関するドキュメントです。

## 📋 目次

1. [プログラム構造](#プログラム構造)
2. [技術仕様](#技術仕様)
3. [Flow変数とアクション](#flow変数とアクション)
4. [データフロー](#データフロー)
5. [メモリ管理](#メモリ管理)
6. [通信仕様](#通信仕様)

---

## プログラム構造

### ファイル構成

```
EEZ_withFlow03_YahooNews/
├── EEZ_withFlow03_YahooNews.ino  # メインプログラム（497行）
├── secrets.h                      # WiFi設定（gitignore対象）
├── secrets.h.example              # WiFi設定テンプレート
├── lv_conf.h                      # LVGL設定
├── ui.h, ui.c                     # EEZ Studio生成（UI定義）
├── screens.h, screens.c           # EEZ Studio生成（画面定義）
├── vars.h                         # EEZ Studio生成（変数定義）
├── eez-flow.h, eez-flow.cpp      # EEZ Studio生成（Flowエンジン）
├── images.c                       # EEZ Studio生成（画像データ）
├── styles.c                       # EEZ Studio生成（スタイル定義）
└── fonts.h                        # EEZ Studio生成（フォント定義）
```

### コードセクション

メインプログラム（EEZ_withFlow03_YahooNews.ino）は以下の9セクションで構成：

| セクション | 内容 | 主要関数/変数 |
|-----------|------|---------------|
| 1. 定数定義 | WiFi、HTTP、RSS設定 | `WIFI_CONNECT_TIMEOUT_MS`, `YAHOO_NEWS_RSS_URL`, `MAX_NEWS_ITEMS` |
| 2. グローバル変数 | LVGLバッファ、アプリ状態 | `g_draw_buf`, `g_color_buf`, `g_wifiStatus`, `g_newsData` |
| 3. LVGLコールバック | 描画、タッチ処理 | `lv_disp_flush()`, `lv_indev_read()` |
| 4. 初期化関数 | ディスプレイ、タッチ初期化 | `allocateDisplayBuffer()`, `initLvglDisplay()`, `initLvglTouch()` |
| 5. WiFi関連 | 接続、状態チェック、再接続 | `connectWiFi()`, `getWiFiStatus()`, `reconnectWiFi()` |
| 6. RSS取得 | XML解析、ニュース抽出 | `fetchYahooNews()`, `extractTag()`, `createNewsList()` |
| 7. Flow変数更新 | EEZ Flow連携 | `updateFlowVariable()` |
| 8. Flowアクション | ボタン押下時の処理 | `action_action_fetch_news()`, `action_action_refresh_display()` |
| 9. Arduino標準 | setup(), loop() | `setup()`, `loop()` |

### 主要関数の詳細

#### 初期化関数

```cpp
bool allocateDisplayBuffer()
```
- **目的**: LVGL描画用のバッファをSPIRAMに確保
- **戻り値**: 成功時true、失敗時false
- **メモリ**: 720x1280x2 = 1,843,200バイト（約1.76MB）

```cpp
bool initLvglDisplay()
```
- **目的**: LVGL表示ドライバを初期化
- **設定**: 90度回転、DMA転送有効
- **戻り値**: 成功時true、失敗時false

```cpp
void initLvglTouch()
```
- **目的**: LVGLタッチ入力ドライバを初期化
- **設定**: 長押し時間400ms、リピート間隔100ms

#### WiFi関連関数

```cpp
bool connectWiFi()
```
- **目的**: WiFiに接続
- **タイムアウト**: 15秒
- **戻り値**: 接続成功時true、タイムアウト時false
- **副作用**: `g_wifiStatus`を更新、Flow変数を更新

```cpp
int getWiFiStatus()
```
- **目的**: 現在のWiFi接続状態を取得
- **戻り値**: 0（切断）または 2（接続完了）

```cpp
void reconnectWiFi()
```
- **目的**: WiFi切断時に自動再接続
- **呼び出し**: loop()内で10秒ごとにチェック

#### RSS取得関連関数

```cpp
String extractTag(String xml, String tagName, int startPos = 0)
```
- **目的**: XMLから指定したタグの内容を抽出
- **引数**:
  - `xml`: XMLデータ
  - `tagName`: 抽出するタグ名
  - `startPos`: 検索開始位置
- **戻り値**: タグの内容（見つからない場合は空文字列）

```cpp
String createNewsList(String xml)
```
- **目的**: RSSフィードから最大MAX_NEWS_ITEMS件のニュースタイトルを抽出
- **引数**: `xml` - RSSフィードのXMLデータ
- **戻り値**: 整形されたニュースリスト（"1. タイトル1\n\n2. タイトル2..." 形式）

```cpp
String fetchYahooNews()
```
- **目的**: Yahoo News RSSを取得してニュースリストを返す
- **通信**: HTTPS通信（タイムアウト15秒）
- **戻り値**: ニュースリストまたはエラーメッセージ
- **副作用**: `g_updateStatus`を更新、Flow変数を更新

#### Flow変数更新関数

```cpp
void updateFlowVariable(const char* varName, String value)
```
- **目的**: EEZ Flow変数にString値を設定
- **対応変数**: `newsData`, `updateStatus`

```cpp
void updateFlowVariable(const char* varName, int value)
```
- **目的**: EEZ Flow変数にint値を設定
- **対応変数**: `wifiStatus`
- **副作用**: WiFi状態ラベルも直接更新

#### Flowアクション関数

```cpp
extern "C" void action_action_fetch_news()
```
- **目的**: ニュース取得アクション
- **呼び出し元**: EEZ Flow（更新ボタン押下時）
- **処理**: ニュース取得 → Flow変数更新 → 表示更新

```cpp
extern "C" void action_action_refresh_display()
```
- **目的**: 表示更新アクション
- **処理**: テキストエリアと状態ラベルを更新

---

## 技術仕様

### ハードウェア

- **デバイス**: M5Stack Tab5 (ESP32-P4)
- **ディスプレイ**: 7インチ、1280x720解像度（論理）
- **物理解像度**: 720x1280（90度回転）
- **タッチ**: 5点マルチタッチ対応

### ソフトウェア

| 項目 | 仕様 |
|------|------|
| Arduino Core | ESP-Arduino >= V3.2 |
| M5Unified | >= 0.2.10 |
| LVGL | = 8.3.11 |
| LVGL設定 | LV_COLOR_DEPTH=16, LV_COLOR_16_SWAP=1 |
| カラーフォーマット | RGB565 (16-bit) |
| 描画方式 | DMA転送 |
| 画面回転 | ソフトウェア回転90度 |

### 定数定義

```cpp
// ディスプレイ
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 1280
#define DEFAULT_BRIGHTNESS 255
#define LVGL_TIMER_DELAY_MS 1

// WiFi
#define WIFI_CONNECT_TIMEOUT_MS 15000
#define WIFI_CHECK_INTERVAL_MS 10000

// HTTP
#define HTTP_TIMEOUT_MS 15000

// RSS
#define YAHOO_NEWS_RSS_URL "https://news.yahoo.co.jp/rss/topics/top-picks.xml"
#define MAX_NEWS_ITEMS 10
```

---

## Flow変数とアクション

### Flow変数

EEZ Studioで定義されたグローバル変数：

| 変数名 | 型 | 初期値 | 説明 | 定数ID |
|--------|-----|-------|------|---------|
| `newsData` | String | "" | 取得したニュースデータ | `FLOW_GLOBAL_VARIABLE_NEWS_DATA` |
| `wifiStatus` | Integer | 0 | WiFi状態（0:切断, 1:接続中, 2:接続済） | `FLOW_GLOBAL_VARIABLE_WIFI_STATUS` |
| `updateStatus` | String | "待機中" | 更新状態メッセージ | `FLOW_GLOBAL_VARIABLE_UPDATE_STATUS` |

### Flowアクション

| アクション名 | 呼び出しタイミング | 処理内容 | 実装関数 |
|-------------|-------------------|----------|----------|
| `action_fetch_news` | 更新ボタン押下時 | ニュース取得→Flow変数更新→表示更新 | `action_action_fetch_news()` |
| `action_refresh_display` | ニュース取得後 | テキストエリア・ラベル更新 | `action_action_refresh_display()` |

### データバインディング

EEZ Studioの`tick_screen_main()`関数が自動的にFlow変数とウィジェットを同期：

```cpp
// screens.c で自動生成
void tick_screen_main() {
    // WiFi状態ラベル
    const char *new_val = evalTextProperty(flowState, 4, 3, "...");
    lv_label_set_text(objects.label_wifi_status, new_val);
    
    // 更新状態ラベル
    new_val = evalTextProperty(flowState, 5, 3, "...");
    lv_label_set_text(objects.label_update_status, new_val);
    
    // ニューステキストエリア
    new_val = evalTextProperty(flowState, 6, 3, "...");
    lv_textarea_set_text(objects.textarea_news, new_val);
}
```

---

## データフロー

### 起動時のシーケンス

```
setup()
  ↓
M5Unified初期化
  ↓
LVGL初期化
  - allocateDisplayBuffer() (SPIRAM: 1.76MB)
  - initLvglDisplay()
  - initLvglTouch()
  ↓
EEZ-Studio UI初期化
  - ui_init()
  - create_screen_main()
  ↓
WiFi接続
  - connectWiFi()
  - g_wifiStatus: 0 → 1 → 2
  - updateFlowVariable("wifiStatus", 2)
  ↓
初回ニュース取得
  - action_action_fetch_news()
  - fetchYahooNews()
  - HTTP GET → Yahoo RSS
  - XML解析 → createNewsList()
  - updateFlowVariable("newsData", list)
  - action_action_refresh_display()
  ↓
loop()
```

### 更新ボタン押下時のシーケンス

```
ユーザー: [更新]ボタンタップ
  ↓
LVGL: LV_EVENT_PRESSED イベント発火
  ↓
EEZ Flow: event_handler_cb_main_obj0()
  ↓
action_action_fetch_news()
  ↓
fetchYahooNews()
  - g_updateStatus = "取得中..."
  - updateFlowVariable("updateStatus", "取得中...")
  - HTTP GET
  - XML解析
  - g_updateStatus = "取得完了"
  ↓
updateFlowVariable("newsData", result)
  ↓
action_action_refresh_display()
  - lv_textarea_set_text(objects.textarea_news, g_newsData)
  - lv_label_set_text(objects.label_update_status, g_updateStatus)
  ↓
画面更新完了
```

### ループ処理

```
loop()
  ↓
M5.update()              // ボタン、タッチ、IMU更新
  ↓
lv_timer_handler()       // LVGL描画処理
  ↓
ui_tick()                // EEZ Flow処理
  ├─ tick_screen_main()  // Flow変数→ウィジェット同期
  └─ eez_flow_tick()     // Flowエンジン実行
  ↓
WiFi状態チェック（10秒ごと）
  - getWiFiStatus()
  - 変化があれば updateFlowVariable()
  - 切断時は reconnectWiFi()
  ↓
delay(1ms)
  ↓
loop() へ戻る
```

---

## メモリ管理

### メモリ配分

| 項目 | サイズ | 種類 | 説明 |
|------|--------|------|------|
| LVGL描画バッファ | 1.76MB | SPIRAM | 720x1280x2バイト |
| ニュースデータ | ~4KB | RAM | String型（可変長） |
| XMLバッファ | ~50KB | RAM | HTTPClient内部 |
| グローバル変数 | ~98KB | RAM | コンパイラ報告値 |
| ローカル変数 | ~229KB | RAM | スタック領域 |

### メモリ使用状況

**コンパイル時**:
```
フラッシュ: 1,850,143 bytes (28%)
RAM: 98,472 bytes (30%)
```

**実行時** (setup()完了後):
```
Free Heap: 約200KB
Free PSRAM: 約7MB
```

### メモリ最適化

- LVGLバッファはSPIRAMに配置（`heap_caps_malloc`使用）
- ニュースデータは最大10件に制限
- XMLデータは一時的にのみ保持（HTTPClient内部で自動解放）

---

## 通信仕様

### WiFi

| 項目 | 仕様 |
|------|------|
| プロトコル | IEEE 802.11 b/g/n |
| 周波数帯 | 2.4GHz のみ（5GHz非対応） |
| セキュリティ | WPA/WPA2 |
| 接続モード | STA（ステーション）モード |
| 接続タイムアウト | 15秒 |
| 再接続間隔 | 10秒ごとにチェック |

### HTTP/HTTPS

| 項目 | 仕様 |
|------|------|
| プロトコル | HTTPS (TLS 1.2) |
| HTTPSライブラリ | HTTPClient (ESP32標準) |
| 証明書検証 | デフォルト（システム証明書） |
| タイムアウト | 15秒 |
| リトライ | なし（手動更新のみ） |
| User-Agent | デフォルト |

### RSS取得

| 項目 | 仕様 |
|------|------|
| URL | https://news.yahoo.co.jp/rss/topics/top-picks.xml |
| HTTPメソッド | GET |
| レスポンス形式 | RSS 2.0 (XML) |
| 文字コード | UTF-8 |
| 最大レスポンスサイズ | 制限なし（HTTPClient依存） |

### XMLパース

- **方式**: 文字列検索（indexOf使用）
- **対象タグ**: `<item>`, `<title>`
- **処理**:
  1. `<item>`タグを順次検索
  2. 各item内の`<title>`を抽出
  3. 最大10件まで処理
  4. 番号付きリスト形式で整形

---

## パフォーマンス

### レスポンスタイム

| 処理 | 所要時間 |
|------|----------|
| 起動～setup()完了 | 約2秒 |
| WiFi接続 | 3～10秒 |
| RSS取得 | 2～5秒 |
| XML解析 | < 100ms |
| 画面更新 | < 50ms |
| **合計（起動～表示）** | **約10～15秒** |

### LVGL更新周期

- **lv_timer_handler()**: 1msごと
- **ui_tick()**: 1msごと
- **WiFi状態チェック**: 10秒ごと

### 電力消費

- **通常動作**: 約1.5W（WiFi接続時）
- **アイドル**: 約0.8W（画面表示のみ）

---

**作成日**: 2026年1月12日  
**最終更新**: 2026年1月12日  
**対象バージョン**: v1.0

