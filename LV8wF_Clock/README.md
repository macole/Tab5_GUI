# LV8wF_Clock - M5Stack Tab5 時計アプリケーション

EEZ StudioとLVGLを使用したM5Stack Tab5向けの時計アプリケーションです。WiFi接続とNTP時刻同期機能を実装し、リアルタイムで正確な時刻を表示します。

## 📋 概要

このプロジェクトは、**EEZ_Template**をベースに開発された時計アプリケーションです。WiFi接続とNTP時刻同期機能を実装し、リアルタイムで正確な時刻を表示します。

### 実装されている機能

- ✅ **WiFi接続機能** - 自動WiFi接続と状態表示
- ✅ **NTP時刻同期** - インターネットから正確な時刻を取得
- ✅ **リアルタイム時計表示** - 時刻、日付、曜日を更新（デフォルト: 0.5秒ごと）
- ✅ **WiFi状態表示** - 接続状態とIPアドレスを表示
- ✅ **EEZ Studio設計** - 視覚的なUIデザインとFlow言語
- ✅ **LVGL 8.3.11** - 高機能GUIライブラリ

## 🔧 ハードウェア要件

- **M5Stack Tab5** (ESP32-P4)
  - 10.1インチディスプレイ（1280x720）
  - 静電容量式タッチスクリーン
  - 8MB PSRAM（必須）
  - WiFi接続機能

## 📚 必要なライブラリ

| ライブラリ | バージョン | 必須 |
|------------|----------|------|
| M5Unified | >= 0.2.10 | ✅ |
| LVGL | 8.3.11 | ✅ |

⚠️ **重要**: LVGL 9.xではなく、必ず8.3.11を使用してください。

## 🚀 クイックスタート

### 1. WiFi設定

`LV8wF_Clock.ino`の以下の行を編集してください：

```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"      // あなたのWiFi SSID
#define WIFI_PASSWORD "YOUR_PASSWORD"   // あなたのWiFiパスワード
```

### 2. ボード設定

Arduino IDEで以下を設定：

```
ボード: M5Tab5
Upload Speed: 921600
Flash Mode: QIO
Flash Frequency: 80MHz
Partition Scheme: Default 4MB with spiffs
PSRAM: Enabled (必須!)
```

### 3. コンパイル・アップロード

1. **検証（✓）**ボタンでコンパイル
2. **アップロード（→）**ボタンでTab5に転送

### 4. 動作確認

- 起動後、WiFi接続を試みます
- 接続成功後、NTPサーバーから時刻を取得します
- `text_area`に時刻とWiFi状態が表示されます

## 📁 プロジェクト構造

```
LV8wF_Clock/
├── LV8wF_Clock.ino              # メインプログラム
├── LV8wF_Clock.eez-project     # EEZ Studioプロジェクトファイル
├── lv_conf.h                    # LVGL設定ファイル
├── README.md                     # このファイル
├── CHANGES.md                    # 変更履歴
└── src/
    └── ui/                       # EEZ Studio生成ファイル
        ├── ui.c / ui.h
        ├── screens.c / screens.h
        ├── eez-flow.cpp / eez-flow.h
        ├── actions.h
        ├── styles.c / styles.h
        ├── images.c / images.h
        ├── fonts.h
        ├── structs.h
        └── vars.h
```

## 🎯 機能詳細

### 1. WiFi接続機能

- **自動接続**: 起動時に自動的にWiFiに接続を試みます
- **接続状態表示**: `text_area`に接続進捗を表示
- **タイムアウト**: 30秒以内に接続できない場合は失敗とみなします
- **IPアドレス表示**: 接続成功時にIPアドレスを表示

### 2. NTP時刻同期

- **自動同期**: WiFi接続後、自動的にNTPサーバーから時刻を取得
- **タイムゾーン設定**: JST（日本標準時、UTC+9）に対応
- **同期状態表示**: 同期の進捗と結果を表示
- **タイムアウト**: 10秒以内に同期できない場合は失敗とみなします

### 3. 時刻表示

- **リアルタイム更新**: 0.5秒ごとに時刻を更新（`TIME_UPDATE_INTERVAL_MS`で変更可能）
- **表示内容**:
  - 時刻（HH:MM:SS形式）
  - 日付（YYYY/MM/DD形式）
  - 曜日（英語表記）
  - WiFi接続状態
  - IPアドレス（接続時のみ）

### 4. text_areaウィジェット

- **動的検索**: EEZ Studioで作成された`text_area`を自動検索
- **多行表示**: 時刻、日付、WiFi状態を複数行で表示
- **英語表記**: LVGLのデフォルトフォントで表示可能な英語のみ使用

## 🔨 Templateから作成する手順

このプロジェクトは**EEZ_Template**をベースに作成されました。以下の手順で同様のプロジェクトを作成できます：

### ステップ1: Templateプロジェクトのコピー

```bash
cp -r EEZ_Template LV8wF_Clock
cd LV8wF_Clock
```

### ステップ2: ファイル名の変更

```bash
mv EEZ_Template.ino LV8wF_Clock.ino
```

### ステップ3: 必要なライブラリの追加

`LV8wF_Clock.ino`に以下のインクルードを追加：

```cpp
#include <WiFi.h>
#include <time.h>
#include <cstdio>
```

### ステップ4: WiFi/NTP設定の追加

定数定義セクションに以下を追加：

```cpp
// WiFi設定（要変更）
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define WIFI_CONNECT_TIMEOUT_SEC 30

// NTP設定
#define NTP_SERVER "pool.ntp.org"
#define TIMEZONE_OFFSET 9
#define NTP_SYNC_TIMEOUT_SEC 10
#define TIME_UPDATE_INTERVAL_MS 500  // 0.5秒ごと（デフォルト）
```

### ステップ5: グローバル変数の追加

```cpp
// 時刻関連
static unsigned long g_lastTimeUpdate = 0;
static bool g_timeInitialized = false;
static lv_obj_t *g_textArea = nullptr;
```

### ステップ6: WiFi接続関数の実装

```cpp
bool connectWiFi(void)
{
    // WiFi接続処理
    // 詳細はLV8wF_Clock.inoを参照
}
```

### ステップ7: NTP同期関数の実装

```cpp
bool syncNTPTime(void)
{
    // NTP時刻同期処理
    // 詳細はLV8wF_Clock.inoを参照
}
```

### ステップ8: text_area検索関数の実装

```cpp
static lv_obj_t* findTextArea(lv_obj_t* parent)
{
    // text_areaウィジェットの再帰的検索
    // 詳細はLV8wF_Clock.inoを参照
}
```

### ステップ9: 時刻表示関数の実装

```cpp
void updateTimeDisplay(void)
{
    // 時刻とWiFi状態の表示更新
    // 詳細はLV8wF_Clock.inoを参照
}
```

### ステップ10: setup()の拡張

`setup()`関数に以下を追加：

```cpp
// text_areaの初期化
initTextArea();

// WiFi接続とNTP時刻同期
if (connectWiFi()) {
    if (syncNTPTime()) {
        g_timeInitialized = true;
        updateTimeDisplay();
    }
}
```

### ステップ11: loop()の拡張

`loop()`関数に以下を追加：

```cpp
// 時刻表示の更新
if (g_timeInitialized) {
    unsigned long currentTime = millis();
    if (currentTime - g_lastTimeUpdate >= TIME_UPDATE_INTERVAL_MS) {
        updateTimeDisplay();
        g_lastTimeUpdate = currentTime;
    }
}
```

### ステップ12: EEZ Studioでtext_areaを作成

1. EEZ Studioでプロジェクトを開く
2. メイン画面に`TextArea`ウィジェットを追加
3. **Build & Export**を実行
4. 生成されたファイルをプロジェクトにコピー

## 📊 コード構造

### 主要な関数

- **`connectWiFi()`**: WiFi接続処理
- **`syncNTPTime()`**: NTP時刻同期処理
- **`findTextArea()`**: text_areaウィジェットの検索
- **`initTextArea()`**: text_areaの初期化
- **`updateTimeDisplay()`**: 時刻表示の更新

### グローバル変数

- **`g_textArea`**: text_areaウィジェットへのポインタ
- **`g_timeInitialized`**: NTP時刻同期済みフラグ
- **`g_lastTimeUpdate`**: 前回の時刻更新時間

## 🔍 Templateからの主な変更点

### 1. 追加された機能

- ✅ WiFi接続機能（`connectWiFi()`）
- ✅ NTP時刻同期機能（`syncNTPTime()`）
- ✅ 時刻表示機能（`updateTimeDisplay()`）
- ✅ text_area検索機能（`findTextArea()`）

### 2. 追加されたライブラリ

```cpp
#include <WiFi.h>      // WiFi接続用
#include <time.h>      // 時刻処理用
#include <cstdio>      // snprintf()用
```

### 3. 追加された定数

- `WIFI_SSID`: WiFi SSID
- `WIFI_PASSWORD`: WiFiパスワード
- `WIFI_CONNECT_TIMEOUT_SEC`: WiFi接続タイムアウト
- `NTP_SERVER`: NTPサーバーアドレス
- `TIMEZONE_OFFSET`: タイムゾーンオフセット
- `NTP_SYNC_TIMEOUT_SEC`: NTP同期タイムアウト
- `TIME_UPDATE_INTERVAL_MS`: 時刻更新間隔

### 4. 変更された処理フロー

**Template**:
```
setup() → loop() → M5.update() → lv_timer_handler() → ui_tick()
```

**LV8wF_Clock**:
```
setup() → WiFi接続 → NTP同期 → loop() → M5.update() → 
lv_timer_handler() → ui_tick() → updateTimeDisplay()
```

### 5. UI要素の追加

- **text_area**: 時刻とWiFi状態を表示するウィジェット（EEZ Studioで作成）

## 🐛 トラブルシューティング

詳細なトラブルシューティングガイドは[../Docs/TROUBLESHOOTING.md](../Docs/TROUBLESHOOTING.md)を参照してください。

### よくある問題

- **画面がチカチカする**: [../Docs/TROUBLESHOOTING.md#画面がチカチカして動かない](../Docs/TROUBLESHOOTING.md#-画面がチカチカして動かない)
- **WiFi接続に失敗する**: [../Docs/TROUBLESHOOTING.md#8-wifi接続に失敗する](../Docs/TROUBLESHOOTING.md#8-wifi接続に失敗する)
- **NTP時刻同期に失敗する**: [../Docs/TROUBLESHOOTING.md#9-ntp時刻同期に失敗する](../Docs/TROUBLESHOOTING.md#9-ntp時刻同期に失敗する)
- **text_areaに時刻が表示されない**: [../Docs/TROUBLESHOOTING.md#10-text_areaに時刻が表示されない](../Docs/TROUBLESHOOTING.md#10-text_areaに時刻が表示されない)
- **文字が□として表示される**: [../Docs/TROUBLESHOOTING.md#11-文字がとして表示される](../Docs/TROUBLESHOOTING.md#11-文字がとして表示される)
- **時刻が更新されない**: [../Docs/TROUBLESHOOTING.md#12-時刻が更新されない](../Docs/TROUBLESHOOTING.md#12-時刻が更新されない)
- **タッチパネルの反応が悪い**: [../Docs/TROUBLESHOOTING.md#7-タッチパネルの反応が悪い](../Docs/TROUBLESHOOTING.md#7-タッチパネルの反応が悪いボタンを何度も押さないと反応しない)

## 📖 技術詳細

### WiFi接続の仕組み

1. `WiFi.mode(WIFI_STA)`でステーションモードに設定
2. `WiFi.begin()`で接続を開始
3. `WiFi.status()`で接続状態を確認（最大30秒）
4. 接続成功時にIPアドレスを取得

### NTP時刻同期の仕組み

1. `configTime()`でタイムゾーンを設定
2. `getLocalTime()`でNTPサーバーから時刻を取得（最大10秒）
3. 取得した時刻をシステム時刻として設定
4. `updateTimeDisplay()`で0.5秒ごとに表示を更新（`TIME_UPDATE_INTERVAL_MS`で変更可能）

### text_area検索の仕組み

1. `findTextArea()`で画面内のすべてのウィジェットを再帰的に検索
2. `lv_obj_get_class()`でtext_areaクラスを判定
3. 見つかったtext_areaを`g_textArea`に保存
4. 以降、`g_textArea`を通して時刻を表示

## 📝 カスタマイズ

### タイムゾーンの変更

`TIMEZONE_OFFSET`を変更してください：

```cpp
#define TIMEZONE_OFFSET 0   // UTC
#define TIMEZONE_OFFSET 9   // JST (日本標準時)
#define TIMEZONE_OFFSET -5  // EST (東部標準時)
```

### NTPサーバーの変更

`NTP_SERVER`を変更してください：

```cpp
#define NTP_SERVER "pool.ntp.org"        // デフォルト
#define NTP_SERVER "time.google.com"     // Google NTP
#define NTP_SERVER "time.cloudflare.com" // Cloudflare NTP
```

### 更新間隔の変更

`TIME_UPDATE_INTERVAL_MS`を変更してください：

```cpp
#define TIME_UPDATE_INTERVAL_MS 500   // 0.5秒ごと（デフォルト）
#define TIME_UPDATE_INTERVAL_MS 1000  // 1秒ごと
#define TIME_UPDATE_INTERVAL_MS 2000  // 2秒ごと
```

## 🔗 参考リンク

- [EEZ Studio公式サイト](https://www.envox.eu/eez-studio/)
- [EEZ Studio ドキュメント](https://github.com/eez-open/studio/wiki)
- [LVGL公式ドキュメント](https://docs.lvgl.io/8.3/)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)
- [ESP32 WiFi ドキュメント](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html)
- [NTP プロトコル](https://en.wikipedia.org/wiki/Network_Time_Protocol)
- [EEZ_Template README](../EEZ_Template/README.md) - ベースとなったTemplateプロジェクト

## 📝 更新履歴

詳細な変更履歴は[CHANGES.md](CHANGES.md)を参照してください。

### 2026-01-27
- ✅ WiFi接続機能の追加
- ✅ NTP時刻同期機能の追加
- ✅ リアルタイム時計表示機能の追加
- ✅ text_areaウィジェットの動的検索機能の追加
- ✅ コードのリファクタリング
- ✅ ドキュメントの整理と更新

### 2026-01-25
- ✅ EEZ Flow統合の修正（リブートループ問題の解決）
- ✅ タッチパネルの反応性改善

## 📚 関連ドキュメント

- **[CHANGES.md](CHANGES.md)** - 詳細な変更履歴
  - WiFi/NTP機能の追加（2026-01-27）
  - EEZ Flow統合の修正（2026-01-25）
  - タッチパネルの反応性改善（2026-01-25）

- **[../Docs/TROUBLESHOOTING.md](../Docs/TROUBLESHOOTING.md)** - トラブルシューティングガイド
  - 画面がチカチカする問題
  - WiFi/NTP接続の問題
  - text_area表示の問題
  - タッチパネルの問題
  - 段階的デバッグ手順

## 📝 ライセンス

このプロジェクトはMITライセンスの下で公開されています。

---

**作成日**: 2026年1月27日  
**バージョン**: 1.0.0  
**対応環境**: M5Stack Tab5 (ESP32-P4), M5Unified 0.2.10+, LVGL 8.3.11  
**ベースプロジェクト**: EEZ_Template
