// M5Stack Tab5 EEZ-Studio UI プログラム
// 依存関係: ESP-Arduino >= V3.2, M5Unified >= 0.2.10, LVGL = V8.3.11
// lv_conf.h設定: LV_COLOR_DEPTH 16, LV_COLOR_16_SWAP 1, LV_MEM_CUSTOM 1, LV_TICK_CUSTOM 1

#include <M5Unified.h>
#include <lvgl.h>
#include "ui.h"
#include <esp_heap_caps.h>

// 定数定義
#define SERIAL_BAUD_RATE 115200      // シリアル通信のボーレート
#define DEFAULT_BRIGHTNESS 255       // 初期画面の明るさ（0-255）
#define LVGL_TIMER_DELAY_MS 1        // LVGLタイマーハンドラーの遅延時間（ミリ秒）

// Tab5の画面解像度（縦向き基準、LVGLで90度回転して横向きで使用）
#define EXAMPLE_LCD_H_RES 720
#define EXAMPLE_LCD_V_RES 1280
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)

// グローバル変数
static lv_disp_draw_buf_t g_draw_buf;       // LVGL描画バッファ
static lv_color_t *g_color_buf = nullptr;   // LVGL描画用のカラーバッファ

int Count_Val = 0;  // カウンター変数



// LVGLコールバック関数
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t width = (area->x2 - area->x1 + 1);   // 描画領域の幅を計算
    uint32_t height = (area->y2 - area->y1 + 1);  // 描画領域の高さを計算
    M5.Display.pushImageDMA(area->x1, area->y1, width, height, (uint16_t *)&color_p->full);  // DMAを使用して画像データをディスプレイに転送
    lv_disp_flush_ready(disp);  // フラッシュ処理の完了をLVGLに通知
}

void my_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    if (area->x1 % 2 != 0) area->x1 += 1;  // X座標の開始位置を偶数に調整
    if (area->y1 % 2 != 0) area->y1 += 1;  // Y座標の開始位置を偶数に調整
    uint32_t width = (area->x2 - area->x1 + 1);   // 幅を計算
    uint32_t height = (area->y2 - area->y1 + 1);  // 高さを計算
    if (width % 2 != 0) area->x2 -= 1;  // 幅が奇数の場合は終了位置を調整
    if (height % 2 != 0) area->y2 -= 1;  // 高さが奇数の場合は終了位置を調整
}

static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    auto touch_detail = M5.Touch.getDetail();  // タッチパネルから詳細情報を取得
    if (touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;  // プレス状態
        data->point.x = touch_detail.x;  // タッチポイントのX座標
        data->point.y = touch_detail.y;  // タッチポイントのY座標
    } else {
        data->state = LV_INDEV_STATE_REL;  // リリース状態
    }
} 



//________________________________________________________________________________ my_button_test_bar_event_handler()
// Callback that is triggered when "my_button_test_bar" is clicked.
static void my_button_test_bar_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
  if (code == LV_EVENT_CLICKED) {
    int RandVal = random(0, 100);

    char buf[4];
    lv_snprintf(buf, sizeof(buf), "%d", RandVal);

    lv_bar_set_value(objects.my_vertical_bar, RandVal, LV_ANIM_ON);
    lv_label_set_text(objects.my_label_vertical_bar, buf);
    
    lv_bar_set_value(objects.my_horizontal_bar, RandVal, LV_ANIM_ON);
    lv_label_set_text(objects.my_label_horizontal_bar, buf);
    
    lv_arc_set_value(objects.my_arc_bar, RandVal);
    lv_label_set_text(objects.my_label_arc_bar, buf);
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ my_button_toggle_event_handler()
// Callback that is triggered when "my_button_toggle" is clicked.
static void my_button_toggle_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);                      //--> Get the event code.
  lv_obj_t * my_button_toggle = (lv_obj_t*) lv_event_get_target(e); //--> Button that generated the event.
  
  if (code == LV_EVENT_VALUE_CHANGED) {
    LV_UNUSED(my_button_toggle);

    bool checked = lv_obj_has_state(my_button_toggle, LV_STATE_CHECKED);

    // Conditions for changing the button color (my_button_toggle).
    // 0xff + color code in hex.
    // Example :
    // 0xff + ff0000 (red in hex).
    // So for red: 0xffff0000.
    if (checked == true) {
      lv_obj_set_style_bg_color(objects.my_button_toggle, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_CHECKED);
      Serial.println("Button Toggle: ON");
    } else {
      lv_obj_set_style_bg_color(objects.my_button_toggle, lv_color_hex(0xffff7f50), LV_PART_MAIN | LV_STATE_DEFAULT);
      Serial.println("Button Toggle: OFF");
    }
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ my_switch_event_handler()
// Callback that is triggered when "my_switch" is clicked/toggled.
static void my_switch_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);                          //--> Get the event code.
  lv_obj_t * my_switch = (lv_obj_t*) lv_event_get_target(e);            //--> Switch that generated the event.
  lv_obj_t * my_label_switch = (lv_obj_t *)lv_event_get_user_data(e);   //--> Label or other UI elements we want to update (Optional).
  
  if (code == LV_EVENT_VALUE_CHANGED) {
    LV_UNUSED(my_switch);

    bool checked = lv_obj_has_state(my_switch, LV_STATE_CHECKED);

    if (checked == true) {
      lv_label_set_text(my_label_switch, "ON");
      lv_obj_set_style_text_color(my_label_switch, lv_color_hex(0xffff7f50), LV_PART_MAIN | LV_STATE_DEFAULT);
      Serial.println("Switch: ON");
    } else {
      lv_label_set_text(my_label_switch, "OFF");
      lv_obj_set_style_text_color(my_label_switch, lv_color_hex(0xffb6b6b6), LV_PART_MAIN | LV_STATE_DEFAULT);
      Serial.println("Switch: OFF");
    }
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ my_horizontal_slider_value_changed_event_cb()
// Callback to print Slider value (my_horizontal_slider) to Label (my_label_horizontal_slider)
// to be displayed on TFT LCD and change Canvas color.
// This callback is triggered when Slider value changes.
static void my_horizontal_slider_value_changed_event_cb(lv_event_t * e) {
  lv_obj_t * my_horizontal_slider = (lv_obj_t *)lv_event_get_target(e);            //--> Slider that generated the event.
  lv_obj_t * my_label_horizontal_slider = (lv_obj_t *)lv_event_get_user_data(e);   //--> Label or other UI elements we want to update (Optional).
  
  char buf[8];
  lv_snprintf(buf, sizeof(buf), "%d", (int)lv_slider_get_value(my_horizontal_slider));
  lv_label_set_text(my_label_horizontal_slider, buf);
}
//________________________________________________________________________________ 



//________________________________________________________________________________ my_vertical_slider_value_changed_event_cb()
// Callback to print Slider value (my_vertical_slider) to Label (my_label_vertical_slider)
// to be displayed on TFT LCD and change Canvas color.
// This callback is triggered when Slider value changes.
static void my_vertical_slider_value_changed_event_cb(lv_event_t * e) {
  lv_obj_t * my_vertical_slider       = (lv_obj_t *)lv_event_get_target(e);     //--> Slider that generated the event.
  lv_obj_t * my_label_vertical_slider = (lv_obj_t *)lv_event_get_user_data(e);  //--> Label or other UI elements we want to update (Optional).

  char buf[8];
  lv_snprintf(buf, sizeof(buf), "%d", (int)lv_slider_get_value(my_vertical_slider));
  lv_label_set_text(my_label_vertical_slider, buf);
}
//________________________________________________________________________________ 



//________________________________________________________________________________ my_image_esp32_event_handler()
// Callback that is triggered when "my_image_esp32" is clicked.
static void my_image_esp32_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
  if (code == LV_EVENT_CLICKED) {
    lv_label_set_text(objects.my_label_img_sta, "ESP32 image is\nTouched.");
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ my_image_lvgl_event_handler()
// Callback that is triggered when "my_image_lvgl" is clicked.
static void my_image_lvgl_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
  if (code == LV_EVENT_CLICKED) {
    lv_label_set_text(objects.my_label_img_sta, "LVGL image is\nTouched.");
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ my_image_eez_event_handler()
// Callback that is triggered when "my_image_eez" is clicked.
static void my_image_eez_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
  if (code == LV_EVENT_CLICKED) {
    lv_label_set_text(objects.my_label_img_sta, "EEZ image is\nTouched.");
  }
}
//________________________________________________________________________________ 



// 初期化関数
void initLvglDisplay(void)
{
    lv_init();  // LVGLライブラリの初期化
    g_color_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);  // SPIRAMを使用して描画バッファを確保（8ビットアライメント）
    
    if (g_color_buf == nullptr) {
        // メモリ確保失敗時のエラー処理
        M5.Display.setCursor(0, 0);
        M5.Display.println("Error: Failed to allocate SPIRAM buffer!");
        while(1) { delay(1000); }
    }
    
    lv_disp_draw_buf_init(&g_draw_buf, g_color_buf, NULL, LVGL_LCD_BUF_SIZE);  // 描画バッファの初期化
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);  // ディスプレイドライバーの初期化
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;  // 水平解像度
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;  // 垂直解像度
    // disp_drv.rounder_cb = my_rounder;  // 描画領域の調整コールバック（現在はコメントアウト）
    disp_drv.flush_cb = lv_disp_flush;  // フラッシュコールバックを設定
    disp_drv.draw_buf = &g_draw_buf;  // 描画バッファを設定
    disp_drv.sw_rotate = 1;  // ソフトウェア回転を有効化
    disp_drv.rotated = LV_DISP_ROT_90;  // ディスプレイを90度回転
    lv_disp_drv_register(&disp_drv);  // ディスプレイドライバーを登録
}

void initLvglTouch(void)
{
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);  // 入力デバイスドライバーの初期化
    indev_drv.type = LV_INDEV_TYPE_POINTER;  // 入力デバイスのタイプをポインタ（タッチパネル）に設定
    indev_drv.read_cb = lv_indev_read;  // 読み取りコールバックを設定
    lv_indev_drv_register(&indev_drv);  // 入力デバイスドライバーを登録
}

void setup()
{
    // M5Unifiedの初期化
    auto cfg = M5.config();  // M5Unifiedの設定を取得
    M5.begin(cfg);  // M5Unifiedの初期化（ディスプレイとタッチパネルを含む）
    
    // シリアル通信の初期化
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println();
    Serial.println("M5Stack Tab5 + LVGL + EEZ Studio");
    Serial.println();
    
    // LVGLバージョン情報
    String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println(LVGL_Arduino);
    
    // LVGL初期化
    Serial.println("Start LVGL Setup.");
    initLvglDisplay();  // LVGLディスプレイの初期化
    initLvglTouch();  // LVGLタッチ入力の初期化
    Serial.println("LVGL Setup Completed.");
    
    // ディスプレイの明るさ設定
    M5.Display.setBrightness(DEFAULT_BRIGHTNESS);

    // EEZ Studio UIの初期化
    ui_init();

    // イベントハンドラーの登録
    lv_obj_add_event_cb(objects.my_button_test_bar, my_button_test_bar_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.my_button_toggle, my_button_toggle_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.my_switch, my_switch_event_handler, LV_EVENT_VALUE_CHANGED, objects.my_label_switch);
    lv_obj_add_event_cb(objects.my_horizontal_slider, my_horizontal_slider_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, objects.my_label_horizontal_slider);
    lv_obj_add_event_cb(objects.my_vertical_slider, my_vertical_slider_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, objects.my_label_vertical_slider);
    lv_obj_add_event_cb(objects.my_image_esp32, my_image_esp32_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.my_image_lvgl, my_image_lvgl_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.my_image_eez, my_image_eez_event_handler, LV_EVENT_ALL, NULL);
    
    Serial.println("Setup Completed.");
}
//________________________________________________________________________________ 



void loop()
{
    M5.update();  // M5Unifiedの更新（タッチ入力などの処理）
    lv_timer_handler();  // LVGLのタイマーハンドラーを実行（イベント処理とアニメーション更新）
    delay(LVGL_TIMER_DELAY_MS);  // 短い遅延を入れてCPU負荷を軽減
}
//________________________________________________________________________________