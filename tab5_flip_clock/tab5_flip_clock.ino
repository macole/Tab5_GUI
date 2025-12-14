/*
 * Tab5 フリップ時計 / ニキシー管時計
 * M5Stack Tab5用のデジタル時計プログラム
 * 
 * 機能：
 * - RTC（リアルタイムクロック）から時刻を取得
 * - パタパタ時計またはニキシー管風の時計を表示
 * - 日付と時刻を表示
 * 
 * 参考：https://github.com/gijin77/M5Stack-Tab5-Nixie-Flip-Clock
 */

// ============================================================================
// ライブラリのインクルード
// ============================================================================
#include "Arduino.h"
#include <M5Unified.h>
#include "img_nixie_tube.h"  // ニキシー管の画像データ
#include "img_flip.h"        // パタパタ時計の画像データ
#include <WiFi.h>
#include <time.h>
#include <ArtronShop_RX8130CE.h>  // RTC（リアルタイムクロック）ライブラリ
#include <Wire.h>

// ============================================================================
// 定数の定義
// ============================================================================
// 時計の表示モード
#define CLOCK_MODE_FLIP    1  // パタパタ時計モード
#define CLOCK_MODE_NIXIE   0  // ニキシー管モード

// ハードウェア設定
#define PIN_BACKLIGHT      22  // バックライト制御用GPIO番号
#define PIN_I2C_SDA        31  // I2C SDAピン
#define PIN_I2C_SCL        32  // I2C SCLピン
#define DISPLAY_ROTATION   3   // ディスプレイの回転角度（0-3）

// タイミング設定
#define UPDATE_INTERVAL_MS     1000  // 時計の更新間隔（ミリ秒）
#define FLIP_ANIMATION_DELAY   275   // パタパタアニメーションの遅延時間（ミリ秒）
#define NIXIE_DISPLAY_DELAY    300   // ニキシー管表示の遅延時間（ミリ秒）
#define STARTUP_DISPLAY_TIME   5000  // 起動時の表示時間（ミリ秒）

// 画像サイズ（パタパタ時計の元画像サイズ）
#define FLIP_IMAGE_WIDTH   60
#define FLIP_IMAGE_HEIGHT  140

// パタパタ時計の表示サイズ（画面サイズに基づいて自動計算されます）
// Tab5の画面サイズ（1280x720）をベースに計算
// 画面幅の1/6を各数字の幅とし、高さは画面の70%を使用

// 画像サイズ（ニキシー管）
#define NIXIE_IMAGE_WIDTH  70
#define NIXIE_IMAGE_HEIGHT 134

// 表示位置の設定
#define COLON_OFFSET_Y      80   // コロン（:）のY軸オフセット
#define COLON_DIAMETER      5    // コロンの直径
#define DATE_DISPLAY_X      10   // 日付表示のX座標
#define DATE_DISPLAY_Y      10   // 日付表示のY座標
#define DATE_DISPLAY_HEIGHT 32   // 日付表示エリアの高さ

// タイムゾーン設定
#define JST_OFFSET_HOURS    9    // 日本標準時（JST）はUTC+9時間

// 曜日の文字列
const char WEEKDAY_NAMES[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// ============================================================================
// グローバル変数の宣言
// ============================================================================
// RTC（リアルタイムクロック）オブジェクト
ArtronShop_RX8130CE rtc(&Wire);

// 時計の設定
int clockMode = CLOCK_MODE_NIXIE; //CLOCL_MODE_FLIP,CLOCK_MODE_NIXIE;
uint16_t displayColor = GREEN;   // 表示色（コロンなど）

// 現在の日時情報
struct DateTime {
  int year;
  int month;
  int day;
  int weekday;
  int hour;
  int minute;
  int second;
} currentTime;

// 前回表示した日時の各桁（変更検出用）
struct PreviousDigits {
  int hour_tens;      // 時の10の位
  int hour_ones;      // 時の1の位
  int minute_tens;    // 分の10の位
  int minute_ones;    // 分の1の位
  int second_tens;    // 秒の10の位
  int second_ones;    // 秒の1の位
} previousDigits;

// 画面サイズ情報
struct DisplayInfo {
  int32_t width;      // 画面の幅
  int32_t height;     // 画面の高さ
  int32_t centerX;    // 画面の中心X座標
  int32_t centerY;    // 画面の中心Y座標
  int32_t flipDigitWidth;   // パタパタ時計の数字の表示幅（画面サイズに基づいて計算）
  int32_t flipDigitHeight;  // パタパタ時計の数字の表示高さ（画面サイズに基づいて計算）
} displayInfo;

// 前回更新した日（日付表示の更新判定用）
int previousDay = 0;

// 前回の更新時刻（ミリ秒）
unsigned long lastUpdateTime = 0;

// ============================================================================
// 関数のプロトタイプ宣言
// ============================================================================
void initializeHardware(void);
void initializeDisplay(void);
void showStartupMessage(void);
void updateDateTimeFromRTC(void);
void convertUTCtoJST(struct tm* utcTime, struct DateTime* jstTime);
void adjustDateAfterHourOverflow(struct DateTime* time);
bool isLeapYear(int year);
int getDaysInMonth(int year, int month);
int calculateWeekday(int year, int month, int day);
void updateDateDisplay(void);
void updateClockDisplay(void);
void drawClockDigit(int position, int digit);
void drawFlipClockDigit(int position, int digit);
void drawNixieClockDigit(int position, int digit);
void drawScaledBitmap(int16_t x, int16_t y, const uint16_t* bitmap, 
                      int16_t srcWidth, int16_t srcHeight, 
                      int16_t dstWidth, int16_t dstHeight);
void drawColon(bool isVisible);

// ============================================================================
// setup関数：プログラム開始時に1回だけ実行される
// ============================================================================
void setup() {
  // シリアル通信の初期化（デバッグ用）
  Serial.begin(115200);
  Serial.println("Tab5 Flip Clock / Nixie Clock - Starting...");
  
  // ハードウェアの初期化
  initializeHardware();
  
  // ディスプレイの初期化
  initializeDisplay();
  
  // 起動メッセージの表示
  showStartupMessage();
  
  // 前回の桁情報を初期化（すべて99に設定して、最初は必ず描画されるようにする）
  previousDigits.hour_tens = 99;
  previousDigits.hour_ones = 99;
  previousDigits.minute_tens = 99;
  previousDigits.minute_ones = 99;
  previousDigits.second_tens = 99;
  previousDigits.second_ones = 99;
  
  // 表示色の設定（モードに応じて）
  if (clockMode == CLOCK_MODE_FLIP) {
    displayColor = WHITE;
  } else {
    displayColor = ORANGE;
  }
  
  Serial.println("Initialization complete!");
}

// ============================================================================
// loop関数：プログラム開始後、繰り返し実行される
// ============================================================================
void loop() {
  // 日付が変わったら日付表示を更新
  if (currentTime.day != previousDay) {
    previousDay = currentTime.day;
    updateDateDisplay();
  }
  
  // 1秒ごとに時計を更新
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateTime >= UPDATE_INTERVAL_MS) {
    // RTCから時刻を取得して更新
    updateDateTimeFromRTC();
    
    // コロンを点灯
    drawColon(true);
    
    // 時計の表示を更新
    updateClockDisplay();
    
    // デバッグ情報の出力
    Serial.printf("%04d/%02d/%02d(%s) %02d:%02d:%02d\n", 
                  currentTime.year, currentTime.month, currentTime.day,
                  WEEKDAY_NAMES[currentTime.weekday],
                  currentTime.hour, currentTime.minute, currentTime.second);
    
    // ニキシー管モードの場合は少し待機
    if (clockMode == CLOCK_MODE_NIXIE) {
      delay(NIXIE_DISPLAY_DELAY);
    }
    
    // コロンを消灯
    drawColon(false);
    
    // 更新時刻を記録
    lastUpdateTime = currentMillis;
  }
}

// ============================================================================
// ハードウェア初期化関数
// ============================================================================
void initializeHardware(void) {
  // M5Stackの初期化
  auto cfg = M5.config();
  M5.begin(cfg);
  
  // I2Cバスの初期化（RTCとの通信用）
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  
  // RTCの初期化（失敗する場合はリトライ）
  Serial.print("Initializing RTC...");
  while (!rtc.begin()) {
    Serial.println(" Failed! Retrying...");
    delay(1000);
  }
  Serial.println(" OK!");
  
  // バックライトの初期化と点灯
  pinMode(PIN_BACKLIGHT, OUTPUT);
  digitalWrite(PIN_BACKLIGHT, HIGH);
}

// ============================================================================
// ディスプレイ初期化関数
// ============================================================================
void initializeDisplay(void) {
  // ディスプレイの回転設定
  M5.Display.setRotation(DISPLAY_ROTATION);
  
  // 画面を黒でクリア
  M5.Display.fillScreen(BLACK);
  
  // 画面サイズ情報を取得・計算
  displayInfo.width = M5.Display.width();
  displayInfo.height = M5.Display.height();
  displayInfo.centerX = displayInfo.width / 2;
  displayInfo.centerY = displayInfo.height / 2;
  
  // パタパタ時計の表示サイズを画面サイズに基づいて計算
  // 6桁の数字を表示するため、画面幅を6等分
  displayInfo.flipDigitWidth = displayInfo.width / 6;
  // 高さは画面の70%を使用（上下に余白を確保）
  displayInfo.flipDigitHeight = (int32_t)(displayInfo.height * 0.7);
  
  Serial.printf("Display size: %dx%d\n", displayInfo.width, displayInfo.height);
  Serial.printf("Flip clock digit size: %dx%d\n", 
                displayInfo.flipDigitWidth, displayInfo.flipDigitHeight);
}

// ============================================================================
// 起動メッセージ表示関数
// ============================================================================
void showStartupMessage(void) {
  M5.Display.setTextSize(6);
  M5.Display.setTextColor(GREEN);
  M5.Display.setCursor(16, displayInfo.centerY - 100);
  M5.Display.println("Tab5_flip_clock Start");
  M5.Display.setCursor(16, displayInfo.centerY + 100);
  M5.Display.println(" By macole");
  
  delay(STARTUP_DISPLAY_TIME);
  M5.Display.fillScreen(BLACK);
}

// ============================================================================
// RTCから日時を取得して更新する関数
// ============================================================================
void updateDateTimeFromRTC(void) {
  struct tm utcTime;
  
  // RTCからUTC時刻を取得
  if (rtc.getTime(&utcTime)) {
    // UTC時刻を日本時間（JST）に変換
    convertUTCtoJST(&utcTime, &currentTime);
    
    // 曜日を計算（RTCの曜日が不正確な場合があるため）
    currentTime.weekday = calculateWeekday(
      currentTime.year, 
      currentTime.month, 
      currentTime.day
    );
  } else {
    Serial.println("Error: Failed to get time from RTC");
  }
}

// ============================================================================
// UTC時刻を日本時間（JST）に変換する関数
// ============================================================================
void convertUTCtoJST(struct tm* utcTime, struct DateTime* jstTime) {
  // UTC時刻を日本時間に変換（+9時間）
  jstTime->hour = utcTime->tm_hour + JST_OFFSET_HOURS;
  jstTime->minute = utcTime->tm_min;
  jstTime->second = utcTime->tm_sec;
  jstTime->day = utcTime->tm_mday;
  jstTime->month = utcTime->tm_mon + 1;  // tm_monは0-11なので+1
  jstTime->year = utcTime->tm_year + 1900;  // tm_yearは1900年からの年数
  
  // 時間が24を超えた場合の日付繰り上げ処理
  if (jstTime->hour >= 24) {
    jstTime->hour -= 24;
    adjustDateAfterHourOverflow(jstTime);
  }
}

// ============================================================================
// 時間が24を超えた場合の日付繰り上げ処理関数
// ============================================================================
void adjustDateAfterHourOverflow(struct DateTime* time) {
  time->day++;
  
  // その月の日数を取得
  int daysInMonth = getDaysInMonth(time->year, time->month);
  
  // 日が月の日数を超えた場合
  if (time->day > daysInMonth) {
    time->day = 1;
    time->month++;
    
    // 月が12を超えた場合
    if (time->month > 12) {
      time->month = 1;
      time->year++;
    }
  }
}

// ============================================================================
// うるう年かどうかを判定する関数
// ============================================================================
bool isLeapYear(int year) {
  // うるう年の条件：
  // 1. 4で割り切れる
  // 2. ただし、100で割り切れる場合は400で割り切れる必要がある
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// ============================================================================
// 指定された年月の日数を取得する関数
// ============================================================================
int getDaysInMonth(int year, int month) {
  // 各月の日数
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  
  // うるう年の2月は29日
  if (month == 2 && isLeapYear(year)) {
    return 29;
  }
  
  return daysInMonth[month - 1];  // 配列は0始まりなので-1
}

// ============================================================================
// 曜日を計算する関数（ツェラーの公式を使用）
// ============================================================================
int calculateWeekday(int year, int month, int day) {
  // ツェラーの公式用に1月と2月を前年の13月、14月として扱う
  if (month < 3) {
    month += 12;
    year--;
  }
  
  int century = year / 100;      // 世紀
  int yearOfCentury = year % 100; // 世紀内の年
  
  // ツェラーの公式で曜日を計算
  // 結果：0=土曜, 1=日曜, ..., 6=金曜
  int weekday = (day + 13 * (month + 1) / 5 + yearOfCentury + 
                 yearOfCentury / 4 + century / 4 + 5 * century) % 7;
  
  // 0=日曜, 1=月曜, ..., 6=土曜に変換
  return (weekday + 6) % 7;
}

// ============================================================================
// 日付表示を更新する関数
// ============================================================================
void updateDateDisplay(void) {
  // 日付文字列を生成
  char dateString[40];
  sprintf(dateString, "%04d/%02d/%02d(%s)", 
          currentTime.year, 
          currentTime.month, 
          currentTime.day,
          WEEKDAY_NAMES[currentTime.weekday]);
  
  // 日付表示エリアをクリア
  M5.Display.fillRect(DATE_DISPLAY_X, DATE_DISPLAY_Y, 
                      displayInfo.width - 2, DATE_DISPLAY_HEIGHT, BLACK);
  
  // 日付を表示
  M5.Display.setTextColor(displayColor);
  M5.Display.setTextSize(4);
  M5.Display.setCursor(DATE_DISPLAY_X, DATE_DISPLAY_Y);
  M5.Display.println(dateString);
}

// ============================================================================
// 時計表示を更新する関数
// ============================================================================
void updateClockDisplay(void) {
  // 各桁の数字を計算
  int hour_tens = currentTime.hour / 10;
  int hour_ones = currentTime.hour % 10;
  int minute_tens = currentTime.minute / 10;
  int minute_ones = currentTime.minute % 10;
  int second_tens = currentTime.second / 10;
  int second_ones = currentTime.second % 10;
  
  // 各桁が前回と異なる場合のみ描画（効率化）
  // 位置0: 時の10の位, 位置1: 時の1の位, ..., 位置5: 秒の1の位
  drawClockDigit(5, second_ones);  // 秒の1の位は常に更新
  
  if (previousDigits.second_tens != second_tens) {
    drawClockDigit(4, second_tens);
    previousDigits.second_tens = second_tens;
  }
  
  if (previousDigits.minute_ones != minute_ones) {
    drawClockDigit(3, minute_ones);
    previousDigits.minute_ones = minute_ones;
  }
  
  if (previousDigits.minute_tens != minute_tens) {
    drawClockDigit(2, minute_tens);
    previousDigits.minute_tens = minute_tens;
  }
  
  if (previousDigits.hour_ones != hour_ones) {
    drawClockDigit(1, hour_ones);
    previousDigits.hour_ones = hour_ones;
  }
  
  if (previousDigits.hour_tens != hour_tens) {
    drawClockDigit(0, hour_tens);
    previousDigits.hour_tens = hour_tens;
  }
  
  // 秒の1の位は常に更新されるので、最後に記録
  previousDigits.second_ones = second_ones;
}

// ============================================================================
// 時計の数字を描画する関数（モードに応じて分岐）
// ============================================================================
void drawClockDigit(int position, int digit) {
  if (clockMode == CLOCK_MODE_FLIP) {
    drawFlipClockDigit(position, digit);
  } else {
    drawNixieClockDigit(position, digit);
  }
}

// ============================================================================
// パタパタ時計の数字を描画する関数
// ============================================================================
void drawFlipClockDigit(int position, int digit) {
  // 数字の表示位置を計算（画面サイズに基づいて自動計算されたサイズを使用）
  int digitSpacing = displayInfo.flipDigitWidth;   // 数字の間隔（幅）
  int digitHeight = displayInfo.flipDigitHeight;   // 数字の高さ
  int x = position * digitSpacing;
  int y = displayInfo.centerY - (digitHeight / 2);
  
  // 画像サイズ
  int imageWidth = FLIP_IMAGE_WIDTH;
  int imageHeight = FLIP_IMAGE_HEIGHT;
  
  // パタパタアニメーション用の遅延時間
  int animationDelay = FLIP_ANIMATION_DELAY;
  
  // 数字に応じて適切な画像を表示
  // パタパタ時計は、数字が変わる際にアニメーションを表示
  switch (digit) {
    case 0:
      drawScaledBitmap(x, y, H95, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H00, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 1:
      drawScaledBitmap(x, y, H05, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H10, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 2:
      drawScaledBitmap(x, y, H15, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H20, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 3:
      drawScaledBitmap(x, y, H25, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H30, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 4:
      drawScaledBitmap(x, y, H35, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H40, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 5:
      drawScaledBitmap(x, y, H45, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H50, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 6:
      drawScaledBitmap(x, y, H55, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H60, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 7:
      drawScaledBitmap(x, y, H65, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H70, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 8:
      drawScaledBitmap(x, y, H75, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H80, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 9:
      drawScaledBitmap(x, y, H85, imageWidth, imageHeight, digitSpacing, digitHeight);
      delay(animationDelay);
      drawScaledBitmap(x, y, H90, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
  }
}

// ============================================================================
// ニキシー管時計の数字を描画する関数
// ============================================================================
void drawNixieClockDigit(int position, int digit) {
  // 数字の表示位置を計算
  int digitSpacing = displayInfo.width / 6;  // 画面幅を6等分
  int digitHeight = 600;  // 数字の高さ
  int x = position * digitSpacing;
  int y = displayInfo.centerY - (digitHeight / 2);
  
  // 画像サイズ
  int imageWidth = NIXIE_IMAGE_WIDTH;
  int imageHeight = NIXIE_IMAGE_HEIGHT;
  
  // 数字に応じて適切な画像を表示
  switch (digit) {
    case 0:
      drawScaledBitmap(x, y, N00, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 1:
      drawScaledBitmap(x, y, N10, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 2:
      drawScaledBitmap(x, y, N20, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 3:
      drawScaledBitmap(x, y, N30, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 4:
      drawScaledBitmap(x, y, N40, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 5:
      drawScaledBitmap(x, y, N50, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 6:
      drawScaledBitmap(x, y, N60, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 7:
      drawScaledBitmap(x, y, N70, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 8:
      drawScaledBitmap(x, y, N80, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
    case 9:
      drawScaledBitmap(x, y, N90, imageWidth, imageHeight, digitSpacing, digitHeight);
      break;
  }
}

// ============================================================================
// 画像をスケーリングして描画する関数（黒い筋を防ぐ改善版）
// ============================================================================
void drawScaledBitmap(int16_t x, int16_t y, const uint16_t* bitmap, 
                      int16_t srcWidth, int16_t srcHeight, 
                      int16_t dstWidth, int16_t dstHeight) {
  // スケール比を計算（逆方向：目標サイズから元画像への変換）
  float scaleX = (float)srcWidth / (float)dstWidth;
  float scaleY = (float)srcHeight / (float)dstHeight;
  
  // 描画を開始
  M5.Display.startWrite();
  
  // 目標サイズの各ピクセルを処理（隙間を防ぐため）
  for (int16_t dstY = 0; dstY < dstHeight; dstY++) {
    for (int16_t dstX = 0; dstX < dstWidth; dstX++) {
      // 元画像の対応する位置を計算
      int16_t srcX = (int16_t)(dstX * scaleX);
      int16_t srcY = (int16_t)(dstY * scaleY);
      
      // 範囲チェック
      if (srcX >= 0 && srcX < srcWidth && srcY >= 0 && srcY < srcHeight) {
        // 元画像からピクセルの色を取得
        uint16_t color = bitmap[srcY * srcWidth + srcX];
        
        // 目標位置にピクセルを描画
        M5.Display.drawPixel(x + dstX, y + dstY, color);
      }
    }
  }
  
  // 描画を終了
  M5.Display.endWrite();
}

// ============================================================================
// コロン（:）を表示する関数
// ============================================================================
void drawColon(bool isVisible) {
  // 数字の間隔を計算
  int digitSpacing = displayInfo.width / 6;
  
  // コロンの位置（時と分の間、分と秒の間）
  int colon1X = digitSpacing * 2;  // 時と分の間
  int colon2X = digitSpacing * 4;  // 分と秒の間
  int colonY = COLON_OFFSET_Y;
  
  // 表示色を決定
  uint16_t color = isVisible ? displayColor : DARKGREY;
  
  // 4つの点を描画（上下2つずつ、2箇所）
  M5.Display.fillCircle(colon1X, displayInfo.centerY - colonY, COLON_DIAMETER, color);
  M5.Display.fillCircle(colon1X, displayInfo.centerY + colonY, COLON_DIAMETER, color);
  M5.Display.fillCircle(colon2X, displayInfo.centerY - colonY, COLON_DIAMETER, color);
  M5.Display.fillCircle(colon2X, displayInfo.centerY + colonY, COLON_DIAMETER, color);
}
