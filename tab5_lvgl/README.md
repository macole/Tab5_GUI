# tab5_arduino_basic.ino の解説（LVGL学習用）

このドキュメントは、M5Stack Tab5（ESP32-P4）上で動作する `tab5_arduino_basic.ino` の役割を、LVGLの観点で解説します。主要トピックは以下です。
- M5Unified でのハード初期化と `M5GFX` 連携
- LVGL の描画バッファとディスプレイドライバ（flushコールバック）
- ソフトウェア回転（横表示）設定
- タッチ入力デバイスドライバ
- UI 初期化（SquareLine Studio 生成コード）
- ループでの `lv_timer_handler()` と状態更新

---

## 1. ハード初期化（M5Unified）と表示オブジェクトの取得

`M5Unified` は Tab5 の電源・バックライト・パネルをまとめて初期化します。その後 `M5.Display` を `M5GFX` の `display` に割り当てて使います。

```c
auto cfg = M5.config();
M5.begin(cfg);
display = M5.Display;  // LovyanGFX互換の描画API
```

ポイント:
- この初期化を省くとバックライトやパネルが有効にならず、画面が真っ暗になります。
- 明るさは後段で `display.setBrightness(255);` で最大に設定しています。

## 2. LVGLの初期化と描画バッファ

LVGLはユーザ空間のピクセルバッファ（描画用ワーク領域）にレンダリングし、完了時にディスプレイドライバの `flush` コールバックで物理表示に転送します。

```c
lv_init();
buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE,
                                     MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
static lv_disp_draw_buf_t draw_buf;
lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_LCD_BUF_SIZE);
```

ポイント:
- `LVGL_LCD_BUF_SIZE` は `pins_config.h` で `H_RES*V_RES` として定義。
- PSRAMを使って大きなバッファを確保（FQBNで PSRAM=enabled を指定）。

## 3. ディスプレイドライバ登録と flush コールバック

LVGLに「どうやって画面へ転送するか」を教えるのがディスプレイドライバです。ここではDMA対応の `pushImageDMA()` を使って高速転送します。

```c
static void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  display.pushImageDMA(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
  lv_disp_flush_ready(disp);
}

static lv_disp_drv_t disp_drv;
lv_disp_drv_init(&disp_drv);
disp_drv.hor_res  = EXAMPLE_LCD_H_RES;  // 720
disp_drv.ver_res  = EXAMPLE_LCD_V_RES;  // 1280
disp_drv.draw_buf = &draw_buf;
disp_drv.flush_cb = lv_disp_flush;      // ここが重要
```

ポイント:
- `flush_cb` は「LVGLがこの矩形領域を転送して」と呼んでくるフック。
- 転送完了後は必ず `lv_disp_flush_ready()` を呼びます（呼び忘れると描画が止まる）。

## 4. ソフトウェア回転（縦パネルを横表示に）

Tab5のパネルは縦基準（1280×720）なので、LVGLのソフト回転を使って横表示にしています。

```c
disp_drv.sw_rotate = 1;              // ソフト回転を有効化
disp_drv.rotated   = LV_DISP_ROT_90; // 90度回転
lv_disp_drv_register(&disp_drv);
```

ポイント:
- ハード回転が不明なため、ソフト回転で確実に横向きに。
- 回転時は転送矩形が偶数境界になるよう `my_rounder` を使う場合もあります（コメントアウト済み）。

## 5. タッチ入力デバイスドライバ

LVGLへタッチ座標を供給するための入力デバイスドライバです。`M5GFX` の `getTouchRaw()` を使い、タッチ座標を `lv_indev_data_t` に渡します。

```c
static void lv_indev_read(lv_indev_drv_t *indev, lv_indev_data_t *data) {
  lgfx::touch_point_t tp[3];
  uint8_t n = display.getTouchRaw(tp, 3);
  if (n > 0) {
    data->state   = LV_INDEV_STATE_PR;
    data->point.x = tp[0].x;
    data->point.y = tp[0].y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

static lv_indev_drv_t indev_drv;
lv_indev_drv_init(&indev_drv);
indev_drv.type    = LV_INDEV_TYPE_POINTER;
indev_drv.read_cb = lv_indev_read;
lv_indev_drv_register(&indev_drv);
```

ポイント:
- `LV_INDEV_TYPE_POINTER` はタッチ/マウスに相当。
- 長押しやドラッグ検出は、LVGLのウィジェット/イベント側で処理されます。

## 6. UI 初期化とバックライト

UIは SquareLine Studio 生成の `ui_init()` を呼ぶだけで構築されます。表示開始前にバックライト最大化と画面クリアを実施。

```c
ui_init();
display.setBrightness(255);
display.fillScreen(TFT_BLACK);
```

ポイント:
- 明るさを上げないと黒く見える場合があります。
- UIツリー（ボタン、ラベル、スライダ等）は `ui_*.c` に実装されています。

## 7. ループ処理と状態更新

`lv_timer_handler()` は LVGL の内部タイマー・アニメーション・入力処理を回します。周期的に呼ぶことでUIが動作します。

```c
void loop() {
  lv_timer_handler();
  delay(1);

  // 自動カウントのUI連動（例）
  if (lv_obj_has_state(ui_Button1, LV_STATE_CHECKED)) {
    lv_arc_set_value(ui_Arc1, count);
    lv_label_set_text_fmt(ui_Label1, "%d", count);
    if (++count == 1000) count = 0;
  }

  // スライダで輝度調整
  uint8_t brightness = lv_slider_get_value(ui_Slider1);
  display.setBrightness(brightness);
}
```

ポイント:
- `lv_timer_handler()` は必ず定期的に呼びます（ブロッキング処理は避ける）。
- UI部品（ラベル、スライダ、ボタン等）を直接操作して画面を更新できます。

---

## 付録: 依存と設定の要点

- ライブラリ: `M5Unified@0.2.10`, `M5GFX@0.2.15`, `LovyanGFX@1.2.7`, `lvgl@8.3.11`
- FQBN（Arduino CLI 再現設定）:
  - `esp32:esp32:esp32p4:PSRAM=enabled,FlashSize=16M,PartitionScheme=custom,CDCOnBoot=cdc,USBMode=hwcdc,UploadSpeed=921600`
- `pins_config.h`:
  - `EXAMPLE_LCD_H_RES 720`, `EXAMPLE_LCD_V_RES 1280`, `LVGL_LCD_BUF_SIZE (H*V)`
- `lv_conf.h`（抜粋）:
  - `LV_COLOR_DEPTH 16`, `LV_COLOR_16_SWAP 1`, `LV_TICK_CUSTOM 1`（`millis()`）、`LV_MEM_CUSTOM 1`

以上を押さえると、LVGLの「描く（バッファ）→転送（flush）→表示」「入力（indev）→イベント処理」の流れが理解できます。

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
