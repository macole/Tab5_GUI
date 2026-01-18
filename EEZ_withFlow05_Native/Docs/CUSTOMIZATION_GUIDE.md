# カスタマイズガイド

このドキュメントでは、EEZ Templateをベースに独自のアプリケーションを開発する方法を説明します。

## 📋 目次

1. [基本的なカスタマイズ](#基本的なカスタマイズ)
2. [UIの変更](#uiの変更)
3. [機能の追加](#機能の追加)
4. [WiFi機能の実装](#wifi機能の実装)
5. [データ通信の実装](#データ通信の実装)
6. [よくあるカスタマイズパターン](#よくあるカスタマイズパターン)

---

## 基本的なカスタマイズ

### プロジェクト名の変更

1. **ファイル名を変更**
   - `EEZ_Template.ino` → `YourApp.ino`

2. **プロジェクトフォルダ名を変更**
   - `EEZ_Template/` → `YourApp/`

3. **ヘッダーコメントを更新**
   ```cpp
   /*******************************************************************************
    * あなたのアプリケーション名
    * 
    * アプリケーションの説明
    ******************************************************************************/
   ```

### アプリケーション定数の変更

`EEZ_Template.ino`の定数定義セクションで設定を変更：

```cpp
// アプリケーション設定
#define APP_UPDATE_INTERVAL_MS 100  // 更新間隔（ミリ秒）
#define DEFAULT_BRIGHTNESS 255       // 初期画面輝度（0-255）
```

---

## UIの変更

### EEZ Studioでの基本操作

#### 1. プロジェクトを開く

```
File → Open Project → LVGLv8withFlow.eez-project
```

#### 2. 新しい画面を追加

1. **Pagesタブ**を選択
2. **+ボタン**をクリック
3. 画面名を入力（例: "Settings"）
4. レイアウトを編集

#### 3. ウィジェットを追加

**左側のウィジェットパネルから選択**：

- **Label**: テキスト表示
- **Button**: ボタン
- **Slider**: スライダー
- **Text Area**: テキスト入力
- **Image**: 画像表示
- **Panel**: コンテナ
- **List**: リスト表示
- **Chart**: グラフ表示

**配置方法**：

1. ウィジェットをドラッグ＆ドロップ
2. プロパティパネルで設定
   - Position (X, Y)
   - Size (Width, Height)
   - Appearance (色、フォント)
   - Behavior (可視性、有効/無効)

#### 4. スタイルのカスタマイズ

**Stylesタブ**：

1. **+ボタン**で新しいスタイルを作成
2. プロパティを設定：
   - Background color
   - Border color/width
   - Text color/font
   - Padding/Margin
3. ウィジェットに適用

#### 5. Flowでの動作定義

**Flowタブ**：

1. アクショ��を追加（ドラッグ＆ドロップ）
   - **Set Text**: テキスト設定
   - **Set Value**: 値設定
   - **Show/Hide**: 表示/非表示
   - **Navigate**: 画面遷移
   - **Delay**: 待機
   - **Loop**: ループ処理

2. トリガーを設定
   - **Button Pressed**: ボタン押下時
   - **Timer**: タイマー
   - **Variable Changed**: 変数変更時

3. アクションを接続（線で繋ぐ）

#### 6. エクスポート

```
Build → Build & Export
```

生成されたファイルをプロジェクトフォルダにコピー：

```
ui.c, ui.h, screens.c, screens.h, eez-flow.cpp, eez-flow.h,
actions.h, styles.c, styles.h, images.c, images.h, fonts.h,
structs.h, vars.h
```

---

## 機能の追加

### センサー読み取り機能の追加

#### 例: 温湿度センサー（SHT40）

```cpp
// 1. ライブラリをインクルード
#include <Wire.h>
#include <SHT4x.h>

// 2. グローバル変数
static SHT4x sht40;
static float g_temperature = 0.0;
static float g_humidity = 0.0;

// 3. setup()で初期化
void setup() {
    // ... 既存の初期化 ...
    
    // センサー初期化
    Wire.begin();
    if (!sht40.begin()) {
        Serial.println("❌ SHT40 sensor not found!");
    } else {
        Serial.println("✅ SHT40 sensor initialized");
    }
}

// 4. 読み取り関数を追加
void updateSensorData() {
    static unsigned long lastReadTime = 0;
    const unsigned long READ_INTERVAL = 2000;  // 2秒ごと
    
    if (millis() - lastReadTime < READ_INTERVAL) {
        return;
    }
    lastReadTime = millis();
    
    // センサー読み取り
    if (sht40.measure()) {
        g_temperature = sht40.getTemperature();
        g_humidity = sht40.getHumidity();
        
        // UIに表示（EEZ Studioで作成したラベルに表示）
        lv_label_set_text_fmt(objects.label_temp, "%.1f°C", g_temperature);
        lv_label_set_text_fmt(objects.label_humi, "%.1f%%", g_humidity);
        
        Serial.printf("Temperature: %.1f°C, Humidity: %.1f%%\n", 
                     g_temperature, g_humidity);
    }
}

// 5. loop()で呼び出し
void loop() {
    M5.update();
    lv_timer_handler();
    ui_tick();
    
    updateSensorData();  // ← 追加
    
    delay(LVGL_TIMER_DELAY_MS);
}
```

### タイマー機能の追加

#### 例: 1秒ごとに時刻を更新

```cpp
// グローバル変数
static unsigned long g_startTime = 0;

// setup()で初期化
void setup() {
    // ... 既存の初期化 ...
    g_startTime = millis();
}

// 時刻更新関数
void updateClock() {
    static unsigned long lastUpdateTime = 0;
    
    if (millis() - lastUpdateTime < 1000) {  // 1秒ごと
        return;
    }
    lastUpdateTime = millis();
    
    // 経過時間を計算
    unsigned long elapsedSeconds = (millis() - g_startTime) / 1000;
    int hours = elapsedSeconds / 3600;
    int minutes = (elapsedSeconds % 3600) / 60;
    int seconds = elapsedSeconds % 60;
    
    // UIに表示
    lv_label_set_text_fmt(objects.label_time, "%02d:%02d:%02d", 
                          hours, minutes, seconds);
}

// loop()で呼び出し
void loop() {
    // ...
    updateClock();
    // ...
}
```

---

## WiFi機能の実装

### HTTPリクエストの送信

```cpp
#ifdef ENABLE_WIFI
#include <HTTPClient.h>

void fetchWeatherData() {
    if (!g_wifiConnected) {
        Serial.println("⚠️  WiFi not connected");
        return;
    }
    
    HTTPClient http;
    const char* url = "http://api.example.com/weather";
    
    Serial.printf("🌐 Fetching: %s\n", url);
    http.begin(url);
    
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("✅ Data received:");
        Serial.println(payload);
        
        // データを解析してUIに表示
        // （JSONパース処理を追加）
    } else {
        Serial.printf("❌ HTTP Error: %d\n", httpCode);
    }
    
    http.end();
}
#endif
```

### MQTTの実装

```cpp
#ifdef ENABLE_WIFI
#include <PubSubClient.h>

// グローバル変数
static WiFiClient g_wifiClient;
static PubSubClient g_mqttClient(g_wifiClient);

// MQTT設定
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "m5stack/tab5/data"

// MQTTコールバック
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.printf("📨 MQTT Message [%s]: ", topic);
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    
    // メッセージをUIに表示
    char message[256];
    strncpy(message, (char*)payload, min(length, 255));
    message[min(length, 255)] = '\0';
    lv_label_set_text(objects.label_mqtt, message);
}

// MQTT初期化
void initMQTT() {
    g_mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    g_mqttClient.setCallback(mqttCallback);
}

// MQTT接続
bool connectMQTT() {
    String clientId = "M5Tab5-" + String(random(0xffff), HEX);
    
    if (g_mqttClient.connect(clientId.c_str())) {
        Serial.println("✅ MQTT connected");
        g_mqttClient.subscribe(MQTT_TOPIC);
        return true;
    } else {
        Serial.printf("❌ MQTT connection failed, rc=%d\n", 
                     g_mqttClient.state());
        return false;
    }
}

// MQTT状態チェック
void checkMQTT() {
    if (!g_wifiConnected) {
        return;
    }
    
    if (!g_mqttClient.connected()) {
        connectMQTT();
    }
    g_mqttClient.loop();
}

// setup()で初期化
void setup() {
    // ... WiFi初期化後 ...
    initMQTT();
    connectMQTT();
}

// loop()で呼び出し
void loop() {
    // ...
    checkMQTT();
    // ...
}
#endif
```

---

## データ通信の実装

### JSON解析

```cpp
#include <ArduinoJson.h>

void parseWeatherJSON(const String& jsonString) {
    StaticJsonDocument<1024> doc;
    
    DeserializationError error = deserializeJson(doc, jsonString);
    
    if (error) {
        Serial.printf("❌ JSON parse error: %s\n", error.c_str());
        return;
    }
    
    // データを取得
    const char* city = doc["city"];
    float temperature = doc["temperature"];
    const char* weather = doc["weather"];
    
    // UIに表示
    lv_label_set_text(objects.label_city, city);
    lv_label_set_text_fmt(objects.label_temp, "%.1f°C", temperature);
    lv_label_set_text(objects.label_weather, weather);
    
    Serial.printf("City: %s, Temp: %.1f°C, Weather: %s\n", 
                 city, temperature, weather);
}
```

### SDカード保存

```cpp
#include <SD.h>
#include <SPI.h>

// SDカード初期化
bool initSD() {
    if (!SD.begin()) {
        Serial.println("❌ SD card initialization failed");
        return false;
    }
    Serial.println("✅ SD card initialized");
    return true;
}

// データ保存
void saveDataToSD(float temperature, float humidity) {
    File file = SD.open("/data.csv", FILE_APPEND);
    
    if (!file) {
        Serial.println("❌ Failed to open file for writing");
        return;
    }
    
    // タイムスタンプ + データ
    file.printf("%lu,%.2f,%.2f\n", millis(), temperature, humidity);
    file.close();
    
    Serial.println("✅ Data saved to SD card");
}
```

---

## よくあるカスタマイズパターン

### パターン1: リスト表示

```cpp
// リストアイテムの追加
void addListItem(const char* text) {
    lv_obj_t* list_btn = lv_list_add_btn(objects.list1, NULL, text);
    // ボタンにイベントハンドラを追加
    lv_obj_add_event_cb(list_btn, list_item_click_cb, 
                        LV_EVENT_CLICKED, NULL);
}

// リストアイテムクリック時のコールバック
static void list_item_click_cb(lv_event_t* e) {
    lv_obj_t* btn = lv_event_get_target(e);
    const char* text = lv_list_get_btn_text(objects.list1, btn);
    Serial.printf("List item clicked: %s\n", text);
}
```

### パターン2: グラフ表示

```cpp
// チャート初期化
void initChart() {
    // データシリーズを追加
    lv_chart_series_t* series = lv_chart_add_series(
        objects.chart1, 
        lv_palette_main(LV_PALETTE_BLUE),
        LV_CHART_AXIS_PRIMARY_Y
    );
    
    // 範囲設定
    lv_chart_set_range(objects.chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
}

// データポイントを追加
void addChartData(int value) {
    lv_chart_set_next_value(objects.chart1, series, value);
    lv_chart_refresh(objects.chart1);
}
```

### パターン3: 画面遷移

```cpp
// 画面遷移（EEZ Studioで複数画面を定義した場合）
void navigateToSettings() {
    // EEZ Flowで定義した画面IDを使用
    lv_scr_load(objects.screen_settings);
}

// ボタンコールバックで画面遷移
static void settings_btn_click_cb(lv_event_t* e) {
    navigateToSettings();
}
```

### パターン4: 通知表示

```cpp
// 通知メッセージボックスの表示
void showNotification(const char* title, const char* message) {
    static const char * btns[] = {"OK", ""};
    
    lv_obj_t* mbox = lv_msgbox_create(NULL, title, message, btns, false);
    lv_obj_center(mbox);
}

// 使用例
showNotification("Success", "Data saved successfully!");
```

---

## デバッグのヒント

### シリアルログの活用

```cpp
// 詳細ログ
#define DEBUG_PRINT(fmt, ...) \
    Serial.printf("[%lu] " fmt "\n", millis(), ##__VA_ARGS__)

// 使用例
DEBUG_PRINT("Temperature: %.1f°C", temperature);
```

### メモリ使用量の監視

```cpp
void printMemoryUsage() {
    Serial.println("📊 Memory Usage:");
    Serial.printf("   Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("   Free PSRAM: %d bytes\n", ESP.getFreePsram());
}

// loop()で定期的に呼び出し
void loop() {
    static unsigned long lastPrintTime = 0;
    if (millis() - lastPrintTime > 10000) {  // 10秒ごと
        printMemoryUsage();
        lastPrintTime = millis();
    }
    // ...
}
```

---

## 次のステップ

1. **EEZ Studioでプロトタイプを作成**
   - 基本的なレイアウトを設計
   - Flowで動作を定義

2. **Arduino側で機能を実装**
   - センサー読み取り
   - データ通信
   - 状態管理

3. **テストと改善**
   - シリアルモニタでデバッグ
   - メモリ使用量を監視
   - パフォーマンスを最適化

4. **完成**
   - ドキュメント作成
   - コードのクリーンアップ
   - 最終テスト

---

**このガイドで疑問が解決しない場合は、[TECHNICAL_GUIDE.md](TECHNICAL_GUIDE.md)を参照してください。**
