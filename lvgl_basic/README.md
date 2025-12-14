# M5Stack Tab5 LVGL Basic Demo

M5Stack Tab5でLVGLを使用した基本的なUIデモプログラムです。**SquareLine Studioなどの外部ツールを使わず、コードだけでUIを作成**しています。初心者でも理解しやすいように、詳細なコメントと関数分割を行っています。

## 概要

このプログラムは、LVGL（Light and Versatile Graphics Library）を使用して、M5Stack Tab5の大画面（1280x720）にUIを表示します。

### 主な機能

- **円形ゲージ（アーク）**: 0-1000の値を表示
- **自動カウント機能**: ボタンでON/OFF切り替え可能
- **明るさ調整**: スライダーでバックライトの明るさを調整
- **タッチ操作**: ボタンやスライダーをタッチで操作可能

## ハードウェア要件

- **M5Stack Tab5** (1280x720 LCD, タッチパネル付き)
- **ESP32-P4** チップ

## ライブラリ依存関係

### 必須ライブラリ

```cpp
#include <M5Unified.h>    // M5Stack統一ライブラリ
#include <M5GFX.h>        // M5Stack GFXライブラリ（M5Unifiedに含まれる）
#include "lvgl.h"         // LVGL 8.3.11
```

### インストール方法

Arduino IDEのライブラリマネージャーから以下をインストール：

1. **M5Unified** (最新版推奨)
2. **LVGL** (バージョン 8.3.11)

または、PlatformIOを使用する場合：

```ini
lib_deps = 
    m5stack/M5Unified@^0.2.10
    lvgl/lvgl@^8.3.11
```

## ビルド設定

### Arduino IDE

1. **ボード**: "ESP32P4 Dev Module"
2. **PSRAM**: "Enabled"（必須）
3. **Flash Size**: "16MB (128Mb)"
4. **Partition Scheme**: "Custom"（`partitions.csv`を使用）
5. **USB CDC on boot**: "Enabled"
6. **Upload Mode**: "UART / Hardware CDC"
7. **USB Mode**: "Hardware CDC and JTAG"

### lv_conf.h の設定

`lv_conf.h`で以下の設定を確認してください：

```c
#define LV_COLOR_DEPTH 16          // 16ビットカラー
#define LV_COLOR_16_SWAP 1         // 色のバイト順をスワップ
#define LV_MEM_CUSTOM 1             // カスタムメモリ管理を使用
#define LV_TICK_CUSTOM 1            // カスタムティック（millis()を使用）
```

## プログラム構造

### 主要なファイル

- **`lvgl_basic.ino`**: メインプログラム
- **`config.h`**: 設定ファイル（画面解像度など）
- **`lv_conf.h`**: LVGL設定ファイル

### 主要な関数

#### `setup()`
プログラム開始時に1回だけ実行される初期化関数：
- シリアル通信の初期化
- M5Stackの初期化
- LVGLの初期化
- UIの作成

#### `loop()`
プログラム開始後、繰り返し実行される関数：
- LVGLタイマーハンドラーの呼び出し
- 自動カウント処理
- 明るさ調整処理

#### `initializeLVGL()`
LVGLライブラリの初期化：
- LVGLの初期化
- 描画バッファの確保（PSRAM）
- ディスプレイドライバの登録
- タッチ入力デバイスの登録

#### `createMainScreen()`
メイン画面のUIを作成：
- 画面オブジェクトの作成
- 各種ウィジェット（ラベル、ボタン、スライダーなど）の作成と配置

#### `lv_disp_flush()`
LVGLから呼ばれる描画転送コールバック：
- 描画バッファの内容を画面に転送
- DMA転送を使用して高速化

#### `lv_indev_read()`
LVGLから呼ばれるタッチ入力読み取りコールバック：
- タッチパネルから座標を取得
- LVGLにタッチ状態を通知

## コードの解説

### 1. LVGLの初期化

```cpp
void initializeLVGL(void) {
    lv_init();  // LVGLライブラリの初期化
    
    // 描画バッファをPSRAMに確保
    buf = (lv_color_t*)heap_caps_malloc(
        sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    
    // ディスプレイドライバの設定
    // ...
}
```

**ポイント**:
- 大きな描画バッファをPSRAMに確保することで、メインメモリを節約
- ディスプレイドライバで画面への転送方法を指定
- ソフトウェア回転で縦画面を横表示に変換

### 2. UIの作成

```cpp
void createMainScreen(void) {
    // 画面を作成
    screenMain = lv_obj_create(NULL);
    
    // ラベルを作成
    labelTitle = lv_label_create(screenMain);
    lv_label_set_text(labelTitle, "LVGL Basic Demo");
    
    // ボタンを作成
    buttonAutomate = lv_btn_create(screenMain);
    // ...
}
```

**ポイント**:
- すべてのウィジェットは親オブジェクト（画面）の子として作成
- `lv_obj_align()`で位置を指定
- `lv_obj_set_style_*()`でスタイルを設定

### 3. ループ処理

```cpp
void loop() {
    lv_timer_handler();  // 重要：定期的に呼ぶ必要がある
    delay(1);
    
    // アプリケーションロジック
    // ...
}
```

**ポイント**:
- `lv_timer_handler()`は定期的に呼ぶ必要がある（通常1-5ms間隔）
- これにより、アニメーション、イベント処理、描画更新が行われる
- 長時間ブロッキング処理は避ける

## カスタマイズ方法

### UI要素の追加

新しいウィジェットを追加する例：

```cpp
// 新しいラベルを追加
lv_obj_t* myLabel = lv_label_create(screenMain);
lv_label_set_text(myLabel, "Hello World");
lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 200);
```

### 色の変更

```cpp
// ラベルの色を変更
lv_obj_set_style_text_color(labelValue, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
```

### フォントの変更

```cpp
// フォントを変更（デフォルトフォントを使用）
lv_obj_set_style_text_font(labelTitle, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
```

### イベントハンドラーの追加

```cpp
// ボタンクリック時のイベントハンドラー
static void button_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        Serial.println("Button clicked!");
    }
}

// イベントハンドラーを登録
lv_obj_add_event_cb(buttonAutomate, button_event_handler, LV_EVENT_CLICKED, NULL);
```

## トラブルシューティング

### 画面が表示されない

**原因と対処法**:
- PSRAMが有効になっているか確認
- バックライトの明るさを確認（`display.setBrightness(255)`）
- シリアルモニターでエラーメッセージを確認

### タッチが反応しない

**原因と対処法**:
- タッチパネルの接続を確認
- `lv_indev_read()`が正しく実装されているか確認
- シリアルモニターでタッチ座標をデバッグ出力

### メモリ不足エラー

**原因と対処法**:
- PSRAMが有効になっているか確認
- 描画バッファサイズを小さくする（`LVGL_LCD_BUF_SIZE`を調整）
- 不要なウィジェットを削除

### 描画が遅い

**原因と対処法**:
- `lv_timer_handler()`の呼び出し間隔を確認（1-5ms推奨）
- 描画バッファサイズを確認
- 不要なアニメーションを無効化

## パフォーマンス

- **描画バッファ**: 全画面分（720×1280×2バイト = 約1.8MB）をPSRAMに確保
- **更新レート**: `lv_timer_handler()`を1ms間隔で呼び出し
- **タッチ応答**: リアルタイム処理

## 学習リソース

### LVGL公式ドキュメント

- [LVGL公式サイト](https://lvgl.io/)
- [LVGLドキュメント](https://docs.lvgl.io/)
- [LVGL APIリファレンス](https://docs.lvgl.io/8.3/API/index.html)

### 参考になるトピック

1. **ウィジェットの作成**: `lv_obj_create()`, `lv_btn_create()`, `lv_label_create()`など
2. **スタイル設定**: `lv_obj_set_style_*()`関数群
3. **イベント処理**: `lv_obj_add_event_cb()`
4. **アニメーション**: `lv_anim_*()`関数群

## 今後の拡張案

- 複数画面の実装
- カスタムフォントの追加
- アニメーション効果
- データバインディング
- 設定画面の追加
- WiFi接続とNTP時刻同期

## ライセンス

このプログラムは教育目的で作成されています。自由に使用・改変してください。

## 参考情報

### 関連ファイル

- `lvgl_basic.ino`: メインプログラム
- `config.h`: 設定ファイル
- `lv_conf.h`: LVGL設定ファイル
- `partitions.csv`: パーティション設定

### ビルドに必要な設定

- ESP32-P4用のボード定義
- PSRAM有効化
- カスタムパーティションスキーム

---

**注意**: このプログラムは、SquareLine Studioなどの外部ツールを使わず、純粋にコードだけでUIを作成しています。初心者がLVGLの基本を学ぶのに最適です。
