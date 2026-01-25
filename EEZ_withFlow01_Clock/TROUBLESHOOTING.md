# トラブルシューティングガイド - EEZ_withFlow01_Clock

## 🐛 画面がチカチカして動かない

### 症状
- プログラムはアップロードできる
- 画面が点滅（チカチカ）する
- 正常な表示がされない

### 原因の可能性

#### 1. PSRAM（SPIRAM）が無効になっている

**最も一般的な原因**です。このプログラムは1.8MBの画面バッファをPSRAMに確保する必要があります。

**確認方法**:
```
シリアルモニター（115200 bps）で以下のメッセージを確認：

正常な場合:
  📊 System Information:
     Free PSRAM: 8388608 bytes (8.00 MB)

異常な場合:
  ❌ FATAL ERROR: PSRAM not available!
  または
  Free PSRAM: 0 bytes
```

**解決策**:
1. Arduino IDEで `ツール` メニューを開く
2. `PSRAM: Enabled` を選択
3. 再度コンパイル＆アップロード

**Arduino CLI の場合**:
```bash
arduino-cli compile --fqbn esp32:esp32:esp32p4:PSRAM=enabled,FlashSize=16M \
  EEZ_withFlow01_Clock/EEZ_withFlow01_Clock.ino
```

---

#### 2. メモリ不足

**症状**: シリアルモニターに以下が表示される
```
❌ ERROR: Failed to allocate SPIRAM buffer!
   Requested: 1843200 bytes
   Free PSRAM: XXXXX bytes
```

**原因**: PSRAMは有効だが、他のプロセスがメモリを使用している

**解決策**:
1. デバイスを再起動
2. 他のプログラムを終了
3. それでも解決しない場合、画面バッファサイズを小さくする（次のセクション参照）

---

#### 3. 画面バッファサイズが大きすぎる

**現在の設定**:
```cpp
#define EXAMPLE_LCD_H_RES 720
#define EXAMPLE_LCD_V_RES 1280
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)
// = 921,600 pixels × 2 bytes = 1.8MB
```

**解決策1**: バッファサイズを半分にする

```cpp
// 変更前
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)

// 変更後（半分のバッファ）
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES / 2)
```

**解決策2**: さらに小さくする（10分の1）

```cpp
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES / 10)
```

**注意**: バッファを小さくすると描画速度が遅くなりますが、動作は安定します。

---

#### 4. M5GFX/M5Displayの問題

**症状**: シリアルモニターには正常なメッセージが表示されるが、画面がチカチカする

**原因**: M5.Display.pushImageDMA()の問題

**解決策**: DMAを使わない通常の転送に変更

```cpp
// lv_disp_flush() 関数を変更

// 変更前
M5.Display.pushImageDMA(area->x1, area->y1, width, height, (uint16_t *)&color_p->full);

// 変更後（DMA無効）
M5.Display.pushImage(area->x1, area->y1, width, height, (uint16_t *)&color_p->full);
```

---

#### 5. EEZ Studio UIの問題

**症状**: 
- LVGL初期化は成功
- `ui_init()`でハング

**原因**: EEZ StudioのUI定義に問題がある

**解決策**:
1. EEZ Studioでプロジェクトを開く
2. エラーがないか確認
3. 再度ビルド＆エクスポート
4. 生成ファイルをプロジェクトにコピー

---

#### 6. EEZ Flowのアサーションエラー（リブートループ）

**症状**: 
- プログラムが起動してすぐにクラッシュ
- リブートを繰り返す
- シリアルモニターに以下のようなエラーが表示される：

```
assert failed: void eez::flow::stopScript() eez-flow.cpp:7497 (false)
Core  1 register dump:
MEPC    : 0x4ff0173a  RA      : 0x4ff09384  SP      : 0x4ff2da30
...
```

**原因**: 
EEZ Flowエンジンの`stopScriptHook`がデフォルトで`assert(false)`を呼び出すため、スクリプト終了時やエラー時にクラッシュする。

**解決策**: EEZ Flowフックを適切に実装する

`setup()`関数内で、`ui_init()`を呼び出す前に以下のコードを追加：

```cpp
// EEZ Flow フックをすべてオーバーライド（安全な実装で）
Serial.println("🔧 Setting up EEZ Flow hooks...");

// stopScriptフックをオーバーライド（デフォルトのassert(false)を回避）
eez::flow::stopScriptHook = []() {
    Serial.println("⚠️  EEZ Flow: stopScript called");
    // 必要に応じてスクリプト再起動処理をここに追加
};

// replacePageフック（画面切り替え）
eez::flow::replacePageHook = [](int16_t pageId, uint32_t animType, uint32_t speed, uint32_t delay) {
    Serial.printf("📄 EEZ Flow: replacePage(%d, %u, %u, %u)\n", pageId, animType, speed, delay);
    lv_obj_t *target_screen = nullptr;
    switch(pageId) {
        case SCREEN_ID_MAIN:
            target_screen = objects.main;
            break;
        case SCREEN_ID_PAGE1:
            target_screen = objects.page1;
            break;
        default:
            Serial.printf("⚠️  Unknown screen ID: %d\n", pageId);
            return;
    }
    if (target_screen) {
        lv_scr_load_anim(target_screen, (lv_scr_load_anim_t)animType, speed, delay, false);
    }
};

// scpiComponentInitフック
eez::flow::scpiComponentInitHook = []() {
    Serial.println("🔌 EEZ Flow: scpiComponentInit called");
};

Serial.println("✅ EEZ Flow hooks configured");
```

**必要なインクルード**:
```cpp
#include "eez-flow.h"
```

**デバッグ方法**:
1. まず`ui_init()`をコメントアウトしてLVGLだけで起動できるか確認
2. LVGLテスト描画で動作確認
3. EEZ Flowフックを設定してから`ui_init()`を有効化

**参考**: 
- この問題は、EEZ Studioが生成するデフォルトのフック実装が開発環境向けであるため発生します
- 本番環境では必ずフックをオーバーライドする必要があります

---

## 🔍 デバッグ手順

### ステップ1: シリアルモニターを開く

```
Arduino IDE: ツール → シリアルモニタ
ボーレート: 115200
```

### ステップ2: リセットボタンを押す

M5Stack Tab5のリセットボタンを押して、起動メッセージを確認

### ステップ3: 起動メッセージを確認

**正常な起動メッセージ**:
```
╔═══════════════════════════════════════╗
║  M5Stack Tab5                         ║
║  EEZ_withFlow01_Clock                 ║
╚═══════════════════════════════════════╝

📊 System Information:
   LVGL Version: 8.3.11
   Free Heap: XXXXX bytes (XXX KB)
   Free PSRAM: 8388608 bytes (8.00 MB)

🔧 Initializing LVGL display...
✅ LVGL initialized
📊 Allocating display buffer: 1843200 bytes (1.76 MB)
✅ Display buffer allocated
✅ LVGL display driver registered
🔧 Initializing LVGL touch...
✅ LVGL touch initialized

🎨 Initializing EEZ Studio UI...
✅ EEZ Studio UI initialized

🚀 Setup completed successfully!

▶️  Entering main loop...
```

**異常な起動メッセージ**:
```
❌ FATAL ERROR: PSRAM not available!
```
→ 「原因1: PSRAMが無効」を参照

```
❌ ERROR: Failed to allocate SPIRAM buffer!
```
→ 「原因2: メモリ不足」を参照

---

## ⚙️ ボード設定チェックリスト

Arduino IDEの `ツール` メニューで以下を確認：

- [ ] **ボード**: M5Tab5 または ESP32P4 Dev Module
- [ ] **PSRAM**: **Enabled** ← ⚠️ 必須！
- [ ] **Flash Size**: 16MB (128Mb)
- [ ] **Partition Scheme**: Default 4MB with spiffs
- [ ] **USB CDC On Boot**: Enabled
- [ ] **Upload Speed**: 921600

---

## 🔧 応急処置: 最小構成で動作確認

問題が解決しない場合、最小構成で動作確認してください。

### 最小構成プログラム

```cpp
#include <M5Unified.h>
#include <lvgl.h>

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 1280

static lv_disp_draw_buf_t g_draw_buf;
static lv_color_t *g_color_buf = nullptr;

void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t width = (area->x2 - area->x1 + 1);
    uint32_t height = (area->y2 - area->y1 + 1);
    M5.Display.pushImageDMA(area->x1, area->y1, width, height, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

void setup()
{
    M5.begin();
    Serial.begin(115200);
    delay(100);
    
    Serial.println("Test start");
    Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
    
    lv_init();
    
    size_t buf_size = SCREEN_WIDTH * SCREEN_HEIGHT / 10;  // 10分の1バッファ
    g_color_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * buf_size, MALLOC_CAP_SPIRAM);
    
    if (g_color_buf == nullptr) {
        Serial.println("ERROR: Buffer allocation failed");
        while(1) delay(1000);
    }
    
    lv_disp_draw_buf_init(&g_draw_buf, g_color_buf, NULL, buf_size);
    
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &g_draw_buf;
    disp_drv.sw_rotate = 1;
    disp_drv.rotated = LV_DISP_ROT_90;
    lv_disp_drv_register(&disp_drv);
    
    // シンプルなラベル表示
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello M5Stack Tab5!");
    lv_obj_center(label);
    
    Serial.println("Setup completed");
}

void loop()
{
    M5.update();
    lv_timer_handler();
    delay(1);
}
```

この最小構成で動作すれば、LVGL自体は正常です。EEZ StudioのUI定義に問題がある可能性があります。

---

## 📞 サポート情報

### 問題が解決しない場合

1. **シリアルモニターの出力を全てコピー**
2. **ボード設定のスクリーンショット**
3. **使用しているライブラリのバージョン**

を添えて、GitHubのIssueに投稿してください。

---

#### 7. タッチパネルの反応が悪い（ボタンを何度も押さないと反応しない）

**症状**: 
- ボタンを押しても3-4回繰り返さないと反応しない
- タッチ座標がずれている（押した位置と反応する位置が違う）
- タッチイベントが検出されない

**原因の可能性**:

##### 7.1 タッチ座標の回転変換がされていない

**最も一般的な原因**です。ディスプレイが90度回転している場合、タッチ座標も回転変換する必要があります。

**確認方法**:
1. シリアルモニターでタッチ座標のログを確認
2. 画面の左上をタッチしたとき、ログに表示される座標が `(0, 0)` に近いか確認
3. 画面の右下をタッチしたとき、座標が `(720, 1280)` に近いか確認

**解決策**: `lv_indev_read()`関数でタッチ座標を回転変換する

```cpp
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    auto touch_detail = M5.Touch.getDetail();
    
    // isPressed()とwasPressed()の両方をチェック（検出確実性向上）
    if (touch_detail.isPressed() || touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;
        
        int16_t raw_x = touch_detail.x;
        int16_t raw_y = touch_detail.y;
        
        // 時計回り90度回転: (x, y) → (y, 720 - x)
        int16_t rotated_x = raw_y;
        int16_t rotated_y = 720 - raw_x;
        
        // 座標範囲チェック
        if (rotated_x < 0) rotated_x = 0;
        if (rotated_x >= 720) rotated_x = 719;
        if (rotated_y < 0) rotated_y = 0;
        if (rotated_y >= 1280) rotated_y = 1279;
        
        data->point.x = rotated_x;
        data->point.y = rotated_y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
```

**回転方向が逆の場合**:
タッチ座標が逆方向にずれている場合は、以下のいずれかを試してください：

```cpp
// パターン1: 反時計回り90度回転
int16_t rotated_x = 1280 - raw_y;
int16_t rotated_y = raw_x;

// パターン2: X/Y反転のみ
int16_t rotated_x = raw_y;
int16_t rotated_y = raw_x;

// パターン3: 座標変換なし（回転していない場合）
int16_t rotated_x = raw_x;
int16_t rotated_y = raw_y;
```

##### 7.2 タッチ検出のタイミング問題

**症状**: `wasPressed()`のみを使用している場合、タッチが検出されにくい

**解決策**: `isPressed()`も併用する

```cpp
// 変更前（検出が不安定）
if (touch_detail.wasPressed()) {
    // ...
}

// 変更後（検出が確実）
if (touch_detail.isPressed() || touch_detail.wasPressed()) {
    // ...
}
```

##### 7.3 読み取り間隔が長すぎる

**症状**: タッチの反応が遅い

**解決策**: `lv_conf.h`で読み取り間隔を短くする

LVGL 8.3.11では、読み取り間隔は`lv_conf.h`の`LV_INDEV_DEF_READ_PERIOD`で設定します：

```c
// lv_conf.h の87行目付近
/*Input device read period in milliseconds*/
/* タッチ入力の応答性を向上させるには、この値を小さくしてください（例: 10ms）*/
#define LV_INDEV_DEF_READ_PERIOD 10     /*[ms]*/  // デフォルト: 30ms
```

**注意**: `lv_indev_set_read_timer()`という関数はLVGL 8.3.11には存在しません。`lv_conf.h`で設定する必要があります。

##### 7.4 デバッグ方法

タッチ座標のデバッグログを有効化：

```cpp
// lv_indev_read()内で
static unsigned long last_touch_log = 0;
if (millis() - last_touch_log > 200) {  // 200msごとにログ出力
    Serial.printf("🖐️  Touch: raw(%d, %d) → rotated(%d, %d)\n", 
                 raw_x, raw_y, rotated_x, rotated_y);
    last_touch_log = millis();
}
```

**確認ポイント**:
1. 画面の四隅をタッチして、座標が正しい範囲内か確認
2. ボタンの位置とタッチ座標が一致しているか確認
3. タッチイベントが連続して検出されているか確認

---

**作成日**: 2026年1月19日  
**対象プロジェクト**: EEZ_withFlow01_Clock  
**最終更新**: 2026年1月25日
