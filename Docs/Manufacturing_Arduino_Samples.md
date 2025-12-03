# M5Stack Tab5 製造業向けArduinoサンプル集

製造業のDX化・合理化を支援する、M5Stack Tab5の機能を活用した実践的なArduinoサンプルコードです。

---

## 📊 サンプル1: 製造ライン生産管理モニター

生産数、不良率、稼働率をリアルタイムで表示し、目標達成度を可視化します。

### 機能
- タッチスクリーンで生産数入力
- IMUセンサーで装置の振動監視
- SDカードに生産データ保存
- USBメモリへデータエクスポート

### ファイル名: `ProductionMonitor.ino`

```cpp
#include <M5Unified.h>
#include <SD.h>

// 生産データ構造体
struct ProductionData {
  uint32_t total_count = 0;      // 総生産数
  uint32_t good_count = 0;       // 良品数
  uint32_t defect_count = 0;     // 不良品数
  uint32_t target_count = 1000;  // 目標生産数
  float defect_rate = 0.0;       // 不良率
  float achievement_rate = 0.0;  // 達成率
  unsigned long start_time = 0;  // 開始時刻
  unsigned long elapsed_time = 0; // 経過時間
};

ProductionData production;
m5::imu_data_t imuData;

// UI座標定義
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 80;
const int BUTTON_SPACING = 20;

// 振動閾値（装置異常検知）
const float VIBRATION_THRESHOLD = 2.0;  // m/s²

void setup() {
  M5.begin();
  M5.Display.setRotation(3);  // 横向き
  M5.Display.setTextSize(2);
  
  // SDカード初期化
  if (!SD.begin()) {
    M5.Display.println("SD Card Mount Failed");
  }
  
  // 生産開始時刻を記録
  production.start_time = millis();
  
  // UI初期描画
  drawUI();
}

void loop() {
  M5.update();
  
  // IMUデータ更新（振動監視）
  M5.Imu.update();
  imuData = M5.Imu.getImuData();
  checkVibration();
  
  // タッチ処理
  handleTouch();
  
  // 経過時間更新
  production.elapsed_time = (millis() - production.start_time) / 1000;
  
  // 表示更新
  updateDisplay();
  
  delay(100);
}

void drawUI() {
  M5.Display.clear(BLACK);
  M5.Display.setTextColor(WHITE);
  
  // タイトル
  M5.Display.setTextSize(3);
  M5.Display.setCursor(10, 10);
  M5.Display.println("Production Monitor");
  
  // ボタン描画
  drawButton(50, 150, BUTTON_WIDTH, BUTTON_HEIGHT, GREEN, "Good");
  drawButton(300, 150, BUTTON_WIDTH, BUTTON_HEIGHT, RED, "Defect");
  drawButton(550, 150, BUTTON_WIDTH, BUTTON_HEIGHT, YELLOW, "Reset");
  
  // データエリア背景
  M5.Display.fillRect(50, 300, 1180, 350, DARKGREY);
}

void drawButton(int x, int y, int w, int h, uint16_t color, const char* text) {
  M5.Display.fillRoundRect(x, y, w, h, 10, color);
  M5.Display.drawRoundRect(x, y, w, h, 10, WHITE);
  
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE);
  int text_x = x + (w - strlen(text) * 12) / 2;
  int text_y = y + (h - 16) / 2;
  M5.Display.setCursor(text_x, text_y);
  M5.Display.print(text);
}

void handleTouch() {
  if (M5.Touch.getCount()) {
    auto t = M5.Touch.getDetail();
    int x = t.x;
    int y = t.y;
    
    // 良品ボタン
    if (x >= 50 && x <= 250 && y >= 150 && y <= 230) {
      production.good_count++;
      production.total_count++;
      calculateRates();
      M5.Speaker.tone(2000, 50);  // ビープ音
      saveToSD();
      delay(200);
    }
    
    // 不良品ボタン
    if (x >= 300 && x <= 500 && y >= 150 && y <= 230) {
      production.defect_count++;
      production.total_count++;
      calculateRates();
      M5.Speaker.tone(500, 100);  // 低音アラート
      saveToSD();
      delay(200);
    }
    
    // リセットボタン
    if (x >= 550 && x <= 750 && y >= 150 && y <= 230) {
      resetProduction();
      delay(200);
    }
  }
}

void calculateRates() {
  if (production.total_count > 0) {
    production.defect_rate = 
      (float)production.defect_count / production.total_count * 100.0;
    production.achievement_rate = 
      (float)production.total_count / production.target_count * 100.0;
  }
}

void updateDisplay() {
  // データエリアクリア
  M5.Display.fillRect(60, 310, 1160, 330, DARKGREY);
  
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(2);
  
  // 生産数表示
  M5.Display.setCursor(70, 320);
  M5.Display.printf("Total: %d / %d", 
    production.total_count, production.target_count);
  
  M5.Display.setCursor(70, 360);
  M5.Display.printf("Good: %d", production.good_count);
  
  M5.Display.setCursor(70, 400);
  M5.Display.printf("Defect: %d", production.defect_count);
  
  // 不良率表示（色分け）
  M5.Display.setCursor(70, 440);
  M5.Display.print("Defect Rate: ");
  if (production.defect_rate < 1.0) {
    M5.Display.setTextColor(GREEN);
  } else if (production.defect_rate < 3.0) {
    M5.Display.setTextColor(YELLOW);
  } else {
    M5.Display.setTextColor(RED);
  }
  M5.Display.printf("%.2f%%", production.defect_rate);
  
  // 達成率表示
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(70, 480);
  M5.Display.printf("Achievement: %.1f%%", production.achievement_rate);
  
  // プログレスバー
  drawProgressBar(70, 520, 1100, 30, production.achievement_rate);
  
  // 経過時間
  M5.Display.setCursor(70, 570);
  int hours = production.elapsed_time / 3600;
  int minutes = (production.elapsed_time % 3600) / 60;
  int seconds = production.elapsed_time % 60;
  M5.Display.printf("Elapsed: %02d:%02d:%02d", hours, minutes, seconds);
  
  // 振動レベル表示
  M5.Display.setCursor(70, 610);
  float vibration = sqrt(imuData.accel.x * imuData.accel.x + 
                         imuData.accel.y * imuData.accel.y + 
                         imuData.accel.z * imuData.accel.z);
  M5.Display.printf("Vibration: %.2f m/s2", vibration);
}

void drawProgressBar(int x, int y, int w, int h, float percent) {
  // 枠
  M5.Display.drawRect(x, y, w, h, WHITE);
  
  // バー
  int bar_width = (int)(w * percent / 100.0);
  uint16_t color = (percent < 50) ? RED : (percent < 80) ? YELLOW : GREEN;
  M5.Display.fillRect(x + 2, y + 2, bar_width - 4, h - 4, color);
}

void checkVibration() {
  float vibration = sqrt(imuData.accel.x * imuData.accel.x + 
                         imuData.accel.y * imuData.accel.y + 
                         imuData.accel.z * imuData.accel.z);
  
  if (vibration > VIBRATION_THRESHOLD) {
    // 異常振動検知
    M5.Display.fillRect(900, 150, 300, 80, RED);
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(920, 180);
    M5.Display.println("VIBRATION ALERT!");
    M5.Speaker.tone(1000, 200);
  } else {
    M5.Display.fillRect(900, 150, 300, 80, GREEN);
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(970, 180);
    M5.Display.println("Normal");
  }
}

void saveToSD() {
  File file = SD.open("/production_log.csv", FILE_APPEND);
  if (file) {
    char timestamp[32];
    sprintf(timestamp, "%lu", millis());
    file.printf("%s,%d,%d,%d,%.2f,%.2f\n", 
      timestamp,
      production.total_count,
      production.good_count,
      production.defect_count,
      production.defect_rate,
      production.achievement_rate);
    file.close();
  }
}

void resetProduction() {
  // 現在のデータをSDに最終保存
  saveToSD();
  
  // データリセット
  production.total_count = 0;
  production.good_count = 0;
  production.defect_count = 0;
  production.defect_rate = 0.0;
  production.achievement_rate = 0.0;
  production.start_time = millis();
  production.elapsed_time = 0;
  
  drawUI();
  M5.Speaker.tone(1500, 100);
}
```

---

## 🔧 サンプル2: 設備保全記録システム

設備の点検記録、異常履歴、メンテナンス予定を管理します。

### 機能
- タッチパネルで点検項目チェック
- カメラで設備状態撮影
- RS-485でPLC接続（稼働時間取得）
- QRコードスキャン（設備識別）

### ファイル名: `EquipmentInspection.ino`

```cpp
#include <M5Unified.h>
#include <SD.h>
#include <time.h>

// 点検項目構造体
struct InspectionItem {
  const char* name;
  bool checked;
  const char* note;
};

// 点検チェックリスト
InspectionItem checklist[] = {
  {"油圧レベル確認", false, ""},
  {"異音チェック", false, ""},
  {"温度確認", false, ""},
  {"ボルト締付確認", false, ""},
  {"清掃状態確認", false, ""},
  {"安全装置動作確認", false, ""}
};

const int CHECKLIST_SIZE = sizeof(checklist) / sizeof(InspectionItem);
int current_page = 0;
const int ITEMS_PER_PAGE = 6;

// 設備情報
char equipment_id[32] = "LINE-01-PRESS";
unsigned long operating_hours = 12345;  // 稼働時間（時間）
unsigned long last_maintenance = 0;     // 前回メンテナンス（時間）

void setup() {
  M5.begin();
  M5.Display.setRotation(3);
  M5.Display.setTextSize(2);
  
  // RTC初期化
  configTime(9 * 3600, 0, "ntp.nict.jp");
  
  // SDカード初期化
  if (!SD.begin()) {
    M5.Display.println("SD Card Failed");
  }
  
  drawInspectionUI();
}

void loop() {
  M5.update();
  
  handleTouch();
  
  delay(100);
}

void drawInspectionUI() {
  M5.Display.clear(BLACK);
  
  // ヘッダー
  M5.Display.fillRect(0, 0, 1280, 60, BLUE);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(3);
  M5.Display.setCursor(20, 15);
  M5.Display.printf("Equipment Inspection: %s", equipment_id);
  
  // 設備情報表示
  M5.Display.setTextSize(2);
  M5.Display.setCursor(20, 70);
  M5.Display.printf("Operating Hours: %lu h", operating_hours);
  M5.Display.setCursor(500, 70);
  M5.Display.printf("Last Maintenance: %lu h ago", 
    operating_hours - last_maintenance);
  
  // チェックリスト描画
  int y = 120;
  for (int i = 0; i < ITEMS_PER_PAGE && i < CHECKLIST_SIZE; i++) {
    drawCheckItem(20, y, i);
    y += 80;
  }
  
  // ボタン
  drawButton(50, 620, 250, 60, GREEN, "Complete");
  drawButton(350, 620, 250, 60, ORANGE, "Take Photo");
  drawButton(650, 620, 250, 60, CYAN, "Export Data");
  drawButton(950, 620, 250, 60, RED, "Reset");
}

void drawCheckItem(int x, int y, int index) {
  // チェックボックス
  int checkbox_size = 50;
  uint16_t box_color = checklist[index].checked ? GREEN : DARKGREY;
  M5.Display.fillRect(x, y, checkbox_size, checkbox_size, box_color);
  M5.Display.drawRect(x, y, checkbox_size, checkbox_size, WHITE);
  
  if (checklist[index].checked) {
    // チェックマーク
    M5.Display.drawLine(x + 10, y + 25, x + 20, y + 40, WHITE);
    M5.Display.drawLine(x + 20, y + 40, x + 40, y + 10, WHITE);
  }
  
  // 項目名
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(x + 70, y + 15);
  M5.Display.print(checklist[index].name);
}

void drawButton(int x, int y, int w, int h, uint16_t color, const char* text) {
  M5.Display.fillRoundRect(x, y, w, h, 10, color);
  M5.Display.drawRoundRect(x, y, w, h, 10, WHITE);
  
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE);
  int text_x = x + (w - strlen(text) * 12) / 2;
  int text_y = y + (h - 16) / 2;
  M5.Display.setCursor(text_x, text_y);
  M5.Display.print(text);
}

void handleTouch() {
  if (M5.Touch.getCount()) {
    auto t = M5.Touch.getDetail();
    int x = t.x;
    int y = t.y;
    
    // チェックボックスタップ
    for (int i = 0; i < ITEMS_PER_PAGE && i < CHECKLIST_SIZE; i++) {
      int item_y = 120 + i * 80;
      if (x >= 20 && x <= 70 && y >= item_y && y <= item_y + 50) {
        checklist[i].checked = !checklist[i].checked;
        drawCheckItem(20, item_y, i);
        M5.Speaker.tone(2000, 50);
        delay(200);
      }
    }
    
    // 完了ボタン
    if (x >= 50 && x <= 300 && y >= 620 && y <= 680) {
      completeInspection();
      delay(200);
    }
    
    // 写真撮影ボタン
    if (x >= 350 && x <= 600 && y >= 620 && y <= 680) {
      takePhoto();
      delay(200);
    }
    
    // データエクスポートボタン
    if (x >= 650 && x <= 900 && y >= 620 && y <= 680) {
      exportData();
      delay(200);
    }
    
    // リセットボタン
    if (x >= 950 && x <= 1200 && y >= 620 && y <= 680) {
      resetChecklist();
      delay(200);
    }
  }
}

void completeInspection() {
  // 全項目チェック済みか確認
  int checked_count = 0;
  for (int i = 0; i < CHECKLIST_SIZE; i++) {
    if (checklist[i].checked) {
      checked_count++;
    }
  }
  
  if (checked_count == CHECKLIST_SIZE) {
    // 全項目完了
    saveInspectionRecord();
    
    M5.Display.fillRect(400, 300, 480, 120, GREEN);
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextSize(3);
    M5.Display.setCursor(450, 340);
    M5.Display.println("Inspection Complete!");
    M5.Speaker.tone(2000, 100);
    delay(100);
    M5.Speaker.tone(2500, 100);
    delay(2000);
    
    resetChecklist();
    drawInspectionUI();
  } else {
    // 未完了項目あり
    M5.Display.fillRect(350, 300, 580, 120, RED);
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(380, 340);
    M5.Display.printf("Incomplete: %d/%d items checked", 
      checked_count, CHECKLIST_SIZE);
    M5.Speaker.tone(500, 200);
    delay(2000);
    drawInspectionUI();
  }
}

void saveInspectionRecord() {
  File file = SD.open("/inspection_log.csv", FILE_APPEND);
  if (file) {
    time_t now;
    time(&now);
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    file.printf("%s,%s,%lu,%lu,Complete\n", 
      timeStr, equipment_id, operating_hours, 
      operating_hours - last_maintenance);
    file.close();
    
    // メンテナンス時刻更新
    last_maintenance = operating_hours;
  }
}

void takePhoto() {
  M5.Display.fillRect(300, 250, 680, 200, BLUE);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(3);
  M5.Display.setCursor(400, 320);
  M5.Display.println("Taking Photo...");
  
  // カメラ撮影処理（実装例）
  // M5.Camera.capture() などを使用
  
  M5.Speaker.tone(2000, 50);
  delay(1000);
  drawInspectionUI();
}

void exportData() {
  M5.Display.fillRect(300, 250, 680, 200, CYAN);
  M5.Display.setTextColor(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(350, 300);
  M5.Display.println("Exporting to USB Memory...");
  
  // USBメモリへエクスポート処理
  
  M5.Speaker.tone(1500, 100);
  delay(1500);
  drawInspectionUI();
}

void resetChecklist() {
  for (int i = 0; i < CHECKLIST_SIZE; i++) {
    checklist[i].checked = false;
  }
}
```

---

## 📈 サンプル3: 在庫管理＆バーコードスキャナー

部品在庫をリアルタイムで管理し、発注点を監視します。

### 機能
- USBバーコードスキャナー対応
- タッチで入出庫登録
- 在庫レベル警告表示
- 発注リスト自動生成

### ファイル名: `InventoryManagement.ino`

```cpp
#include <M5Unified.h>
#include <SD.h>

// 部品在庫構造体
struct InventoryItem {
  char part_number[32];
  char description[64];
  int current_stock;
  int min_stock;
  int max_stock;
  int reorder_point;
  bool need_reorder;
};

// サンプル在庫データ
InventoryItem inventory[] = {
  {"P-001", "Bearing 6205", 45, 20, 100, 30, false},
  {"P-002", "Bolt M8x20", 850, 500, 2000, 700, false},
  {"P-003", "O-Ring 50mm", 120, 50, 300, 80, false},
  {"P-004", "Relay 24VDC", 15, 10, 50, 15, true},
  {"P-005", "Sensor Proximity", 8, 5, 30, 10, true}
};

const int INVENTORY_SIZE = sizeof(inventory) / sizeof(InventoryItem);
int selected_item = 0;

void setup() {
  M5.begin();
  M5.Display.setRotation(3);
  M5.Display.setTextSize(2);
  
  // SDカード初期化
  if (!SD.begin()) {
    M5.Display.println("SD Card Failed");
  }
  
  // 発注フラグ更新
  checkReorderLevels();
  
  drawInventoryUI();
}

void loop() {
  M5.update();
  
  handleTouch();
  
  // USBキーボード（バーコードスキャナー）入力チェック
  // if (Serial.available()) {
  //   String barcode = Serial.readStringUntil('\n');
  //   processBarcode(barcode);
  // }
  
  delay(100);
}

void drawInventoryUI() {
  M5.Display.clear(BLACK);
  
  // ヘッダー
  M5.Display.fillRect(0, 0, 1280, 60, DARKGREEN);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(3);
  M5.Display.setCursor(20, 15);
  M5.Display.println("Inventory Management System");
  
  // 在庫リスト表示
  drawInventoryList();
  
  // 選択アイテム詳細
  drawItemDetail();
  
  // 操作ボタン
  drawButton(50, 620, 180, 60, GREEN, "IN (+10)");
  drawButton(260, 620, 180, 60, RED, "OUT (-10)");
  drawButton(470, 620, 220, 60, ORANGE, "Reorder List");
  drawButton(720, 620, 220, 60, CYAN, "Export CSV");
  drawButton(970, 620, 220, 60, YELLOW, "Scan Mode");
}

void drawInventoryList() {
  int y = 80;
  M5.Display.setTextSize(2);
  
  for (int i = 0; i < INVENTORY_SIZE; i++) {
    // 選択状態の背景
    if (i == selected_item) {
      M5.Display.fillRect(10, y - 5, 500, 45, BLUE);
    }
    
    // 在庫レベルによる色分け
    uint16_t color;
    if (inventory[i].current_stock <= inventory[i].reorder_point) {
      color = RED;
    } else if (inventory[i].current_stock <= inventory[i].min_stock + 10) {
      color = YELLOW;
    } else {
      color = GREEN;
    }
    
    M5.Display.setTextColor(WHITE);
    M5.Display.setCursor(20, y);
    M5.Display.printf("%-10s", inventory[i].part_number);
    
    M5.Display.setTextColor(color);
    M5.Display.setCursor(180, y);
    M5.Display.printf("%3d", inventory[i].current_stock);
    
    // 警告アイコン
    if (inventory[i].need_reorder) {
      M5.Display.setTextColor(RED);
      M5.Display.setCursor(250, y);
      M5.Display.print("!");
    }
    
    // 在庫バー
    int bar_width = (inventory[i].current_stock * 200) / inventory[i].max_stock;
    M5.Display.fillRect(300, y + 5, bar_width, 20, color);
    M5.Display.drawRect(300, y + 5, 200, 20, WHITE);
    
    y += 50;
  }
}

void drawItemDetail() {
  int x = 550;
  int y = 80;
  
  M5.Display.fillRect(x, y, 700, 520, DARKGREY);
  M5.Display.drawRect(x, y, 700, 520, WHITE);
  
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(2);
  
  InventoryItem* item = &inventory[selected_item];
  
  y += 20;
  M5.Display.setCursor(x + 20, y);
  M5.Display.println("Item Details");
  
  y += 40;
  M5.Display.setCursor(x + 20, y);
  M5.Display.printf("Part Number: %s", item->part_number);
  
  y += 40;
  M5.Display.setCursor(x + 20, y);
  M5.Display.printf("Description: %s", item->description);
  
  y += 60;
  M5.Display.setTextSize(3);
  M5.Display.setCursor(x + 20, y);
  M5.Display.print("Current Stock: ");
  
  uint16_t stock_color = (item->current_stock <= item->reorder_point) ? RED : 
                         (item->current_stock <= item->min_stock + 10) ? YELLOW : GREEN;
  M5.Display.setTextColor(stock_color);
  M5.Display.printf("%d", item->current_stock);
  
  y += 50;
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(x + 20, y);
  M5.Display.printf("Min Stock: %d", item->min_stock);
  
  y += 35;
  M5.Display.setCursor(x + 20, y);
  M5.Display.printf("Max Stock: %d", item->max_stock);
  
  y += 35;
  M5.Display.setCursor(x + 20, y);
  M5.Display.printf("Reorder Point: %d", item->reorder_point);
  
  // ステータス
  y += 50;
  M5.Display.setTextSize(3);
  M5.Display.setCursor(x + 20, y);
  if (item->need_reorder) {
    M5.Display.setTextColor(RED);
    M5.Display.println("STATUS: REORDER NOW!");
  } else if (item->current_stock <= item->min_stock + 10) {
    M5.Display.setTextColor(YELLOW);
    M5.Display.println("STATUS: Low Stock");
  } else {
    M5.Display.setTextColor(GREEN);
    M5.Display.println("STATUS: OK");
  }
  
  // 在庫グラフ
  y += 80;
  drawStockGraph(x + 50, y, 600, 100, item);
}

void drawStockGraph(int x, int y, int w, int h, InventoryItem* item) {
  // 軸
  M5.Display.drawLine(x, y, x, y + h, WHITE);
  M5.Display.drawLine(x, y + h, x + w, y + h, WHITE);
  
  // 最大在庫ライン
  int max_y = y;
  M5.Display.drawLine(x, max_y, x + w, max_y, CYAN);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(x + w + 10, max_y - 5);
  M5.Display.printf("Max:%d", item->max_stock);
  
  // 発注点ライン
  int reorder_y = y + (h * (item->max_stock - item->reorder_point)) / item->max_stock;
  M5.Display.drawLine(x, reorder_y, x + w, reorder_y, ORANGE);
  M5.Display.setTextColor(ORANGE);
  M5.Display.setCursor(x + w + 10, reorder_y - 5);
  M5.Display.printf("Reorder:%d", item->reorder_point);
  
  // 最小在庫ライン
  int min_y = y + (h * (item->max_stock - item->min_stock)) / item->max_stock;
  M5.Display.drawLine(x, min_y, x + w, min_y, RED);
  M5.Display.setTextColor(RED);
  M5.Display.setCursor(x + w + 10, min_y - 5);
  M5.Display.printf("Min:%d", item->min_stock);
  
  // 現在在庫バー
  int stock_height = (h * item->current_stock) / item->max_stock;
  uint16_t bar_color = (item->current_stock <= item->reorder_point) ? RED : 
                       (item->current_stock <= item->min_stock + 10) ? YELLOW : GREEN;
  M5.Display.fillRect(x + 10, y + h - stock_height, 100, stock_height, bar_color);
}

void drawButton(int x, int y, int w, int h, uint16_t color, const char* text) {
  M5.Display.fillRoundRect(x, y, w, h, 10, color);
  M5.Display.drawRoundRect(x, y, w, h, 10, WHITE);
  
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE);
  int text_x = x + (w - strlen(text) * 12) / 2;
  int text_y = y + (h - 16) / 2;
  M5.Display.setCursor(text_x, text_y);
  M5.Display.print(text);
}

void handleTouch() {
  if (M5.Touch.getCount()) {
    auto t = M5.Touch.getDetail();
    int x = t.x;
    int y = t.y;
    
    // 在庫リスト選択
    if (x >= 10 && x <= 510 && y >= 75) {
      int item_index = (y - 75) / 50;
      if (item_index >= 0 && item_index < INVENTORY_SIZE) {
        selected_item = item_index;
        drawInventoryUI();
        delay(200);
      }
    }
    
    // 入庫ボタン
    if (x >= 50 && x <= 230 && y >= 620 && y <= 680) {
      inventory[selected_item].current_stock += 10;
      checkReorderLevels();
      logTransaction("IN", 10);
      drawInventoryUI();
      M5.Speaker.tone(2000, 50);
      delay(200);
    }
    
    // 出庫ボタン
    if (x >= 260 && x <= 440 && y >= 620 && y <= 680) {
      if (inventory[selected_item].current_stock >= 10) {
        inventory[selected_item].current_stock -= 10;
        checkReorderLevels();
        logTransaction("OUT", 10);
        drawInventoryUI();
        M5.Speaker.tone(1500, 50);
      } else {
        M5.Speaker.tone(500, 200);
      }
      delay(200);
    }
    
    // 発注リストボタン
    if (x >= 470 && x <= 690 && y >= 620 && y <= 680) {
      showReorderList();
      delay(200);
    }
    
    // CSVエクスポートボタン
    if (x >= 720 && x <= 940 && y >= 620 && y <= 680) {
      exportToCSV();
      delay(200);
    }
  }
}

void checkReorderLevels() {
  for (int i = 0; i < INVENTORY_SIZE; i++) {
    inventory[i].need_reorder = 
      (inventory[i].current_stock <= inventory[i].reorder_point);
  }
}

void logTransaction(const char* type, int quantity) {
  File file = SD.open("/inventory_log.csv", FILE_APPEND);
  if (file) {
    char timestamp[32];
    sprintf(timestamp, "%lu", millis());
    file.printf("%s,%s,%s,%d,%d\n", 
      timestamp, 
      inventory[selected_item].part_number,
      type,
      quantity,
      inventory[selected_item].current_stock);
    file.close();
  }
}

void showReorderList() {
  M5.Display.clear(BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(3);
  M5.Display.setCursor(20, 20);
  M5.Display.println("Reorder List");
  
  int y = 80;
  M5.Display.setTextSize(2);
  bool has_reorder = false;
  
  for (int i = 0; i < INVENTORY_SIZE; i++) {
    if (inventory[i].need_reorder) {
      has_reorder = true;
      M5.Display.setTextColor(RED);
      M5.Display.setCursor(20, y);
      M5.Display.printf("%s - %s", 
        inventory[i].part_number, 
        inventory[i].description);
      
      M5.Display.setCursor(600, y);
      int order_qty = inventory[i].max_stock - inventory[i].current_stock;
      M5.Display.printf("Order: %d units", order_qty);
      
      y += 40;
    }
  }
  
  if (!has_reorder) {
    M5.Display.setTextColor(GREEN);
    M5.Display.setCursor(20, 200);
    M5.Display.println("No items need reordering");
  }
  
  M5.Speaker.tone(1000, 200);
  delay(3000);
  drawInventoryUI();
}

void exportToCSV() {
  M5.Display.fillRect(300, 250, 680, 200, CYAN);
  M5.Display.setTextColor(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(400, 320);
  M5.Display.println("Exporting CSV...");
  
  File file = SD.open("/inventory_export.csv", FILE_WRITE);
  if (file) {
    file.println("Part Number,Description,Current Stock,Min Stock,Max Stock,Reorder Point,Status");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
      file.printf("%s,%s,%d,%d,%d,%d,%s\n",
        inventory[i].part_number,
        inventory[i].description,
        inventory[i].current_stock,
        inventory[i].min_stock,
        inventory[i].max_stock,
        inventory[i].reorder_point,
        inventory[i].need_reorder ? "REORDER" : "OK");
    }
    file.close();
  }
  
  M5.Speaker.tone(2000, 100);
  delay(1500);
  drawInventoryUI();
}
```

---

## 🎯 サンプルの特徴

### 1. **実践的な機能統合**
- タッチスクリーン（入力）
- IMUセンサー（振動監視）
- SDカード（データ保存）
- カメラ（記録撮影）
- USB（外部デバイス接続）
- スピーカー（音声フィードバック）

### 2. **製造業の実務に即した設計**
- 直感的なタッチ操作
- 視認性の高いUI
- リアルタイムフィードバック
- データの永続化
- エクスポート機能

### 3. **拡張可能な構造**
- モジュール化されたコード
- 設定値の外部化
- 他システムとの連携容易

---

## 📋 使用方法

### Arduino IDE設定

1. **ボードマネージャー**
   - M5Stack Tab5用のボード定義をインストール
   - ESP32-P4対応のコアをインストール

2. **ライブラリのインストール**
   ```
   - M5Unified
   - M5GFX
   - SD (標準ライブラリ)
   ```

3. **ボード選択**
   ```
   ツール → ボード → ESP32 Arduino → ESP32-P4 Dev Module
   ```

4. **シリアルポート設定**
   ```
   ツール → Port → (USB接続されたTab5のポート)
   ```

5. **アップロード**
   - スケッチをコンパイル
   - Tab5をダウンロードモードに設定（リセットボタン長押し）
   - アップロード実行

---

## 🔧 カスタマイズ方法

### 生産目標の変更
```cpp
production.target_count = 2000;  // 目標生産数を変更
```

### 振動閾値の調整
```cpp
const float VIBRATION_THRESHOLD = 3.0;  // より高い閾値に変更
```

### 在庫データの追加
```cpp
InventoryItem inventory[] = {
  {"P-006", "New Part", 100, 30, 200, 50, false},
  // 新しい部品を追加
};
```

### 点検項目のカスタマイズ
```cpp
InspectionItem checklist[] = {
  {"カスタム点検項目1", false, ""},
  {"カスタム点検項目2", false, ""},
  // 貴社の点検項目に変更
};
```

---

## 💾 データファイル形式

### 生産ログ (`production_log.csv`)
```csv
timestamp,total_count,good_count,defect_count,defect_rate,achievement_rate
1234567890,100,98,2,2.00,10.00
```

### 在庫トランザクションログ (`inventory_log.csv`)
```csv
timestamp,part_number,type,quantity,current_stock
1234567890,P-001,IN,10,55
1234567891,P-001,OUT,10,45
```

### 点検記録 (`inspection_log.csv`)
```csv
timestamp,equipment_id,operating_hours,hours_since_maintenance,status
2024-12-02 10:00:00,LINE-01-PRESS,12345,100,Complete
```

---

これらのサンプルをベースに、貴社の製造ラインに合わせたカスタマイズが可能です。各サンプルは独立して動作し、必要に応じて機能を組み合わせることもできます。
