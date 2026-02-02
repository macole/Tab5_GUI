# EEZ_01 — Canvas お絵かきアプリ

M5Stack Tab5 向けの、EEZ Studio + LVGL ベースの **Canvas 指描き（お絵かき）** サンプルアプリケーションです。  
EEZ_Template をベースに、Canvas 上で指で線を描き、Button1 でクリアする機能を追加しています。

---

## 📋 概要

- **Canvas1**: 指でタッチ・ドラッグして線を描画（黒・太さ 4px、滑らか描画）
- **Button1**: 押すと Canvas1 を白でクリア（EEZ Studio のアクション `action_clear_canvas1` で実装）
- LVGL の更新間隔・タッチ読み取り間隔を短くし、線を滑らかに表示

### 主な機能

| 機能 | 説明 |
|------|------|
| **Canvas 指描き** | Canvas1 上でタッチ／ドラッグすると黒い線が描画される |
| **Button1 でクリア** | Button1 クリックで Canvas1 を白で塗りつぶし |
| **滑らかな描画** | LVGL ループ遅延・入力読み取り間隔を短くして線を滑らかに |
| **SPIRAM 使用** | Canvas 用バッファを SPIRAM に確保（約 1.3MB） |

### ハードウェア・依存関係

- **M5Stack Tab5** (ESP32-P4)、10.1 インチディスプレイ、8MB PSRAM 必須
- **M5Unified** >= 0.2.10、**LVGL** 8.3.11

---

## 🚀 使い方

1. Arduino IDE で `EEZ_01.ino` を開く
2. ボード: **M5Tab5**、PSRAM: **Enabled** を選択
3. コンパイル・アップロード
4. **Canvas1** の白い領域を指でなぞって描画、**Button1** を押すとクリア

---

## 📁 プロジェクト構成

```
EEZ_01/
├── EEZ_01.ino              # メインプログラム（Canvas 描画・アクション実装）
├── EEZ_01.eez-project      # EEZ Studio プロジェクト
├── lv_conf.h               # LVGL 設定（入力読み取り間隔など）
├── README.md               # 本ドキュメント
└── src/ui/
    ├── screens.c / screens.h   # 画面・Canvas1/Button1 定義
    ├── actions.h               # action_clear_canvas1 宣言
    ├── ui.c / ui.h             # UI 初期化・actions 配列
    └── （その他 EEZ 生成ファイル）
```

---

## 🔧 実装の要点

### Canvas 指描き

- **バッファ**: `g_canvas1_buf` を SPIRAM に確保（991×676×2 バイト、RGB565）
- **タッチ受付**: LVGL の Image 系はデフォルトでクリック無効のため、`LV_OBJ_FLAG_CLICKABLE` を Canvas1 に付与
- **イベント**: `LV_EVENT_PRESSED` / `PRESSING` / `RELEASED` で座標を取得し、`lv_canvas_set_px_color` と `lv_canvas_draw_line` で描画

### Button1 → クリア

- EEZ Studio で Button1 のクリックに **アクション `action_clear_canvas1`** を割り当て
- `action_clear_canvas1` は **EEZ_01.ino** 内で `extern "C"` 付きで実装し、`lv_canvas_fill_bg(..., 白, ...)` で Canvas1 をクリア
- C から参照されるため `extern "C"` が必須（リンクエラー回避）

### 滑らか描画のための設定

- **EEZ_01.ino**: `LVGL_TIMER_DELAY_MS` を 50 → 5ms に短縮
- **lv_conf.h**: `LV_INDEV_DEF_READ_PERIOD` を 10 → 5ms に短縮（設定している場合）

---

## 📌 EEZ_Template からの変更点

EEZ_01 は **EEZ_Template** をベースにしています。以下は Template との差分をファイル・内容単位でまとめたものです。

### 変更したファイル一覧

| ファイル | 種別 | 概要 |
|----------|------|------|
| **EEZ_01.ino** | 変更・追加 | Canvas 描画、Button1 アクション、定数・変数・初期化の追加 |
| **lv_conf.h** | 変更 | 入力デバイス読み取り間隔の短縮（任意） |
| **src/ui/screens.c** | EEZ 生成 | Canvas1 / Button1 配置・イベント（EEZ Studio で編集） |
| **src/ui/screens.h** | EEZ 生成 | `objects.canvas1` 等のオブジェクト定義 |
| **src/ui/actions.h** | EEZ 生成 | `action_clear_canvas1` の宣言 |
| **src/ui/ui.c** | EEZ 生成 | `actions[]` に `action_clear_canvas1` を登録 |

※ `screens.c` / `actions.h` / `ui.c` は EEZ Studio の「Build & Export」で生成されます。

### EEZ_01.ino の変更点（Template との差分）

**定数・マクロ**

| 箇所 | Template | EEZ_01 | 備考 |
|------|----------|--------|------|
| LVGL ループ遅延 | `LVGL_TIMER_DELAY_MS 50` | `LVGL_TIMER_DELAY_MS 5` | 描画を滑らかにするため短縮 |
| アプリ更新間隔 | `APP_UPDATE_INTERVAL_MS 100` | `APP_UPDATE_INTERVAL_MS 10` | 必要に応じて変更可能 |
| Canvas 関連 | なし | `CANVAS1_WIDTH` / `CANVAS1_HEIGHT` / `CANVAS1_BUF_SIZE` | screens.c の Canvas1 サイズに合わせる |

**グローバル変数の追加**

| 変数 | 型 | 説明 |
|------|-----|------|
| `g_canvas1_buf` | `static uint8_t *` | Canvas1 用描画バッファ（SPIRAM） |

**追加した処理ブロック（Template にないもの）**

1. **「canvas1 指描き機能」ブロック**
   - `s_canvas_last_x` / `s_canvas_last_y` / `s_canvas_drawing`（静的変数）
   - `CANVAS_LINE_WIDTH` / `CANVAS_LINE_COLOR`（線の太さ・色）
   - **`canvas_draw_event_cb()`** — PRESSED/PRESSING/RELEASED で座標取得し、`lv_canvas_set_px_color` / `lv_canvas_draw_line` で描画
   - **`setupCanvasDrawing()`** — Canvas1 に CLICKABLE 付与、バッファ確保・設定、白塗り、イベントコールバック登録

2. **EEZ Studio アクション実装**
   - **`action_clear_canvas1(lv_event_t *e)`** — Canvas1 を白でクリア（`extern "C"` で C からリンク可能に）

**setup() の変更**

| Template | EEZ_01 |
|----------|--------|
| `ui_init();` のみ | `ui_init();` の後に **`setupCanvasDrawing();`** を追加 |

**loop()** — 処理内容は同じ。使用する `LVGL_TIMER_DELAY_MS` の値が 5 に変わっているだけ。

### lv_conf.h の変更点

| 定数 | Template（想定） | EEZ_01 | 備考 |
|------|------------------|--------|------|
| `LV_INDEV_DEF_READ_PERIOD` | 10 (ms) | 5 (ms) | タッチ読み取り間隔を短くし、線を滑らかに |

### EEZ Studio 側（生成コード）の変更点

- **画面レイアウト**: Canvas1 の配置・サイズ（例: 991×676）、Button1 を Canvas クリア用として配置
- **Button1 のイベント**: クリック（または押下）に **アクション `action_clear_canvas1`** を割り当て → `screens.c` の `event_handler_cb_main_button1` から `action_clear_canvas1(e)` が呼ばれる
- **生成ファイルでの対応**:
  - **actions.h**: `extern void action_clear_canvas1(lv_event_t * e);` の宣言
  - **ui.c**: `ActionExecFunc actions[] = { action_clear_canvas1 };` で登録
  - **screens.c**: Button1 のイベントハンドラ内で `action_clear_canvas1(e);` を呼び出し
  - **screens.h**: `objects_t` に `lv_obj_t *canvas1;` が含まれる

※ これらは EEZ Studio の「Build & Export」で上書きされます。手動編集した場合は再エクスポート時に失われないよう注意してください。

### 変更点サマリ（チェックリスト）

- [ ] **EEZ_01.ino**: Canvas 用定数・変数・`canvas_draw_event_cb`・`setupCanvasDrawing`・`action_clear_canvas1` を追加
- [ ] **EEZ_01.ino**: `LVGL_TIMER_DELAY_MS` を 5ms に変更（任意だが推奨）
- [ ] **EEZ_01.ino**: `setup()` で `setupCanvasDrawing()` を呼び出し
- [ ] **lv_conf.h**: `LV_INDEV_DEF_READ_PERIOD` を 5ms に変更（任意）
- [ ] **EEZ Studio**: Canvas1 を配置し、Button1 にアクション `action_clear_canvas1` を割り当て
- [ ] **生成コード**: actions.h に `action_clear_canvas1` 宣言、ui.c に actions 登録、screens.c で Button1 から呼び出し

### Template から EEZ_01 を作り直す手順（参考）

1. EEZ_Template をコピーして EEZ_01 とする
2. EEZ Studio でプロジェクトを開き、Canvas1 と Button1 を配置し、Button1 に `action_clear_canvas1` を割り当てて Build & Export
3. 上記「EEZ_01.ino の変更点」に従い、EEZ_01.ino に定数・変数・関数・`setupCanvasDrawing()` 呼び出しを追加
4. 必要に応じて lv_conf.h の `LV_INDEV_DEF_READ_PERIOD` を 5ms に変更
5. ビルド・実機で Canvas 描画と Button1 クリアを確認

---

## 📖 関連ドキュメント

- [../Docs/GUIDE.md](../Docs/GUIDE.md) — プロジェクト全体のガイド
- [../Docs/TROUBLESHOOTING.md](../Docs/TROUBLESHOOTING.md) — トラブルシューティング

---

## 📝 ライセンス

MIT ライセンス。ベース: EEZ_Template（M5Stack Tab5 向け EEZ Studio テンプレート）。

---

**作成日**: 2026-01-18  
**最終更新**: 2026-02-01  
**ベースプロジェクト**: EEZ_Template
