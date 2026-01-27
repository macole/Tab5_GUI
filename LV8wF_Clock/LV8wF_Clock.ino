// ==================================================================
// EEZ_withFlow01_Clock - M5Stack Tab5 時計アプリケーション
// ==================================================================
// EEZ Studio + LVGL 8.3.11 で作成
// 
// 依存関係:
//   - ESP-Arduino >= V3.2
//   - M5Unified >= 0.2.10
//   - LVGL = 8.3.11
// 
// lv_conf.h設定:
//   - LV_COLOR_DEPTH 16
//   - LV_COLOR_16_SWAP 1
//   - LV_MEM_CUSTOM 1
//   - LV_TICK_CUSTOM 1
// 
// 作成日: 2025年1月
// 最終更新: 2026年1月27日
// 
// 機能:
//   - WiFi接続とNTP時刻同期
//   - リアルタイム時計表示
//   - WiFi接続状態の表示
// 
// 重要な変更履歴:
//   2026-01-25: EEZ Flow統合の修正（リブートループ問題の解決）
//               - stopScriptフックのオーバーライド実装
//               - replacePageフックの実装
//               詳細: CHANGES.md 参照
// 
// 関連ドキュメント:
//   - README.md: プロジェクト概要と使用方法
//   - TROUBLESHOOTING.md: トラブルシューティングガイド
//   - CHANGES.md: 変更履歴
// ==================================================================

#include <M5Unified.h>
#include <lvgl.h>
#include "src/ui/ui.h"
#include "src/ui/screens.h"
#include "src/ui/eez-flow.h"
#include <esp_heap_caps.h>
#include <WiFi.h>
#include <time.h>
#include <cstdio>

// ==================================================================
// 定数定義
// ==================================================================

// ディスプレイ設定
#define DEFAULT_BRIGHTNESS 255      // 初期画面の明るさ（0-255）
#define EXAMPLE_LCD_H_RES 720       // 水平解像度（縦向き時の幅）
#define EXAMPLE_LCD_V_RES 1280      // 垂直解像度（縦向き時の高さ）
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)  // 画面バッファサイズ

// タイミング設定
#define LVGL_TIMER_DELAY_MS 50       // LVGLタイマーハンドラーの遅延時間（ミリ秒）
#define EEZ_FLOW_INIT_DELAY_MS 1000 // EEZ Flowエンジンの初期化待ち時間（ミリ秒）
#define TIME_UPDATE_INTERVAL_MS 500 // 時刻表示の更新間隔（ミリ秒）

// WiFi設定（要変更）
#define WIFI_SSID "wifi_ssid"      // WiFi SSIDを設定してください
#define WIFI_PASSWORD "wifi_password"   // WiFiパスワードを設定してください
#define WIFI_CONNECT_TIMEOUT_SEC 30  // WiFi接続タイムアウト（秒）

// NTP設定
#define NTP_SERVER "pool.ntp.org"   // NTPサーバー
#define TIMEZONE_OFFSET 9           // タイムゾーンオフセット（JST: +9時間）
#define NTP_SYNC_TIMEOUT_SEC 10     // NTP同期タイムアウト（秒）

// ==================================================================
// グローバル変数
// ==================================================================
static lv_disp_draw_buf_t g_draw_buf;            // LVGL描画バッファ
static lv_color_t *g_color_buf = nullptr;        // LVGL描画用のカラーバッファ

// 時刻関連
static unsigned long g_lastTimeUpdate = 0;       // 前回の時刻更新時間
static bool g_timeInitialized = false;           // NTP時刻取得済みフラグ
static lv_obj_t *g_textArea = nullptr;           // 時刻表示用テキストエリア

// ==================================================================
// エラーハンドリング関数
// ==================================================================
static void fatalError(void)
{
    while(1) {
        M5.Display.fillScreen(TFT_RED);
        delay(500);
        M5.Display.fillScreen(TFT_BLACK);
        delay(500);
    }
}

// ==================================================================
// LVGLコールバック関数
// ==================================================================
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    if (area == nullptr || color_p == nullptr) {
        lv_disp_flush_ready(disp);
        return;
    }
    
    uint32_t width = (area->x2 - area->x1 + 1);  // 描画領域の幅を計算
    uint32_t height = (area->y2 - area->y1 + 1);  // 描画領域の高さを計算
    
    // DMAを使用して画像データをディスプレイに転送
    M5.Display.pushImageDMA(area->x1, area->y1, width, height, (uint16_t *)&color_p->full);
    
    // フラッシュ処理の完了をLVGLに通知
    lv_disp_flush_ready(disp);
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

// ==================================================================
// タッチ入力読み取り関数
// ==================================================================
// ディスプレイの回転はLVGL側で処理されるため、タッチ座標は変換不要
// ==================================================================
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    auto touch_detail = M5.Touch.getDetail();
    
    // タッチ状態の判定: isPressed()とwasPressed()の両方をチェック
    // 両方をチェックすることで、タッチ検出の確実性を向上
    if (touch_detail.isPressed() || touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;
        
        // タッチ座標を取得
        int16_t touch_x = touch_detail.x;
        int16_t touch_y = touch_detail.y;
        
        // 座標範囲のチェック
        if (touch_x < 0) touch_x = 0;
        if (touch_x >= EXAMPLE_LCD_H_RES) touch_x = EXAMPLE_LCD_H_RES - 1;
        if (touch_y < 0) touch_y = 0;
        if (touch_y >= EXAMPLE_LCD_V_RES) touch_y = EXAMPLE_LCD_V_RES - 1;
        
        data->point.x = touch_x;
        data->point.y = touch_y;
        
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ==================================================================
// LVGLディスプレイの初期化
// ==================================================================
bool initLvglDisplay(void)
{
    lv_init();  // LVGLライブラリの初期化
    
    // SPIRAMを使用して描画バッファを確保（8ビットアライメント）
    size_t buffer_size = sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE;
    g_color_buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    
    if (g_color_buf == nullptr) {
        return false;
    }
    
    // 描画バッファの初期化
    lv_disp_draw_buf_init(&g_draw_buf, g_color_buf, NULL, LVGL_LCD_BUF_SIZE);
    
    // ディスプレイドライバーの初期化
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;  // 水平解像度
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;  // 垂直解像度
    disp_drv.flush_cb = lv_disp_flush;  // フラッシュコールバックを設定
    disp_drv.draw_buf = &g_draw_buf;  // 描画バッファを設定
    disp_drv.sw_rotate = 1;  // ソフトウェア回転を有効化
    disp_drv.rotated = LV_DISP_ROT_90;  // ディスプレイを90度回転
    lv_disp_drv_register(&disp_drv);  // ディスプレイドライバーを登録
    
    return true;
}

// ==================================================================
// LVGLタッチ入力の初期化
// ==================================================================
void initLvglTouch(void)
{
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_indev_read;
    
    lv_indev_drv_register(&indev_drv);
}

// ==================================================================
// WiFi接続
// ==================================================================
/**
 * @brief WiFiに接続する
 * 
 * @return true 接続成功
 * @return false 接続失敗
 */
bool connectWiFi(void)
{
    if (g_textArea) {
        lv_textarea_set_text(g_textArea, "Connecting WiFi...\n");
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    // 接続試行（最大WIFI_CONNECT_TIMEOUT_SEC秒）
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < WIFI_CONNECT_TIMEOUT_SEC) {
        delay(1000);
        attempts++;
        // 3秒ごとに進捗を表示
        if (g_textArea && attempts % 3 == 0) {
            char status[64];
            snprintf(status, sizeof(status), "Connecting WiFi... (%ds)\n", attempts);
            lv_textarea_set_text(g_textArea, status);
        }
    }
    
    bool connected = (WiFi.status() == WL_CONNECTED);
    if (g_textArea) {
        if (connected) {
            char status[128];
            snprintf(status, sizeof(status), "WiFi Connected\nSSID: %s\nIP: %s\n\n", 
                     WIFI_SSID, WiFi.localIP().toString().c_str());
            lv_textarea_set_text(g_textArea, status);
        } else {
            lv_textarea_set_text(g_textArea, "WiFi Failed\nCheck SSID\n");
        }
    }
    
    return connected;
}

// ==================================================================
// NTP時刻同期
// ==================================================================
/**
 * @brief NTPサーバーから時刻を取得して同期する
 * 
 * @return true 同期成功
 * @return false 同期失敗
 */
bool syncNTPTime(void)
{
    if (g_textArea) {
        char status[128];
        snprintf(status, sizeof(status), "%sSyncing NTP...\n", lv_textarea_get_text(g_textArea));
        lv_textarea_set_text(g_textArea, status);
    }
    
    // タイムゾーン設定
    configTime(TIMEZONE_OFFSET * 3600, 0, NTP_SERVER);
    
    // 時刻取得を試行（最大NTP_SYNC_TIMEOUT_SEC秒）
    struct tm timeinfo;
    int attempts = 0;
    while (!getLocalTime(&timeinfo) && attempts < NTP_SYNC_TIMEOUT_SEC) {
        delay(1000);
        attempts++;
    }
    
    bool success = (attempts < NTP_SYNC_TIMEOUT_SEC);
    if (g_textArea) {
        if (success) {
            char status[128];
            snprintf(status, sizeof(status), "%sNTP Synced\n", lv_textarea_get_text(g_textArea));
            lv_textarea_set_text(g_textArea, status);
        } else {
            char status[128];
            snprintf(status, sizeof(status), "%sNTP Failed\n", lv_textarea_get_text(g_textArea));
            lv_textarea_set_text(g_textArea, status);
        }
    }
    
    return success;
}

// ==================================================================
// text_areaの検索
// ==================================================================
/**
 * @brief 親オブジェクト内のtext_areaウィジェットを再帰的に検索する
 * 
 * @param parent 検索対象の親オブジェクト
 * @return lv_obj_t* 見つかったtext_areaオブジェクト、見つからない場合はnullptr
 */
static lv_obj_t* findTextArea(lv_obj_t* parent)
{
    if (!parent) {
        return nullptr;
    }
    
    // 子要素を検索
    uint32_t child_cnt = lv_obj_get_child_cnt(parent);
    for (uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(parent, i);
        // LVGL 8.3.11では、get_class()で型を確認
        if (lv_obj_get_class(child) == &lv_textarea_class) {
            return child;
        }
        // 再帰的に検索
        lv_obj_t* found = findTextArea(child);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

// ==================================================================
// text_areaの初期化
// ==================================================================
/**
 * @brief text_areaウィジェットを検索して初期化する
 * 
 * EEZ Studioで生成されたtext_areaを検索し、初期メッセージを設定します。
 */
void initTextArea(void)
{
    if (!objects.main) {
        return;
    }
    
    // objects構造体にtext_areaがあるか確認（EEZ Studioで生成された場合）
    // もし存在しなければ、動的に検索
    g_textArea = findTextArea(objects.main);
    
    if (g_textArea) {
        // テキストエリアの設定
        lv_textarea_set_text(g_textArea, "Initializing...\n");
        lv_textarea_set_placeholder_text(g_textArea, "");
    }
}

// ==================================================================
// 時刻表示の更新
// ==================================================================
/**
 * @brief 現在の時刻とWiFi状態をtext_areaに表示する
 * 
 * NTPで取得した時刻を1秒ごとに更新し、WiFi接続状態も表示します。
 */
void updateTimeDisplay(void)
{
    if (!g_timeInitialized || !g_textArea) {
        return;
    }
    
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return;
    }
    
    // 時刻文字列を生成（HH:MM:SS形式）
    char timeStr[32];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d",
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    
    // 日付文字列を生成（YYYY/MM/DD形式）
    char dateStr[32];
    snprintf(dateStr, sizeof(dateStr), "%04d/%02d/%02d",
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    
    // 曜日文字列を生成（英語、LVGLフォント対応）
    const char* weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char weekdayStr[16];
    snprintf(weekdayStr, sizeof(weekdayStr), "(%s)", weekdays[timeinfo.tm_wday]);
    
    // WiFi接続状態を取得
    const char* wifiStatus = (WiFi.status() == WL_CONNECTED) ? "Connected" : "Disconnected";
    char ipStr[32] = "";
    if (WiFi.status() == WL_CONNECTED) {
        snprintf(ipStr, sizeof(ipStr), "\nIP: %s", WiFi.localIP().toString().c_str());
    }
    
    // text_areaに表示（複数行）
    char displayText[256];
    snprintf(displayText, sizeof(displayText),
             "=== Clock App ===\n\n"
             "Time: %s\n"
             "Date: %s %s\n"
             "WiFi: %s%s\n",
             timeStr, dateStr, weekdayStr, wifiStatus, ipStr);
    
    lv_textarea_set_text(g_textArea, displayText);
}

// ==================================================================
// セットアップ関数
// ==================================================================
void setup()
{
    // M5Unifiedの初期化
    auto cfg = M5.config();
    M5.begin(cfg);
    delay(100);
    
    // PSRAMチェック
    if (ESP.getFreePsram() == 0) {
        fatalError();
    }
    
    // LVGLディスプレイの初期化
    if (!initLvglDisplay()) {
        fatalError();
    }
    
    // LVGLタッチ入力の初期化
    initLvglTouch();
    
    // ==================================================================
    // 🔴 重要: EEZ Flow フックの設定
    // ==================================================================
    // EEZ Studioが生成するデフォルトのフック実装は assert(false) を呼び出すため、
    // ui_init() を呼び出す前に、すべてのフックをオーバーライドする必要があります。
    // これを行わないと、リブートループが発生します。
    // 詳細: TROUBLESHOOTING.md「6. EEZ Flowのアサーションエラー」参照
    // ==================================================================
    
    // stopScriptフック: Flowスクリプト終了時に呼ばれる
    // デフォルト実装は assert(false) なので、オーバーライドが必須
    eez::flow::stopScriptHook = []() {
        // 何もしない
    };
    
    // replacePageフック: 画面切り替え時に呼ばれる
    eez::flow::replacePageHook = [](int16_t pageId, uint32_t animType, uint32_t speed, uint32_t delay) {
        lv_obj_t *target_screen = nullptr;
        switch(pageId) {
            case SCREEN_ID_MAIN:
                target_screen = objects.main;
                break;
            case SCREEN_ID_PAGE1:
                target_screen = objects.page1;
                break;
            default:
                return;
        }
        
        if (target_screen) {
            lv_scr_load_anim(target_screen, (lv_scr_load_anim_t)animType, speed, delay, false);
        }
    };
    
    // scpiComponentInitフック: SCPI機能の初期化時に呼ばれる
    eez::flow::scpiComponentInitHook = []() {
        // 何もしない
    };
    
    // EEZ Studio UIの初期化
    lv_obj_clean(lv_scr_act());
    ui_init();
    
    // ディスプレイの明るさを初期値に設定
    M5.Display.setBrightness(DEFAULT_BRIGHTNESS);
    
    // text_areaの初期化
    initTextArea();
    
    // ==================================================================
    // WiFi接続とNTP時刻同期
    // ==================================================================
    if (connectWiFi()) {
        if (syncNTPTime()) {
            g_timeInitialized = true;
            updateTimeDisplay();  // 初期時刻表示
        }
    }
    
    // 接続失敗時の表示
    if (!g_timeInitialized && g_textArea) {
        char errorText[128];
        snprintf(errorText, sizeof(errorText),
                 "%s\n\n=== Error ===\n"
                 "WiFi or NTP connection failed\n"
                 "Check settings\n",
                 lv_textarea_get_text(g_textArea));
        lv_textarea_set_text(g_textArea, errorText);
    }
}

// ==================================================================
// メインループ
// ==================================================================
// M5.update() → lv_timer_handler() → ui_tick() の順で呼び出すことで、
// タッチ入力 → UI更新 → Flowスクリプト実行 のサイクルが実現されます
// ==================================================================
void loop()
{
    static bool firstLoop = true;
    static unsigned long loopStartTime = 0;
    
    // 初回ループの処理
    if (firstLoop) {
        loopStartTime = millis();
        firstLoop = false;
    }
    
    // M5Unifiedの更新: タッチ入力、ボタン、IMU等のハードウェア情報を更新
    M5.update();
    
    // LVGLのタイマーハンドラー: UIイベント処理とアニメーション更新
    lv_timer_handler();
    
    // EEZ Flow Tickハンドラー: Flow言語スクリプトを実行
    // 注意: EEZ Flowエンジンの初期化に時間がかかるため、起動後一定時間は待機
    if (millis() - loopStartTime > EEZ_FLOW_INIT_DELAY_MS) {
        ui_tick();
    }
    
    // 時刻表示の更新
    if (g_timeInitialized) {
        unsigned long currentTime = millis();
        if (currentTime - g_lastTimeUpdate >= TIME_UPDATE_INTERVAL_MS) {
            updateTimeDisplay();
            g_lastTimeUpdate = currentTime;
        }
    }
    
    // CPU負荷を軽減するため、短い遅延を挿入
    // この遅延により、Watchdog Timerのタイムアウトも防止される
    delay(LVGL_TIMER_DELAY_MS);
    
    // =====================================================================
    // 📝 重要な注意事項
    // =====================================================================
    // アプリケーションロジック（カウンター、明るさ調整、タイマーなど）は、
    // このloop()関数内ではなく、EEZ StudioのFlowで実装することを強く推奨します。
    // 
    // Flowで実装する利点:
    // - ビジュアルプログラミングで直感的
    // - 再コンパイル不要でロジック変更可能
    // - UIとロジックの連携が容易
    // 
    // 詳細: README.md「EEZ Studio Flowでの実装」セクション参照
    // =====================================================================
}
