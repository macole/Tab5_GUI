/*
 * M5Stack Tab5 LVGL Basic Demo
 * 
 * このプログラムは、M5Stack Tab5でLVGLを使用した基本的なUIデモです。
 * SquareLine Studioなどの外部ツールを使わず、コードだけでUIを作成しています。
 * 
 * 機能：
 * - 円形ゲージ（アーク）の表示と更新
 * - 自動カウント機能（ボタンでON/OFF）
 * - 明るさ調整スライダー
 * - タッチ操作対応
 * 
 * 依存ライブラリ：
 * - M5Unified
 * - LVGL 8.3 (Arduinoライブラリマネージャーからインストール)
 * 
 * ビルド設定：
 * - Board: ESP32P4 Dev Module
 * - PSRAM: Enabled
 * - Flash Size: 16MB
 * - Partition Scheme: Custom (partitions.csv)
 */

#include <M5Unified.h>  // M5GFX.hも自動的にインクルードされる
#include "lvgl.h"
#include "config.h"

// ============================================================================
// 定数定義
// ============================================================================
// デバッグ出力の有効/無効（0で無効、1で有効）
#define DEBUG_ENABLED 1

// カウンター設定
#define COUNTER_MIN 0
#define COUNTER_MAX 1000
#define COUNTER_UPDATE_INTERVAL_MS 50  // カウンター更新間隔（ミリ秒）

// バックライト設定
#define BRIGHTNESS_MIN 1
#define BRIGHTNESS_MAX 255
#define BRIGHTNESS_DEFAULT 255

// UI要素のサイズと位置
#define ARC_GAUGE_SIZE 400
#define ARC_GAUGE_OFFSET_Y -50
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 50
#define BUTTON_OFFSET_Y 150
#define SLIDER_WIDTH 20
#define SLIDER_HEIGHT 400
#define SLIDER_OFFSET_X -50
#define LABEL_BRIGHTNESS_OFFSET_Y 220

// 色定義（16進数）
#define COLOR_BACKGROUND 0x000000
#define COLOR_TEXT_TITLE 0x808080
#define COLOR_TEXT_VALUE 0xEEFB02
#define COLOR_BUTTON_BORDER 0x919191
#define COLOR_BUTTON_CHECKED 0x6A3F63
#define COLOR_SLIDER_BG 0x333333
#define COLOR_SLIDER_INDICATOR 0xDA88EA
#define COLOR_SLIDER_KNOB 0xFFFFFF

// デバッグ出力間隔
#define DEBUG_STATUS_INTERVAL_MS 5000
#define DEBUG_TOUCH_INTERVAL_MS 100
#define DEBUG_FLUSH_INTERVAL_MS 5000
#define DEBUG_COUNTER_INTERVAL 10

// DMA転送タイムアウト
#define DMA_WAIT_TIMEOUT_MS 100

// ============================================================================
// グローバル変数
// ============================================================================
M5GFX display;  // ディスプレイオブジェクト

// UIオブジェクト（LVGLウィジェットへのポインタ）
lv_obj_t* screenMain;        // メイン画面
lv_obj_t* arcGauge;          // 円形ゲージ
lv_obj_t* labelValue;        // 数値表示ラベル
lv_obj_t* buttonAutomate;    // 自動カウントボタン
lv_obj_t* labelTitle;        // タイトルラベル
lv_obj_t* sliderBrightness;  // 明るさ調整スライダー
lv_obj_t* labelBrightness;   // 明るさラベル

// アプリケーション状態
uint16_t counter = COUNTER_MIN;         // カウンター値（0-1000）
bool isAutomateMode = false;            // 自動カウントモードのON/OFF
unsigned long lastCounterUpdate = 0;     // 前回のカウンター更新時刻

// LVGL描画バッファ（LVGL 8.x API）
static lv_disp_draw_buf_t draw_buf;
static lv_color_t* buf;

// LVGLディスプレイと入力デバイスのドライバ（LVGL 8.x API）
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

// ============================================================================
// デバッグ用マクロ
// ============================================================================
#if DEBUG_ENABLED
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(...)
#endif

// ============================================================================
// イベントコールバック関数
// ============================================================================
/**
 * ボタンのイベントコールバック
 * ボタンの状態変化を処理し、デバッグ情報を出力
 */
void button_event_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);  // LVGL 8.xではget_target()を使用
    
    if (code == LV_EVENT_VALUE_CHANGED) {
        bool checked = (lv_obj_get_state(target) & LV_STATE_CHECKED) != 0;
        DEBUG_PRINTF("Button event: VALUE_CHANGED, checked=%d\n", checked);
    } else if (code == LV_EVENT_CLICKED) {
        DEBUG_PRINTLN("Button event: CLICKED");
    } else if (code == LV_EVENT_PRESSED) {
        DEBUG_PRINTLN("Button event: PRESSED");
    } else if (code == LV_EVENT_RELEASED) {
        DEBUG_PRINTLN("Button event: RELEASED");
    }
}

// ============================================================================
// LVGLディスプレイドライバ：画面への描画転送（LVGL 8.x API）
// ============================================================================
/**
 * ディスプレイへの描画転送コールバック
 * LVGLが描画データを画面に転送するために呼び出される
 */
void lv_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    // 描画領域のサイズを計算
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
#if DEBUG_ENABLED
    // デバッグ：最初の数回の描画を確認
    static uint32_t flushCount = 0;
    static unsigned long lastFlushDebugTime = 0;
    static bool firstFlush = true;
    unsigned long currentTime = millis();
    
    flushCount++;
    
    // 最初の描画を必ずログに出力
    if (firstFlush) {
        DEBUG_PRINTF("First display flush! area: (%d,%d)-(%d,%d) size=%dx%d\n",
                     area->x1, area->y1, area->x2, area->y2, w, h);
        firstFlush = false;
    }
    
    // 定期的に描画回数を確認
    if (currentTime - lastFlushDebugTime >= DEBUG_FLUSH_INTERVAL_MS) {
        DEBUG_PRINTF("Display flush: %lu times in %d sec, last area: (%d,%d)-(%d,%d) size=%dx%d\n",
                     flushCount, DEBUG_FLUSH_INTERVAL_MS / 1000,
                     area->x1, area->y1, area->x2, area->y2, w, h);
        flushCount = 0;
        lastFlushDebugTime = currentTime;
    }
#endif
    
    // DMA転送で高速に画面へ描画
    // 注意: pushImageDMAは非同期だが、LVGLはバッファを保護するため、
    // すぐにlv_disp_flush_ready()を呼んでも問題ない
    display.pushImageDMA(area->x1, area->y1, w, h, (uint16_t *)&color_p->full); 
    
    // 描画完了をLVGLに通知（重要：これを呼ばないと描画が止まる）
    // waitDMA()は呼ばない - 非同期で動作させ、LVGLの描画ループをブロックしない
    lv_disp_flush_ready(disp_drv);
}

// ============================================================================
// LVGL入力デバイスドライバ：タッチ入力の処理（LVGL 8.x API）
// ============================================================================
/**
 * タッチ入力の読み取りコールバック
 * LVGLがタッチ入力の状態を取得するために呼び出される
 */
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    // タッチポイントを取得
    // getTouchRaw()を使用して生のタッチ座標を取得
    // LVGLでソフトウェア回転（LV_DISP_ROT_90）を使用しているため、
    // 座標変換はLVGL側で行われる
    lgfx::touch_point_t tp[3];
    uint8_t touchCount = display.getTouchRaw(tp, 3);  // getTouchRaw()を使用（生の座標）
    
#if DEBUG_ENABLED
    // デバッグ：コールバックが呼ばれているか確認
    static uint32_t callCount = 0;
    static unsigned long lastDebugTime = 0;
    unsigned long currentTime = millis();
    
    callCount++;
    // 1秒ごとにコールバックが呼ばれているか確認
    if (currentTime - lastDebugTime >= 1000) {
        DEBUG_PRINTF("lv_indev_read: called %lu times/sec\n", callCount);
        callCount = 0;
        lastDebugTime = currentTime;
    }
#else
    unsigned long currentTime = millis();
#endif
    
    if (touchCount > 0) {
        // タッチされている場合
        data->state = LV_INDEV_STATE_PR;  // プレス状態（LVGL 8.x）
        
        // getTouchRaw()は生の座標を返すので、そのまま使用
        // LVGLのソフトウェア回転（LV_DISP_ROT_90）が座標変換を行う
        data->point.x = tp[0].x;
        data->point.y = tp[0].y;
        
#if DEBUG_ENABLED
        // デバッグ用（タッチ座標を確認）
        static unsigned long lastTouchTime = 0;
        if (currentTime - lastTouchTime >= DEBUG_TOUCH_INTERVAL_MS) {
            DEBUG_PRINTF("Touch: (%d, %d) count=%d\n", tp[0].x, tp[0].y, touchCount);
            lastTouchTime = currentTime;
        }
#endif
    } else {
        // タッチされていない場合
        data->state = LV_INDEV_STATE_REL;  // リリース状態（LVGL 8.x）
    }
}

// ============================================================================
// ヘルパー関数：UI要素の作成
// ============================================================================
/**
 * タイトルラベルを作成
 */
static void createTitleLabel(void) {
    labelTitle = lv_label_create(screenMain);
    lv_label_set_text(labelTitle, "LVGL Basic Demo");
    lv_obj_set_style_text_color(labelTitle, lv_color_hex(COLOR_TEXT_TITLE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(labelTitle, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(labelTitle, LV_ALIGN_TOP_MID, 0, 20);
}

/**
 * 円形ゲージ（アーク）を作成
 */
static void createArcGauge(void) {
    arcGauge = lv_arc_create(screenMain);
    lv_obj_set_size(arcGauge, ARC_GAUGE_SIZE, ARC_GAUGE_SIZE);
    lv_obj_align(arcGauge, LV_ALIGN_CENTER, 0, ARC_GAUGE_OFFSET_Y);
    lv_arc_set_range(arcGauge, COUNTER_MIN, COUNTER_MAX);
    lv_arc_set_value(arcGauge, COUNTER_MIN);
    lv_obj_remove_style(arcGauge, NULL, LV_PART_KNOB);  // ノブを非表示
    // 注意: クリック可能のままにしておく（Arcを操作できるようにする）
}

/**
 * 数値表示ラベルを作成
 */
static void createValueLabel(void) {
    labelValue = lv_label_create(screenMain);
    lv_label_set_text(labelValue, "0");
    lv_obj_set_style_text_color(labelValue, lv_color_hex(COLOR_TEXT_VALUE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(labelValue, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(labelValue, LV_ALIGN_CENTER, 0, ARC_GAUGE_OFFSET_Y);
}

/**
 * 自動カウントボタンを作成
 */
static void createAutomateButton(void) {
    buttonAutomate = lv_btn_create(screenMain);
    lv_obj_set_size(buttonAutomate, BUTTON_WIDTH, BUTTON_HEIGHT);
    lv_obj_align(buttonAutomate, LV_ALIGN_CENTER, 0, BUTTON_OFFSET_Y);
    lv_obj_add_flag(buttonAutomate, LV_OBJ_FLAG_CHECKABLE);  // トグルボタンとして設定
    
    // ボタンのスタイル設定
    lv_obj_set_style_bg_color(buttonAutomate, lv_color_hex(COLOR_BACKGROUND), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(buttonAutomate, lv_color_hex(COLOR_BUTTON_BORDER), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(buttonAutomate, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(buttonAutomate, lv_color_hex(COLOR_BUTTON_CHECKED), LV_PART_MAIN | LV_STATE_CHECKED);
    
    // ボタンのラベルを作成
    lv_obj_t* labelButton = lv_label_create(buttonAutomate);
    lv_label_set_text(labelButton, "Automate");
    lv_obj_center(labelButton);
    
    // ボタンのイベントコールバックを追加
    lv_obj_add_event_cb(buttonAutomate, button_event_cb, LV_EVENT_ALL, NULL);
}

/**
 * 明るさ調整スライダーを作成
 */
static void createBrightnessSlider(void) {
    sliderBrightness = lv_slider_create(screenMain);
    lv_slider_set_range(sliderBrightness, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    lv_slider_set_value(sliderBrightness, BRIGHTNESS_DEFAULT, LV_ANIM_OFF);
    lv_obj_set_size(sliderBrightness, SLIDER_WIDTH, SLIDER_HEIGHT);
    lv_obj_align(sliderBrightness, LV_ALIGN_RIGHT_MID, SLIDER_OFFSET_X, 0);
    
    // スライダーのスタイルを設定
    lv_obj_set_style_bg_color(sliderBrightness, lv_color_hex(COLOR_SLIDER_BG), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sliderBrightness, lv_color_hex(COLOR_SLIDER_INDICATOR), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sliderBrightness, lv_color_hex(COLOR_SLIDER_KNOB), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sliderBrightness, 10, LV_PART_KNOB | LV_STATE_DEFAULT);
}

/**
 * 明るさラベルを作成
 */
static void createBrightnessLabel(void) {
    labelBrightness = lv_label_create(screenMain);
    lv_label_set_text(labelBrightness, "Brightness");
    lv_obj_set_style_text_color(labelBrightness, lv_color_hex(COLOR_SLIDER_INDICATOR), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(labelBrightness, LV_ALIGN_RIGHT_MID, SLIDER_OFFSET_X, LABEL_BRIGHTNESS_OFFSET_Y);
}

// ============================================================================
// LVGL初期化関数（LVGL 8.x API対応）
// ============================================================================
/**
 * LVGLライブラリとディスプレイ、入力デバイスを初期化
 */
void initializeLVGL(void) {
    // LVGLライブラリの初期化
    lv_init();
    
    // 描画バッファをPSRAMに確保（大きなバッファを確保するため）
    buf = (lv_color_t*)heap_caps_malloc(
        sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    
    if (buf == NULL) {
        DEBUG_PRINTLN("Error: Failed to allocate LVGL buffer!");
        return;
    }
    
    // 描画バッファを初期化（LVGL 8.x API）
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_LCD_BUF_SIZE);
    
    // ディスプレイドライバを初期化（LVGL 8.x API）
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_HORIZONTAL_RES;  // 物理横解像度（720）
    disp_drv.ver_res = LCD_VERTICAL_RES;   // 物理縦解像度（1280）
    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.sw_rotate = 1;  // ソフトウェア回転を有効化
    disp_drv.rotated = LV_DISP_ROT_90;  // 90度回転（横向き：1280x720）
    lv_disp_drv_register(&disp_drv);
    
    // 入力デバイス（タッチ）を初期化（LVGL 8.x API）
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_indev_read;
    lv_indev_drv_register(&indev_drv);
    
    DEBUG_PRINTLN("LVGL initialized successfully");
    DEBUG_PRINTLN("Input device created and linked to display");
    Serial.flush();  // シリアル出力を確実に送信
}

// ============================================================================
// UI作成関数：メイン画面の構築
// ============================================================================
/**
 * メイン画面を作成し、すべてのUI要素を配置する
 */
void createMainScreen(void) {
    DEBUG_PRINTLN("createMainScreen() called");
    Serial.flush();
    
    // メイン画面を作成
    DEBUG_PRINTLN("Creating main screen object...");
    screenMain = lv_obj_create(NULL);
    if (screenMain == NULL) {
        DEBUG_PRINTLN("ERROR: Failed to create main screen!");
        return;
    }
    DEBUG_PRINTLN("Main screen object created");
    Serial.flush();
    
    // 画面の基本設定
    lv_obj_clear_flag(screenMain, LV_OBJ_FLAG_SCROLLABLE);  // スクロール無効
    lv_obj_set_style_bg_color(screenMain, lv_color_hex(COLOR_BACKGROUND), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(screenMain, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // UI要素を作成（個別の関数に分割）
    createTitleLabel();
    createArcGauge();
    createValueLabel();
    createAutomateButton();
    createBrightnessSlider();
    createBrightnessLabel();
    
    DEBUG_PRINTLN("Loading main screen...");
    Serial.flush();
    
    // メイン画面を表示（LVGL 8.x API）
    lv_scr_load(screenMain);
    
    DEBUG_PRINTLN("Invalidating screen for refresh...");
    Serial.flush();
    
    // 強制的に画面全体を再描画
    lv_obj_invalidate(screenMain);
    
    // 画面全体を無効化して再描画を強制
    lv_area_t full_area;
    full_area.x1 = 0;
    full_area.y1 = 0;
    full_area.x2 = LCD_HORIZONTAL_RES - 1;
    full_area.y2 = LCD_VERTICAL_RES - 1;
    lv_obj_invalidate_area(screenMain, &full_area);
    
    DEBUG_PRINTLN("Main screen created and loaded");
    DEBUG_PRINTLN("UI elements should be visible now");
    DEBUG_PRINTLN("Waiting for LVGL timer to trigger first refresh...");
    
    // 注意: lv_refr_now()は削除 - これがハングの原因の可能性がある
    // LVGLのタイマーが自動的に描画を開始するまで待つ
}

// ============================================================================
// setup関数：プログラム開始時に1回だけ実行される
// ============================================================================
/**
 * ハードウェアの初期化
 */
static void initializeHardware(void) {
    // シリアル通信の初期化（デバッグ用）
    Serial.begin(115200);
    delay(1000);
    DEBUG_PRINTLN("M5Stack Tab5 LVGL Basic Demo - Starting...");
    
    // M5Stackの初期化
    auto cfg = M5.config();
    M5.begin(cfg);
    display = M5.Display;  // ディスプレイオブジェクトを取得
    
    // 注意: display.setRotation()は使用しない
    // LVGLでソフトウェア回転（LV_DISP_ROT_90）を使用するため、
    // ハードウェア回転は行わない
    
    // バイトスワップの設定
    // LVGL 8.xでLV_COLOR_16_SWAP 1が設定されている場合、
    // LVGLはバイトスワップされたデータを生成する
    // M5GFXのデフォルト設定を使用（setSwapBytes()を設定しない）
    // これにより、LVGLが生成したバイトスワップ済みデータが正しく表示される
    
    // 実際のディスプレイサイズを確認（物理サイズ）
    int32_t actualWidth = display.width();
    int32_t actualHeight = display.height();
    DEBUG_PRINTF("Display physical size: %dx%d\n", actualWidth, actualHeight);
    DEBUG_PRINTF("LVGL config: %dx%d (will be rotated to %dx%d)\n", 
                 LCD_HORIZONTAL_RES, LCD_VERTICAL_RES,
                 LCD_VERTICAL_RES, LCD_HORIZONTAL_RES);
    
    // 解像度が一致しているか確認（物理サイズ）
    if (actualWidth != LCD_HORIZONTAL_RES || actualHeight != LCD_VERTICAL_RES) {
        DEBUG_PRINTLN("WARNING: Display size mismatch!");
    }
    
    // タッチパネルのテスト
    lgfx::touch_point_t testTp[1];
    uint8_t testTouch = display.getTouchRaw(testTp, 1);
    if (testTouch > 0) {
        DEBUG_PRINTLN("Touch panel detected!");
    } else {
        DEBUG_PRINTLN("Touch panel ready (waiting for touch...)");
    }
}

/**
 * setup関数：プログラム開始時に1回だけ実行される
 */
void setup() {
    // ハードウェアの初期化
    initializeHardware();
    
    // LVGLの初期化
    initializeLVGL();
    
    DEBUG_PRINTLN("Starting UI creation...");
    Serial.flush();
    
    // UIの作成
    createMainScreen();
    
    DEBUG_PRINTLN("UI creation completed");
    Serial.flush();
    
    // バックライトを最大に設定
    display.setBrightness(BRIGHTNESS_DEFAULT);
    
    // 画面をクリア（黒で塗りつぶし）
    display.fillScreen(TFT_BLACK);
    
    DEBUG_PRINTLN("Setup complete!");
    DEBUG_PRINTLN("Entering main loop - LVGL will handle screen refresh automatically");
    
    // 注意: lv_refr_now()は削除 - これがハングの原因の可能性がある
    // LVGLのタイマーが自動的に描画を開始するまで待つ
    // loop()内のlv_timer_handler()が定期的に描画を実行する
}

// ============================================================================
// loop関数：プログラム開始後、繰り返し実行される
// ============================================================================
void loop() {
    // LVGLのタイマーハンドラーを呼び出す（重要：定期的に呼ぶ必要がある）
    // これにより、アニメーション、イベント処理、描画更新が行われる
    uint32_t time_till_next = lv_timer_handler();
    
    // 注意：LVGL 8.xでは、入力デバイスの読み取りは自動的にタイマーで行われる
    
#if DEBUG_ENABLED
    // デバッグ：LVGLの動作確認（定期的に）
    static unsigned long lastStatusTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastStatusTime >= DEBUG_STATUS_INTERVAL_MS) {
        DEBUG_PRINTF("LVGL status: time_till_next=%lu ms\n", time_till_next);
        DEBUG_PRINTF("Button state: 0x%04X, Automate mode: %s\n", 
                     lv_obj_get_state(buttonAutomate), 
                     isAutomateMode ? "ON" : "OFF");
        DEBUG_PRINTF("Counter: %d\n", counter);
        lastStatusTime = currentTime;
    }
#endif
    
    // 自動カウントモードの状態を確認
    // LVGL 8.xでは、lv_obj_has_state()で状態を確認
    bool currentAutomateState = lv_obj_has_state(buttonAutomate, LV_STATE_CHECKED);
    
#if DEBUG_ENABLED
    // 状態が変わったらデバッグ出力
    static bool lastAutomateState = false;
    if (currentAutomateState != lastAutomateState) {
        DEBUG_PRINTF("Automate button state changed: %s\n", 
                     currentAutomateState ? "ON" : "OFF");
        lastAutomateState = currentAutomateState;
    }
#endif
    
    isAutomateMode = currentAutomateState;
    
    // 自動カウントモードがONの場合、一定間隔でカウンターを更新
    if (isAutomateMode) {
        unsigned long currentMillis = millis();
        // 定期的にカウンターを更新（滑らかなアニメーションのため）
        if (currentMillis - lastCounterUpdate >= COUNTER_UPDATE_INTERVAL_MS) {
            lastCounterUpdate = currentMillis;
            
            // カウンターをインクリメント
            counter++;
            
            // 最大値に達したら最小値にリセット
            if (counter >= COUNTER_MAX) {
                counter = COUNTER_MIN;
            }
            
            // ゲージの値を更新
            lv_arc_set_value(arcGauge, counter);
            
            // ラベルのテキストを更新
            lv_label_set_text_fmt(labelValue, "%d", counter);
            
#if DEBUG_ENABLED
            // デバッグ出力（定期的に）
            static uint16_t debugCounter = 0;
            if (++debugCounter >= DEBUG_COUNTER_INTERVAL) {
                debugCounter = 0;
                DEBUG_PRINTF("Counter updated: %d\n", counter);
            }
#endif
        }
    } else {
        // オフの時はカウンターをリセット
        if (counter != COUNTER_MIN) {
            counter = COUNTER_MIN;
            lv_arc_set_value(arcGauge, COUNTER_MIN);
            lv_label_set_text(labelValue, "0");
        }
    }
    
    // スライダーの値に応じてバックライトの明るさを調整
    static uint8_t lastBrightness = BRIGHTNESS_DEFAULT;
    uint8_t brightness = lv_slider_get_value(sliderBrightness);
    if (brightness != lastBrightness) {
        display.setBrightness(brightness);
        lastBrightness = brightness;
        DEBUG_PRINTF("Brightness changed: %d\n", brightness);
    }
    
    // 待機時間を調整（LVGLが推奨する時間を使用）
    if (time_till_next > 0 && time_till_next < 100) {
        delay(time_till_next);
    } else {
        delay(1);  // 1ms待機（CPU負荷を軽減）
    }
}
