/*
M5Stack Tab5 LVGL / SLS Initial Test - 23.05.25 - nikthefix

This preliminary demo just uses the display and touch features.
Peripherals like audio, gyro, camera etc will be added soon.

Dependencies:

ESP-Arduino >= V3.2 (tested also working with 3.3.0-alpha1)
M5GFX >= V0.2.8
LVGL = V9.4 (updated from V8.3.11)

lv_conf.h:

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1
#define LV_MEM_CUSTOM 1
#define LV_TICK_CUSTOM 1

Build Options:

Board: "ESP32P4 Dev Module"
USB CDC on boot: "Enabled"
Flash Size: "16MB (128Mb)"
Partition Scheme: "Custom" (the supplied partitions.csv file allows almost full use of the flash for the main app)
PSRAM: "Enabled"
Upload Mode: "UART / Hardware CDC"
USB Mode: "Hardware CDC and JTAG"

Notes:

This demo uses a software rotate in order to give us landscape mode (lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90))
I've not been able to find the make and model of the display unit used in the Tab5 so I can't be sure if it supports a hardware accelerated rotation.
The display driver chip is ili9881c with native portrait orientation.

A Square Line Studio V1.5.1 project is included so that you can experiment with your own exported UI files.

*/



#include <M5Unified.h>
#include <M5GFX.h>
#include "lvgl.h"
#include "ui.h"
#include "pins_config.h"

M5GFX display;

uint16_t count = 0;
bool automate = false;
 
// LVGL 9.xでは描画バッファはlv_display_set_buffers()で設定
static lv_color_t *buf;

// LVGL 9.xのディスプレイと入力デバイスのハンドル
static lv_display_t* lvgl_display = NULL;
static lv_indev_t* lvgl_indev = NULL;

// LVGL 9.xのディスプレイドライバ：画面への描画転送
void display_flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    display.pushImageDMA(area->x1, area->y1, w, h, (uint16_t*)px_map); 
    lv_display_flush_ready(disp);
}

// DMA転送の完了を待つコールバック
void display_flush_wait_cb(lv_display_t* disp) {
    display.waitDMA();
}

// LVGL 9.xの入力デバイスドライバ：タッチ入力の処理
void touchpad_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{  
    lgfx::touch_point_t tp[3];
    // getTouch()を使用すると、setRotation()に合わせて自動的に座標変換される
    uint8_t touchpad = display.getTouch(tp, 3);
    if (touchpad > 0)
    {
        data->state = LV_INDEV_STATE_PRESSED;  // LVGL 9.xではPRESSED
        // getTouch()は既に回転後の座標を返すので、そのまま使用
        data->point.x = tp[0].x;
        data->point.y = tp[0].y;
        //Serial.printf("Touch: (%d, %d)\n", tp[0].x, tp[0].y); //for testing
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;  // LVGL 9.xではRELEASED
    }
}



void setup()
{
    // Initialize Tab5 (power, backlight, panel) via M5Unified, then bind display
    auto cfg = M5.config();
    M5.begin(cfg);
    display = M5.Display;

    Serial.begin(115200);//for debug
    
    // ディスプレイを270度回転（横向き：1280x720）
    // 注意: Tab5の物理ディスプレイは縦向き（720x1280）だが、横向きで使用する
    display.setRotation(3);
    
    // 実際のディスプレイサイズを確認
    int32_t actualWidth = display.width();
    int32_t actualHeight = display.height();
    Serial.printf("Display size after rotation: %dx%d\n", actualWidth, actualHeight);

    /*Initialize LVGL*/
    lv_init();
    
    // 描画バッファをPSRAMに確保
    // 注意: 横向き（1280x720）として使用するため、バッファサイズを計算
    #define LCD_HORIZONTAL_RES 1280
    #define LCD_VERTICAL_RES 720
    uint32_t buf_size = LCD_HORIZONTAL_RES * LCD_VERTICAL_RES;
    buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (buf == NULL) {
        Serial.println("Error: Failed to allocate LVGL buffer!");
        return;
    }
    
    // LVGL 9.x API: ディスプレイを作成（横向き：1280x720）
    // 注意: M5.Display.setRotation(3)で270度回転しているので、
    // LVGLは横向き（1280x720）として作成
    lvgl_display = lv_display_create(LCD_HORIZONTAL_RES, LCD_VERTICAL_RES);
    
    // 色フォーマットを設定（RGB565、バイトスワップあり）
    lv_display_set_color_format(lvgl_display, LV_COLOR_FORMAT_RGB565_SWAPPED);
    
    // 描画バッファを設定
    lv_display_set_buffers(lvgl_display, buf, NULL, 
                          sizeof(lv_color_t) * buf_size, 
                          LV_DISPLAY_RENDER_MODE_FULL);
    
    // 描画転送コールバックを設定
    lv_display_set_flush_cb(lvgl_display, display_flush_cb);
    
    // DMA転送の完了を待つコールバックを設定
    lv_display_set_flush_wait_cb(lvgl_display, display_flush_wait_cb);
    
    // デフォルトディスプレイとして設定
    lv_display_set_default(lvgl_display);
    
    // 注意: LVGLの回転は不要（M5.Display側で回転済み）

    /*Initialize touch - LVGL 9.x API*/
    lvgl_indev = lv_indev_create();
    if (lvgl_indev == NULL) {
        Serial.println("Error: Failed to create input device!");
        return;
    }
    
    lv_indev_set_type(lvgl_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(lvgl_indev, touchpad_read_cb);
    lv_indev_set_display(lvgl_indev, lvgl_display);     



    /*Start UI*/
    ui_init();   
    display.setBrightness(255);
    display.fillScreen(TFT_BLACK);
}



void loop()
{
    // LVGL 9.xではlv_timer_handler()を使用
    uint32_t time_till_next = lv_timer_handler();
    
    if(lv_obj_has_state(ui_Button1, LV_STATE_CHECKED)) automate = true;
    else automate = false;
    
    if(automate == true)
    {
        lv_arc_set_value(ui_Arc1, count);
        lv_label_set_text_fmt(ui_Label1, "%d", count);
        count++;
        if (count == 1000) count = 0; 
    }

    uint8_t brightness = lv_slider_get_value(ui_Slider1);
    display.setBrightness(brightness);
    
    // 待機時間を調整
    if (time_till_next > 0 && time_till_next < 100) {
        delay(time_till_next);
    } else {
        delay(1);
    }
}
