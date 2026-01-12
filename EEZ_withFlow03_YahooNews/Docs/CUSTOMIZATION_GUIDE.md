# Customization Guide - Yahoo News Viewer

Yahoo News Viewer for M5Stack Tab5 のカスタマイズ方法とテンプレート活用ガイドです。

## 📋 目次

1. [設定変更（初級）](#設定変更初級)
2. [EEZ Studio UI設計（中級）](#eez-studio-ui設計中級)
3. [コード改造（上級）](#コード改造上級)
4. [テンプレート活用](#テンプレート活用)

---

## 設定変更（初級）

### RSS URLの変更

**場所**: `EEZ_withFlow03_YahooNews.ino` 47行目付近

```cpp
#define YAHOO_NEWS_RSS_URL "https://news.yahoo.co.jp/rss/topics/top-picks.xml"
```

**Yahoo News RSS一覧**:

| カテゴリ | URL |
|---------|-----|
| 主要ニュース | `/rss/topics/top-picks.xml` |
| 国内ニュース | `/rss/topics/domestic.xml` |
| 国際ニュース | `/rss/topics/world.xml` |
| 経済ニュース | `/rss/topics/business.xml` |
| IT・科学 | `/rss/topics/it.xml` |
| スポーツ | `/rss/topics/sports.xml` |
| エンタメ | `/rss/topics/entertainment.xml` |
| 地域 | `/rss/topics/local.xml` |

**他のRSSフィード**:
- はてなブックマーク: `http://feeds.feedburner.com/hatena/b/hotentry`
- NHKニュース: `https://www3.nhk.or.jp/rss/news/cat0.xml`

### ニュース取得数の変更

**場所**: `EEZ_withFlow03_YahooNews.ino` 48行目付近

```cpp
#define MAX_NEWS_ITEMS 10  // 10件
```

**推奨値**:
- 少ない: 5件（表示が速い）
- 標準: 10件（バランス良い）
- 多い: 20件（スクロール多め）
- 最大: 50件（テキストエリアの制限に注意）

### タイムアウト時間の変更

**WiFi接続タイムアウト**:
```cpp
// 39行目付近
#define WIFI_CONNECT_TIMEOUT_MS 15000  // 15秒
```

**HTTP通信タイムアウト**:
```cpp
// 42行目付近
#define HTTP_TIMEOUT_MS 15000  // 15秒
```

**WiFi状態チェック間隔**:
```cpp
// 40行目付近
#define WIFI_CHECK_INTERVAL_MS 10000  // 10秒
```

### 画面の明るさ変更

**場所**: `EEZ_withFlow03_YahooNews.ino` 37行目付近

```cpp
#define DEFAULT_BRIGHTNESS 255  // 最大輝度（0～255）
```

**推奨値**:
- 明るい場所: 255（最大）
- 通常: 180
- 暗い場所: 100
- 省電力: 50

---

## EEZ Studio UI設計（中級）

### EEZ Studioのインストール

1. [EEZ Studio公式サイト](https://www.envox.eu/eez-studio/download/)からダウンロード
2. インストール（macOS: DMGをApplicationsへ、Windows: インストーラー実行）

### プロジェクトを開く

既存のUIを編集する場合：

1. EEZ Studioを起動
2. 「File」→「Open Project」
3. `eez-project/YahooNewsViewer_Tab5.eez-project`を選択

新規作成する場合：

1. 「File」→「New Project」
2. プロジェクト名: 任意
3. Target Platform: LVGL
4. Display Size: 800 x 480
5. Color Format: RGB565

### 画面設計

#### 基本レイアウト

現在の画面構成（`screen_main`）:

```
┌────────────────────────────────────┐
│ [更新]  label_update_status         │ ← 上部エリア
│                                    │
│          label_title               │ ← タイトル
│          (Yahoo News)              │
│                                    │
│ ┌────────────────────────────────┐ │
│ │                                │ │
│ │    textarea_news               │ │ ← メインエリア
│ │    (スクロール可能)             │ │
│ │                                │ │
│ └────────────────────────────────┘ │
│                    label_wifi_status│ ← 下部エリア
└────────────────────────────────────┘
```

#### ウィジェット一覧

| ウィジェット | 型 | 位置 | サイズ | 説明 |
|-------------|-----|------|--------|------|
| `label_title` | Label | (356, 92) | AUTO | タイトル表示 |
| `button_refresh` | Button | (36, 67) | 100x50 | 更新ボタン |
| `label_wifi_status` | Label | (517, 19) | AUTO | WiFi状態 |
| `label_update_status` | Label | (36, 11) | AUTO | 更新状態 |
| `textarea_news` | Textarea | (36, 133) | 728x334 | ニュース表示 |

### ウィジェットのカスタマイズ

#### タイトルの変更

1. `label_title`を選択
2. Propertiesパネル → Text: "Yahoo News" → 任意のテキストに変更
3. フォントサイズを変更: Style → Text Font → サイズ選択

#### ボタンの追加

1. 左側パネルから「Button」をドラッグ＆ドロップ
2. 位置とサイズを調整
3. Propertiesパネル:
   - Name: `button_category_switch`（任意）
   - Text: "カテゴリ変更"
4. Eventsタブ:
   - Event: `PRESSED`
   - Action: Call Native Action `action_switch_category`

#### テキストエリアのサイズ変更

1. `textarea_news`を選択
2. ハンドルをドラッグしてサイズ調整
3. または、Properties → Geometry:
   - Width: 728 → 任意の値
   - Height: 334 → 任意の値

#### 日本語フォントの設定

1. 「Fonts」タブを選択
2. 「Add Font」をクリック
3. Font file: IPAexGothic.ttf または Noto Sans JP を選択
4. Name: `ui_font_ipa_go`
5. Size: 16
6. Range: 以下を選択
   - Basic Latin (0x0020-0x007F)
   - Hiragana (0x3040-0x309F)
   - Katakana (0x30A0-0x30FF)
   - CJK Unified Ideographs（必要な範囲のみ）

### Flow変数の追加

1. 「Variables」タブを選択
2. 「Add Variable」をクリック
3. 設定:
   - Name: `currentCategory`
   - Type: `string`
   - Default Value: `"top-picks"`
   - Description: "現在表示中のニュースカテゴリ"

### Flowアクションの追加

1. 「Actions」タブを選択
2. 「Add Action」をクリック
3. 設定:
   - Name: `action_switch_category`
   - Type: `Native`
   - Description: "ニュースカテゴリを切り替える"

### プロジェクトのExport

1. 「File」→「Build」またはツールバーの「Build」ボタン
2. 以下のファイルが生成される:
   ```
   ui.h, ui.c
   screens.h, screens.c
   vars.h
   eez-flow.h, eez-flow.cpp
   images.c
   styles.c
   fonts.h
   ```
3. 生成されたファイルをArduinoプロジェクトフォルダにコピー:
   ```bash
   cp -v ui.h ui.c screens.h screens.c vars.h eez-flow.h eez-flow.cpp \
      images.c styles.c fonts.h ../
   ```

---

## コード改造（上級）

### 新しいFlow変数の実装

**例**: カテゴリ切り替え機能を追加

1. **vars.hに定数を追加** (EEZ Studioで生成後):
   ```cpp
   enum FlowGlobalVariables {
       FLOW_GLOBAL_VARIABLE_UPDATE_STATUS = 0,
       FLOW_GLOBAL_VARIABLE_NEWS_DATA = 1,
       FLOW_GLOBAL_VARIABLE_WIFI_STATUS = 2,
       FLOW_GLOBAL_VARIABLE_CURRENT_CATEGORY = 3  // 追加
   };
   ```

2. **.inoファイルにグローバル変数を追加**:
   ```cpp
   // グローバル変数セクション
   static String g_currentCategory = "top-picks";
   ```

3. **updateFlowVariable()を拡張**:
   ```cpp
   void updateFlowVariable(const char* varName, String value)
   {
       if (strcmp(varName, "newsData") == 0) {
           setGlobalVariable(FLOW_GLOBAL_VARIABLE_NEWS_DATA, Value(value.c_str()));
       } else if (strcmp(varName, "updateStatus") == 0) {
           setGlobalVariable(FLOW_GLOBAL_VARIABLE_UPDATE_STATUS, Value(value.c_str()));
       } else if (strcmp(varName, "currentCategory") == 0) {
           setGlobalVariable(FLOW_GLOBAL_VARIABLE_CURRENT_CATEGORY, Value(value.c_str()));
           g_currentCategory = value;
       }
   }
   ```

4. **新しいアクション関数を追加**:
   ```cpp
   extern "C" void action_switch_category()
   {
       Serial.println("Action: switch_category");
       
       // カテゴリをローテーション
       if (g_currentCategory == "top-picks") {
           g_currentCategory = "domestic";
       } else if (g_currentCategory == "domestic") {
           g_currentCategory = "business";
       } else {
           g_currentCategory = "top-picks";
       }
       
       updateFlowVariable("currentCategory", g_currentCategory);
       
       // URLを更新してニュース取得
       // （fetchYahooNews()を改造して動的URLに対応させる必要あり）
       action_action_fetch_news();
   }
   ```

### RSS URLの動的変更

**fetchYahooNews()を改造**:

```cpp
String fetchYahooNews()
{
    // ...既存のコード...
    
    // URLを動的に構築
    String url = "https://news.yahoo.co.jp/rss/topics/" + g_currentCategory + ".xml";
    
    if (http.begin(url)) {
        // ...既存のコード...
    }
    
    // ...既存のコード...
}
```

### 自動更新機能の追加

```cpp
// グローバル変数セクションに追加
static bool g_autoRefresh = false;
static unsigned long g_lastRefresh = 0;
#define AUTO_REFRESH_INTERVAL_MS 300000  // 5分

// loop()に追加
void loop()
{
    M5.update();
    lv_timer_handler();
    ui_tick();
    
    // WiFi状態チェック
    static unsigned long lastWiFiCheck = 0;
    if (::millis() - lastWiFiCheck > WIFI_CHECK_INTERVAL_MS) {
        // ...既存のコード...
        lastWiFiCheck = ::millis();
    }
    
    // 自動更新（追加）
    if (g_autoRefresh && ::millis() - g_lastRefresh > AUTO_REFRESH_INTERVAL_MS) {
        Serial.println("Auto refresh triggered");
        action_action_fetch_news();
        g_lastRefresh = ::millis();
    }
    
    delay(LVGL_TIMER_DELAY_MS);
}
```

---

## テンプレート活用

### このプロジェクトをテンプレートとして使用

Yahoo News Viewer は、他のWebサービス連携アプリのテンプレートとして使用できます。

### 応用例

#### 1. 天気予報アプリ

**変更点**:
- RSS URL → 気象庁API URL
- XML解析 → JSON解析（ArduinoJson使用）
- ニュースタイトル → 天気情報・気温・降水確率

**実装手順**:
1. プロジェクトをコピー
   ```bash
   cp -r EEZ_withFlow03_YahooNews WeatherApp
   ```

2. `fetchYahooNews()`を`fetchWeatherData()`に改名・改造
   ```cpp
   String fetchWeatherData()
   {
       String url = "https://www.jma.go.jp/bosai/forecast/data/forecast/130000.json";
       // JSON取得・解析
       // ...
   }
   ```

3. EEZ StudioでUIを変更（天気アイコン、気温表示など）

#### 2. 株価表示アプリ

**変更点**:
- RSS URL → 株価API URL
- ニュースリスト → 銘柄リスト・価格・変動率

**実装手順**:
1. プロジェクトをコピー
2. `fetchYahooNews()`を`fetchStockData()`に改造
3. API キーを`secrets.h`に追加
4. EEZ StudioでUI変更（チャート表示など）

#### 3. IoTダッシュボード

**変更点**:
- RSS URL → センサーデータAPI URL
- ニュース表示 → センサー値・グラフ

**実装手順**:
1. プロジェクトをコピー
2. `fetchYahooNews()`を`fetchSensorData()`に改造
3. 複数センサー対応（温度、湿度、CO2等）
4. EEZ Studioでグラフウィジェット追加

### テンプレート化の手順

#### ステップ1: プロジェクトのコピー

```bash
cd /path/to/Tab5_GUI
cp -r EEZ_withFlow03_YahooNews MyNewApp
cd MyNewApp
```

#### ステップ2: プロジェクト名の変更

**ファイル名変更**:
```bash
mv EEZ_withFlow03_YahooNews.ino MyNewApp.ino
```

**コード内のプロジェクト名変更**:
```cpp
// ファイルヘッダー
// M5Stack Tab5 EEZ-Studio Yahoo News Viewer
// ↓
// M5Stack Tab5 EEZ-Studio MyNewApp
```

#### ステップ3: API部分の書き換え

**変更対象**:
- `fetchYahooNews()` → 新しいAPI用に書き換え
- `createNewsList()` → 新しいデータ整形用に書き換え
- `extractTag()` → JSON解析用に変更（必要に応じて）

**例（JSON API対応）**:
```cpp
#include <ArduinoJson.h>

String fetchApiData()
{
    HTTPClient http;
    http.begin("https://api.example.com/data");
    
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        
        // JSON解析
        StaticJsonDocument<4096> doc;
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error) {
            // データを整形
            String result = "";
            JsonArray items = doc["items"];
            for (JsonObject item : items) {
                result += item["name"].as<String>() + "\n";
            }
            return result;
        }
    }
    
    http.end();
    return "Error";
}
```

#### ステップ4: secrets.hの更新

**新しいAPI情報を追加**:
```cpp
// WiFi接続情報
const char* ssid = "your_ssid";
const char* password = "your_password";

// API情報（追加）
const char* api_key = "your_api_key";
const char* api_endpoint = "https://api.example.com/v1";
```

#### ステップ5: UIのカスタマイズ

1. EEZ Studioで新規プロジェクト作成またはインポート
2. レイアウト・色・フォントを変更
3. 必要に応じてウィジェット追加
4. Flow変数・アクション定義
5. Build & Export

#### ステップ6: テスト

1. コンパイル
   ```bash
   arduino-cli compile -b m5stack:esp32:m5stack_tab5
   ```

2. アップロード
   ```bash
   arduino-cli upload -b m5stack:esp32:m5stack_tab5 -p /dev/cu.usbserial-*
   ```

3. 動作確認
   - シリアルモニターでログ確認
   - 画面表示確認
   - エラーハンドリング確認

---

## トラブルシューティング

### EEZ Studio関連

**問題**: Buildボタンをクリックしてもファイルが生成されない

**対策**:
- エラーメッセージを確認
- すべてのウィジェットに適切な名前が設定されているか確認
- Flow変数の型が正しいか確認

**問題**: `objects.textarea_news`が未定義

**対策**:
- EEZ Studioで`textarea_news`の名前が正しいか確認
- 生成された`screens.h`を確認
- ウィジェットが「User Widget」としてマークされているか確認

### コンパイル関連

**問題**: JSON解析でエラー

**対策**:
```bash
# ArduinoJsonライブラリをインストール
arduino-cli lib install "ArduinoJson@6.21.3"
```

**問題**: メモリ不足

**対策**:
- ニュース取得数を減らす（MAX_NEWS_ITEMS）
- 不要な機能を削除
- SPIRAMを活用（大きなバッファはheap_caps_malloc使用）

---

**作成日**: 2026年1月12日  
**最終更新**: 2026年1月12日  
**対象バージョン**: v1.0

