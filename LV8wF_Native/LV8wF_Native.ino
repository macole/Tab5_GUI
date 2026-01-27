/*******************************************************************************
 * M5Stack Tab5 - EEZ Studio テンプレートプロジェクト
 * 依存関係:
 * - ESP-Arduino >= V3.2
 * - M5Unified >= 0.2.10
 * - LVGL = V8.3.11
 * 
 * 作成日: 2026-01-18
 * ライセンス: MIT
 ******************************************************************************/

#include <M5Unified.h>
#include <lvgl.h>
#include "src/ui/ui.h"
#include "src/ui/screens.h"
#include "src/ui/vars.h"
#include "src/ui/eez-flow.h"
#include <esp_heap_caps.h>
#include <cstdio>

// ============================================================================
// 定数定義
// ============================================================================

// ディスプレイ設定
#define SCREEN_WIDTH 720        // 実際の物理的な幅（縦向き時）
#define SCREEN_HEIGHT 1280      // 実際の物理的な高さ（縦向き時）
#define SCREEN_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

// LVGL設定
#define LVGL_TIMER_DELAY_MS 50   // LVGLタイマー遅延（ミリ秒）

// アプリケーション設定（カスタマイズ可能）
#define APP_UPDATE_INTERVAL_MS 100  // アプリケーション更新間隔

// ============================================================================
// グローバル変数
// ============================================================================

// LVGL描画バッファ（SPIRAMに確保）
static lv_disp_draw_buf_t g_draw_buf;
static lv_color_t *g_color_buf = nullptr;

// アプリケーション状態
static unsigned long g_lastUpdateTime = 0;
static int32_t g_arc1Value = 0;        // arc1の現在の値（0-100）
static int32_t g_lastCounter = -1;     // 前回のcounter値（変更検出用）

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

// ============================================================================
// LVGLコールバック関数
// ============================================================================
/**
 * @brief ディスプレイフラッシュコールバック
 * 
 * LVGLから呼ばれ、描画バッファの内容をディスプレイに転送します。
 * DMA転送を使用して高速化しています。
 * 
 * @param disp ディスプレイドライバ
 * @param area 更新領域
 * @param color_p カラーバッファ
 */
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    const uint32_t width = area->x2 - area->x1 + 1;
    const uint32_t height = area->y2 - area->y1 + 1;
    
    // DMA転送で画面更新（非ブロッキング）
    M5.Display.pushImageDMA(area->x1, area->y1, width, height, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

/**
 * @brief タッチ入力コールバック
 * 
 * LVGLから呼ばれ、タッチスクリーンの状態を報告します。
 * 
 * @param indev_driver 入力デバイスドライバ
 * @param data 入力データ
 */
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

// ============================================================================
// 初期化関数
// ============================================================================
/**
 * @brief SPIRAMにディスプレイバッファを確保
 * 
 * 全画面バッファ（1280x720x2バイト = 約1.8MB）をSPIRAMに確保します。
 * 
 * @return true 成功
 * @return false 失敗
 */
bool allocateDisplayBuffer()
{
    g_color_buf = (lv_color_t *)heap_caps_malloc(
        sizeof(lv_color_t) * SCREEN_BUFFER_SIZE,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    
    if (g_color_buf == nullptr) {
        M5.Display.setCursor(0, 0);
        M5.Display.println("Error: SPIRAM allocation failed!");
        return false;
    }
    
    return true;
}

/**
 * @brief ディスプレイドライバの設定
 * 
 * LVGLディスプレイドライバを設定し、90度回転して横向き表示にします。
 * 
 * @param disp_drv ディスプレイドライバ
 */
void configureDisplayDriver(lv_disp_drv_t &disp_drv)
{
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &g_draw_buf;
    disp_drv.sw_rotate = 1;              // ソフトウェア回転有効
    disp_drv.rotated = LV_DISP_ROT_90;   // 90度回転（横向き表示）
    lv_disp_drv_register(&disp_drv);
}

/**
 * @brief LVGLディスプレイ初期化
 * 
 * LVGL、描画バッファ、ディスプレイドライバを初期化します。
 * 
 * @return true 成功
 * @return false 失敗
 */
bool initLvglDisplay()
{
    lv_init();
    
    if (!allocateDisplayBuffer()) {
        return false;
    }
    
    lv_disp_draw_buf_init(&g_draw_buf, g_color_buf, NULL, SCREEN_BUFFER_SIZE);
    
    static lv_disp_drv_t disp_drv;
    configureDisplayDriver(disp_drv);
    
    return true;
}

/**
 * @brief LVGLタッチ入力初期化
 * 
 * LVGLタッチスクリーン入力デバイスを初期化します。
 */
void initLvglTouch()
{
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_indev_read;
    lv_indev_drv_register(&indev_drv);
}

// ============================================================================
// アプリケーション機能
// ============================================================================

/**
 * @brief arc1の値を自動更新
 * 
 * flagがtrueの時に、arc1の値を0-100まで自動更新します。
 */
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

/**
 * @brief counterの値をtext_areaに表示
 * 
 * counterの値が変更された場合のみ、text_areaに表示を更新します。
 */
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

/**
 * @brief アプリケーション更新処理
 * 
 * 定期的に実行されるアプリケーション固有の処理を記述します。
 */
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

/**
 * @brief エラー表示（無限ループ）
 * 
 * 初期化エラー時に赤画面を点滅させます。
 */
static void showFatalError()
{
    while(1) {
        M5.Display.fillScreen(TFT_RED);
        delay(500);
        M5.Display.fillScreen(TFT_BLACK);
        delay(500);
    }
}

void setup()
{
    auto cfg = M5.config();
    M5.begin(cfg);
    delay(100);
    
    // LVGL初期化
    if (!initLvglDisplay()) {
        showFatalError();
    }
    
    initLvglTouch();
    ui_init();  // EEZ-Studio UI初期化
}

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
