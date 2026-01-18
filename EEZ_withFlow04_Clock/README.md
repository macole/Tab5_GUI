# EEZ_withFlow04_Clock - M5Stack Tab5 時計アプリケーション

M5Stack Tab5向けのSquareLine Studio + LVGL 8.3ベースの時計アプリケーションです。

## 📋 概要

このプロジェクトは、M5Stack Tab5でSquareLine Studioを使用して設計したGUI時計アプリケーションです。カスタムフォント、アニメーション、タッチ操作をサポートしています。

### 主な特徴

- ✅ **SquareLine Studio設計**: 視覚的なUIデザイン
- ✅ **LVGL 8.3.11**: 高機能GUIライブラリ
- ✅ **M5Unified統合**: M5Stack統合ライブラリ使用
- ✅ **カスタムフォント**: デジタル時計用セブンセグメントフォント
- ✅ **高速描画**: DMA転送とSPIRAM使用
- ✅ **タッチ操作**: インタラクティブなUI

### サンプル機能

- カウンター表示（円形プログレスバー）
- 自動カウント機能のON/OFF
- 画面の明るさ調整（スライダー）
- スピナーアニメーション
- カスタムフォント表示

## 🔧 ハードウェア要件

- **M5Stack Tab5** (ESP32-P4)
  - 10.1インチディスプレイ（1280x720）
  - 静電容量式タッチスクリーン
  - 8MB PSRAM

## 📚 必要なライブラリ

| ライブラリ | バージョン | 必須 |
|------------|----------|------|
| M5Unified | >= 0.2.10 | ✅ |
| LVGL | 8.3.11 | ✅ |

### インストール方法

Arduino CLIを使用する場合：

```bash
arduino-cli lib install "M5Unified@0.2.10"
arduino-cli lib install "lvgl@8.3.11"
```

⚠️ **重要**: LVGL 9.xではなく、必ず8.3.11を使用してください。

## 🚀 セットアップ

### ボード設定

Arduino IDEで以下を設定：

```
ボード: ESP32P4 Dev Module または M5Tab5
Upload Speed: 921600
Flash Mode: QIO
Flash Frequency: 80MHz
Partition Scheme: Custom (partitions.csv使用)
PSRAM: Enabled (必須!)
USB CDC On Boot: Enabled
USB Mode: Hardware CDC and JTAG
```

### コンパイル・アップロード

1. Arduino IDEで `EEZ_withFlow04_Clock.ino` を開く
2. **検証（✓）**ボタンでコンパイル
3. **アップロード（→）**ボタンでTab5に転送

## 📁 プロジェクト構造

```
EEZ_withFlow04_Clock/
├── EEZ_withFlow04_Clock.ino  # メインプログラム
├── ui.h / ui.c                # UI初期化（SquareLine Studio生成）
├── ui_Screen1.c               # 画面1の実装
├── ui_helpers.h / ui_helpers.c # UIヘルパー関数
├── ui_events.h                # イベント定義
├── ui_comp_hook.c             # コンポーネントフック
├── ui_font_*.c                # フォントデータ
│   ├── ui_font_conthrax24.c  # Conthraxフォント
│   ├── ui_font_Font1.c       # カスタムフォント1
│   └── ui_font_sevenSeg.c    # セブンセグメントフォント
├── lv_conf.h                  # LVGL設定ファイル
├── pins_config.h              # ディスプレイ設定
├── config.h                   # アプリケーション設定
├── partitions.csv             # パーティション設定
├── CMakeLists.txt             # CMake設定
├── filelist.txt               # ファイルリスト
├── README.md                  # このファイル
└── SLS_Project/               # SquareLine Studioプロジェクト
    ├── Tab5_Basic.spj         # プロジェクトファイル
    ├── Tab5_Basic.sll         # レイアウトファイル
    ├── Themes.slt             # テーマ設定
    └── assets/                # アセット（フォント、画像）
        ├── Conthrax-SemiBold.otf
        ├── digital-7 (mono).ttf
        ├── flip1.jpg
        └── ui_font_*.fcfg
```

## 🎨 UIコンポーネント

### 現在のUI構成

プロジェクトには以下のUIコンポーネントが含まれています：

- **ui_Arc1**: 円形プログレスバー（カウンター表示）
- **ui_Label1**: カウンター値のラベル
- **ui_Button1**: 自動カウントON/OFFボタン
- **ui_Label2**: ボタンラベル
- **ui_Label3**: 追加情報ラベル
- **ui_Spinner2**: スピナーアニメーション
- **ui_Slider1**: 画面の明るさ調整スライダー
- **ui_Label4**: スライダーラベル

### 使用フォント

- **ui_font_conthrax24**: Conthrax SemiBoldフォント（24px）
- **ui_font_Font1**: カスタムフォント1
- **ui_font_sevenSeg**: デジタル時計用セブンセグメントフォント

## 💻 プログラムの解説

### 主要機能

#### 1. 自動カウント機能 (`updateAutomation()`)

```cpp
void updateAutomation(void)
{
    g_automateEnabled = lv_obj_has_state(ui_Button1, LV_STATE_CHECKED);
    if (g_automateEnabled) {
        lv_arc_set_value(ui_Arc1, g_counter);
        lv_label_set_text_fmt(ui_Label1, "%d", g_counter);
        g_counter++;
        if (g_counter >= COUNTER_MAX_VALUE) g_counter = 0;
    }
}
```

- ボタンの状態をチェック
- 有効時、円形プログレスバーとラベルを更新
- カウンターを0〜1000の範囲で循環

#### 2. 画面の明るさ調整 (`updateBrightness()`)

```cpp
void updateBrightness(void)
{
    uint8_t brightness = lv_slider_get_value(ui_Slider1);
    M5.Display.setBrightness(brightness);
}
```

- スライダーの値（0-255）を取得
- ディスプレイの明るさをリアルタイムで調整

### LVGL統合

#### ディスプレイ初期化

```cpp
void initLvglDisplay(void)
{
    lv_init();
    // SPIRAMに全画面バッファを確保
    g_color_buf = (lv_color_t *)heap_caps_malloc(
        sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    // ディスプレイドライバー設定
    disp_drv.sw_rotate = 1;
    disp_drv.rotated = LV_DISP_ROT_90;  // 90度回転（横向き）
}
```

#### タッチ入力処理

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

## 🎨 SquareLine Studioでの編集

### UIの編集方法

1. **SquareLine Studioをインストール**
   - [公式サイト](https://squareline.io/)からダウンロード

2. **プロジェクトを開く**
   ```
   File → Open Project → SLS_Project/Tab5_Basic.spj
   ```

3. **UIを編集**
   - Pages: 画面レイアウトを編集
   - Widgets: ウィジェットを追加・編集
   - Styles: 色、フォント、サイズを変更
   - Events: イベントハンドラーを設定

4. **エクスポート**
   ```
   Export → Arduino
   ```

5. **生成ファイルを反映**
   - 生成された `ui_*` ファイルをプロジェクトフォルダにコピー
   - Arduino IDEで再コンパイル

## カスタマイズ方法

### 新しいUIコンポーネントを追加

1. SquareLine Studioでウィジェットをデザイン
2. エクスポートして生成されたコードを反映
3. `loop()`関数内でコンポーネントを操作

### フォントの追加

1. `.ttf`または`.otf`フォントをSquareLine Studioにインポート
2. テキストウィジェットに適用
3. エクスポート時に自動的に`.c`ファイルが生成される

### カスタム機能の追加

`loop()`関数や新しい関数を追加して、独自の機能を実装：

```cpp
void loop()
{
    M5.update();
    lv_timer_handler();
    delay(LVGL_TIMER_DELAY_MS);
    
    updateAutomation();
    updateBrightness();
    
    // カスタム機能を追加
    updateCustomFunction();
}
```

## 🐛 トラブルシューティング

### 画面が表示されない

**チェックリスト**:
1. ✅ PSRAM: Enabledになっているか
2. ✅ `M5.begin()`が呼ばれているか
3. ✅ `lv_conf.h`の設定が正しいか
4. ✅ シリアルモニターでエラーを確認

### タッチが反応しない

**解決策**:
1. `M5.update()`が`loop()`の最初で呼ばれているか確認
2. `initLvglTouch()`が`setup()`で呼ばれているか確認
3. タッチパネルの接続を確認

### コンパイルエラー

**よくあるエラー**:

#### "ui.h: No such file or directory"
- SquareLine Studioでエクスポートしたファイルが不足
- `SLS_Project/`からファイルをエクスポート

#### "lv_conf.h: LV_COLOR_DEPTH undefined"
- `lv_conf.h`の設定を確認
- `LV_COLOR_DEPTH 16`を設定

#### メモリ不足エラー
- ボード設定で`PSRAM: Enabled`を確認
- 描画バッファサイズを調整

## 📊 メモリ使用量

### 典型的なメモリ使用量

```
コンパイル後:
- プログラムストレージ: 約800KB / 16MB (5%)
- 動的メモリ: 約100KB / 512KB (20%)

実行時:
- 画面バッファ (SPIRAM): 1.8MB / 8MB (23%)
- 空きヒープ: 約400KB
- 空きPSRAM: 約6MB
```

### バッファサイズ

```cpp
// pins_config.h
#define EXAMPLE_LCD_H_RES 720
#define EXAMPLE_LCD_V_RES 1280
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)
// = 921,600ピクセル × 2バイト = 1.8MB
```

## 📖 参考資料

- [LVGL公式ドキュメント](https://docs.lvgl.io/)
- [SquareLine Studio公式サイト](https://squareline.io/)
- [SquareLine Studioドキュメント](https://docs.squareline.io/)
- [M5Unified GitHub](https://github.com/m5stack/M5Unified)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)

## 📝 ライセンス

このプロジェクトはMITライセンスで公開されています。

## 🤝 開発履歴

- **作成日**: 2025年1月
- **最終更新**: 2026年1月18日
- **対象デバイス**: M5Stack Tab5 (ESP32-P4)
- **LVGLバージョン**: 8.3.11
- **M5Unifiedバージョン**: >= 0.2.10
- **SquareLine Studioバージョン**: 1.5.1

---

**注意**: このプロジェクトはSquareLine Studioで作成されています。EEZ StudioとFlow機能を使用する場合は、別のプロジェクト（EEZ_withFlow03_YahooNewsやEEZ_Template）を参照してください。
