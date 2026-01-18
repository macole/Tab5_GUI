// M5Stack Tab5 EEZ-Studio UI プログラム (with Flow)
// 依存関係: ESP-Arduino >= V3.2, M5Unified >= 0.2.10, LVGL = V8.3.11
// lv_conf.h設定: LV_COLOR_DEPTH 16, LV_COLOR_16_SWAP 1, LV_MEM_CUSTOM 1, LV_TICK_CUSTOM 1

#include <M5Unified.h>
#include <lvgl.h>
#include "ui.h"
#include "screens.h"
#include <esp_heap_caps.h>

// ==================== 定数定義 ====================
// シリアル通信
#define SERIAL_BAUD_RATE 115200

// ディスプレイ設定
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 1280
#define SCREEN_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
#define DEFAULT_BRIGHTNESS 255

// タイマー設定
#define LVGL_TIMER_DELAY_MS 1

// アプリケーション設定
#define COUNTER_MAX_VALUE 255

// ==================== グローバル変数 ====================
// LVGL描画バッファ
static lv_disp_draw_buf_t g_draw_buf;
static lv_color_t *g_color_buf = nullptr;

// アプリケーション状態
static uint16_t g_counter = 0;
static bool g_automateEnabled = false;

// ==================== LVGLコールバック関数 ====================

// ディスプレイフラッシュコールバック
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    const uint32_t width = area->x2 - area->x1 + 1;
    const uint32_t height = area->y2 - area->y1 + 1;
    
    M5.Display.pushImageDMA(area->x1, area->y1, width, height, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

// タッチ入力コールバック
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    const auto touch_detail = M5.Touch.getDetail();
    
    if (touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_detail.x;
        data->point.y = touch_detail.y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ==================== 初期化関数 ====================

// SPIRAMバッファの確保
bool allocateDisplayBuffer()
{
    g_color_buf = (lv_color_t *)heap_caps_malloc(
        sizeof(lv_color_t) * SCREEN_BUFFER_SIZE,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    
    if (g_color_buf == nullptr) {
        Serial.println("Error: Failed to allocate SPIRAM buffer!");
        M5.Display.setCursor(0, 0);
        M5.Display.println("Error: SPIRAM allocation failed!");
        return false;
    }
    
    Serial.printf("Display buffer allocated: %d bytes (%.2f MB)\n", 
                  sizeof(lv_color_t) * SCREEN_BUFFER_SIZE,
                  (sizeof(lv_color_t) * SCREEN_BUFFER_SIZE) / 1048576.0);
    return true;
}

// ディスプレイドライバの設定
void configureDisplayDriver(lv_disp_drv_t &disp_drv)
{
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &g_draw_buf;
    disp_drv.sw_rotate = 1;
    disp_drv.rotated = LV_DISP_ROT_90;
    lv_disp_drv_register(&disp_drv);
}

// LVGLディスプレイ初期化
bool initLvglDisplay()
{
    Serial.println("Initializing LVGL display...");
    
    lv_init();
    
    if (!allocateDisplayBuffer()) {
        return false;
    }
    
    lv_disp_draw_buf_init(&g_draw_buf, g_color_buf, NULL, SCREEN_BUFFER_SIZE);
    
    static lv_disp_drv_t disp_drv;
    configureDisplayDriver(disp_drv);
    
    Serial.println("LVGL display initialized");
    return true;
}

// LVGLタッチ初期化
void initLvglTouch()
{
    Serial.println("Initializing LVGL touch...");
    
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_indev_read;
    lv_indev_drv_register(&indev_drv);
    
    Serial.println("LVGL touch initialized");
}

// ==================== アプリケーション機能 ====================

// 自動カウント機能
void updateAutomation()
{
    g_automateEnabled = lv_obj_has_state(objects.button1, LV_STATE_CHECKED);
    
    if (g_automateEnabled) {
        lv_label_set_text_fmt(objects.counter, "%d", g_counter);
        g_counter++;
        if (g_counter >= COUNTER_MAX_VALUE) {
            g_counter = 0;
        }
    }
}

// 画面の明るさ調整
void updateBrightness()
{
    const uint8_t brightness = lv_slider_get_value(objects.slider);
    M5.Display.setBrightness(brightness);
}

// ==================== Arduino標準関数 ====================

void setup()
{
    // M5Unified初期化
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // シリアル通信初期化
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println();
    Serial.println("==============================");
    Serial.println("  M5Stack Tab5 EEZ-Studio UI  ");
    Serial.println("  with Flow Support           ");
    Serial.println("==============================");
    Serial.printf("LVGL Version: %d.%d.%d\n", 
                  lv_version_major(), lv_version_minor(), lv_version_patch());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    
    // LVGL初期化
    if (!initLvglDisplay()) {
        Serial.println("FATAL: Display initialization failed!");
        while(1) { delay(1000); }
    }
    initLvglTouch();
    
    // EEZ-Studio UI初期化
    Serial.println("Initializing EEZ-Studio UI...");
    ui_init();
    Serial.println("EEZ-Studio UI initialized");
    
    // 画面の明るさ設定
    M5.Display.setBrightness(DEFAULT_BRIGHTNESS);
    
    Serial.println("Setup completed");
    Serial.println("==============================");
    Serial.printf("Free Heap after setup: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM after setup: %d bytes\n", ESP.getFreePsram());
    Serial.println();
}

void loop()
{
    M5.update();
    lv_timer_handler();
    ui_tick();
    updateAutomation();
    updateBrightness();
    delay(LVGL_TIMER_DELAY_MS);
}

