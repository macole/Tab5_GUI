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
// 最終更新: 2026年1月25日
// 
// 重要な変更履歴:
//   2026-01-25: EEZ Flow統合の修正（リブートループ問題の解決）
//               - stopScriptフックのオーバーライド実装
//               - replacePageフックの実装
//               - 詳細なデバッグログの追加
//               詳細: CHANGES.md 参照
// 
// 関連ドキュメント:
//   - README.md: プロジェクト概要と使用方法
//   - TROUBLESHOOTING.md: トラブルシューティングガイド
//   - CHANGES.md: 変更履歴
// ==================================================================

#include <M5Unified.h>
#include <lvgl.h>
#include "ui.h"
#include "screens.h"
#include "eez-flow.h"
#include <esp_heap_caps.h>

// 定数定義
#define SERIAL_BAUD_RATE 115200     // シリアル通信のボーレート
#define COUNTER_MAX_VALUE 1000      // カウンターの最大値（オーバーフロー時に0にリセット）
#define DEFAULT_BRIGHTNESS 255      // 初期画面の明るさ（0-255）
#define LVGL_TIMER_DELAY_MS 1       // LVGLタイマーハンドラーの遅延時間（ミリ秒）
#define MAX_TOUCH_POINTS 3          // タッチポイントの最大数

// ディスプレイ設定
#define EXAMPLE_LCD_H_RES 720       // 水平解像度（縦向き時の幅）
#define EXAMPLE_LCD_V_RES 1280      // 垂直解像度（縦向き時の高さ）
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)  // 画面バッファサイズ

// グローバル変数
static lv_disp_draw_buf_t g_draw_buf;            // LVGL描画バッファ
static lv_color_t *g_color_buf = nullptr;        // LVGL描画用のカラーバッファ

// LVGLコールバック関数
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    if (area == nullptr || color_p == nullptr) {
        Serial.println("⚠️ Warning: Invalid flush parameters");
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
// ディスプレイが90度回転しているため、タッチ座標も回転変換が必要です。
// M5Stack Tab5のタッチパネル座標系: 1280x720（横長）
// LVGL表示座標系: 720x1280（縦長、90度回転）
// 
// 時計回り90度回転の変換式: (x, y) → (y, 720 - x)
// ==================================================================
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    auto touch_detail = M5.Touch.getDetail();
    
    // タッチ状態の判定: isPressed()とwasPressed()の両方をチェック
    // - wasPressed(): タッチが開始された瞬間のみtrue
    // - isPressed(): タッチされている間ずっとtrue
    // 両方をチェックすることで、タッチ検出の確実性を向上
    if (touch_detail.isPressed() || touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;
        
        // タッチ座標を取得（元の座標系: 1280x720）
        int16_t raw_x = touch_detail.x;
        int16_t raw_y = touch_detail.y;
        
        // ディスプレイが90度回転しているため、タッチ座標も回転変換
        // 
        // 現在の実装: 時計回り90度回転
        //   変換式: (x, y) → (y, 720 - x)
        //   元の座標系: 1280x720（横長）
        //   回転後の座標系: 720x1280（縦長）
        //
        // もしタッチ座標がずれている場合、以下のいずれかを試してください:
        //   1. 反時計回り90度: (x, y) → (1280 - y, x)
        //   2. 座標変換なし: (x, y) → (x, y) のまま
        //   3. X/Y反転: (x, y) → (y, x)
        //
        // デバッグログを有効化して、実際の座標を確認してください
        int16_t rotated_x = raw_y;
        int16_t rotated_y = 720 - raw_x;
        
        // 座標範囲のチェック（念のため）
        if (rotated_x < 0) rotated_x = 0;
        if (rotated_x >= EXAMPLE_LCD_H_RES) rotated_x = EXAMPLE_LCD_H_RES - 1;
        if (rotated_y < 0) rotated_y = 0;
        if (rotated_y >= EXAMPLE_LCD_V_RES) rotated_y = EXAMPLE_LCD_V_RES - 1;
        
        data->point.x = rotated_x;
        data->point.y = rotated_y;
        
        // デバッグ用: タッチ座標を表示
        // タッチが反応しない場合、このコメントを有効化して座標を確認してください
        // 座標がずれている場合は、回転方向を変更する必要があります
        static unsigned long last_touch_log = 0;
        if (millis() - last_touch_log > 200) {  // 200msごとにログ出力（連続出力を防ぐ）
            Serial.printf("🖐️  Touch: raw(%d, %d) → rotated(%d, %d)\n", 
                         raw_x, raw_y, rotated_x, rotated_y);
            last_touch_log = millis();
        }
        
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// 初期化関数
bool initLvglDisplay(void)
{
    Serial.println("🔧 Initializing LVGL display...");
    
    lv_init();  // LVGLライブラリの初期化
    Serial.println("✅ LVGL initialized");
    
    // SPIRAMを使用して描画バッファを確保（8ビットアライメント）
    size_t buffer_size = sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE;
    Serial.printf("📊 Allocating display buffer: %d bytes (%.2f MB)\n", 
                  buffer_size, buffer_size / 1048576.0);
    
    g_color_buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    
    if (g_color_buf == nullptr) {
        Serial.println("❌ ERROR: Failed to allocate SPIRAM buffer!");
        Serial.printf("   Requested: %d bytes\n", buffer_size);
        Serial.printf("   Free PSRAM: %d bytes\n", ESP.getFreePsram());
        return false;
    }
    
    Serial.println("✅ Display buffer allocated");
    
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
    
    Serial.println("✅ LVGL display driver registered");
    return true;
}

// ==================================================================
// LVGLタッチ入力の初期化
// ==================================================================
// タッチパネルの設定と最適化を行います。
// - タッチ座標の回転変換を適用
// - 読み取り間隔を短くして応答性を向上（lv_conf.hで設定）
// ==================================================================
void initLvglTouch(void)
{
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    
    // 入力デバイスのタイプをポインタ（タッチパネル）に設定
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    
    // 読み取りコールバックを設定
    indev_drv.read_cb = lv_indev_read;
    
    // 入力デバイスドライバーを登録
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);
    
    if (indev) {
        // 注意: LVGL 8.3.11では、読み取り間隔は lv_conf.h の
        // LV_INDEV_DEF_READ_PERIOD で設定します（デフォルト: 30ms）
        // より応答性を向上させたい場合は、lv_conf.h で以下のように設定してください:
        // #define LV_INDEV_DEF_READ_PERIOD 10
        
        Serial.println("✅ Touch input device registered");
        Serial.println("   Read interval: Set in lv_conf.h (LV_INDEV_DEF_READ_PERIOD)");
    } else {
        Serial.println("❌ ERROR: Failed to register touch input device");
    }
}

void setup()
{
    // M5Unifiedの初期化
    auto cfg = M5.config();
    M5.begin(cfg);
    delay(100);  // M5Unified初期化の安定化待ち
    
    // シリアル通信の初期化（デバッグ用）
    Serial.begin(SERIAL_BAUD_RATE);
    delay(100);  // シリアル通信安定化待ち
    
    Serial.println();
    Serial.println("╔═══════════════════════════════════════╗");
    Serial.println("║  M5Stack Tab5                         ║");
    Serial.println("║  EEZ_withFlow01_Clock                 ║");
    Serial.println("╚═══════════════════════════════════════╝");
    Serial.println();
    
    // システム情報表示
    Serial.println("📊 System Information:");
    Serial.printf("   LVGL Version: %d.%d.%d\n", 
                  lv_version_major(), lv_version_minor(), lv_version_patch());
    Serial.printf("   Free Heap: %d bytes (%.2f KB)\n", 
                  ESP.getFreeHeap(), ESP.getFreeHeap() / 1024.0);
    Serial.printf("   Free PSRAM: %d bytes (%.2f MB)\n", 
                  ESP.getFreePsram(), ESP.getFreePsram() / 1048576.0);
    Serial.println();
    
    // PSRAMチェック
    if (ESP.getFreePsram() == 0) {
        Serial.println("❌ FATAL ERROR: PSRAM not available!");
        Serial.println("   Please enable PSRAM in board settings:");
        Serial.println("   Tools → PSRAM → Enabled");
        while(1) {
            M5.Display.fillScreen(TFT_RED);
            delay(500);
            M5.Display.fillScreen(TFT_BLACK);
            delay(500);
        }
    }
    
    // LVGLディスプレイの初期化
    Serial.println("📍 Checkpoint 1: Before LVGL display initialization");
    if (!initLvglDisplay()) {
        Serial.println("❌ FATAL ERROR: Display initialization failed!");
        Serial.println("   Check PSRAM is enabled in board settings.");
        while(1) {
            M5.Display.fillScreen(TFT_RED);
            delay(500);
            M5.Display.fillScreen(TFT_BLACK);
            delay(500);
        }
    }
    Serial.println("📍 Checkpoint 2: After LVGL display initialization");
    
    // LVGLタッチ入力の初期化
    Serial.println("📍 Checkpoint 3: Before LVGL touch initialization");
    Serial.println("🔧 Initializing LVGL touch...");
    initLvglTouch();
    Serial.println("✅ LVGL touch initialized");
    Serial.println("📍 Checkpoint 4: After LVGL touch initialization");
    Serial.println();
    
    // ==================================================================
    // 🔴 重要: EEZ Flow フックの設定
    // ==================================================================
    // EEZ Studioが生成するデフォルトのフック実装は assert(false) を呼び出すため、
    // ui_init() を呼び出す前に、すべてのフックをオーバーライドする必要があります。
    // これを行わないと、リブートループが発生します。
    // 詳細: TROUBLESHOOTING.md「6. EEZ Flowのアサーションエラー」参照
    // ==================================================================
    
    Serial.println("🔧 Setting up EEZ Flow hooks...");
    
    // stopScriptフック: Flowスクリプト終了時に呼ばれる
    // デフォルト実装は assert(false) なので、オーバーライドが必須
    eez::flow::stopScriptHook = []() {
        Serial.println("⚠️  EEZ Flow: stopScript called");
        // 必要に応じて、ここでスクリプトの再起動処理を実装
    };
    
    // replacePageフック: 画面切り替え時に呼ばれる
    // EEZ StudioのFlowから "REPLACE_PAGE" アクションで使用される
    eez::flow::replacePageHook = [](int16_t pageId, uint32_t animType, uint32_t speed, uint32_t delay) {
        Serial.printf("📄 EEZ Flow: replacePage(%d, %u, %u, %u)\n", pageId, animType, speed, delay);
        
        // スクリーンIDから対応するLVGLオブジェクトを取得
        // screens.h で定義された SCREEN_ID_* 定数を使用
        lv_obj_t *target_screen = nullptr;
        switch(pageId) {
            case SCREEN_ID_MAIN:
                target_screen = objects.main;
                break;
            case SCREEN_ID_PAGE1:
                target_screen = objects.page1;
                break;
            default:
                Serial.printf("⚠️  Unknown screen ID: %d\n", pageId);
                return;
        }
        
        // アニメーション付きで画面を切り替え
        if (target_screen) {
            lv_scr_load_anim(target_screen, (lv_scr_load_anim_t)animType, speed, delay, false);
        }
    };
    
    // scpiComponentInitフック: SCPI機能の初期化時に呼ばれる
    // 通常はログ出力のみで問題ない
    eez::flow::scpiComponentInitHook = []() {
        Serial.println("🔌 EEZ Flow: scpiComponentInit called");
    };
    
    Serial.println("✅ EEZ Flow hooks configured");
    
    // EEZ Studio UIの初期化
    Serial.println("🎨 Initializing EEZ Studio UI...");
    Serial.println("   Step 1: Clearing screen...");
    lv_obj_clean(lv_scr_act());
    
    Serial.println("   Step 2: Calling ui_init()...");
    ui_init();
    
    Serial.println("✅ EEZ Studio UI initialized successfully!");
    Serial.println();
    
    // ディスプレイの明るさを初期値に設定
    Serial.println("📍 Checkpoint 5: Setting brightness");
    M5.Display.setBrightness(DEFAULT_BRIGHTNESS);
    Serial.println("📍 Checkpoint 6: Brightness set");
    
    // メモリ状況を表示
    Serial.println("📊 Memory Status:");
    Serial.printf("   Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("   Free PSRAM: %d bytes\n", ESP.getFreePsram());
    Serial.printf("   Largest Free Block: %d bytes\n", ESP.getMaxAllocHeap());
    
    Serial.println("🚀 Setup completed successfully!");
    Serial.println("📍 Checkpoint 7: Entering loop...");
    Serial.println();
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
    static unsigned long loopCount = 0;
    
    // 初回ループの処理
    if (firstLoop) {
        Serial.println("📍 Checkpoint 8: First loop iteration");
        Serial.println("▶️  Entering main loop...");
        loopStartTime = millis();
        firstLoop = false;
    }
    
    // デバッグ用: 1000回ごとにループカウントを表示
    loopCount++;
    if (loopCount % 1000 == 0) {
        Serial.printf("📍 Loop count: %lu (uptime: %lu ms)\n", loopCount, millis());
    }
    
    // M5Unifiedの更新: タッチ入力、ボタン、IMU等のハードウェア情報を更新
    M5.update();
    
    // LVGLのタイマーハンドラー: UIイベント処理とアニメーション更新
    lv_timer_handler();
    
    // EEZ Flow Tickハンドラー: Flow言語スクリプトを実行
    // 注意: EEZ Flowエンジンの初期化に時間がかかるため、
    //       起動後1秒間は ui_tick() を呼び出さない
    if (millis() - loopStartTime > 1000) {
        ui_tick();
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
