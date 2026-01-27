# 開発ガイド

このドキュメントは、M5Stack Tab5向けEEZ Studio + LVGLアプリケーション開発の完全ガイドです。

## 📋 目次

1. [クイックスタート](#クイックスタート)
2. [プロジェクト概要](#プロジェクト概要)
3. [カスタマイズガイド](#カスタマイズガイド)
4. [技術詳細](#技術詳細)

---

## クイックスタート

### 📋 必要なもの

#### ハードウェア
- ✅ M5Stack Tab5
- ✅ USB-Cケーブル
- ✅ PC (Windows/Mac/Linux)

#### ソフトウェア
- ✅ Arduino IDE 2.x
- ✅ M5Unifiedライブラリ (>= 0.2.10)
- ✅ LVGLライブラリ (= 8.3.11)

### 🚀 ステップ1: 環境セットアップ

#### 1-1. Arduino IDEのインストール

1. [Arduino公式サイト](https://www.arduino.cc/en/software)からダウンロード
2. インストーラーを実行
3. Arduino IDEを起動

#### 1-2. ESP32ボードサポートの追加

1. **ファイル** → **環境設定**を開く
2. **追加のボードマネージャのURL**に以下を追加：
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. **OK**をクリック
4. **ツール** → **ボード** → **ボードマネージャ**を開く
5. **esp32**で検索
6. **esp32 by Espressif Systems**をインストール（バージョン3.2以上）

#### 1-3. 必要なライブラリのインストール

**方法1: ライブラリマネージャを使用（推奨）**

1. **スケッチ** → **ライブラリをインクルード** → **ライブラリを管理**
2. **M5Unified**で検索してインストール
3. **lvgl**で検索して**バージョン8.3.11**をインストール

⚠️ **重要**: LVGL 9.xではなく、必ず8.3.11をインストールしてください！

### 🔧 ステップ2: プロジェクトを開く

#### 2-1. プロジェクトを開く

1. Arduino IDEを起動
2. **ファイル** → **開く**
3. `EEZ_Template/EEZ_Template.ino`を選択
4. **開く**をクリック

#### 2-2. ボード設定

**ツール**メニューで以下を設定：

```
ボード: "M5Tab5"
Upload Speed: 921600
Flash Mode: QIO
Flash Frequency: 80MHz
Partition Scheme: Default 4MB with spiffs
PSRAM: Enabled ← 必須！
USB CDC On Boot: Enabled
USB Mode: Hardware CDC and JTAG
```

⚠️ **PSRAM: Enabled**を忘れずに！これがないと動作しません。

#### 2-3. シリアルポートの選択

1. M5Stack Tab5をUSBケーブルでPCに接続
2. **ツール** → **シリアルポート**で接続されたポートを選択
   - Mac: `/dev/cu.usbmodem*****`
   - Windows: `COM*`
   - Linux: `/dev/ttyACM*`

### ▶️ ステップ3: コンパイル・アップロード

#### 3-1. コンパイル

1. **検証（✓）**ボタンをクリック
2. コンパイル完了を待つ（初回は数分かかる場合があります）

#### 3-2. アップロード

1. **アップロード（→）**ボタンをクリック
2. アップロード完了を待つ

#### 3-3. 動作確認

Tab5の画面に以下が表示されれば成功です：
- ✅ GUI要素が表示される
- ✅ タッチ操作が正常に動作する
- ✅ EEZ Studioで作成したUI要素が表示される

### 📊 ステップ4: シリアルモニタでログを確認

#### 4-1. シリアルモニタを開く

1. **ツール** → **シリアルモニタ**
2. 右下のボーレートを**115200**に設定

#### 4-2. 起動ログの確認

正常に動作している場合、以下のようなログが表示されます：

```
╔═══════════════════════════════════════╗
║  M5Stack Tab5                         ║
║  EEZ Studio Template Project          ║
╚═══════════════════════════════════════╝

📊 System Information:
   LVGL Version: 8.3.11
   Free Heap: 423456 bytes (413.53 KB)
   Free PSRAM: 8388608 bytes (8.00 MB)

🔧 Initializing LVGL display...
✅ Display buffer allocated: 1843200 bytes (1.76 MB)
✅ LVGL display initialized
🔧 Initializing LVGL touch...
✅ LVGL touch initialized

🎨 Initializing EEZ-Studio UI...
✅ EEZ-Studio UI initialized

╔═══════════════════════════════════════╗
║  Setup Completed Successfully!        ║
╚═══════════════════════════════════════╝
   Free Heap after setup: 412345 bytes (402.68 KB)
   Free PSRAM after setup: 6545408 bytes (6.24 MB)

🚀 Application started!
```

---

## プロジェクト概要

### プロジェクトの目的

このテンプレートプロジェクトは、M5Stack Tab5でEEZ StudioとLVGLを使用したアプリケーションを迅速に開発するための基盤を提供します。

### 設計思想

- **シンプルさ**: 最小限の機能で最大の拡張性
- **明確性**: 各ファイルの役割が明確
- **ドキュメント化**: すべての機能に詳細なコメント
- **モダン**: 最新のライブラリとベストプラクティス

### 主な用途

- IoTダッシュボードの作成
- センサーデータ可視化アプリ
- デバイス制御パネル
- データロガー
- Webサービス連携アプリ

### 全体アーキテクチャ

```
┌─────────────────────────────────────────────────────────┐
│                    M5Stack Tab5                          │
│                  (ESP32-P4 MCU)                          │
├─────────────────────────────────────────────────────────┤
│  ┌───────────────────────────────────────────────────┐  │
│  │            EEZ_Template.ino                       │  │
│  │          (メインアプリケーション)                   │  │
│  └───┬───────────────────────────────────────────┬───┘  │
│      │                                           │      │
│  ┌───▼────────────┐                    ┌────────▼────┐ │
│  │   M5Unified    │                    │    LVGL     │ │
│  │  ライブラリ     │                    │  GUI ライブラリ │ │
│  └───┬────────────┘                    └────────┬────┘ │
│      │                                           │      │
│  ┌───▼────────────┐                    ┌────────▼────┐ │
│  │  ハードウェア   │                    │ EEZ Flow    │ │
│  │  - Display     │                    │  Engine     │ │
│  │  - Touch       │                    │             │ │
│  │  - Buttons     │                    │(eez-flow.cpp)│ │
│  └────────────────┘                    └────────┬────┘ │
│                                                  │      │
│                                        ┌─────────▼────┐ │
│                                        │  UI定義ファイル │ │
│                                        │ - ui.c/h     │ │
│                                        │ - screens.c/h│ │
│                                        │ - styles.c/h │ │
│                                        └──────────────┘ │
└─────────────────────────────────────────────────────────┘
```

### ファイル構造詳細

#### 📂 プロジェクトルート

```
EEZ_Template/
├── EEZ_Template.ino                    # メインプログラム
├── LV8wF_Template.eez-project         # EEZ Studioプロジェクト
├── lv_conf.h                           # LVGL設定ファイル
├── LICENSE                              # MITライセンス
├── README.md                           # プロジェクトドキュメント
└── src/
    └── ui/                             # EEZ Studio生成ファイル
        ├── ui.c / ui.h                 # UI初期化コード
        ├── screens.c / screens.h       # 画面定義
        ├── eez-flow.cpp / eez-flow.h   # EEZ Flow言語エンジン
        ├── styles.c / styles.h         # スタイル定義
        ├── actions.h                   # アクション定義
        ├── images.c / images.h         # 画像データ
        ├── fonts.h                     # フォント定義
        ├── structs.h                   # 構造体定義
        └── vars.h                      # 変数定義
```

#### 📝 各ファイルの役割

**EEZ_Template.ino** (メインプログラム)
- プロジェクトのエントリーポイント
- Arduino標準関数（`setup()`, `loop()`）
- M5Unified初期化
- LVGL初期化とコールバック設定
- タッチ入力処理
- メモリ管理（SPIRAM）
- アプリケーションロジック

**LV8wF_Template.eez-project** (EEZ Studioプロジェクト)
- EEZ Studioで編集するプロジェクトファイル
- 画面レイアウト定義
- Flow言語プログラム
- スタイル設定
- 変数定義
- アクション定義

**lv_conf.h** (LVGL設定)
- LVGLライブラリの動作設定
- フォントの有効化/無効化
- ウィジェットの有効化
- デバッグ機能の切り替え
- メモリ設定



### データフロー

#### 起動シーケンス

```
[電源投入]
    │
    ▼
[setup() 実行開始]
    │
    ├─▶ M5.begin(cfg)                    // M5Unified初期化
    │
    ├─▶ Serial.begin(115200)             // シリアル通信開始
    │
    ├─▶ システム情報表示
    │   - LVGL バージョン
    │   - 空きヒープ/PSRAM
    │
    ├─▶ initLvglDisplay()
    │   ├─▶ lv_init()                    // LVGL初期化
    │   ├─▶ allocateDisplayBuffer()      // SPIRAMバッファ確保（1.8MB）
    │   └─▶ lv_disp_drv_register()       // ディスプレイドライバ登録
    │
    ├─▶ initLvglTouch()
    │   └─▶ lv_indev_drv_register()      // タッチドライバ登録
    │
    ├─▶ ui_init()
    │   ├─▶ eez_flow_init()              // EEZ Flow初期化
    │   ├─▶ create_screens()             // 画面作成
    │   └─▶ loadScreen(SCREEN_ID_MAIN)   // メイン画面表示
    │
    ├─▶ M5.Display.setBrightness(255)    // 明るさ設定
    │
    └─▶ [setup完了]
         │
         ▼
    [loop() 実行開始]
```

#### メインループの流れ

```
loop() (毎フレーム実行)
    │
    ├─▶ M5.update()
    │   └─ ボタン、タッチ状態更新
    │
    ├─▶ lv_timer_handler()
    │   ├─ UI描画更新
    │   ├─ アニメーション処理
    │   └─ イベント処理
    │
    ├─▶ ui_tick()
    │   ├─ eez_flow_tick()              // Flow実行
    │   └─ tick_screen()                // 現在の画面更新
    │
    ├─▶ updateApplication()
    │   └─ アプリケーション固有処理
    │      (カスタマイズポイント)
    │
    └─▶ delay(1)                        // 1ms待機
         │
         └─▶ (loop()へ戻る)
```

### 主要コンポーネント

#### 1. M5Unified

**役割**: M5Stack統合ライブラリ

**提供機能**:
- ディスプレイ制御（`M5.Display`）
- タッチスクリーン（`M5.Touch`）
- ボタン入力（`M5.BtnA`, `M5.BtnB`, `M5.BtnC`）
- 電源管理（`M5.Power`）
- I2C、SPI、UART

#### 2. LVGL (Light and Versatile Graphics Library)

**バージョン**: 8.3.11

**役割**: 高機能GUIライブラリ

**主要機能**:
- ウィジェット（ボタン、スライダー、ラベルなど）
- スタイル管理（色、フォント、サイズ）
- アニメーション
- イベント処理
- スクリーン管理

#### 3. EEZ Studio Flow

**役割**: 視覚的プログラミング環境

**特徴**:
- ノードベースのFlowエディタ
- ビジュアルUIデザイナー
- 変数、アクション、イベント管理
- C/C++コード自動生成

### メモリ管理

#### メモリマップ

```
ESP32-P4メモリ構成:
┌──────────────────────────┐
│  内部RAM (512KB)          │
│  ├─ Program Stack        │
│  ├─ Heap (変数、小さなバッファ) │
│  └─ LVGL内部データ        │
├──────────────────────────┤
│  SPIRAM (8MB) - 外部拡張RAM │
│  ├─ 画面バッファ (1.8MB)  │
│  ├─ 大きな配列データ      │
│  └─ 画像データ            │
├──────────────────────────┤
│  Flash (16MB)            │
│  ├─ プログラムコード      │
│  ├─ LVGLライブラリ        │
│  ├─ M5Unifiedライブラリ   │
│  └─ 定数データ (fonts, images) │
└──────────────────────────┘
```

---

## カスタマイズガイド

### 基本的なカスタマイズ

#### プロジェクト名の変更

1. **ファイル名を変更**
   - `EEZ_Template.ino` → `YourApp.ino`

2. **プロジェクトフォルダ名を変更**
   - `EEZ_Template/` → `YourApp/`

3. **ヘッダーコメントを更新**
   ```cpp
   /*******************************************************************************
    * あなたのアプリケーション名
    * 
    * アプリケーションの説明
    ******************************************************************************/
   ```

#### アプリケーション定数の変更

`EEZ_Template.ino`の定数定義セクションで設定を変更：

```cpp
// アプリケーション設定
#define APP_UPDATE_INTERVAL_MS 100  // 更新間隔（ミリ秒）
#define DEFAULT_BRIGHTNESS 255       // 初期画面輝度（0-255）
```

### UIの変更

#### EEZ Studioでの基本操作

##### 1. プロジェクトを開く

```
File → Open Project → LV8wF_Template.eez-project
```

##### 2. 新しい画面を追加

1. **Pagesタブ**を選択
2. **+ボタン**をクリック
3. 画面名を入力（例: "Settings"）
4. レイアウトを編集

##### 3. ウィジェットを追加

**左側のウィジェットパネルから選択**：

- **Label**: テキスト表示
- **Button**: ボタン
- **Slider**: スライダー
- **Text Area**: テキスト入力
- **Image**: 画像表示
- **Panel**: コンテナ
- **List**: リスト表示
- **Chart**: グラフ表示

**配置方法**：

1. ウィジェットをドラッグ＆ドロップ
2. プロパティパネルで設定
   - Position (X, Y)
   - Size (Width, Height)
   - Appearance (色、フォント)
   - Behavior (可視性、有効/無効)

##### 4. スタイルのカスタマイズ

**Stylesタブ**：

1. **+ボタン**で新しいスタイルを作成
2. プロパティを設定：
   - Background color
   - Border color/width
   - Text color/font
   - Padding/Margin
3. ウィジェットに適用

##### 5. Flowでの動作定義

**Flowタブ**：

1. アクションを追加（ドラッグ＆ドロップ）
   - **Set Text**: テキスト設定
   - **Set Value**: 値設定
   - **Show/Hide**: 表示/非表示
   - **Navigate**: 画面遷移
   - **Delay**: 待機
   - **Loop**: ループ処理

2. トリガーを設定
   - **Button Pressed**: ボタン押下時
   - **Timer**: タイマー
   - **Variable Changed**: 変数変更時

3. アクションを接続（線で繋ぐ）

##### 6. エクスポート

```
Build → Build & Export
```

生成されたファイルをプロジェクトフォルダの`src/ui/`にコピー

### 機能の追加

#### センサー読み取り機能の追加

##### 例: 温湿度センサー（SHT40）

```cpp
// 1. ライブラリをインクルード
#include <Wire.h>
#include <SHT4x.h>

// 2. グローバル変数
static SHT4x sht40;
static float g_temperature = 0.0;
static float g_humidity = 0.0;

// 3. setup()で初期化
void setup() {
    // ... 既存の初期化 ...
    
    // センサー初期化
    Wire.begin();
    if (!sht40.begin()) {
        Serial.println("❌ SHT40 sensor not found!");
    } else {
        Serial.println("✅ SHT40 sensor initialized");
    }
}

// 4. 読み取り関数を追加
void updateSensorData() {
    static unsigned long lastReadTime = 0;
    const unsigned long READ_INTERVAL = 2000;  // 2秒ごと
    
    if (millis() - lastReadTime < READ_INTERVAL) {
        return;
    }
    lastReadTime = millis();
    
    // センサー読み取り
    if (sht40.measure()) {
        g_temperature = sht40.getTemperature();
        g_humidity = sht40.getHumidity();
        
        // UIに表示（EEZ Studioで作成したラベルに表示）
        lv_label_set_text_fmt(objects.label_temp, "%.1f°C", g_temperature);
        lv_label_set_text_fmt(objects.label_humi, "%.1f%%", g_humidity);
        
        Serial.printf("Temperature: %.1f°C, Humidity: %.1f%%\n", 
                     g_temperature, g_humidity);
    }
}

// 5. loop()で呼び出し
void loop() {
    M5.update();
    lv_timer_handler();
    ui_tick();
    
    updateSensorData();  // ← 追加
    
    delay(LVGL_TIMER_DELAY_MS);
}
```

#### WiFi機能の実装

##### WiFi接続

```cpp
#include <WiFi.h>

// WiFi設定
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

bool connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int timeout = 30;
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(500);
        Serial.print(".");
        timeout--;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ WiFi connected!");
        Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
        return true;
    } else {
        Serial.println("\n❌ WiFi connection failed");
        return false;
    }
}

void setup() {
    // ... 既存の初期化 ...
    
    if (connectWiFi()) {
        // WiFi接続成功後の処理
    }
}
```

##### HTTPリクエストの送信

```cpp
#include <HTTPClient.h>

void fetchWeatherData() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("⚠️  WiFi not connected");
        return;
    }
    
    HTTPClient http;
    const char* url = "http://api.example.com/weather";
    
    Serial.printf("🌐 Fetching: %s\n", url);
    http.begin(url);
    
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("✅ Data received:");
        Serial.println(payload);
        
        // データを解析してUIに表示
    } else {
        Serial.printf("❌ HTTP Error: %d\n", httpCode);
    }
    
    http.end();
}
```

#### Native変数の実装

Native変数を使用すると、C++コードから直接変数を制御できます。

```cpp
// Native変数: counter（カウンター）
static int32_t g_counter = 0;

int32_t get_var_counter() {
    return g_counter;
}

void set_var_counter(int32_t value) {
    g_counter = value;
}

// Native変数: flag（フラグ）
static bool g_flag = false;

bool get_var_flag() {
    return g_flag;
}

void set_var_flag(bool value) {
    g_flag = value;
}
```

EEZ StudioでNative変数を定義する方法は、[LV8wF_Native](../LV8wF_Native/README.md)を参照してください。

---

## 技術詳細

### アーキテクチャ概要

#### システム構成

```
┌─────────────────────────────────────────────────┐
│                 Application Layer                │
│        (updateApplication, custom logic)         │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────┐
│              EEZ Flow Engine                     │
│          (ui_tick, Flow execution)               │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────┐
│                   LVGL Layer                     │
│       (lv_timer_handler, UI rendering)           │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────┐
│              Hardware Abstraction                │
│              (M5Unified, Display)                │
└─────────────────────────────────────────────────┘
```

### LVGL統合

#### カラーフォーマット

```cpp
// lv_conf.h
#define LV_COLOR_DEPTH 16        // RGB565 (16bit)
#define LV_COLOR_16_SWAP 1       // バイトスワップ有効
```

#### ディスプレイドライバ設定

```cpp
void configureDisplayDriver(lv_disp_drv_t &disp_drv)
{
    lv_disp_drv_init(&disp_drv);
    
    // 解像度設定（物理サイズ）
    disp_drv.hor_res = 720;
    disp_drv.ver_res = 1280;
    
    // フラッシュコールバック
    disp_drv.flush_cb = lv_disp_flush;
    
    // 描画バッファ
    disp_drv.draw_buf = &g_draw_buf;
    
    // ソフトウェア回転（90度）
    disp_drv.sw_rotate = 1;
    disp_drv.rotated = LV_DISP_ROT_90;
    
    lv_disp_drv_register(&disp_drv);
}
```

#### タッチ入力処理

```cpp
static void lv_indev_read(lv_indev_drv_t *indev_driver, 
                          lv_indev_data_t *data)
{
    const auto touch_detail = M5.Touch.getDetail();
    
    // isPressed()とwasPressed()の両方をチェックすることで、
    // タッチ開始時だけでなく、ドラッグ中も連続的に検出できる
    if (touch_detail.isPressed() || touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;  // 押下状態
        data->point.x = touch_detail.x;
        data->point.y = touch_detail.y;
    } else {
        data->state = LV_INDEV_STATE_REL; // 離された状態
    }
}
```

### EEZ Flow統合

#### Flowエンジンの実行

```cpp
void loop() {
    M5.update();            // ハードウェア更新
    lv_timer_handler();     // LVGL UI更新
    ui_tick();              // Flow実行 ← EEZ Flow
    updateApplication();    // アプリケーション処理
    delay(LVGL_TIMER_DELAY_MS);
}
```

### 描画パイプライン

#### 描画フロー

```
1. lv_timer_handler()
   ↓
2. LVGL内部処理（ダーティ領域計算）
   ↓
3. lv_disp_flush() コールバック呼び出し
   ↓
4. M5.Display.pushImageDMA() (DMA転送開始)
   ↓
5. lv_disp_flush_ready() (転送完了通知)
```

#### DMA転送の詳細

```cpp
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, 
                   lv_color_t *color_p)
{
    // 更新領域のサイズを計算
    const uint32_t width = area->x2 - area->x1 + 1;
    const uint32_t height = area->y2 - area->y1 + 1;
    
    // DMA転送（非ブロッキング）
    // CPUは他の処理を継続できる
    M5.Display.pushImageDMA(
        area->x1,              // X座標
        area->y1,              // Y座標
        width,                 // 幅
        height,                // 高さ
        (uint16_t *)&color_p->full  // データポインタ
    );
    
    // LVGLに転送完了を通知
    lv_disp_flush_ready(disp);
}
```

**DMA転送のメリット**:
- ✅ CPUリソースを節約
- ✅ 高速なデータ転送
- ✅ 並行処理が可能

### パフォーマンス最適化

#### 更新頻度の最適化

```cpp
// 悪い例: 毎フレーム更新
void loop() {
    updateSensorData();  // 毎回実行（無駄）
}

// 良い例: 必要な時だけ更新
void loop() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {  // 1秒ごと
        updateSensorData();
        lastUpdate = millis();
    }
}
```

#### LVGL再描画の最小化

```cpp
// 悪い例: 値が変わらなくても更新
lv_label_set_text_fmt(label, "%d", value);

// 良い例: 値が変わった時だけ更新
static int lastValue = -1;
if (value != lastValue) {
    lv_label_set_text_fmt(label, "%d", value);
    lastValue = value;
}
```

### デバッグ手法

#### シリアルログマクロ

```cpp
// デバッグレベル定義
#define DEBUG_LEVEL_NONE  0
#define DEBUG_LEVEL_ERROR 1
#define DEBUG_LEVEL_WARN  2
#define DEBUG_LEVEL_INFO  3
#define DEBUG_LEVEL_DEBUG 4

// 現在のデバッグレベル
#define CURRENT_DEBUG_LEVEL DEBUG_LEVEL_INFO

// デバッグマクロ
#define DEBUG_ERROR(fmt, ...) \
    if (CURRENT_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR) \
        Serial.printf("[ERROR] " fmt "\n", ##__VA_ARGS__)

#define DEBUG_INFO(fmt, ...) \
    if (CURRENT_DEBUG_LEVEL >= DEBUG_LEVEL_INFO) \
        Serial.printf("[INFO]  " fmt "\n", ##__VA_ARGS__)

// 使用例
void someFunction() {
    DEBUG_INFO("Function started");
    DEBUG_INFO("Value: %d", someValue);
    
    if (error) {
        DEBUG_ERROR("Something went wrong!");
    }
}
```

#### メモリ使用量の監視

```cpp
void printMemoryUsage() {
    Serial.println("📊 Memory Usage:");
    Serial.printf("   Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("   Free PSRAM: %d bytes\n", ESP.getFreePsram());
}

// loop()で定期的に呼び出し
void loop() {
    static unsigned long lastPrintTime = 0;
    if (millis() - lastPrintTime > 10000) {  // 10秒ごと
        printMemoryUsage();
        lastPrintTime = millis();
    }
    // ...
}
```

---

## まとめ

このガイドでは、M5Stack Tab5向けEEZ Studio + LVGLアプリケーション開発の基礎から応用までを説明しました。

**開発の流れ**:
```
EEZ Studioで設計 → エクスポート → Arduinoでコンパイル → Tab5で実行
```

**次のステップ**:
- [README.md](README.md) - プロジェクト概要
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - トラブルシューティング
- [LV8wF_Clock](../LV8wF_Clock/README.md) - WiFi/NTP実装例
- [LV8wF_Native](../LV8wF_Native/README.md) - Native変数実装例

このテンプレートをベースに、独自のアプリケーションを自由に開発してください！

---

**作成日**: 2026年1月27日  
**バージョン**: 1.0.0  
**対象**: M5Stack Tab5 (ESP32-P4), M5Unified 0.2.10+, LVGL 8.3.11
