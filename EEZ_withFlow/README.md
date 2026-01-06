# M5Stack Tab5 EEZ-Studio UI プロジェクト

このプロジェクトは、EEZ StudioでデザインしたLVGL UIをM5Stack Tab5で実行するためのArduinoプロジェクトです。

## 📋 目次

- [概要](#概要)
- [ハードウェア要件](#ハードウェア要件)
- [必要なライブラリ](#必要なライブラリ)
- [セットアップ手順](#セットアップ手順)
- [プロジェクト構造](#プロジェクト構造)
- [重要な設定](#重要な設定)
- [コード解説](#コード解説)
- [トラブルシューティング](#トラブルシューティング)
- [カスタマイズ](#カスタマイズ)

## 概要

EEZ Studioは、LVGLベースのグラフィカルUIを視覚的にデザインできる強力なツールです。このプロジェクトでは、EEZ Studioで作成したUIをM5Stack Tab5に移植し、M5Unifiedライブラリを使用して実行します。

### 主な特徴

- ✅ M5Unified.hライブラリ対応
- ✅ LVGL 8.3.11使用
- ✅ 1280x720解像度サポート
- ✅ タッチ入力対応
- ✅ DMA高速描画
- ✅ RGB565カラー（16ビット）

## ハードウェア要件

- **M5Stack Tab5** (ESP32-P4搭載)
  - ディスプレイ: 10.1インチ 1280x720 IPS液晶
  - タッチパネル: 静電容量式タッチスクリーン
  - メモリ: PSRAMサポート

## 必要なライブラリ

Arduino IDEまたはPlatformIOで以下のライブラリをインストールしてください：

### 必須ライブラリ

| ライブラリ名 | バージョン | 説明 |
|------------|----------|------|
| M5Unified | >= 0.2.10 | M5Stack統合ライブラリ |
| LVGL | 8.3.11 | LVGLグラフィックライブラリ |

### Arduino IDEでのインストール

```bash
# Arduino Library Managerから以下をインストール
- M5Unified
- lvgl (バージョン8.3.11を指定)
```

## セットアップ手順

### 1. プロジェクトのクローン

```bash
git clone <repository-url>
cd Tab5_GUI/EEZ-Studio
```

### 2. EEZ Studio UIファイルの配置

EEZ Studioでエクスポートしたファイルを `ui/` ディレクトリに配置します：

- `ui.c`, `ui.h`
- `screens.c`, `screens.h`
- `styles.c`, `styles.h`
- `images.c`, `images.h`
- その他生成されたファイル

### 3. lv_conf.hの設定確認

`ui/lv_conf.h` で以下の設定を確認：

```c
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1
#define LV_MEM_CUSTOM 1
#define LV_TICK_CUSTOM 1
#define LV_FONT_MONTSERRAT_24 1  // 使用するフォントを有効化
```

### 4. Arduino IDEでコンパイル・アップロード

1. `ui/ui.ino` を開く
2. ボード設定: **M5Stack Tab5**
3. コンパイルしてアップロード

## プロジェクト構造

```
EEZ-Studio/
├── README.md                 # このファイル
├── LICENSE                   # ライセンス情報
├── libraries/                # ライブラリ配置場所（参考用）
│   ├── lv_conf.h            # LVGL設定ファイル（参考）
│   └── TFT_eSPI/            # TFT_eSPI設定（使用しない）
└── ui/                      # メインプロジェクト
    ├── ui.ino               # Arduinoメインスケッチ
    ├── lv_conf.h            # LVGL設定ファイル（実際に使用）
    ├── ui.c, ui.h           # EEZ Studio生成ファイル
    ├── screens.c, screens.h # 画面定義
    ├── styles.c, styles.h   # スタイル定義
    ├── images.c, images.h   # 画像データ
    ├── fonts.h              # フォント定義
    ├── actions.h            # アクション定義
    └── vars.h               # 変数定義
```

## 重要な設定

### lv_conf.h の主要設定

#### カラー設定

```c
#define LV_COLOR_DEPTH 16        // RGB565カラー
#define LV_COLOR_16_SWAP 1       // バイトスワップ有効（Tab5推奨）
```

#### メモリ設定

```c
#define LV_MEM_CUSTOM 1          // カスタムメモリ管理使用
#define LV_MEM_CUSTOM_INCLUDE <stdlib.h>
#define LV_MEM_CUSTOM_ALLOC   malloc
#define LV_MEM_CUSTOM_FREE    free
```

#### タイマー設定

```c
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
```

#### フォント設定

使用するフォントサイズを有効化：

```c
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_24 1
// 必要に応じて他のサイズも有効化
```

### ui.ino の主要設定

#### 画面解像度

```c
static const uint16_t screenWidth  = 720;   // 縦向き基準
static const uint16_t screenHeight = 1280;  // LVGL側で90度回転
```

#### ディスプレイドライバ設定

```c
disp_drv.sw_rotate = 1;              // ソフトウェア回転有効
disp_drv.rotated = LV_DISP_ROT_90;   // 90度回転（横向き表示）
```

## コード解説

### my_disp_flush() - ディスプレイ更新

```c
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t width = area->x2 - area->x1 + 1;
    uint32_t height = area->y2 - area->y1 + 1;

    // DMAを使用して効率的に画像データをディスプレイに転送
    M5.Display.pushImageDMA(area->x1, area->y1, width, height, 
                            (uint16_t *)&color_p->full);
    
    lv_disp_flush_ready(disp);
}
```

**ポイント**:
- `pushImageDMA()` でDMA転送による高速描画
- 更新領域のみを転送（効率的）

### my_touchpad_read() - タッチ入力

```c
void my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    auto touch_detail = M5.Touch.getDetail();
    
    if(touch_detail.wasPressed())
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_detail.x;
        data->point.y = touch_detail.y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}
```

**ポイント**:
- `wasPressed()` で正確なタッチ検出
- 座標は自動的にLVGLの回転に対応

### setup() - 初期化処理

```c
void setup()
{
    // M5Unifiedの初期化
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // LVGLの初期化
    lv_init();

    // ディスプレイ設定
    M5.Display.setColorDepth(16);
    
    // LVGLディスプレイバッファ初期化
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

    // ディスプレイドライバ登録
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.sw_rotate = 1;
    disp_drv.rotated = LV_DISP_ROT_90;
    lv_disp_drv_register(&disp_drv);

    // タッチ入力ドライバ登録
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // EEZ-Studio UIの初期化
    ui_init();
}
```

### loop() - メインループ

```c
void loop() {
    M5.update();           // M5Unifiedの状態更新
    lv_timer_handler();    // LVGLタイマー処理
    ui_tick();             // EEZ-Studio UI更新
    delay(5);              // CPU負荷軽減
}
```

## トラブルシューティング

### 色がおかしい

**原因**: `LV_COLOR_16_SWAP` の設定が間違っている

**解決策**: `lv_conf.h` で以下を確認
```c
#define LV_COLOR_16_SWAP 1  // Tab5では1を推奨
```

### フォントが見つからないエラー

```
error: 'lv_font_montserrat_XX' undeclared
```

**解決策**: `lv_conf.h` で使用するフォントを有効化
```c
#define LV_FONT_MONTSERRAT_24 1  // 0 → 1に変更
```

### コンパイルエラー: メモリ不足

**原因**: バッファサイズが大きすぎる

**解決策**: `ui.ino` でバッファサイズを調整
```c
static lv_color_t buf[screenWidth * 10];  // 10を5などに減らす
```

### 画面が表示されない

**チェックリスト**:
1. ボード設定が **M5Stack Tab5** になっているか
2. `ui_init()` が正しく呼ばれているか
3. `lv_timer_handler()` がループ内で呼ばれているか
4. EEZ Studioのエクスポートファイルが正しく配置されているか

### タッチが反応しない

**原因**: タッチドライバの初期化漏れ

**解決策**: `setup()` で `lv_indev_drv_register()` が呼ばれているか確認

## カスタマイズ

### 画面の向きを変更

縦向きにする場合：

```c
// ui.ino の解像度を変更
static const uint16_t screenWidth  = 1280;
static const uint16_t screenHeight = 720;

// 回転設定をコメントアウト
// disp_drv.sw_rotate = 1;
// disp_drv.rotated = LV_DISP_ROT_90;
```

### バッファサイズの最適化

メモリに余裕がある場合、バッファを大きくして描画速度を向上：

```c
// より大きなバッファ（メモリ使用量増加）
static lv_color_t buf[screenWidth * 20];  // 10 → 20
lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 20);
```

### デバッグ出力の追加

```c
void setup() {
    Serial.begin(115200);
    Serial.println("EEZ-Studio UI Starting...");
    // ... 残りの初期化コード
}

void loop() {
    // デバッグ情報
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
        lastPrint = millis();
    }
    // ... 残りのループコード
}
```

## EEZ Studioでの開発フロー

1. **EEZ Studioでデザイン**
   - 画面レイアウトを作成
   - ウィジェットを配置
   - スタイルを設定

2. **エクスポート**
   - File → Export → LVGL
   - 生成されたファイルを確認

3. **Arduinoに移植**
   - エクスポートファイルを `ui/` にコピー
   - 必要なフォントを `lv_conf.h` で有効化
   - コンパイル・アップロード

4. **テスト・デバッグ**
   - 動作確認
   - 必要に応じて調整

## 参考リンク

- [EEZ Studio公式サイト](https://www.envox.eu/eez-studio/)
- [LVGL公式ドキュメント](https://docs.lvgl.io/)
- [M5Stack公式サイト](https://m5stack.com/)
- [M5Unified GitHubリポジトリ](https://github.com/m5stack/M5Unified)

## ライセンス

このプロジェクトは元の[EEZ-Studio-Arduino-Starter](https://github.com/RealEEZStudios/EEZ-Studio-Arduino-Starter)をベースにM5Stack Tab5用にカスタマイズしたものです。

---

**最終更新**: 2026年1月2日
