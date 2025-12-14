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
 * - LVGL 9.x
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
// グローバル変数
// ============================================================================
M5GFX display;  // ディスプレイオブジェクト

// UIオブジェクト（LVGLウィジェットへのポインタ）
lv_obj_t* screenMain;        // メイン画面
lv_obj_t* arcGauge;          // 円形ゲージ
lv_obj_t* labelValue;        // 数値表示ラベル
lv_obj_t* buttonAutomate;    // 自動カウントボタン
lv_obj_t* labelTitle;         // タイトルラベル
lv_obj_t* sliderBrightness;   // 明るさ調整スライダー
lv_obj_t* labelBrightness;   // 明るさラベル

// アプリケーション状態
uint16_t counter = 0;         // カウンター値（0-1000）
bool isAutomateMode = false;  // 自動カウントモードのON/OFF
unsigned long lastCounterUpdate = 0;  // 前回のカウンター更新時刻

// LVGL描画バッファ
static lv_color_t* buf;

// LVGLディスプレイと入力デバイスのハンドル
static lv_display_t* lvgl_display = NULL;
static lv_indev_t* lvgl_indev = NULL;

// ============================================================================
// イベントコールバック関数
// ============================================================================
void button_event_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target_obj(e);  // LVGL 9.xではget_target_obj()を使用
    
    if (code == LV_EVENT_VALUE_CHANGED) {
        bool checked = (lv_obj_get_state(target) & LV_STATE_CHECKED) != 0;
        Serial.printf("Button event: VALUE_CHANGED, checked=%d\n", checked);
    } else if (code == LV_EVENT_CLICKED) {
        Serial.println("Button event: CLICKED");
    } else if (code == LV_EVENT_PRESSED) {
        Serial.println("Button event: PRESSED");
    } else if (code == LV_EVENT_RELEASED) {
        Serial.println("Button event: RELEASED");
    }
}

// ============================================================================
// LVGLディスプレイドライバ：画面への描画転送（LVGL 9.x API）
// ============================================================================

// DMA転送の完了を待つコールバック
void display_flush_wait_cb(lv_display_t* disp) {
    // DMA転送の完了を待つ（タイムアウト付き）
    // 注意: waitDMA()が無限に待つ可能性があるため、タイムアウトを設定
    unsigned long waitStartTime = millis();
    const unsigned long TIMEOUT_MS = 100;  // 100msタイムアウト
    
    // タイムアウトチェック付きでDMA完了を待つ
    while (millis() - waitStartTime < TIMEOUT_MS) {
        // DMA転送が完了しているかチェック（非ブロッキング）
        // 注意: M5GFXのwaitDMA()はブロッキングなので、短い待機時間で試す
        display.waitDMA();
        break;  // waitDMA()が完了したらループを抜ける
    }
    
    // タイムアウトした場合の警告（最初の数回のみ）
    static uint32_t timeoutCount = 0;
    if (millis() - waitStartTime >= TIMEOUT_MS) {
        timeoutCount++;
        if (timeoutCount <= 3) {
            Serial.printf("WARNING: DMA wait timeout! (count=%lu)\n", timeoutCount);
        }
    }
}

void display_flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
    // 描画領域のサイズを計算
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    // デバッグ：最初の数回の描画を確認
    static uint32_t flushCount = 0;
    static unsigned long lastFlushDebugTime = 0;
    static bool firstFlush = true;
    unsigned long currentTime = millis();
    
    flushCount++;
    
    // 最初の描画を必ずログに出力
    if (firstFlush) {
        Serial.printf("First display flush! area: (%d,%d)-(%d,%d) size=%dx%d\n",
                     area->x1, area->y1, area->x2, area->y2, w, h);
        firstFlush = false;
    }
    
    // 5秒ごとに描画回数を確認
    if (currentTime - lastFlushDebugTime >= 5000) {
        Serial.printf("Display flush: %lu times in 5 sec, last area: (%d,%d)-(%d,%d) size=%dx%d\n",
                     flushCount, area->x1, area->y1, area->x2, area->y2, w, h);
        flushCount = 0;
        lastFlushDebugTime = currentTime;
    }
    
    // DMA転送で高速に画面へ描画
    // 注意: pushImageDMAは非同期だが、LVGLはバッファを保護するため、
    // すぐにlv_display_flush_ready()を呼んでも問題ない
    display.pushImageDMA(area->x1, area->y1, w, h, (uint16_t*)px_map);
    
    // 描画完了をLVGLに通知（重要：これを呼ばないと描画が止まる）
    // waitDMA()は呼ばない - 非同期で動作させ、LVGLの描画ループをブロックしない
    lv_display_flush_ready(disp);
}

// ============================================================================
// LVGL入力デバイスドライバ：タッチ入力の処理（LVGL 9.x API）
// ============================================================================
void touchpad_read_cb(lv_indev_t* indev, lv_indev_data_t* data) {
    // タッチポイントを取得
    // getTouch()を使用すると、setRotation()に合わせて自動的に座標変換される
    lgfx::touch_point_t tp[3];
    uint8_t touchCount = display.getTouch(tp, 3);  // getTouch()を使用（自動座標変換）
    
    // デバッグ：コールバックが呼ばれているか確認
    static uint32_t callCount = 0;
    static unsigned long lastDebugTime = 0;
    unsigned long currentTime = millis();
    
    callCount++;
    // 1秒ごとにコールバックが呼ばれているか確認
    if (currentTime - lastDebugTime >= 1000) {
        Serial.printf("touchpad_read_cb: called %lu times/sec\n", callCount);
        callCount = 0;
        lastDebugTime = currentTime;
    }
    
    if (touchCount > 0) {
        // タッチされている場合
        data->state = LV_INDEV_STATE_PRESSED;  // プレス状態
        
        // getTouch()は既に回転後の座標を返すので、そのまま使用
        data->point.x = tp[0].x;
        data->point.y = tp[0].y;
        
        // デバッグ用（タッチ座標を確認）
        static unsigned long lastTouchTime = 0;
        if (currentTime - lastTouchTime >= 100) {  // 100msごとに出力
            Serial.printf("Touch: (%d, %d) count=%d\n", tp[0].x, tp[0].y, touchCount);
            lastTouchTime = currentTime;
        }
    } else {
        // タッチされていない場合
        data->state = LV_INDEV_STATE_RELEASED;  // リリース状態
    }
}

// ============================================================================
// LVGL初期化関数（LVGL 9.x API対応）
// ============================================================================
void initializeLVGL(void) {
    // LVGLライブラリの初期化
    lv_init();
    
    // 描画バッファをPSRAMに確保（大きなバッファを確保するため）
    buf = (lv_color_t*)heap_caps_malloc(
        sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    
    if (buf == NULL) {
        Serial.println("Error: Failed to allocate LVGL buffer!");
        return;
    }
    
    // ディスプレイを作成（LVGL 9.x API）
    // 注意: M5.Display.setRotation(3)で270度回転しているので、
    // LVGLは横向き（1280x720）として作成
    // lv_display_create(幅, 高さ) の順序
    lvgl_display = lv_display_create(LCD_HORIZONTAL_RES, LCD_VERTICAL_RES);
    
    // 色フォーマットを設定（RGB565、バイトスワップあり）
    // lv_conf.hでLV_COLOR_16_SWAP 1が設定されているので、SWAPPED形式を使用
    lv_display_set_color_format(lvgl_display, LV_COLOR_FORMAT_RGB565_SWAPPED);
    
    // 描画バッファを設定（全画面バッファを使用）
    // PARTIALモード: 変更部分のみ描画（高速だが、複雑なUIでは問題がある場合がある）
    // FULLモード: 全画面を毎回描画（シンプルだが重い）
    // ここではFULLモードを使用（シンプルで確実）
    lv_display_set_buffers(lvgl_display, buf, NULL, 
                          sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE, 
                          LV_DISPLAY_RENDER_MODE_FULL);
    
    // 描画転送コールバックを設定
    lv_display_set_flush_cb(lvgl_display, display_flush_cb);
    
    // DMA転送の完了を待つコールバックを設定
    // これにより、LVGLはDMA転送の完了を待ってから次の描画を開始する
    lv_display_set_flush_wait_cb(lvgl_display, display_flush_wait_cb);
    
    // デフォルトディスプレイとして設定（重要）
    lv_display_set_default(lvgl_display);
    
    // LVGLの回転は不要（M5.Display側で回転済み）
    
    // 入力デバイス（タッチ）を作成（LVGL 9.x API）
    lvgl_indev = lv_indev_create();
    if (lvgl_indev == NULL) {
        Serial.println("Error: Failed to create input device!");
        return;
    }
    
    lv_indev_set_type(lvgl_indev, LV_INDEV_TYPE_POINTER);  // ポインタ（タッチ/マウス）タイプ
    lv_indev_set_read_cb(lvgl_indev, touchpad_read_cb);     // タッチ読み取りコールバック
    lv_indev_set_display(lvgl_indev, lvgl_display);         // ディスプレイに関連付け（重要）
    
    // 入力デバイスのタイマーを確認
    lv_timer_t* indev_timer = lv_indev_get_read_timer(lvgl_indev);
    if (indev_timer != NULL) {
        Serial.println("Input device timer created successfully");
    } else {
        Serial.println("WARNING: Input device timer is NULL!");
    }
    
    Serial.println("LVGL initialized successfully");
    Serial.println("Input device created and linked to display");
    Serial.flush();  // シリアル出力を確実に送信
}

// ============================================================================
// UI作成関数：メイン画面の構築
// ============================================================================
void createMainScreen(void) {
    Serial.println("createMainScreen() called");
    Serial.flush();
    
    // メイン画面を作成
    Serial.println("Creating main screen object...");
    screenMain = lv_obj_create(NULL);
    if (screenMain == NULL) {
        Serial.println("ERROR: Failed to create main screen!");
        return;
    }
    Serial.println("Main screen object created");
    Serial.flush();
    lv_obj_clear_flag(screenMain, LV_OBJ_FLAG_SCROLLABLE);  // スクロール無効
    lv_obj_set_style_bg_color(screenMain, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(screenMain, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // タイトルラベルを作成
    labelTitle = lv_label_create(screenMain);
    lv_label_set_text(labelTitle, "LVGL Basic Demo");
    lv_obj_set_style_text_color(labelTitle, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    // デフォルトフォントを使用（利用可能なフォントに変更）
    lv_obj_set_style_text_font(labelTitle, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(labelTitle, LV_ALIGN_TOP_MID, 0, 20);
    
    // 円形ゲージ（アーク）を作成
    arcGauge = lv_arc_create(screenMain);
    lv_obj_set_size(arcGauge, 400, 400);
    lv_obj_align(arcGauge, LV_ALIGN_CENTER, 0, -50);
    lv_arc_set_range(arcGauge, 0, 1000);      // 範囲：0-1000
    lv_arc_set_value(arcGauge, 0);            // 初期値：0
    lv_obj_remove_style(arcGauge, NULL, LV_PART_KNOB);  // ノブを非表示
    lv_obj_clear_flag(arcGauge, LV_OBJ_FLAG_CLICKABLE); // クリック無効
    
    // 数値表示ラベルを作成（ゲージの中央に配置）
    labelValue = lv_label_create(screenMain);
    lv_label_set_text(labelValue, "0");
    lv_obj_set_style_text_color(labelValue, lv_color_hex(0xEEFB02), LV_PART_MAIN | LV_STATE_DEFAULT);
    // デフォルトフォントを使用（利用可能なフォントに変更）
    lv_obj_set_style_text_font(labelValue, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(labelValue, LV_ALIGN_CENTER, 0, -50);
    
    // 自動カウントボタンを作成
    buttonAutomate = lv_btn_create(screenMain);
    lv_obj_set_size(buttonAutomate, 200, 50);
    lv_obj_align(buttonAutomate, LV_ALIGN_CENTER, 0, 150);
    lv_obj_add_flag(buttonAutomate, LV_OBJ_FLAG_CHECKABLE);  // トグルボタンとして設定
    lv_obj_set_style_bg_color(buttonAutomate, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(buttonAutomate, lv_color_hex(0x919191), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(buttonAutomate, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(buttonAutomate, lv_color_hex(0x6A3F63), LV_PART_MAIN | LV_STATE_CHECKED);
    
    // ボタンのラベルを作成
    lv_obj_t* labelButton = lv_label_create(buttonAutomate);
    lv_label_set_text(labelButton, "Automate");
    lv_obj_center(labelButton);
    
    // ボタンのイベントコールバックを追加（デバッグ用）
    lv_obj_add_event_cb(buttonAutomate, button_event_cb, LV_EVENT_ALL, NULL);
    
    // 明るさ調整スライダーを作成
    sliderBrightness = lv_slider_create(screenMain);
    lv_slider_set_range(sliderBrightness, 1, 255);  // 範囲：1-255
    lv_slider_set_value(sliderBrightness, 255, LV_ANIM_OFF);  // 初期値：最大
    lv_obj_set_size(sliderBrightness, 20, 400);
    lv_obj_align(sliderBrightness, LV_ALIGN_RIGHT_MID, -50, 0);
    
    // スライダーのスタイルを設定（視覚的なフィードバックを改善）
    lv_obj_set_style_bg_color(sliderBrightness, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sliderBrightness, lv_color_hex(0xDA88EA), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sliderBrightness, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sliderBrightness, 10, LV_PART_KNOB | LV_STATE_DEFAULT);
    
    // 明るさラベルを作成
    labelBrightness = lv_label_create(screenMain);
    lv_label_set_text(labelBrightness, "Brightness");
    lv_obj_set_style_text_color(labelBrightness, lv_color_hex(0xDA88EA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(labelBrightness, LV_ALIGN_RIGHT_MID, -50, 220);
    
    Serial.println("Loading main screen...");
    Serial.flush();
    
    // メイン画面を表示
    lv_screen_load(screenMain);
    
    Serial.println("Invalidating screen for refresh...");
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
    
    Serial.println("Main screen created and loaded");
    Serial.println("UI elements should be visible now");
    Serial.println("Waiting for LVGL timer to trigger first refresh...");
    
    // 注意: lv_refr_now()は削除 - これがハングの原因の可能性がある
    // LVGLのタイマーが自動的に描画を開始するまで待つ
}

// ============================================================================
// setup関数：プログラム開始時に1回だけ実行される
// ============================================================================
void setup() {
    // シリアル通信の初期化（デバッグ用）
    Serial.begin(115200);
    delay(1000);
    Serial.println("M5Stack Tab5 LVGL Basic Demo - Starting...");
    
    // M5Stackの初期化
    auto cfg = M5.config();
    M5.begin(cfg);
    display = M5.Display;  // ディスプレイオブジェクトを取得
    
    // ディスプレイを270度回転（横向き：1280x720）
    display.setRotation(3);
    
    // 実際のディスプレイサイズを確認
    int32_t actualWidth = display.width();
    int32_t actualHeight = display.height();
    Serial.printf("Display size: %dx%d\n", actualWidth, actualHeight);
    Serial.printf("LVGL config: %dx%d\n", LCD_HORIZONTAL_RES, LCD_VERTICAL_RES);
    
    // 解像度が一致しているか確認
    if (actualWidth != LCD_HORIZONTAL_RES || actualHeight != LCD_VERTICAL_RES) {
        Serial.println("WARNING: Display size mismatch!");
    }
    
    // タッチパネルのテスト
    lgfx::touch_point_t testTp[1];
    uint8_t testTouch = display.getTouch(testTp, 1);
    if (testTouch > 0) {
        Serial.println("Touch panel detected!");
    } else {
        Serial.println("Touch panel ready (waiting for touch...)");
    }
    
    // LVGLの初期化
    initializeLVGL();
    
    Serial.println("Starting UI creation...");
    Serial.flush();
    
    // UIの作成
    createMainScreen();
    
    Serial.println("UI creation completed");
    Serial.flush();
    
    // バックライトを最大に設定
    display.setBrightness(255);
    
    // 画面をクリア（黒で塗りつぶし）
    display.fillScreen(TFT_BLACK);
    
    Serial.println("Setup complete!");
    Serial.println("Entering main loop - LVGL will handle screen refresh automatically");
    
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
    
    // 入力デバイスを手動で読み取る（確実に動作させるため）
    // 注意：LVGL 9.xでは、入力デバイスの読み取りは自動的にタイマーで行われるが、
    // 手動で呼ぶことで確実にタッチイベントを処理できる
    if (lvgl_indev != NULL) {
        lv_indev_read(lvgl_indev);
    }
    
    // デバッグ：LVGLの動作確認（5秒ごと）
    static unsigned long lastStatusTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastStatusTime >= 5000) {
        Serial.printf("LVGL status: time_till_next=%lu ms\n", time_till_next);
        Serial.printf("Button state: 0x%04X, Automate mode: %s\n", 
                     lv_obj_get_state(buttonAutomate), 
                     isAutomateMode ? "ON" : "OFF");
        Serial.printf("Counter: %d\n", counter);
        lastStatusTime = currentTime;
    }
    
    // 自動カウントモードの状態を確認
    // LVGL 9.xでは、lv_obj_get_state()で状態を取得し、ビット演算でチェック
    lv_state_t buttonState = lv_obj_get_state(buttonAutomate);
    bool currentAutomateState = (buttonState & LV_STATE_CHECKED) != 0;
    
    // 状態が変わったらデバッグ出力
    static bool lastAutomateState = false;
    if (currentAutomateState != lastAutomateState) {
        Serial.printf("Automate button state changed: %s (state=0x%04X)\n", 
                     currentAutomateState ? "ON" : "OFF", buttonState);
        lastAutomateState = currentAutomateState;
    }
    
    isAutomateMode = currentAutomateState;
    
    // 自動カウントモードがONの場合、一定間隔でカウンターを更新
    if (isAutomateMode) {
        unsigned long currentMillis = millis();
        // 50msごとにカウンターを更新（滑らかなアニメーションのため）
        if (currentMillis - lastCounterUpdate >= 50) {
            lastCounterUpdate = currentMillis;
            
            // カウンターをインクリメント
            counter++;
            
            // 1000に達したら0にリセット
            if (counter >= 1000) {
                counter = 0;
            }
            
            // ゲージの値を更新
            lv_arc_set_value(arcGauge, counter);
            
            // ラベルのテキストを更新
            lv_label_set_text_fmt(labelValue, "%d", counter);
            
            // デバッグ出力（10回に1回）
            static uint16_t debugCounter = 0;
            if (++debugCounter >= 10) {
                debugCounter = 0;
                Serial.printf("Counter updated: %d\n", counter);
            }
        }
    } else {
        // オフの時はカウンターをリセット
        if (counter != 0) {
            counter = 0;
            lv_arc_set_value(arcGauge, 0);
            lv_label_set_text(labelValue, "0");
        }
    }
    
    // スライダーの値に応じてバックライトの明るさを調整
    static uint8_t lastBrightness = 255;
    uint8_t brightness = lv_slider_get_value(sliderBrightness);
    if (brightness != lastBrightness) {
        display.setBrightness(brightness);
        lastBrightness = brightness;
        Serial.printf("Brightness changed: %d\n", brightness);
    }
    
    // 待機時間を調整（LVGLが推奨する時間を使用）
    if (time_till_next > 0 && time_till_next < 100) {
        delay(time_till_next);
    } else {
        delay(1);  // 1ms待機（CPU負荷を軽減）
    }
}
