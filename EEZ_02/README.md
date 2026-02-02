# EEZ_02 — Chart デモアプリ

M5Stack Tab5 向けの、EEZ Studio + LVGL ベースの **Chart（グラフ）デモ** サンプルアプリケーションです。  
EEZ_Template をベースに、chart1 にデモ用の折れ線グラフ（2系列）を表示し、**slider1 で画面の明るさ**（0～255、初期値 255）を変更する機能を追加しています。

---

## 📋 概要

- **chart1**: 起動時にデモ用の折れ線グラフを表示（赤・青の2系列、各24点）
- **slider1**: スライダーの値（0～255）で画面の明るさを変更。初期値は 255（最大輝度）
- LVGL の Chart / Slider ウィジェットを使用したサンプル

### 主な機能

| 機能 | 説明 |
|------|------|
| **chart1 デモグラフ** | 折れ線グラフで赤・青2系列のサンプルデータを表示 |
| **slider1 で明るさ** | スライダー操作で画面輝度を 0～255 で変更（初期値 255） |
| **Y軸範囲** | 0 ～ 100 |
| **データ点数** | 各系列 24 点 |
| **分割線** | 横 5 本、縦 8 本 |

### ハードウェア・依存関係

- **M5Stack Tab5** (ESP32-P4)、10.1 インチディスプレイ、8MB PSRAM 必須
- **M5Unified** >= 0.2.10、**LVGL** 8.3.11

---

## 🚀 使い方

1. Arduino IDE で `EEZ_02.ino` を開く
2. ボード: **M5Tab5**、PSRAM: **Enabled** を選択
3. コンパイル・アップロード
4. 起動後、chart1 に赤・青の折れ線グラフが表示され、slider1 を動かすと画面の明るさが変わることを確認

---

## 📁 プロジェクト構成

```
EEZ_02/
├── EEZ_02.ino              # メインプログラム（chart1 デモ・slider1 明るさ制御）
├── EEZ_02.eez-project      # EEZ Studio プロジェクト
├── lv_conf.h               # LVGL 設定
├── README.md               # 本ドキュメント
└── src/ui/
    ├── screens.c / screens.h   # 画面・chart1 / slider1 定義
    ├── ui.c / ui.h             # UI 初期化
    └── （その他 EEZ 生成ファイル）
```

---

## 📌 EEZ_Template からの変更点（コード付き）

EEZ_02 は **EEZ_Template** をベースにしています。以下は変更したファイルと、追加・修正したコードをまとめたものです。

### 1. 変更したファイル一覧

| ファイル | 種別 | 内容 |
|----------|------|------|
| **EEZ_02.ino** | 手動追加・変更 | 定数 `CHART1_POINT_COUNT` / `BRIGHTNESS_INIT`、`setupChart1Demo()` / `setupSlider1Brightness()` の追加。`setup()` で両方を呼び出し。 |
| **src/ui/screens.c** | EEZ Studio 生成 | Chart1 / Slider1 ウィジェットの作成、位置・サイズ・範囲（0～255）・初期値（255）の設定。 |
| **src/ui/screens.h** | EEZ Studio 生成 | `objects_t` に `lv_obj_t *chart1` / `*slider1` を追加。 |

---

### 2. EEZ_02.ino の変更

#### 2.1 追加した定数

Template にはありません。chart1 のデータ点数用です。

```c
/** デモ用データ点数 */
#define CHART1_POINT_COUNT 24
```

#### 2.2 追加したブロック「chart1 デモグラフ」

Template にはないブロック全体です。`initLvglTouch()` の直後、「アプリケーション機能」の前に挿入します。

```c
// ============================================================================
// chart1 デモグラフ
// ============================================================================
/** デモ用データ点数 */
#define CHART1_POINT_COUNT 24

/**
 * @brief chart1 にデモ用の折れ線グラフを表示する
 * ui_init() の後に一度だけ呼ぶ。
 */
static void setupChart1Demo()
{
    if (!objects.chart1) return;

    lv_obj_t *chart = objects.chart1;

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_point_count(chart, CHART1_POINT_COUNT);
    lv_chart_set_div_line_count(chart, 5, 8);

    /* 系列1: 赤（サンプル値） */
    lv_chart_series_t *ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    static const lv_coord_t demo1[] = { 10, 20, 35, 45, 55, 65, 75, 85, 80, 70, 60, 50, 45, 55, 65, 70, 60, 50, 40, 35, 45, 55, 65, 75 };
    for (uint16_t i = 0; i < CHART1_POINT_COUNT; i++) {
        lv_chart_set_next_value(chart, ser1, demo1[i]);
    }

    /* 系列2: 青（別パターン） */
    lv_chart_series_t *ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    static const lv_coord_t demo2[] = { 90, 75, 65, 55, 45, 40, 50, 60, 70, 75, 70, 60, 50, 55, 60, 55, 50, 60, 70, 80, 85, 80, 75, 70 };
    for (uint16_t i = 0; i < CHART1_POINT_COUNT; i++) {
        lv_chart_set_next_value(chart, ser2, demo2[i]);
    }

    lv_chart_refresh(chart);
}
```

- `lv_chart_set_type` … 折れ線グラフに設定。
- `lv_chart_set_range` … Y軸を 0～100 に固定。
- `lv_chart_set_point_count` … 1系列あたり 24 点。
- `lv_chart_set_div_line_count` … 横 5・縦 8 の分割線。
- `lv_chart_add_series` … 赤・青 2 系列を追加。
- `lv_chart_set_next_value` … 各点の値を順に設定。
- `lv_chart_refresh` … 設定反映のため再描画。

#### 2.3 追加したブロック「slider1 で画面明るさ制御」

Template にはありません。chart1 デモブロックの直後、「アプリケーション機能」の前に挿入します。

```c
// ============================================================================
// slider1 で画面明るさ制御
// ============================================================================
/** 画面明るさの初期値（slider1 の初期値と一致） */
#define BRIGHTNESS_INIT 255

/**
 * @brief slider1 の値変更時に画面明るさを更新する
 */
static void slider1_brightness_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t val = lv_slider_get_value(slider);
    if (val < 0) val = 0;
    if (val > 255) val = 255;
    M5.Display.setBrightness((uint8_t)val);
}

/**
 * @brief slider1 を画面明るさ用に設定する（初期値 255）
 * ui_init() の後に一度だけ呼ぶ。
 */
static void setupSlider1Brightness()
{
    if (!objects.slider1) return;

    M5.Display.setBrightness(BRIGHTNESS_INIT);

    lv_obj_add_event_cb(objects.slider1, slider1_brightness_cb, LV_EVENT_VALUE_CHANGED, NULL);
}
```

- `BRIGHTNESS_INIT` … 画面明るさの初期値 255（slider1 の初期値と一致）。
- `slider1_brightness_cb` … `LV_EVENT_VALUE_CHANGED` で slider1 の値を取得し、`M5.Display.setBrightness()` で画面輝度を更新。
- `setupSlider1Brightness()` … 起動時に輝度を 255 に設定し、slider1 にコールバックを登録。

#### 2.4 setup() の変更

**Template（変更前）:**

```c
void setup()
{
    // ... 省略 ...
    initLvglTouch();
    
    ui_init();    // EEZ-Studio UI初期化
}
```

**EEZ_02（変更後）:**

```c
void setup()
{
    // ... 省略 ...
    initLvglTouch();
    
    ui_init();    // EEZ-Studio UI初期化
    setupChart1Demo();   // chart1 にデモグラフを表示
    setupSlider1Brightness();  // slider1 で画面明るさ制御（初期値 255）
}
```

差分は `setupChart1Demo();` と `setupSlider1Brightness();` の 2 行追加です。

---

### 3. src/ui/screens.c の変更（EEZ Studio 生成）

Template では Chart が無く、EEZ_02 では Main 画面に Chart1 を追加した想定です。生成されるコードの例は以下のとおりです。

```c
{
    // Chart1
    lv_obj_t *obj = lv_chart_create(parent_obj);
    objects.chart1 = obj;
    lv_obj_set_pos(obj, 184, 27);
    lv_obj_set_size(obj, 1071, 663);
}
```

- `lv_chart_create(parent_obj)` で chart オブジェクトを作成。
- `objects.chart1` に代入して後から参照。
- 位置 (184, 27)、サイズ 1071×663。

**Slider1 の例（明るさ用、範囲 0～255・初期値 255）:**

```c
{
    // Slider1
    lv_obj_t *obj = lv_slider_create(parent_obj);
    objects.slider1 = obj;
    lv_obj_set_pos(obj, 18, 147);
    lv_obj_set_size(obj, 13, 543);
    lv_slider_set_range(obj, 0, 255);
    lv_slider_set_value(obj, 255, LV_ANIM_OFF);
}
```

オブジェクト名の登録例:

```c
static const char *object_names[] = { "main", "button1", "chart1", "slider1" };
```

---

### 4. src/ui/screens.h の変更（EEZ Studio 生成）

`objects_t` に `chart1` / `slider1` メンバを追加します。

**追加されるメンバ:**

```c
typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *button1;
    lv_obj_t *chart1;   // 追加
    lv_obj_t *slider1;  // 追加（明るさ用）
} objects_t;
```

EEZ Studio で Chart / Slider ウィジェットを配置し「Build & Export」すると、上記のように `screens.c` / `screens.h` が更新されます。

---

### 5. 変更点サマリ（チェックリスト）

- [ ] **EEZ_02.ino**: 定数 `CHART1_POINT_COUNT` / `BRIGHTNESS_INIT` を追加
- [ ] **EEZ_02.ino**: 関数 `setupChart1Demo()` を追加（chart1 デモ用コードブロック全体）
- [ ] **EEZ_02.ino**: 関数 `setupSlider1Brightness()` と `slider1_brightness_cb` を追加（slider1 明るさ用コードブロック全体）
- [ ] **EEZ_02.ino**: `setup()` で `setupChart1Demo();` と `setupSlider1Brightness();` を呼び出し
- [ ] **EEZ Studio**: Main 画面に Chart と Slider を配置（Slider は範囲 0～255、初期値 255 推奨）、名前を「chart1」「slider1」に設定
- [ ] **EEZ Studio**: Build & Export で `screens.c` / `screens.h` を再生成し、`objects.chart1` / `objects.slider1` が使える状態にする

---

## 📖 関連ドキュメント

- [../Docs/GUIDE.md](../Docs/GUIDE.md) — プロジェクト全体のガイド
- [../Docs/TROUBLESHOOTING.md](../Docs/TROUBLESHOOTING.md) — トラブルシューティング
- [../EEZ_Template/README.md](../EEZ_Template/README.md) — ベーステンプレート

---

## 📝 ライセンス

MIT ライセンス。ベース: EEZ_Template（M5Stack Tab5 向け EEZ Studio テンプレート）。

---

**作成日**: 2026-01-18  
**最終更新**: 2026-02-01  
**ベースプロジェクト**: EEZ_Template
