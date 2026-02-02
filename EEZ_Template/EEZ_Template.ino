/*******************************************************************************
 * M5Stack Tab5 - EEZ Studio テンプレートプロジェクト
 * 
 * このテンプレートは、EEZ StudioとLVGLを使用したM5Stack Tab5アプリケーション
 * の基礎となるプロジェクトです。カスタマイズして独自のアプリケーションを
 * 作成してください。
 * 
 * 機能:
 * - LVGL 8.3.11ベースのUI
 * - EEZ Studio Flow言語サポート
 * - M5Unified統合
 * 
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
#include <esp_heap_caps.h>

// ============================================================================
// 定数定義
// ============================================================================

// ディスプレイ設定
#define SCREEN_WIDTH 720        // 実際の物理的な幅（縦向き時）
#define SCREEN_HEIGHT 1280      // 実際の物理的な高さ（縦向き時）
#define SCREEN_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

// LVGL設定
#define LVGL_TIMER_DELAY_MS 5   // LVGLタイマー遅延（ミリ秒）

// アプリケーション設定（カスタマイズ可能）
#define APP_UPDATE_INTERVAL_MS 50  // アプリケーション更新間隔

// ============================================================================
// グローバル変数
// ============================================================================

// LVGL描画バッファ（SPIRAMに確保）
static lv_disp_draw_buf_t g_draw_buf;
static lv_color_t *g_color_buf = nullptr;

// アプリケーション状態
static unsigned long g_lastUpdateTime = 0;

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
 * @brief アプリケーション更新処理
 * 
 * 定期的に実行されるアプリケーション固有の処理を記述します。
 * この関数をカスタマイズして独自の機能を追加してください。
 * 
 */
void updateApplication()
{
    unsigned long currentTime = millis();
    
    // 指定間隔で更新
    if (currentTime - g_lastUpdateTime < APP_UPDATE_INTERVAL_MS) {
        return;
    }
    
    g_lastUpdateTime = currentTime;
    
    // ここにアプリケーション固有の処理を追加
    // 例: センサー読み取り、データ送信、状態更新など
}

// ============================================================================
// Arduino標準関数
// ============================================================================

void setup()
{
    auto cfg = M5.config();    // M5Unified初期化
    M5.begin(cfg);
    delay(100);
    
    // LVGL初期化
    if (!initLvglDisplay()) {
        while(1) { 
            M5.Display.fillScreen(TFT_RED);
            delay(500);
            M5.Display.fillScreen(TFT_BLACK);
            delay(500);
        }
    }
    initLvglTouch();
    
    ui_init();    // EEZ-Studio UI初期化
}

void loop()
{
    M5.update();    // M5Unifiedの更新（ボタン、タッチなど）
    lv_timer_handler();
    ui_tick();    // EEZ Flow Tickハンドラ（Flow言語の実行）
    updateApplication();
    delay(LVGL_TIMER_DELAY_MS);    // LVGL処理のためのわずかな遅延
}
