# EEZ_03 — M5Stack Tab5 向け EEZ Studio プロジェクト（縦表示・画像対応）

M5Stack Tab5 向けの、EEZ Studio + LVGL ベースのサンプルです。  
**縦表示**・**色の一致（GUI と取り込み PNG の両方）**・**EEZ Flow の安定動作** のために、テンプレートから以下の設定・修正を加えています。

---

## 📋 概要

- **表示**: 縦向き 720×1280（Tab5 のネイティブ 1280×720 を回転）
- **色**: GUI と EEZ Studio で取り込んだ PNG 画像の両方が正しく表示されるよう、`lv_conf.h` とフラッシュコールバックで RGB565 のバイトスワップを調整
- **EEZ Flow**: `stopScript` 等のフックをオーバーライドし、assert によるリブートを防止
- **描画**: 画面点滅を防ぐため、`pushImage`（同期転送）を使用

### 主な変更点（EEZ_Template からの差分）

| 項目 | 内容 |
|------|------|
| 縦表示 | LVGL `rotated = (lv_disp_rot_t)0`、`M5.Display.setRotation(3)`（または 1） |
| 色（GUI＋画像） | `LV_COLOR_16_SWAP 0` ＋ フラッシュ時にバッファ全体をバイトスワップして送信 |
| 点滅防止 | `pushImageDMA` → `pushImage` に変更 |
| EEZ Flow | `stopScriptHook` / `replacePageHook` / `scpiComponentInitHook` を設定 |

---

## 🖥️ 縦表示の設定

Tab5 の物理解像度は **1280×720（横）** です。縦表示 720×1280 にするために以下を設定しています。

### EEZ_03.ino

1. **LVGL の回転（描画バッファの論理解像度）**
   - `configureDisplayDriver()` 内:  
     `disp_drv.rotated = (lv_disp_rot_t)0;`  
     （LVGL 8 では `LV_DISP_ROT_0` が無いためキャストで 0 を指定）

2. **M5.Display の回転（転送時の向き）**
   - `setup()` の `ui_init()` の**後**で:  
     `M5.Display.setRotation(3);`  
     - `3` = 270° 回転で縦向き。縦にならない場合は `1`（90°）を試す。

### 解像度定数

- `SCREEN_WIDTH 720`、`SCREEN_HEIGHT 1280`（縦向き時の論理サイズ）
- バッファは `SCREEN_BUFFER_SIZE = 720 * 1280` でフルスクリーン 1 枚

---

## 🎨 色の設定（GUI と取り込み PNG の両方を正しく表示）

### 現象

- **GUI の色**: 正しい  
- **EEZ Studio で取り込んだ PNG から生成した画像**: 黒が緑っぽく見えるなど、色がずれる  

### 原因

- GUI は LVGL が **LV_COLOR_16_SWAP 1** で描画した「スワップ済み」バッファをそのまま送ると正しく表示される。
- 取り込み PNG は EEZ Studio が **スワップなし** の RGB565（RGB565A8）で C 配列にしている。
- 同じバッファを「スワップ済み」として送ると、画像部分だけバイト順が合わず、黒が緑に見えるなどの不具合が出る。

### 対応方針

- **LVGL 内部と画像データを「スワップなし」で揃える**
- **パネルへ送る直前だけ**、バッファ全体をバイトスワップする

### lv_conf.h

```c
#define LV_COLOR_16_SWAP 0
```

- 描画バッファも画像データも「スワップなし」で統一する。
- スワップはフラッシュコールバック側で行う。

### EEZ_03.ino のフラッシュコールバック

1. **swapRgb565Bytes(buf, pixelCount)**  
   - 各ピクセル（uint16_t）の上位・下位バイトを入れ替える。
2. **M5.Display.pushImage(...)**  
   - スワップしたバッファをそのまま送信（パネルはスワップ済みを期待）。
3. **swapRgb565Bytes(buf, pixelCount)**  
   - 再度スワップして元に戻す（LVGL がバッファを再利用するため）。

これで **GUI と取り込み PNG の両方** が正しい色で表示されます。

### 過去に試したこと（参考）

- **R と G の入れ替え**: 赤が緑になる等の症状用。本プロジェクトでは不要。
- **R と B の入れ替え（BGR）**: オレンジと水色が入れ替わる症状用。本プロジェクトではパネルは RGB 想定のため、BGR 変換は外し、上記「LV_COLOR_16_SWAP 0 ＋ フラッシュ時のみバイトスワップ」で統一。

---

## 🔧 画面点滅の防止

### 現象

コンパイル・転送はできるが、画面が点滅する。

### 原因

`pushImageDMA` は非同期のため、転送完了前に `lv_disp_flush_ready()` を呼んでいた。  
その結果、LVGL が同じバッファを次の描画で上書きしている間に DMA が読み、バッファ競合で点滅していた。

### 対応

- **pushImageDMA** をやめ、**pushImage**（同期転送）に変更。
- 転送完了後に `lv_disp_flush_ready()` が呼ばれるため、点滅が解消する。

※ 描画負荷はやや増えますが、表示は安定します。

---

## ⚠️ EEZ Flow のアサーションエラー（リブートループ）の防止

### 現象

起動後すぐにクラッシュ・リブートを繰り返し、シリアルに次のような出力が出る。

```
assert failed: void eez::flow::stopScript() eez-flow.cpp:7497 (false)
```

### 原因

EEZ Flow の `stopScript()` が内部で **stopScriptHook** を呼ぶ。  
未設定のときはデフォルト実装の `assert(false)` が実行され、リブートする。

### 対応

`ui_init()` の**前**に、次の 3 つのフックを設定する。

1. **stopScriptHook**  
   スクリプト終了時に呼ばれる。空のラムダ `[]() { }` を代入して assert を起こさないようにする。

2. **replacePageHook**  
   画面切り替え用。`pageId == SCREEN_ID_MAIN_1` のとき `objects.main_1` に切り替える。  
   （EEZ_03 は main_1 のみの想定。）

3. **scpiComponentInitHook**  
   SCPI 用。未使用のため空のラムダを代入。

### コード例（EEZ_03.ino の setup() 内）

```cpp
#include "src/ui/eez-flow.h"

// initLvglTouch() の後、ui_init() の前
eez::flow::stopScriptHook = []() { };
eez::flow::replacePageHook = [](int16_t pageId, uint32_t animType, uint32_t speed, uint32_t delay) {
    lv_obj_t *target = nullptr;
    if (pageId == SCREEN_ID_MAIN_1) target = objects.main_1;
    if (target) lv_scr_load_anim(target, (lv_scr_load_anim_t)animType, speed, delay, false);
};
eez::flow::scpiComponentInitHook = []() { };

ui_init();  // この後に setRotation(3) など
```

---

## 📁 プロジェクト構成

```
EEZ_03/
├── EEZ_03.ino           # メインプログラム（縦表示・色・Flow フック・pushImage）
├── EEZ_03.eez-project   # EEZ Studio プロジェクト
├── lv_conf.h            # LVGL 設定（LV_COLOR_16_SWAP 0）
├── README.md            # 本ドキュメント
└── src/ui/
    ├── screens.c / screens.h   # 画面（main_1 等）
    ├── eez-flow.h / eez-flow.cpp
    ├── images.c / images.h    # 画像（RGB565A8）
    └── （その他 EEZ 生成ファイル）
```

---

## 📌 設定・修正の一覧

| ファイル | 設定・修正内容 |
|----------|----------------|
| **EEZ_03.ino** | 縦表示（rotated=0, setRotation(3)）、フラッシュ時の swapRgb565Bytes、pushImage、EEZ Flow フック、eez-flow.h の include |
| **lv_conf.h** | `LV_COLOR_16_SWAP 0`（コメントで「スワップは flush で実施」と明記） |

---

## 📖 関連ドキュメント

- [../Docs/GUIDE.md](../Docs/GUIDE.md) — プロジェクト全体の開発ガイド
- [../Docs/TROUBLESHOOTING.md](../Docs/TROUBLESHOOTING.md) — 画面がチカチカする／EEZ Flow の assert など

---

## 📝 ライセンス

MIT ライセンス。ベース: EEZ_Template（M5Stack Tab5 向け EEZ Studio テンプレート）。

---

**作成日**: 2026-02-01  
**ベースプロジェクト**: EEZ_Template
