# Tab5 Arduino Basic - LVGLアプリケーション開発ガイド

このプロジェクトは、M5Stack Tab5でLVGLを使用したGUIアプリケーションを開発するためのベーシックなテンプレートです。このプログラムをベースに、独自のLVGLアプリケーションを構築できます。

## 📋 目次

- [概要](#概要)
- [必要な環境](#必要な環境)
- [依存関係](#依存関係)
- [プロジェクト構造](#プロジェクト構造)
- [プログラムの解説](#プログラムの解説)
- [カスタマイズ方法](#カスタマイズ方法)
- [SquareLine Studioとの連携](#squareline-studioとの連携)
- [トラブルシューティング](#トラブルシューティング)

## 概要

このプログラムは、M5Stack Tab5のMIPI-DSIディスプレイとタッチパネルを使用して、LVGL（Light and Versatile Graphics Library）によるGUIアプリケーションを実現します。

### 主な機能

- LVGLとM5Unifiedの統合
- タッチパネル入力のサポート
- DMA転送による高速描画
- SPIRAMを使用した描画バッファの確保
- 90度回転ディスプレイのサポート

### サンプル機能

- ボタンによる自動カウント機能のON/OFF
- アーク（円形プログレスバー）とラベルによるカウンター表示
- スライダーによる画面の明るさ調整

## 必要な環境

### ハードウェア

- M5Stack Tab5 (ESP32-P4)

### ソフトウェア

- Arduino IDE 2.x または Arduino CLI
- ESP32 Arduino Core >= 3.2 (3.3.0-alpha1でも動作確認済み)

## 依存関係

以下のライブラリが必要です。Arduino IDEのライブラリマネージャーからインストールしてください。

```
M5Unified >= 0.2.10
LVGL = 8.3.11
```

**注意**: M5UnifiedはM5GFXを含む統一ライブラリです。M5GFXを個別にインストールする必要はありません。

### インストール方法

Arduino CLIを使用する場合：

```bash
arduino-cli lib install "M5Unified@0.2.10"
arduino-cli lib install "lvgl@8.3.11"
```

## プロジェクト構造

```
tab5_arduino_basic/
├── tab5_arduino_basic.ino    # メインプログラム
├── ui.h                       # UIヘッダーファイル（SquareLine Studio生成）
├── ui.c                       # UI実装ファイル（SquareLine Studio生成）
├── ui_Screen1.c              # 画面1の実装
├── ui_helpers.h/.c           # UIヘルパー関数
├── ui_events.h               # イベント定義
├── ui_comp_hook.c            # コンポーネントフック
├── ui_font_*.c               # フォントデータ
├── lv_conf.h                 # LVGL設定ファイル
├── pins_config.h             # ピン設定（必要に応じて）
├── CMakeLists.txt            # CMake設定
├── partitions.csv            # パーティション設定
└── SLS_Project/              # SquareLine Studioプロジェクト
    ├── Tab5_Basic.spj
    ├── Tab5_Basic.sll
    └── Themes.slt
```

## プログラムの解説

### 1. 定数定義

```cpp
#define SERIAL_BAUD_RATE 115200  // シリアル通信のボーレート
#define COUNTER_MAX_VALUE 1000   // カウンターの最大値
#define DEFAULT_BRIGHTNESS 255   // 初期画面の明るさ（0-255）
#define LVGL_TIMER_DELAY_MS 1    // LVGLタイマーハンドラーの遅延時間
#define MAX_TOUCH_POINTS 3        // タッチポイントの最大数
```

これらの定数は、プログラムの動作を制御します。必要に応じて変更してください。

### 2. グローバル変数

```cpp
static uint16_t g_counter = 0;                   // 自動カウント用のカウンター
static bool g_automateEnabled = false;           // 自動カウント機能の有効/無効フラグ
static lv_disp_draw_buf_t g_draw_buf;            // LVGL描画バッファ
static lv_color_t *g_color_buf = nullptr;        // LVGL描画用のカラーバッファ
```

**注意**: M5Unifiedを使用する場合、`M5GFX display;`のようなディスプレイオブジェクトの宣言は不要です。`M5.Display`と`M5.Touch`を通じてアクセスします。

`g_`プレフィックスは、グローバル変数であることを示します。

### 3. LVGLコールバック関数

#### `lv_disp_flush()` - ディスプレイフラッシュコールバック

LVGLが描画バッファの内容をディスプレイに転送する際に呼ばれます。DMAを使用して高速転送を実現しています。M5Unifiedでは`M5.Display`を通じてアクセスします。

```cpp
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t width = (area->x2 - area->x1 + 1);
    uint32_t height = (area->y2 - area->y1 + 1);
    M5.Display.pushImageDMA(area->x1, area->y1, width, height, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}
```

#### `lv_indev_read()` - タッチ入力読み取りコールバック

タッチパネルの状態を読み取り、LVGLに通知します。M5Unifiedでは`M5.Touch.getDetail()`を使用してタッチ情報を取得します。

```cpp
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    auto touch_detail = M5.Touch.getDetail();
    if (touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_detail.x;
        data->point.y = touch_detail.y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
```

### 4. 初期化関数

#### `initLvglDisplay()` - LVGLディスプレイの初期化

LVGLライブラリの初期化、描画バッファの確保、ディスプレイドライバーの設定を行います。

**重要な設定：**

- `LVGL_LCD_BUF_SIZE`: 描画バッファのサイズ（`pins_config.h`で定義）
- `EXAMPLE_LCD_H_RES`: 水平解像度（`pins_config.h`で定義）
- `EXAMPLE_LCD_V_RES`: 垂直解像度（`pins_config.h`で定義）
- `disp_drv.sw_rotate = 1`: ソフトウェア回転を有効化
- `disp_drv.rotated = LV_DISP_ROT_90`: 90度回転

#### `initLvglTouch()` - LVGLタッチ入力の初期化

タッチパネルの入力デバイスドライバーを設定します。

### 5. メインループ処理関数

#### `updateAutomation()` - 自動カウント機能の更新

ボタンの状態に応じてカウンターを更新し、UIコンポーネント（アーク、ラベル）を更新します。

#### `updateBrightness()` - 画面の明るさの更新

スライダーの値に応じてディスプレイの明るさを設定します。

### 6. `setup()` 関数

初期化処理の順序：

1. M5Unifiedの初期化（`M5.begin()`でディスプレイとタッチパネルを含む全デバイスを初期化）
2. シリアル通信の初期化（デバッグ用）
3. LVGLディスプレイの初期化
4. LVGLタッチ入力の初期化
5. UIの初期化
6. ディスプレイの明るさを設定（`M5.Display.setBrightness()`）

**M5Unifiedの初期化**:
```cpp
auto cfg = M5.config();  // デフォルト設定を取得
M5.begin(cfg);  // M5Unifiedの初期化
```

カスタム設定が必要な場合は、`cfg`を編集してから`M5.begin()`を呼び出します。

### 7. `loop()` 関数

メインループでは、以下の処理を繰り返し実行します：

1. `M5.update()`: M5Unifiedの更新（タッチ入力などの処理）
2. `lv_timer_handler()`: LVGLのタイマーハンドラー（イベント処理とアニメーション更新）
3. `delay(LVGL_TIMER_DELAY_MS)`: CPU負荷軽減のための短い遅延
4. `updateAutomation()`: 自動カウント機能の更新
5. `updateBrightness()`: 画面の明るさの更新

**重要**: `M5.update()`は`loop()`の最初で呼び出す必要があります。これにより、タッチ入力などのM5Unifiedの機能が正しく動作します。

## カスタマイズ方法

### 新しいUIコンポーネントを追加する

1. SquareLine StudioでUIをデザインする（後述）
2. 生成されたコードをプロジェクトに追加
3. `loop()`関数内でコンポーネントを操作

例：ボタンのクリックイベントを処理する

```cpp
void loop()
{
    lv_timer_handler();
    delay(LVGL_TIMER_DELAY_MS);
    
    // ボタンのクリック状態を確認
    if (lv_obj_has_state(ui_Button1, LV_STATE_PRESSED)) {
        // ボタンが押されたときの処理
    }
    
    updateAutomation();
    updateBrightness();
}
```

### 新しい画面を追加する

1. SquareLine Studioで新しい画面を作成
2. 生成された`ui_Screen2.c`などのファイルを追加
3. `ui.h`に新しい画面の宣言が追加される
4. 画面切り替え処理を実装

例：画面2に切り替える

```cpp
lv_scr_load(ui_Screen2);
```

### カスタムイベントハンドラーを追加する

`ui_events.h`にイベントハンドラーを定義し、SquareLine Studioでイベントに紐付けます。

例：

```cpp
void my_button_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        // ボタンがクリックされたときの処理
    }
}
```

### 描画バッファサイズの調整

`pins_config.h`で`LVGL_LCD_BUF_SIZE`を定義します。大きいほど滑らかな描画が可能ですが、メモリ使用量が増えます。

```cpp
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES / 10)
```

### ディスプレイの回転角度を変更する

`initLvglDisplay()`関数内で、`disp_drv.rotated`の値を変更します：

```cpp
disp_drv.rotated = LV_DISP_ROT_0;   // 0度
disp_drv.rotated = LV_DISP_ROT_90;  // 90度（デフォルト）
disp_drv.rotated = LV_DISP_ROT_180; // 180度
disp_drv.rotated = LV_DISP_ROT_270; // 270度
```

## SquareLine Studioとの連携

このプロジェクトは、SquareLine StudioでUIをデザインし、コードを生成するワークフローに対応しています。

### SquareLine Studioのセットアップ

1. [SquareLine Studio](https://squareline.io/)をダウンロード・インストール
2. LVGL 8.3.11を選択してプロジェクトを作成
3. `SLS_Project/Tab5_Basic.spj`を開く

### UIの編集手順

1. SquareLine StudioでUIをデザイン
2. 「Export」→「Arduino」を選択
3. 生成されたファイルをプロジェクトにコピー
4. `tab5_arduino_basic.ino`でUIコンポーネントを操作

### 生成されるファイル

- `ui.h`: UIコンポーネントの宣言
- `ui.c`: UIの初期化関数
- `ui_Screen*.c`: 各画面の実装
- `ui_font_*.c`: 使用するフォントデータ
- `ui_helpers.c/h`: UIヘルパー関数

### UIコンポーネントへのアクセス

SquareLine Studioで作成したUIコンポーネントは、`ui.h`で宣言されたグローバル変数としてアクセスできます。

例：

```cpp
// ラベルのテキストを変更
lv_label_set_text(ui_Label1, "Hello World");

// スライダーの値を取得
int32_t value = lv_slider_get_value(ui_Slider1);

// ボタンの状態を確認
bool is_checked = lv_obj_has_state(ui_Button1, LV_STATE_CHECKED);
```

## トラブルシューティング

### ディスプレイが表示されない

1. `M5.begin()`が正しく呼ばれているか確認
   - `setup()`関数の最初で`M5.begin()`を呼び出しているか
   - カスタム設定を使用している場合は、設定が正しいか確認

2. `lv_conf.h`の設定を確認
   - `LV_COLOR_DEPTH`が16に設定されているか
   - `LV_COLOR_16_SWAP`が1に設定されているか
   - `LV_MEM_CUSTOM`が1に設定されているか

3. `pins_config.h`の設定を確認
   - `LVGL_LCD_BUF_SIZE`が適切に設定されているか
   - `EXAMPLE_LCD_H_RES`と`EXAMPLE_LCD_V_RES`が正しいか

4. シリアルモニターでエラーメッセージを確認

### タッチが反応しない

1. `M5.update()`が`loop()`関数の最初で呼ばれているか確認
2. `lv_indev_read()`関数が正しく実装されているか確認
   - `M5.Touch.getDetail()`を使用しているか確認
3. `initLvglTouch()`が`setup()`で呼ばれているか確認
4. `M5.begin()`でタッチパネルが正しく初期化されているか確認
5. タッチパネルの接続を確認

### メモリ不足エラー

1. 描画バッファサイズを小さくする
2. 使用するフォントの数を減らす
3. 画像アセットのサイズを小さくする
4. `lv_conf.h`でメモリ関連の設定を調整

### コンパイルエラー

1. 必要なライブラリがインストールされているか確認
2. Arduino IDEのボード設定が正しいか確認（ESP32-P4）
3. `lv_conf.h`の設定が正しいか確認

### パフォーマンスの問題

1. `LVGL_TIMER_DELAY_MS`の値を調整（1msが推奨）
2. 描画バッファサイズを大きくする
3. 不要なアニメーションを無効化
4. `lv_conf.h`でパフォーマンス関連の設定を最適化

## M5UnifiedとM5GFXの違い

このプロジェクトは**M5Unified**を使用しています。M5UnifiedはM5Stackの統一APIライブラリで、以下の利点があります：

- **統一されたAPI**: すべてのM5Stackデバイスで同じAPIを使用可能
- **自動初期化**: `M5.begin()`でディスプレイ、タッチ、その他のデバイスを自動初期化
- **簡潔なコード**: `M5.Display`、`M5.Touch`など、直感的なアクセス方法
- **M5GFXを含む**: M5UnifiedはM5GFXを含むため、個別にインストールする必要がない

M5GFXを直接使用する場合は、`#include <M5GFX.h>`を使用し、`M5GFX display;`オブジェクトを宣言して使用します。

## 参考資料

- [LVGL公式ドキュメント](https://docs.lvgl.io/)
- [M5Unified GitHub](https://github.com/m5stack/M5Unified)
- [M5GFX GitHub](https://github.com/m5stack/M5GFX)
- [SquareLine Studio](https://squareline.io/)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)

## ライセンス

このプロジェクトはMITライセンスで公開されています。詳細は`LICENSE`ファイルを参照してください。

---

**最終更新日**: 2025年1月  
**対象デバイス**: M5Stack Tab5 (ESP32-P4)  
**LVGLバージョン**: 8.3.11  
**M5Unifiedバージョン**: >= 0.2.10

