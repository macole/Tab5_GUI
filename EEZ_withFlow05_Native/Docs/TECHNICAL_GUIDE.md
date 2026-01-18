# 技術ガイド

このドキュメントでは、EEZ Templateの内部実装と技術的な詳細について説明します。

## 📋 目次

1. [アーキテクチャ概要](#アーキテクチャ概要)
2. [メモリ管理](#メモリ管理)
3. [LVGL統合](#lvgl統合)
4. [EEZ Flow統合](#eez-flow統合)
5. [描画パイプライン](#描画パイプライン)
6. [パフォーマンス最適化](#パフォーマンス最適化)
7. [デバッグ手法](#デバッグ手法)

---

## アーキテクチャ概要

### システム構成

```
┌─────────────────────────────────────────────────┐
│                 Application Layer                │
│        (updateAutomation, updateBrightness)      │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────┐
│              EEZ Flow Engine                     │
│          (ui_tick, Flow execution)               │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────┐
│                   LVGL Layer                     │
│       (lv_timer_handler, UI rendering)           │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────┐
│              Hardware Abstraction                │
│              (M5Unified, Display)                │
└─────────────────────────────────────────────────┘
```

### コンポーネント図

```
EEZ_Template.ino
    │
    ├── M5Unified          : ハードウェア制御
    ├── LVGL               : GUI描画
    │   ├── lv_disp_flush : ディスプレイコールバック
    │   └── lv_indev_read : タッチ入力コールバック
    │
    ├── EEZ Flow Engine    : Flow言語実行
    │   ├── ui_init       : UI初期化
    │   ├── ui_tick       : Flow定期実行
    │   └── screens.c     : 画面定義
    │
    └── Application Logic  : アプリケーション固有処理
        ├── updateAutomation
        ├── updateBrightness
        └── updateApplication
```

---

## メモリ管理

### メモリマップ

```
ESP32-P4メモリ構成:
┌──────────────────────────┐
│   Flash (16MB)           │
│   - Program: ~600KB      │
│   - SPIFFS: 残り         │
├──────────────────────────┤
│   SRAM (512KB)           │
│   - Heap: ~400KB         │
│   - Stack: ~80KB         │
│   - システム: 残り        │
├──────────────────────────┤
│   PSRAM (8MB)            │
│   - LVGLバッファ: 1.8MB  │
│   - 利用可能: ~6MB       │
└──────────────────────────┘
```

### SPIRAMバッファ確保

```cpp
bool allocateDisplayBuffer()
{
    // SCREEN_BUFFER_SIZE = 720 * 1280 = 921,600ピクセル
    // 1ピクセル = 2バイト (RGB565)
    // 合計 = 1,843,200バイト (約1.8MB)
    
    g_color_buf = (lv_color_t *)heap_caps_malloc(
        sizeof(lv_color_t) * SCREEN_BUFFER_SIZE,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    
    if (g_color_buf == nullptr) {
        return false;
    }
    
    return true;
}
```

**メモリアロケーション戦略**:
- **SPIRAM**: 大容量バッファ（LVGL描画バッファ）
- **SRAM**: 小容量・高速アクセスデータ（変数、スタック）
- **Flash**: 定数データ、画像、フォント

### メモリ使用量の監視

```cpp
void printMemoryInfo() {
    Serial.println("Memory Usage:");
    Serial.printf("  Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("  Heap Size: %d bytes\n", ESP.getHeapSize());
    Serial.printf("  Free PSRAM: %d bytes\n", ESP.getFreePsram());
    Serial.printf("  PSRAM Size: %d bytes\n", ESP.getPsramSize());
    Serial.printf("  Largest Free Block: %d bytes\n", 
                  ESP.getMaxAllocHeap());
}
```

---

## LVGL統合

### カラーフォーマット

```cpp
// lv_conf.h
#define LV_COLOR_DEPTH 16        // RGB565 (16bit)
#define LV_COLOR_16_SWAP 1       // バイトスワップ有効
```

**RGB565フォーマット**:
```
15                              0
[R R R R R | G G G G G G | B B B B B]
  5 bits     6 bits         5 bits
```

### ディスプレイドライバ設定

```cpp
void configureDisplayDriver(lv_disp_drv_t &disp_drv)
{
    lv_disp_drv_init(&disp_drv);
    
    // 解像度設定（物理サイズ）
    disp_drv.hor_res = 720;
    disp_drv.ver_res = 1280;
    
    // フラッシュコールバック
    disp_drv.flush_cb = lv_disp_flush;
    
    // 描画バッファ
    disp_drv.draw_buf = &g_draw_buf;
    
    // ソフトウェア回転（90度）
    disp_drv.sw_rotate = 1;
    disp_drv.rotated = LV_DISP_ROT_90;
    
    lv_disp_drv_register(&disp_drv);
}
```

**画面回転の仕組み**:
```
物理的な配置:     ソフトウェア回転後:
┌─────┐           ┌──────────────┐
│     │           │              │
│ 720 │    →      │    1280      │
│     │           │              │
│1280 │           └──────────────┘
└─────┘                 720
縦向き                  横向き
```

### タッチ入力処理

```cpp
static void lv_indev_read(lv_indev_drv_t *indev_driver, 
                          lv_indev_data_t *data)
{
    const auto touch_detail = M5.Touch.getDetail();
    
    if (touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;  // 押下状態
        data->point.x = touch_detail.x;
        data->point.y = touch_detail.y;
    } else {
        data->state = LV_INDEV_STATE_REL; // 離された状態
    }
}
```

---

## EEZ Flow統合

### Flowエンジンの実行

```cpp
void loop() {
    M5.update();            // ハードウェア更新
    lv_timer_handler();     // LVGL UI更新
    ui_tick();              // Flow実行 ← EEZ Flow
    // ...
}
```

### Flow変数の定義

```cpp
// vars.h (EEZ Studio生成)
enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_COUNTER = 0,   // カウンター
    FLOW_GLOBAL_VARIABLE_FLAG = 1,      // フラグ
    FLOW_GLOBAL_VARIABLE_COUNTER2 = 2   // カウンター2
};
```

### Flowイベント処理

```cpp
// screens.c (EEZ Studio生成)
static void event_handler_cb_main_button1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    
    if (event == LV_EVENT_PRESSED) {
        // Flow言語で定義されたアクションを実行
        flowPropagateValueLVGLEvent(flowState, 8, 0, e);
    }
}
```

### Arduino側からFlow変数へのアクセス

```cpp
// Flow変数の値を取得
int getFlowCounter() {
    // EEZ Flowエンジン経由で変数を取得
    return getFlowGlobalVariable(FLOW_GLOBAL_VARIABLE_COUNTER);
}

// Flow変数の値を設定
void setFlowCounter(int value) {
    setFlowGlobalVariable(FLOW_GLOBAL_VARIABLE_COUNTER, value);
}
```

---

## 描画パイプライン

### 描画フロー

```
1. lv_timer_handler()
   ↓
2. LVGL内部処理（ダーティ領域計算）
   ↓
3. lv_disp_flush() コールバック呼び出し
   ↓
4. M5.Display.pushImageDMA() (DMA転送開始)
   ↓
5. lv_disp_flush_ready() (転送完了通知)
```

### DMA転送の詳細

```cpp
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, 
                   lv_color_t *color_p)
{
    // 更新領域のサイズを計算
    const uint32_t width = area->x2 - area->x1 + 1;
    const uint32_t height = area->y2 - area->y1 + 1;
    
    // DMA転送（非ブロッキング）
    // CPUは他の処理を継続できる
    M5.Display.pushImageDMA(
        area->x1,              // X座標
        area->y1,              // Y座標
        width,                 // 幅
        height,                // 高さ
        (uint16_t *)&color_p->full  // データポインタ
    );
    
    // LVGLに転送完了を通知
    lv_disp_flush_ready(disp);
}
```

**DMA転送のメリット**:
- ✅ CPUリソースを節約
- ✅ 高速なデータ転送
- ✅ 並行処理が可能

---

## パフォーマンス最適化

### FPS測定

```cpp
void measureFPS() {
    static unsigned long frameCount = 0;
    static unsigned long lastTime = 0;
    
    frameCount++;
    
    unsigned long currentTime = millis();
    if (currentTime - lastTime >= 1000) {
        float fps = frameCount * 1000.0 / (currentTime - lastTime);
        Serial.printf("FPS: %.2f\n", fps);
        
        frameCount = 0;
        lastTime = currentTime;
    }
}

void loop() {
    // ...
    measureFPS();
}
```

### CPU使用率の監視

```cpp
void printCPUUsage() {
    static unsigned long lastIdle = 0;
    static unsigned long lastTotal = 0;
    
    unsigned long idle = esp_timer_get_time();  // アイドル時間
    unsigned long total = millis() * 1000;      // 総時間
    
    float usage = 100.0 * (1.0 - (idle - lastIdle) / 
                                  (float)(total - lastTotal));
    Serial.printf("CPU Usage: %.1f%%\n", usage);
    
    lastIdle = idle;
    lastTotal = total;
}
```

### 最適化のヒント

#### 1. 更新頻度の最適化

```cpp
// 悪い例: 毎フレーム更新
void loop() {
    updateSensorData();  // 毎回実行（無駄）
}

// 良い例: 必要な時だけ更新
void loop() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {  // 1秒ごと
        updateSensorData();
        lastUpdate = millis();
    }
}
```

#### 2. LVGL再描画の最小化

```cpp
// 悪い例: 値が変わらなくても更新
lv_label_set_text_fmt(label, "%d", value);

// 良い例: 値が変わった時だけ更新
static int lastValue = -1;
if (value != lastValue) {
    lv_label_set_text_fmt(label, "%d", value);
    lastValue = value;
}
```

#### 3. メモリアロケーションの削減

```cpp
// 悪い例: 毎回String生成
void loop() {
    String msg = "Temperature: " + String(temp);
    lv_label_set_text(label, msg.c_str());
}

// 良い例: 静的バッファ使用
void loop() {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "Temperature: %.1f", temp);
    lv_label_set_text(label, buffer);
}
```

---

## デバッグ手法

### シリアルログマクロ

```cpp
// デバッグレベル定義
#define DEBUG_LEVEL_NONE  0
#define DEBUG_LEVEL_ERROR 1
#define DEBUG_LEVEL_WARN  2
#define DEBUG_LEVEL_INFO  3
#define DEBUG_LEVEL_DEBUG 4

// 現在のデバッグレベル
#define CURRENT_DEBUG_LEVEL DEBUG_LEVEL_INFO

// デバッグマクロ
#define DEBUG_ERROR(fmt, ...) \
    if (CURRENT_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR) \
        Serial.printf("[ERROR] " fmt "\n", ##__VA_ARGS__)

#define DEBUG_WARN(fmt, ...) \
    if (CURRENT_DEBUG_LEVEL >= DEBUG_LEVEL_WARN) \
        Serial.printf("[WARN]  " fmt "\n", ##__VA_ARGS__)

#define DEBUG_INFO(fmt, ...) \
    if (CURRENT_DEBUG_LEVEL >= DEBUG_LEVEL_INFO) \
        Serial.printf("[INFO]  " fmt "\n", ##__VA_ARGS__)

#define DEBUG_DEBUG(fmt, ...) \
    if (CURRENT_DEBUG_LEVEL >= DEBUG_LEVEL_DEBUG) \
        Serial.printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)

// 使用例
void someFunction() {
    DEBUG_INFO("Function started");
    DEBUG_DEBUG("Value: %d", someValue);
    
    if (error) {
        DEBUG_ERROR("Something went wrong!");
    }
}
```

### スタックトレース

```cpp
void printStackTrace() {
    void* buffer[10];
    int count = backtrace(buffer, 10);
    
    Serial.println("Stack trace:");
    char** symbols = backtrace_symbols(buffer, count);
    for (int i = 0; i < count; i++) {
        Serial.printf("  [%d] %s\n", i, symbols[i]);
    }
    free(symbols);
}
```

### パフォーマンスプロファイリング

```cpp
class Profiler {
private:
    unsigned long startTime;
    const char* name;
    
public:
    Profiler(const char* n) : name(n) {
        startTime = micros();
    }
    
    ~Profiler() {
        unsigned long elapsed = micros() - startTime;
        Serial.printf("[PROFILE] %s: %lu us\n", name, elapsed);
    }
};

// 使用例
void expensiveFunction() {
    Profiler prof("expensiveFunction");
    // 処理...
}
// 関数終了時に自動的に時間が表示される
```

---

## まとめ

このテンプレートは、以下の技術を統合しています：

- **M5Unified**: ハードウェア抽象化
- **LVGL**: 高機能GUI
- **EEZ Flow**: 視覚的なプログラミング
- **DMA転送**: 高速描画
- **SPIRAM**: 大容量メモリ

これらを理解することで、より高度なアプリケーションを開発できます。

---

## 参考資料

- [LVGL公式ドキュメント](https://docs.lvgl.io/)
- [EEZ Studio Wiki](https://github.com/eez-open/studio/wiki)
- [ESP32-P4 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-p4_technical_reference_manual_en.pdf)
- [M5Unified API Reference](https://github.com/m5stack/M5Unified)
