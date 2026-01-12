// M5Stack Tab5 EEZ-Studio Yahoo News Viewer
// 依存関係: ESP-Arduino >= V3.2, M5Unified >= 0.2.10, LVGL = V8.3.11
// lv_conf.h設定: LV_COLOR_DEPTH 16, LV_COLOR_16_SWAP 1, LV_MEM_CUSTOM 1, LV_TICK_CUSTOM 1
//
// 機能:
// - WiFi接続と状態表示
// - Yahoo News RSS取得（HTTPS）
// - EEZ Studio Flow連携

#include <M5Unified.h>
#include <lvgl.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ui.h"
#include "screens.h"
#include "vars.h"
#include "eez-flow.h"
#include <esp_heap_caps.h>
#include "secrets.h"  // WiFi接続情報（SSIDとパスワード）

// EEZ Flow名前空間を使用
using namespace eez;
using namespace eez::flow;

// ==================== 定数定義 ====================
// シリアル通信
#define SERIAL_BAUD_RATE 115200

// ディスプレイ設定
#define SCREEN_WIDTH 720     // Tab5物理解像度（縦向き）
#define SCREEN_HEIGHT 1280   // Tab5物理解像度（縦向き）

#define SCREEN_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
#define DEFAULT_BRIGHTNESS 255


#define LVGL_TIMER_DELAY_MS 1          // タイマー設定
#define WIFI_CONNECT_TIMEOUT_MS 15000  // WiFi接続タイムアウト（15秒）
#define WIFI_CHECK_INTERVAL_MS 10000   // WiFi状態チェック間隔（10秒）
#define HTTP_TIMEOUT_MS 15000          // HTTPタイムアウト（15秒）

// RSS設定
#define YAHOO_NEWS_RSS_URL "https://news.yahoo.co.jp/rss/topics/top-picks.xml"
#define MAX_NEWS_ITEMS 10              // 最大ニュース取得数

// ==================== グローバル変数 ====================
// LVGL描画バッファ
static lv_disp_draw_buf_t g_draw_buf;
static lv_color_t *g_color_buf = nullptr;

// アプリケーションデータ
static int g_wifiStatus = 0;                // WiFi接続状態（0:切断, 1:接続中, 2:接続完了）
static String g_newsData = "";              // 取得したニュースデータ
static String g_updateStatus = "待機中";    // 更新状態メッセージ
static String g_cmdData = "";               // コマンド/ログデータ（シリアル出力と同じ内容）

// ログ管理設定
#define MAX_LOG_LINES 30                  // 最大保持行数
#define MAX_LOG_LENGTH 4096               // 最大文字数

// ==================== LVGLコールバック関数 ====================
// ディスプレイフラッシュコールバック
// LVGL描画データをM5Displayに転送
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    const uint32_t width = area->x2 - area->x1 + 1;
    const uint32_t height = area->y2 - area->y1 + 1;
    
    M5.Display.pushImageDMA(area->x1, area->y1, width, height, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

// M5Touchのタッチ情報をLVGLに渡す
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    auto touch_detail = M5.Touch.getDetail();
    
    // タッチ状態の判定（isPressed: タッチされている間ずっとtrue）
    if (touch_detail.isPressed() || touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;  // プレス状態
        data->point.x = touch_detail.x;
        data->point.y = touch_detail.y;
    } else {
        data->state = LV_INDEV_STATE_REL;  // リリース状態
    }
}

// ==================== ログ出力関数 ====================
// ログに行を追加
// シリアル出力と同時にg_cmdDataに追記
void addLog(const char* message)
{
    // シリアル出力
    Serial.println(message);    
    // cmdDataに追記
    g_cmdData += message;
    g_cmdData += "\n";
    
    // 行数制限チェック
    int lineCount = 0;
    for (int i = 0; i < g_cmdData.length(); i++) {
        if (g_cmdData[i] == '\n') lineCount++;
    }
    
    // 最大行数を超えた場合、古い行を削除
    while (lineCount > MAX_LOG_LINES) {
        int firstNewline = g_cmdData.indexOf('\n');
        if (firstNewline != -1) {
            g_cmdData = g_cmdData.substring(firstNewline + 1);
            lineCount--;
        } else {
            break;
        }
    }
    
    // 最大文字数制限
    if (g_cmdData.length() > MAX_LOG_LENGTH) {
        g_cmdData = g_cmdData.substring(g_cmdData.length() - MAX_LOG_LENGTH);
    }
}

// フォーマット付きログ追加
void addLogf(const char* format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    addLog(buffer);
}

// ログをクリア
void clearLog()
{
    g_cmdData = "";
}

// ログをtextareaに表示
void updateLogDisplay()
{
    if (objects.textarea_news) {
        lv_textarea_set_text(objects.textarea_cmd, g_cmdData.c_str());
        // 最下部にスクロール
        lv_obj_scroll_to_y(objects.textarea_cmd, LV_COORD_MAX, LV_ANIM_OFF);
    }
}

// ==================== 初期化関数 ====================

// SPIRAMバッファの確保
// LVGL描画用のバッファをSPIRAMに確保
bool allocateDisplayBuffer()
{
    g_color_buf = (lv_color_t *)heap_caps_malloc(
        sizeof(lv_color_t) * SCREEN_BUFFER_SIZE,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    
    if (g_color_buf == nullptr) {
        addLog("Error: Failed to allocate SPIRAM buffer!");
        M5.Display.setCursor(0, 0);
        M5.Display.println("Error: SPIRAM allocation failed!");
        return false;
    }
    
    addLogf("Display buffer allocated: %d bytes (%.2f MB)", 
            sizeof(lv_color_t) * SCREEN_BUFFER_SIZE,
            (sizeof(lv_color_t) * SCREEN_BUFFER_SIZE) / 1048576.0);
    return true;
}

// ディスプレイドライバの設定
// LVGL表示ドライバを設定（90度回転、DMA転送）
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
    addLog("Initializing LVGL display...");
    
    lv_init();
    
    if (!allocateDisplayBuffer()) {
        return false;
    }
    
    lv_disp_draw_buf_init(&g_draw_buf, g_color_buf, NULL, SCREEN_BUFFER_SIZE);
    
    static lv_disp_drv_t disp_drv;
    configureDisplayDriver(disp_drv);
    
    addLog("LVGL display initialized");
    return true;
}

// LVGLタッチ初期化
void initLvglTouch()
{
    addLog("Initializing LVGL touch...");
    
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_indev_read;
    
    // タッチの反応性を改善
    indev_drv.long_press_time = 400;
    indev_drv.long_press_repeat_time = 100;
    
    lv_indev_drv_register(&indev_drv);
    
    addLog("LVGL touch initialized");
}

// ==================== WiFi関連関数 ====================

// WiFi接続
// 指定されたSSID/パスワードでWiFiに接続
bool connectWiFi()
{
    addLog("==============================");
    addLog("WiFi Connection:");
    addLogf("  SSID: %s", ssid);
    
    g_wifiStatus = 1;  // 接続中
    updateFlowVariable("wifiStatus", g_wifiStatus);
    updateLogDisplay();
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    unsigned long startTime = ::millis();
    int dotCount = 0;
    while (WiFi.status() != WL_CONNECTED) {
        if (::millis() - startTime > WIFI_CONNECT_TIMEOUT_MS) {
            addLog("  Status: Connection timeout");
            g_wifiStatus = 0;  // 切断
            updateFlowVariable("wifiStatus", g_wifiStatus);
            updateLogDisplay();
            return false;
        }
        delay(500);
        Serial.print(".");
        dotCount++;
        if (dotCount % 10 == 0) {
            addLog("  接続中...");
            updateLogDisplay();
        }
    }
    
    addLog("");
    addLog("  Status: Connected");
    addLogf("  IP: %s", WiFi.localIP().toString().c_str());
    
    g_wifiStatus = 2;  // 接続完了
    updateFlowVariable("wifiStatus", g_wifiStatus);
    updateLogDisplay();
    return true;
}

// WiFi状態チェック
// 現在のWiFi接続状態を返す（0:切断, 2:接続完了）
int getWiFiStatus()
{
    return (WiFi.status() == WL_CONNECTED) ? 2 : 0;
}

// WiFi再接続
// WiFi切断時に自動的に再接続を試みる
void reconnectWiFi()
{
    if (WiFi.status() != WL_CONNECTED) {
        addLog("WiFi disconnected. Reconnecting...");
        updateLogDisplay();
        connectWiFi();
    }
}

// ==================== RSS取得関連関数 ====================

// XMLからタグの内容を抽出
// 指定したタグ名の内容を文字列として取得
String extractTag(String xml, String tagName, int startPos = 0)
{
    String openTag = "<" + tagName + ">";
    String closeTag = "</" + tagName + ">";
    
    int start = xml.indexOf(openTag, startPos);
    if (start == -1) return "";
    
    int end = xml.indexOf(closeTag, start);
    if (end == -1) return "";
    
    return xml.substring(start + openTag.length(), end);
}

// ニュースリストを作成
// RSSフィードから最大MAX_NEWS_ITEMS件のニュースタイトルを抽出
String createNewsList(String xml)
{
    String newsList = "";
    int newsCount = 0;
    int searchPos = 0;
    
    while (newsCount < MAX_NEWS_ITEMS) {
        // <item>タグを検索
        int itemStart = xml.indexOf("<item>", searchPos);
        if (itemStart == -1) break;
        
        int itemEnd = xml.indexOf("</item>", itemStart);
        if (itemEnd == -1) break;
        
        // <title>タグからタイトルを抽出
        String title = extractTag(xml, "title", itemStart);
        if (title.length() > 0) {
            newsCount++;
            newsList += String(newsCount) + ". " + title + "\n\n";
        }
        
        searchPos = itemEnd + 7;  // "</item>"の次から検索
    }
    
    if (newsList.length() == 0) {
        newsList = "ニュースの取得に失敗しました。";
    }
    
    return newsList;
}

// Yahoo News RSSを取得
// Yahoo NewsのRSSフィードを取得してニュースリストを返す
String fetchYahooNews()
{
    addLog("==============================");
    addLog("Fetching Yahoo News RSS...");
    updateLogDisplay();
    
    g_updateStatus = "取得中...";
    updateFlowVariable("updateStatus", g_updateStatus);
    
    // WiFi接続確認
    if (WiFi.status() != WL_CONNECTED) {
        addLog("Error: WiFi not connected");
        g_updateStatus = "エラー: WiFi未接続";
        updateFlowVariable("updateStatus", g_updateStatus);
        updateLogDisplay();
        return "エラー: WiFiに接続されていません。";
    }
    
    HTTPClient http;
    http.setTimeout(HTTP_TIMEOUT_MS);
    
    String result = "";
    
    if (http.begin(YAHOO_NEWS_RSS_URL)) {
        addLog("Sending HTTP GET request...");
        updateLogDisplay();
        
        int httpCode = http.GET();
        
        addLogf("HTTP Response code: %d", httpCode);
        updateLogDisplay();
        
        if (httpCode > 0) {
            if (httpCode == HTTP_CODE_OK) {
                String xml = http.getString();
                addLogf("Received XML: %d bytes", xml.length());
                updateLogDisplay();
                
                // ニュースリストを作成
                result = createNewsList(xml);
                
                g_updateStatus = "取得完了";
                addLog("News fetched successfully");
            } else {
                result = "エラー: HTTP " + String(httpCode);
                g_updateStatus = "エラー: HTTP " + String(httpCode);
                addLogf("HTTP error: %d", httpCode);
            }
        } else {
            result = "エラー: 接続失敗";
            g_updateStatus = "エラー: 接続失敗";
            addLog("HTTP connection failed");
        }
        
        http.end();
    } else {
        result = "エラー: HTTP初期化失敗";
        g_updateStatus = "エラー: HTTP初期化失敗";
        addLog("HTTP begin failed");
    }
    
    updateFlowVariable("updateStatus", g_updateStatus);
    addLogf("Free Heap: %d bytes", ESP.getFreeHeap());
    addLog("==============================");
    updateLogDisplay();
    
    return result;
}

// ==================== Flow変数更新関数 ====================

// Flow変数を更新（String）
// EEZ Flow変数にString値を設定
void updateFlowVariable(const char* varName, String value)
{
    if (strcmp(varName, "newsData") == 0) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_NEWS_DATA, Value(value.c_str()));
        addLogf("Flow variable updated: newsData (length=%d)", value.length());
    } else if (strcmp(varName, "updateStatus") == 0) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_UPDATE_STATUS, Value(value.c_str()));
        addLogf("Flow variable updated: updateStatus = %s", value.c_str());
    }
}

// Flow変数を更新（int）
// EEZ Flow変数にint値を設定し、WiFi状態ラベルも更新
void updateFlowVariable(const char* varName, int value)
{
    if (strcmp(varName, "wifiStatus") == 0) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_WIFI_STATUS, Value(value));
        addLogf("Flow variable updated: wifiStatus = %d", value);
        
        // WiFi状態をラベルに直接反映
        const char* statusText = "";
        switch(value) {
            case 0: statusText = "WiFi: 切断"; break;
            case 1: statusText = "WiFi: 接続中..."; break;
            case 2: statusText = "WiFi: 接続済"; break;
        }
        if (objects.label_wifi_status) {
            lv_label_set_text(objects.label_wifi_status, statusText);
        }
    }
}

// ==================== Flowアクション関数 ====================

// ニュース取得アクション
// EEZ Flowから呼ばれる。Yahoo Newsを取得して表示を更新
extern "C" void action_action_fetch_news()
{
    addLog("Action: action_fetch_news");
    updateLogDisplay();
    
    // ニュースを取得
    g_newsData = fetchYahooNews();
    
    // Flow変数を更新
    updateFlowVariable("newsData", g_newsData);
    
    // 表示を更新
    action_action_refresh_display();
}

// 表示更新アクション
// ログデータをテキストエリアに表示
extern "C" void action_action_refresh_display()
{
    addLog("Action: action_refresh_display");
    
    // テキストエリアにログデータを反映（cmdDataを表示）
    if (objects.textarea_news) {
        lv_textarea_set_text(objects.textarea_news, g_cmdData.c_str());
        // 最下部にスクロール
        lv_obj_scroll_to_y(objects.textarea_news, LV_COORD_MAX, LV_ANIM_OFF);
        addLogf("Display updated: %d bytes written to textarea", g_cmdData.length());
    } else {
        addLog("Warning: textarea_news object is NULL!");
    }
    
    // 更新状態ラベルを反映
    if (objects.label_update_status) {
        lv_label_set_text(objects.label_update_status, g_updateStatus.c_str());
        addLogf("Update status label: %s", g_updateStatus.c_str());
    } else {
        addLog("Warning: label_update_status object is NULL!");
    }
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
    addLog("==============================");
    addLog(" Yahoo News Viewer for Tab5  ");
    addLog("==============================");
    addLogf("LVGL Version: %d.%d.%d", 
            lv_version_major(), lv_version_minor(), lv_version_patch());
    addLogf("Free Heap: %d bytes", ESP.getFreeHeap());
    addLogf("Free PSRAM: %d bytes", ESP.getFreePsram());
    
    // ディスプレイのカラー深度を16ビットに設定
    M5.Display.setColorDepth(16);
    addLog("Display color depth set to 16-bit");
    
    // LVGL初期化
    if (!initLvglDisplay()) {
        addLog("FATAL: Display initialization failed!");
        while(1) { delay(1000); }
    }
    initLvglTouch();
    
    // EEZ-Studio UI初期化
    addLog("Initializing EEZ-Studio UI...");
    ui_init();
    addLog("EEZ-Studio UI initialized");
    
    // 初期ログ表示
    updateLogDisplay();
    
    // 画面の明るさ設定
    M5.Display.setBrightness(DEFAULT_BRIGHTNESS);
    addLog("Initial brightness set to maximum");
    
    // WiFi接続
    if (connectWiFi()) {
        // 初回ニュース取得
        addLog("Fetching initial news...");
        updateLogDisplay();
        action_action_fetch_news();
    }
    
    addLog("==============================");
    addLog("Setup completed");
    addLog("==============================");
    addLogf("Free Heap after setup: %d bytes", ESP.getFreeHeap());
    addLogf("Free PSRAM after setup: %d bytes", ESP.getFreePsram());
    updateLogDisplay();
}

void loop()
{
    // M5Unified更新（ボタン、タッチ、IMUなど）
    M5.update();
    
    // LVGL更新（画面描画処理）
    lv_timer_handler();
    
    // EEZ-Studio UI更新（Flow処理）
    ui_tick();
    
    // WiFi状態を定期的にチェック（WIFI_CHECK_INTERVAL_MS間隔）
    static unsigned long lastWiFiCheck = 0;
    if (::millis() - lastWiFiCheck > WIFI_CHECK_INTERVAL_MS) {
        int currentStatus = getWiFiStatus();
        if (currentStatus != g_wifiStatus) {
            g_wifiStatus = currentStatus;
            updateFlowVariable("wifiStatus", g_wifiStatus);
            
            // WiFi切断時は再接続を試みる
            if (g_wifiStatus == 0) {
                reconnectWiFi();
            }
        }
        lastWiFiCheck = ::millis();
    }
    
    delay(LVGL_TIMER_DELAY_MS);
}

