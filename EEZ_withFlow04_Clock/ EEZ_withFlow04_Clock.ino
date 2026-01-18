// EEZ_withFlow04_Clock - M5Stack Tab5 時計アプリケーション
// SquareLine Studio + LVGL 8.3.11 で作成
// 依存関係: ESP-Arduino >= V3.2, M5Unified >= 0.2.10, LVGL = V8.3.11
// lv_conf.h設定: LV_COLOR_DEPTH 16, LV_COLOR_16_SWAP 1, LV_MEM_CUSTOM 1, LV_TICK_CUSTOM 1

#include <M5Unified.h>
#include "lvgl.h"
#include "ui.h"
#include "pins_config.h"

// 定数定義
#define SERIAL_BAUD_RATE 115200  // シリアル通信のボーレート
#define COUNTER_MAX_VALUE 1000   // カウンターの最大値（オーバーフロー時に0にリセット）
#define DEFAULT_BRIGHTNESS 255   // 初期画面の明るさ（0-255）
#define LVGL_TIMER_DELAY_MS 1    // LVGLタイマーハンドラーの遅延時間（ミリ秒）
#define MAX_TOUCH_POINTS 3        // タッチポイントの最大数

// グローバル変数
static uint16_t g_counter = 0;                   // 自動カウント用のカウンター
static bool g_automateEnabled = false;           // 自動カウント機能の有効/無効フラグ
static lv_disp_draw_buf_t g_draw_buf;            // LVGL描画バッファ
static lv_color_t *g_color_buf = nullptr;        // LVGL描画用のカラーバッファ

// LVGLコールバック関数
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t width = (area->x2 - area->x1 + 1);  // 描画領域の幅を計算
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

// 初期化関数
void initLvglDisplay(void)
{
    lv_init();  // LVGLライブラリの初期化
    g_color_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);  // SPIRAMを使用して描画バッファを確保（8ビットアライメント）
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
    auto cfg = M5.config();  // M5Unifiedの設定を取得
    M5.begin(cfg);  // M5Unifiedの初期化（ディスプレイとタッチパネルを含む）
    Serial.begin(SERIAL_BAUD_RATE);  // シリアル通信の初期化（デバッグ用）
    initLvglDisplay();  // LVGLディスプレイの初期化
    initLvglTouch();  // LVGLタッチ入力の初期化
    ui_init();  // UIの初期化
    M5.Display.setBrightness(DEFAULT_BRIGHTNESS);  // ディスプレイの明るさを初期値に設定
}

// メインループ処理関数
void updateAutomation(void)
{
    g_automateEnabled = lv_obj_has_state(ui_Button1, LV_STATE_CHECKED);  // ボタンのチェック状態を確認して自動カウント機能の有効/無効を設定
    if (g_automateEnabled) {
        lv_arc_set_value(ui_Arc1, g_counter);  // アーク（円形プログレスバー）の値を更新
        lv_label_set_text_fmt(ui_Label1, "%d", g_counter);  // ラベルのテキストをカウンターの値に更新
        g_counter++;  // カウンターをインクリメント
        if (g_counter >= COUNTER_MAX_VALUE) g_counter = 0;  // カウンターが最大値に達したら0にリセット
    }
}

void updateBrightness(void)
{
    uint8_t brightness = lv_slider_get_value(ui_Slider1);  // スライダーから明るさの値を取得（0-255）
    M5.Display.setBrightness(brightness);  // ディスプレイの明るさを設定
}

void loop()
{
    M5.update();  // M5Unifiedの更新（タッチ入力などの処理）
    lv_timer_handler();  // LVGLのタイマーハンドラーを実行（イベント処理とアニメーション更新）
    delay(LVGL_TIMER_DELAY_MS);  // 短い遅延を入れてCPU負荷を軽減
    updateAutomation();  // 自動カウント機能の更新
    updateBrightness();  // 画面の明るさの更新
}
