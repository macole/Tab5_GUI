# EEZ_Template プロジェクト概要

M5Stack Tab5向けEEZ Studio + LVGLアプリケーション開発用テンプレートの詳細な概要ドキュメントです。

## 📋 目次

1. [プロジェクトの目的](#プロジェクトの目的)
2. [全体アーキテクチャ](#全体アーキテクチャ)
3. [ファイル構造詳細](#ファイル構造詳細)
4. [データフロー](#データフロー)
5. [主要コンポーネント](#主要コンポーネント)
6. [メモリ管理](#メモリ管理)
7. [カスタマイズポイント](#カスタマイズポイント)

---

## プロジェクトの目的

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

---

## 全体アーキテクチャ

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

---

## ファイル構造詳細

### 📂 プロジェクトルート

```
EEZ_Template/
├── EEZ_Template.ino          # メインプログラム（11KB）
├── LVGLv8withFlow.eez-project # EEZ Studioプロジェクト（30KB）
├── lv_conf.h                  # LVGL設定ファイル（25KB）
├── README.md                  # プロジェクトドキュメント（9KB）
├── LICENSE                    # MITライセンス（1KB）
├── .gitignore                 # Git除外設定
│
├── Docs/                      # ドキュメントフォルダ
│   ├── PROJECT_OVERVIEW.md    # このファイル
│   ├── QUICK_START.md         # クイックスタート
│   ├── CUSTOMIZATION_GUIDE.md # カスタマイズガイド
│   └── TECHNICAL_GUIDE.md     # 技術詳細
│
├── eez-flow.cpp (396KB)       # EEZ Flow言語エンジン
├── eez-flow.h (165KB)         # EEZ Flow定義
│
├── ui.c / ui.h                # UI初期化コード
├── screens.c / screens.h      # 画面定義
├── styles.c / styles.h        # スタイル定義
├── actions.h                  # アクション定義
├── images.c / images.h        # 画像データ
├── fonts.h                    # フォント定義
├── structs.h                  # 構造体定義
└── vars.h                     # 変数定義
```

### 📝 各ファイルの役割

#### 🔵 **EEZ_Template.ino** (メインプログラム)

**サイズ**: 11KB (347行)

**役割**: プロジェクトのエントリーポイント

**主要な機能**:
- Arduino標準関数（`setup()`, `loop()`）
- M5Unified初期化
- LVGL初期化とコールバック設定
- タッチ入力処理
- メモリ管理（SPIRAM）
- アプリケーションロジック

**重要な関数**:
```cpp
setup()                    // 起動時の初期化
loop()                     // メインループ
initLvglDisplay()          // LVGL表示初期化
initLvglTouch()            // タッチ入力初期化
lv_disp_flush()            // 画面更新コールバック
lv_indev_read()            // タッチ入力コールバック
allocateDisplayBuffer()    // SPIRAMバッファ確保
updateApplication()        // アプリケーション処理
```

**カスタマイズポイント**:
- `updateApplication()`: 独自の処理を追加
- 定数定義セクション: 画面サイズや更新間隔の調整

---

#### 🔵 **LVGLv8withFlow.eez-project** (EEZ Studioプロジェクト)

**サイズ**: 30KB (767行)

**役割**: EEZ Studioで編集するプロジェクトファイル

**内容**:
- 画面レイアウト定義
- Flow言語プログラム
- スタイル設定
- 変数定義
- アクション定義

**編集方法**:
1. EEZ Studioで開く
2. UI/Flowを編集
3. ビルド＆エクスポート
4. 生成ファイルをプロジェクトにコピー

**生成されるファイル**:
- `ui.c / ui.h`
- `screens.c / screens.h`
- `eez-flow.cpp / eez-flow.h`
- `styles.c / styles.h`
- `images.c / images.h`
- その他

---

#### 🔵 **lv_conf.h** (LVGL設定)

**サイズ**: 25KB (770行)

**役割**: LVGLライブラリの動作設定

**主要設定**:
```c
#define LV_COLOR_DEPTH 16        // 16ビットカラー
#define LV_USE_PERF_MONITOR 0    // パフォーマンスモニタ無効
#define LV_FONT_MONTSERRAT_14 1  // フォント有効化
#define LV_USE_LOG 0             // ログ無効
```

**カスタマイズ**:
- フォントの有効化/無効化
- ウィジェットの有効化
- デバッグ機能の切り替え
- メモリ設定

---

#### 🔵 **eez-flow.cpp / eez-flow.h** (Flow言語エンジン)

**サイズ**: 
- eez-flow.cpp: 396KB (11,255行)
- eez-flow.h: 165KB (4,800行)

**役割**: EEZ Studio Flow言語の実行エンジン

**機能**:
- Flow言語の解釈と実行
- 変数管理
- アクション実行
- イベント処理

**重要な関数**:
```cpp
eez_flow_init()  // Flow エンジン初期化
eez_flow_tick()  // Flow 実行（毎フレーム）
```

**注意**: 
- このファイルは**EEZ Studioが自動生成**します
- **手動で編集しないでください**

---

#### 🔵 **ui.c / ui.h** (UI初期化)

**サイズ**: ui.c 5.7KB (112行)

**役割**: UIシステムの初期化

**主要な関数**:
```cpp
void ui_init()   // UI初期化（setup()から呼ばれる）
void ui_tick()   // UI更新（loop()から呼ばれる）
```

**データ構造**:
```c
const uint8_t assets[]      // UIアセットデータ
native_var_t native_vars[]  // ネイティブ変数
ActionExecFunc actions[]    // アクション関数
```

**フロー**:
1. `ui_init()` → EEZ Flowエンジン初期化
2. `ui_tick()` → 毎フレーム呼ばれてFlowを実行

---

#### 🔵 **screens.c / screens.h** (画面定義)

**サイズ**: screens.c 3.6KB (117行)

**役割**: 各画面のLVGLオブジェクト定義

**構造**:
```c
lv_obj_t *objects[]          // すべてのUIオブジェクト
void create_screens()        // 画面作成
void tick_screen(int index)  // 画面更新
void create_screen_main()    // メイン画面作成
```

**画面管理**:
- 複数画面の定義
- 画面遷移制御
- オブジェクトイベント処理

---

#### 🔵 **styles.c / styles.h** (スタイル定義)

**サイズ**: styles.c 99B (9行)

**役割**: UIスタイル（色、フォント、サイズなど）の定義

**内容**:
```c
void apply_style()  // スタイルを適用
```

**カスタマイズ**:
- EEZ Studioでスタイルを編集
- 自動的に生成される

---

#### 🔵 **images.c / images.h** (画像データ)

**サイズ**: images.c 65B (6行)

**役割**: UIで使用する画像データ

**構造**:
```c
const ext_img_desc_t images[]  // 画像記述子配列
```

**画像追加**:
1. EEZ Studioで画像をインポート
2. ビルド時に自動的にC配列に変換
3. LVGLで表示

---

#### 🔵 **fonts.h / actions.h / structs.h / vars.h**

これらはEEZ Studioが生成する補助ファイルです。

| ファイル | 役割 |
|---------|------|
| `fonts.h` | カスタムフォント定義 |
| `actions.h` | アクション関数宣言 |
| `structs.h` | データ構造体定義 |
| `vars.h` | グローバル変数定義 |

---

## データフロー

### 起動シーケンス

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

### メインループの流れ

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

### 画面更新の流れ

```
[LVGLが画面更新を要求]
    │
    ▼
lv_disp_flush() コールバック
    │
    ├─ 更新領域 (area) を取得
    ├─ カラーバッファ (color_p) を取得
    │
    └─▶ M5.Display.pushImageDMA()
         │
         ├─ DMAで画面転送（非ブロッキング）
         ├─ 転送中もCPUは他の処理可能
         │
         └─▶ lv_disp_flush_ready()    // LVGL に完了通知
```

### タッチ入力の流れ

```
[ユーザーがタッチ]
    │
    ▼
M5.update()
    │
    └─ タッチセンサーから座標読取
         │
         ▼
lv_indev_read() コールバック
    │
    ├─ touch_detail = M5.Touch.getDetail()
    │
    └─ LVGLにタッチ情報を返す
        ├─ data->state = PRESSED or RELEASED
        ├─ data->point.x = X座標
        └─ data->point.y = Y座標
             │
             ▼
         [LVGLがイベント処理]
             │
             └─▶ ボタン、スライダーなどが反応
```

---

## 主要コンポーネント

### 1. M5Unified

**役割**: M5Stack統合ライブラリ

**提供機能**:
- ディスプレイ制御（`M5.Display`）
- タッチスクリーン（`M5.Touch`）
- ボタン入力（`M5.BtnA`, `M5.BtnB`, `M5.BtnC`）
- 電源管理（`M5.Power`）
- I2C、SPI、UART

**使用例**:
```cpp
M5.Display.setBrightness(128);       // 明るさ設定
M5.Display.pushImageDMA(x, y, w, h, buf);  // 画面転送
auto touch = M5.Touch.getDetail();   // タッチ情報取得
```

---

### 2. LVGL (Light and Versatile Graphics Library)

**バージョン**: 8.3.11

**役割**: 高機能GUIライブラリ

**主要機能**:
- ウィジェット（ボタン、スライダー、ラベルなど）
- スタイル管理（色、フォント、サイズ）
- アニメーション
- イベント処理
- スクリーン管理

**ウィジェット例**:
- `lv_btn`: ボタン
- `lv_label`: テキストラベル
- `lv_slider`: スライダー
- `lv_arc`: 円弧
- `lv_chart`: グラフ
- `lv_table`: テーブル

**使用例**:
```cpp
lv_obj_t *btn = lv_btn_create(lv_scr_act());
lv_obj_t *label = lv_label_create(btn);
lv_label_set_text(label, "Push Me!");
```

---

### 3. EEZ Studio Flow

**役割**: 視覚的プログラミング環境

**特徴**:
- ノードベースのFlowエディタ
- ビジュアルUIデザイナー
- 変数、アクション、イベント管理
- C/C++コード自動生成

**Flowの構成要素**:

| 要素 | 説明 |
|------|------|
| **Start** | 開始ノード |
| **Action** | 処理実行 |
| **Condition** | 条件分岐 |
| **Loop** | ループ |
| **Set Variable** | 変数設定 |
| **Delay** | 待機 |

**例: カウンターアプリのFlow**:
```
[Start] → [Condition: enabled?]
              ├─ Yes → [Increment counter]
              │         └─ [Update label]
              └─ No  → [Skip]
```

---

### 4. メモリアロケータ

**使用メモリ**:

| 領域 | サイズ | 用途 |
|------|--------|------|
| **内部RAM** | 512KB | プログラム実行、スタック、ヒープ |
| **SPIRAM (PSRAM)** | 8MB | 画面バッファ、大きなデータ |
| **Flash** | 16MB | プログラムコード、定数データ |

**画面バッファの確保**:
```cpp
g_color_buf = (lv_color_t *)heap_caps_malloc(
    sizeof(lv_color_t) * SCREEN_BUFFER_SIZE,  // 1280 x 720 x 2 = 1.8MB
    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT       // SPIRAMに確保
);
```

**メモリ使用量の確認**:
```cpp
ESP.getFreeHeap()    // 内部RAM空き容量
ESP.getFreePsram()   // SPIRAM空き容量
```

---

## メモリ管理

### メモリマップ

```
┌─────────────────────────────────────────┐
│           ESP32-P4 メモリ構成            │
├─────────────────────────────────────────┤
│  内部RAM (512KB)                         │
│  ├─ Program Stack                       │
│  ├─ Heap (変数、小さなバッファ)          │
│  └─ LVGL内部データ                       │
├─────────────────────────────────────────┤
│  SPIRAM (8MB) - 外部拡張RAM              │
│  ├─ 画面バッファ (1.8MB)                 │
│  ├─ 大きな配列データ                     │
│  └─ 画像データ                           │
├─────────────────────────────────────────┤
│  Flash (16MB)                           │
│  ├─ プログラムコード (.ino → .bin)      │
│  ├─ LVGLライブラリ                       │
│  ├─ M5Unifiedライブラリ                  │
│  └─ 定数データ (fonts, images)          │
└─────────────────────────────────────────┘
```

### 典型的なメモリ使用量

**コンパイル後**:
```
プログラムストレージ: 約 600KB / 16MB (4%)
動的メモリ (RAM):     約 80KB / 512KB (16%)
```

**実行時**:
```
┌──────────────────────────────────────┐
│ 内部RAM (512KB)                      │
├──────────────────────────────────────┤
│ 使用済み:     約 100KB               │
│ 空き:         約 400KB               │
└──────────────────────────────────────┘

┌──────────────────────────────────────┐
│ SPIRAM (8MB)                         │
├──────────────────────────────────────┤
│ 画面バッファ: 1.8MB                  │
│ その他:       約 0.2MB               │
│ 空き:         約 6MB                 │
└──────────────────────────────────────┘
```

### メモリ最適化のヒント

1. **大きなバッファはSPIRAMに**
   ```cpp
   uint8_t *buf = (uint8_t *)heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
   ```

2. **不要な機能はlv_conf.hで無効化**
   ```c
   #define LV_USE_PERF_MONITOR 0
   #define LV_USE_LOG 0
   ```

3. **画像は圧縮形式を使用**
   - PNG/JPGをそのまま使用（デコードは実行時）

4. **フォントは必要なものだけ有効化**
   ```c
   #define LV_FONT_MONTSERRAT_14 1
   #define LV_FONT_MONTSERRAT_48 0  // 不要なら無効化
   ```

---

## カスタマイズポイント

### 🎯 レベル1: 簡単なカスタマイズ

#### 1. 定数の変更

**ファイル**: `EEZ_Template.ino`

```cpp
// 画面の明るさを変更
#define DEFAULT_BRIGHTNESS 200  // 0〜255

// アプリ更新間隔を変更
#define APP_UPDATE_INTERVAL_MS 50  // ミリ秒
```

#### 2. アプリケーション処理の追加

**ファイル**: `EEZ_Template.ino`

```cpp
void updateApplication()
{
    unsigned long currentTime = millis();
    
    if (currentTime - g_lastUpdateTime < APP_UPDATE_INTERVAL_MS) {
        return;
    }
    
    g_lastUpdateTime = currentTime;
    
    // ここに独自の処理を追加
    // 例: センサー読み取り
    float temperature = readTemperature();
    updateTemperatureDisplay(temperature);
}
```

---

### 🎯 レベル2: UIのカスタマイズ

#### EEZ Studioでの編集

1. **EEZ Studioを開く**
   ```
   File → Open Project → LVGLv8withFlow.eez-project
   ```

2. **ページ編集**
   - 左パネルから「Pages」を選択
   - ウィジェットをドラッグ&ドロップ
   - プロパティを編集

3. **Flowプログラミング**
   - 「Flow」タブを開く
   - ノードを追加・接続
   - ロジックを構築

4. **エクスポート**
   ```
   Build → Build & Export
   ```

5. **ファイルをコピー**
   - 生成された全ファイルをプロジェクトフォルダに上書き

---

### 🎯 レベル3: 高度なカスタマイズ

#### WiFi機能の追加

**手順**:

1. **WiFi.hをインクルード**
   ```cpp
   #include <WiFi.h>
   ```

2. **WiFi接続関数を追加**
   ```cpp
   bool connectWiFi(const char* ssid, const char* password) {
       WiFi.begin(ssid, password);
       int timeout = 30;
       while (WiFi.status() != WL_CONNECTED && timeout > 0) {
           delay(500);
           timeout--;
       }
       return WiFi.status() == WL_CONNECTED;
   }
   ```

3. **setup()で接続**
   ```cpp
   void setup() {
       // ... 既存の初期化 ...
       
       if (connectWiFi("your_ssid", "your_password")) {
           Serial.println("WiFi connected!");
       }
   }
   ```

#### HTTP通信の追加

```cpp
#include <HTTPClient.h>

void fetchData() {
    HTTPClient http;
    http.begin("http://api.example.com/data");
    int httpCode = http.GET();
    
    if (httpCode == 200) {
        String payload = http.getString();
        // データを処理
    }
    
    http.end();
}
```

#### センサーの追加

```cpp
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void setup() {
    // ... 既存の初期化 ...
    
    if (bme.begin(0x76)) {
        Serial.println("BME280 initialized");
    }
}

void updateApplication() {
    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    
    // UIに表示
    updateDisplay(temp, hum);
}
```

---

## トラブルシューティング

### コンパイルエラー

| エラー | 原因 | 解決策 |
|--------|------|--------|
| `lv_conf.h not found` | lv_conf.hが見つからない | プロジェクトフォルダに配置 |
| `undefined reference to ui_init` | eez-flow.cppが未コンパイル | すべてのファイルが同じフォルダにあることを確認 |
| `PSRAM not found` | PSRAMが無効 | ボード設定でPSRAM: Enabled |

### 実行時エラー

| 症状 | 原因 | 解決策 |
|------|------|--------|
| 画面が真っ暗 | 初期化失敗 | シリアルモニタでエラーを確認 |
| タッチが反応しない | タッチドライバ未初期化 | `initLvglTouch()`が呼ばれているか確認 |
| メモリ不足 | SPIRAMが無効 | ボード設定でPSRAMを有効化 |

---

## 参考資料

### 公式ドキュメント

- [EEZ Studio](https://www.envox.eu/eez-studio/)
- [EEZ Studio GitHub](https://github.com/eez-open/studio)
- [LVGL Docs](https://docs.lvgl.io/8.3/)
- [M5Unified GitHub](https://github.com/m5stack/M5Unified)

### チュートリアル

- [LVGL Quick Start](https://docs.lvgl.io/8.3/get-started/quick-overview.html)
- [EEZ Studio Tutorial](https://github.com/eez-open/studio/wiki)

---

## まとめ

このプロジェクトは、以下の要素で構成されています：

1. **EEZ_Template.ino**: メインプログラム（カスタマイズの起点）
2. **EEZ Studio**: ビジュアルUI/Flow開発ツール
3. **LVGL**: 高機能GUIライブラリ
4. **M5Unified**: ハードウェア統合ライブラリ

**開発の流れ**:
```
EEZ Studioで設計 → エクスポート → Arduinoでコンパイル → Tab5で実行
```

このテンプレートをベースに、独自のアプリケーションを自由に開発してください！

---

**作成日**: 2026-01-18  
**バージョン**: 1.0.0  
**作者**: macole
