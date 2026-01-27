# LV8wF_Native - M5Stack Tab5 アプリケーション

EEZ StudioとLVGLを使用したM5Stack Tab5向けのサンプルアプリケーションです。Templateプロジェクトをベースに、Native変数、自動更新機能、スムーズなタッチ操作などの実装例を含んでいます。

## 📋 概要

このプロジェクトは、**EEZ_Template**をベースに開発された実用的なアプリケーションです。以下の機能を実装しています：

### 主な機能

- ✅ **Native変数の実装** - `counter`、`flag`、`light`の3つのNative変数
- ✅ **画面明るさ制御** - `light`変数で画面の明るさを0-255の範囲で制御
- ✅ **自動アーク更新** - `flag`が`true`の時に`arc1`を0-100まで自動更新
- ✅ **カウンター表示** - `counter`の値を`text_area`に自動表示
- ✅ **スムーズなタッチ操作** - スライダーなどの連続操作に対応
- ✅ **リファクタリング済み** - 保守しやすいコード構造

## 🔧 ハードウェア要件

- **M5Stack Tab5** (ESP32-P4)
  - 10.1インチディスプレイ（1280x720）
  - 静電容量式タッチスクリーン
  - 8MB PSRAM（必須）

## 📚 必要なライブラリ

| ライブラリ | バージョン | 必須/オプション |
|------------|----------|----------------|
| M5Unified | >= 0.2.10 | 必須 |
| LVGL | 8.3.11 | 必須 |

⚠️ **重要**: LVGL 9.xは互換性がありません。必ず8.3.11を使用してください。

## 🚀 クイックスタート

### 1. プロジェクトを開く

Arduino IDEで`LV8wF_Native.ino`を開きます。

### 2. ボード設定

**ツール**メニューで以下を設定：

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

- 画面にGUIが表示されることを確認
- スライダーで画面の明るさを調整
- ボタンで`flag`を切り替えて`arc1`の自動更新を確認
- `counter`の値が`text_area`に表示されることを確認

## 📁 プロジェクト構造

```
LV8wF_Native/
├── LV8wF_Native.ino                # メインプログラム
├── LV8wF_Native.eez-project        # EEZ Studioプロジェクトファイル
├── lv_conf.h                       # LVGL設定ファイル（読み取り間隔10ms）
├── README.md                       # このファイル
├── LICENSE                         # ライセンスファイル
└── src/
    └── ui/                         # EEZ Studio生成ファイル
        ├── ui.c / ui.h
        ├── screens.c / screens.h
        ├── eez-flow.cpp / eez-flow.h
        ├── actions.h
        ├── styles.c / styles.h
        ├── images.c / images.h
        ├── fonts.h
        ├── structs.h
        └── vars.h                   # Native変数の宣言
```

## 🎯 実装されている機能

### 1. Native変数の実装

#### `counter` (int32_t)
- カウンター値
- `get_var_counter()` / `set_var_counter()` でアクセス
- 値が変更されると`text_area`に自動表示

#### `flag` (bool)
- フラグ値
- `get_var_flag()` / `set_var_flag()` でアクセス
- `true`の時に`arc1`が自動更新される

#### `light` (int32_t)
- 画面の明るさ（0-255）
- `get_var_light()` / `set_var_light()` でアクセス
- 値が変更されると即座に画面の明るさが更新される
- 範囲外の値は自動的に0-255に制限

### 2. 自動アーク更新機能

`flag`が`true`の時に、`arc1`の値が0から100まで自動的に更新されます。

- 更新間隔: 100ms（`APP_UPDATE_INTERVAL_MS`）
- 動作: 0 → 1 → 2 → ... → 100 → 0（ループ）
- `flag`が`false`になると更新が停止（現在の値を維持）

### 3. カウンター表示機能

`counter`の値が変更されると、自動的に`text_area`に表示されます。

- 変更検出: 前回の値と比較して変更時のみ更新
- 表示形式: 整数値（例: "123"）

### 4. スムーズなタッチ操作

タッチ入力の応答性を向上させるため、以下の最適化を実装：

- **タッチ検出の改善**: `isPressed()`と`wasPressed()`の両方をチェック
- **読み取り間隔の短縮**: `LV_INDEV_DEF_READ_PERIOD`を30ms → 10msに変更

これにより、スライダーなどの連続操作が滑らかに動作します。

## 🔨 Templateから作成する手順

このプロジェクトは**EEZ_Template**をベースに作成されました。以下の手順で同様のプロジェクトを作成できます：

### ステップ1: Templateプロジェクトのコピー

```bash
# Templateプロジェクトをコピー
cp -r EEZ_Template LV8wF_Native
cd LV8wF_Native
```

### ステップ2: ファイル名の変更

```bash
# メインファイルの名前を変更
mv EEZ_Template.ino LV8wF_Native.ino
```

### ステップ3: Native変数の実装

`LV8wF_Native.ino`に以下のコードを追加：

```cpp
// ============================================================================
// Native変数実装
// ============================================================================

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

// Native変数: light（画面の明るさ 0-255）
static int32_t g_light = 255;

int32_t get_var_light() {
    return g_light;
}

void set_var_light(int32_t value) {
    g_light = value;
    // 値の範囲を制限して画面の明るさを更新
    if (g_light < 0) g_light = 0;
    if (g_light > 255) g_light = 255;
    M5.Display.setBrightness(g_light);
}
```

### ステップ4: EEZ StudioでNative変数を定義

1. **EEZ Studioを開く**
   ```
   File → Open Project → LV8wF_Native.eez-project
   ```

2. **Variablesタブを開く**
   - `counter`: 型を`Integer`、種類を`Native`に設定
   - `flag`: 型を`Boolean`、種類を`Native`に設定
   - `light`: 型を`Integer`、種類を`Native`に設定

3. **Build & Export**
   ```
   Build → Build & Export
   ```

4. **生成ファイルをコピー**
   - 生成された`src/ui/vars.h`をプロジェクトにコピー
   - `get_var_*()`と`set_var_*()`の宣言が含まれていることを確認

### ステップ5: タッチ入力の改善

`lv_indev_read()`関数を以下のように変更：

```cpp
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    const auto touch_detail = M5.Touch.getDetail();
    
    // isPressed()とwasPressed()の両方をチェックすることで、
    // タッチ開始時だけでなく、ドラッグ中も連続的に検出できる
    if (touch_detail.isPressed() || touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_detail.x;
        data->point.y = touch_detail.y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
```

### ステップ6: lv_conf.hの更新

`lv_conf.h`の`LV_INDEV_DEF_READ_PERIOD`を変更：

```cpp
/*Input device read period in milliseconds*/
/* スライダーの連続操作を滑らかにするため、読み取り間隔を短く設定 */
#define LV_INDEV_DEF_READ_PERIOD 10     /*[ms]*/
```

### ステップ7: アプリケーション機能の追加

`updateApplication()`関数に以下の機能を追加：

```cpp
// アプリケーション状態
static unsigned long g_lastUpdateTime = 0;
static int32_t g_arc1Value = 0;        // arc1の現在の値（0-100）
static int32_t g_lastCounter = -1;     // 前回のcounter値（変更検出用）

// arc1の値を自動更新
static void updateArc1()
{
    if (!get_var_flag()) {
        return;
    }
    
    g_arc1Value++;
    if (g_arc1Value > 100) {
        g_arc1Value = 0;  // 100に達したら0に戻す（ループ）
    }
    lv_arc_set_value(objects.arc1, g_arc1Value);
}

// counterの値をtext_areaに表示
static void updateCounterDisplay()
{
    int32_t counter = get_var_counter();
    if (counter == g_lastCounter) {
        return;
    }
    
    g_lastCounter = counter;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%ld", (long)counter);
    lv_textarea_set_text(objects.text_area, buffer);
}

void updateApplication()
{
    unsigned long currentTime = millis();
    
    // 指定間隔で更新
    if (currentTime - g_lastUpdateTime < APP_UPDATE_INTERVAL_MS) {
        return;
    }
    
    g_lastUpdateTime = currentTime;
    
    // 各機能の更新
    updateArc1();
    updateCounterDisplay();
}
```

### ステップ8: loop()の更新

`loop()`関数に`arc1`の値の再設定処理を追加：

```cpp
void loop()
{
    M5.update();                // M5Unifiedの更新（ボタン、タッチなど）
    lv_timer_handler();         // LVGLタイマーハンドラ（UIの更新）    
    ui_tick();                  // EEZ Flow Tickハンドラ（Flow言語の実行）   
    updateApplication();        // アプリケーション機能の更新
    
    // flagがtrueの時は、arc1の値を再設定（ui_tick()で上書きされないように）
    if (get_var_flag()) {
        lv_arc_set_value(objects.arc1, g_arc1Value);
    }
    
    delay(LVGL_TIMER_DELAY_MS);
}
```

### ステップ9: 必要なインクルードの追加

```cpp
#include <cstdio>  // snprintf()用
```

### ステップ10: コンパイルとテスト

1. Arduino IDEでコンパイル
2. エラーがあれば修正
3. Tab5にアップロード
4. 動作確認

## 📊 コード構造

### 主要な関数

- **`updateArc1()`**: `flag`が`true`の時に`arc1`を自動更新
- **`updateCounterDisplay()`**: `counter`の値を`text_area`に表示
- **`updateApplication()`**: 定期的に実行されるアプリケーション更新処理
- **`showFatalError()`**: 初期化エラー時の表示

### グローバル変数

### グローバル変数

- **`g_counter`**: `counter` Native変数の値
- **`g_flag`**: `flag` Native変数の値
- **`g_light`**: `light` Native変数の値
- **`g_arc1Value`**: `arc1`の現在の値（0-100）
- **`g_lastCounter`**: 前回の`counter`値（変更検出用）

## 🐛 トラブルシューティング

### リンカーエラー: undefined reference to 'get_var_*'

**原因**: Native変数のgetter/setter関数が実装されていない

**解決策**: `LV8wF_Native.ino`に`get_var_*()`と`set_var_*()`関数を実装してください。

### arc1が0にリセットされる

**原因**: `ui_tick()`がEEZ Flowの値で`arc1`を上書きしている

**解決策**: `loop()`の最後で`flag`が`true`の時に`arc1`の値を再設定する処理を追加してください。

### スライダーが滑らかに動かない

**原因**: タッチ入力の読み取り間隔が長い、または`isPressed()`をチェックしていない

**解決策**: 
1. `lv_conf.h`の`LV_INDEV_DEF_READ_PERIOD`を10msに設定
2. `lv_indev_read()`で`isPressed()`もチェック

### counterがtext_areaに表示されない

**原因**: `updateCounterDisplay()`が呼ばれていない、または`text_area`が正しく初期化されていない

**解決策**: 
1. `updateApplication()`内で`updateCounterDisplay()`が呼ばれているか確認
2. `ui_init()`が`setup()`で呼ばれているか確認

## 📖 技術詳細

### Native変数とは

EEZ StudioのNative変数は、C++コードで直接実装する変数です。Global変数とは異なり、getter/setter関数を自分で実装する必要があります。

**メリット**:
- C++コードから直接制御可能
- 値の変更時に追加処理を実行可能（例: `light`の変更時に画面の明るさを更新）
- 型変換やバリデーションを実装可能

**実装の流れ**:
1. EEZ StudioでNative変数を定義
2. `vars.h`にgetter/setterの宣言が生成される
3. `LV8wF_Native.ino`に実装を追加

### タッチ入力の最適化

スライダーなどの連続操作を滑らかにするため、以下の最適化を実装：

1. **`isPressed()`のチェック**: タッチ開始時だけでなく、ドラッグ中も検出
2. **読み取り間隔の短縮**: 30ms → 10msに変更して応答性を向上

### アーク更新の仕組み

`flag`が`true`の時に`arc1`を自動更新する処理は、以下の流れで動作します：

1. `updateApplication()`が100ms間隔で呼ばれる
2. `updateArc1()`で`g_arc1Value`をインクリメント
3. `lv_arc_set_value()`で`arc1`の値を更新
4. `ui_tick()`がEEZ Flowの値で上書きする可能性があるため、`loop()`の最後で再設定

## 🔗 参考リンク

- [EEZ Studio公式サイト](https://www.envox.eu/eez-studio/)
- [EEZ Studio ドキュメント](https://github.com/eez-open/studio/wiki)
- [LVGL公式ドキュメント](https://docs.lvgl.io/8.3/)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)
- [EEZ_Template README](../EEZ_Template/README.md) - ベースとなったTemplateプロジェクト

## 📝 更新履歴

### 2026-01-27
- ✅ Native変数（`counter`、`flag`、`light`）の実装
- ✅ 自動アーク更新機能の追加
- ✅ カウンター表示機能の追加
- ✅ タッチ入力の最適化
- ✅ コードのリファクタリング
- ✅ ドキュメントの作成

## 📝 ライセンス

このプロジェクトはMITライセンスの下で公開されています。

---

**作成日**: 2026年1月27日  
**バージョン**: 1.0.0  
**対応環境**: M5Stack Tab5 (ESP32-P4), M5Unified 0.2.10+, LVGL 8.3.11  
**ベースプロジェクト**: EEZ_Template
