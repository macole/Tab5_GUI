# Arduino C++基本構文ガイド - M5Stack Tab5対応版

M5Stack Tab5開発で必要なC++の基本構文、データ型、文法、およびTab5特有の注意点を解説します。

---

## 📋 目次

1. [スコープ解決演算子 `::`](#1-スコープ解決演算子-)
2. [インクルードディレクティブ: `""` と `<>`](#2-インクルードディレクティブ--と-)
3. [C++基本データ型](#3-c基本データ型)
4. [Arduino固有のデータ型](#4-arduino固有のデータ型)
5. [基本文法とプログラム構造](#5-基本文法とプログラム構造)
6. [制御構文](#6-制御構文)
7. [関数とポインタ](#7-関数とポインタ)
8. [M5Stack Tab5固有の注意点](#8-m5stack-tab5固有の注意点)
9. [よくある間違いとベストプラクティス](#9-よくある間違いとベストプラクティス)

---

## 1. スコープ解決演算子 `::`

### 概要
`::` は**スコープ解決演算子**（scope resolution operator）で、「〜の中にある」という意味を持ちます。

### 基本的な使い方

```cpp
m5::imu_data_t imuData;
```

**構文の意味:**
- `m5` = 名前空間（namespace）の名前
- `::` = 「m5名前空間の中にある」
- `imu_data_t` = データ型（構造体またはクラス）
- `imuData` = 変数名

つまり、**「m5という名前空間の中で定義されている`imu_data_t`型の変数`imuData`を宣言する」**という意味です。

### なぜ名前空間を使うのか

名前空間は、同じ名前の衝突を防ぐために使われます。

```cpp
// M5Stackライブラリの中
namespace m5 {
    struct imu_data_t {
        float accelX, accelY, accelZ;
        float gyroX, gyroY, gyroZ;
    };
}

// 別のライブラリでも同じ名前を使える
namespace other_lib {
    struct imu_data_t {
        // 全く違う定義でもOK
        int x, y, z;
    };
}
```

### M5Stack Tab5での使用例

```cpp
m5::M5.begin();              // m5名前空間のM5オブジェクト
m5::M5.Display.print("Hi");  // m5名前空間のDisplayオブジェクト
m5::imu_data_t imuData;      // m5名前空間のimu_data_t型

// IMUデータ取得
m5::M5.Imu.update();
m5::M5.Imu.getImuData(&imuData);
```

### その他のスコープ解決演算子の使い方

```cpp
// クラスのメンバ関数定義
class MyClass {
    void myFunction();
};

void MyClass::myFunction() {  // クラス名::関数名
    // 実装
}

// グローバルスコープへのアクセス
int value = 10;  // グローバル変数

void test() {
    int value = 20;      // ローカル変数
    ::value = 30;        // ::でグローバル変数にアクセス
}
```

---

## 2. インクルードディレクティブ: `""` と `<>`

### 基本的な違い

#### `#include <ファイル名>` - システムインクルード

```cpp
#include <Arduino.h>
#include <M5Unified.h>
#include <lvgl.h>
```

**特徴:**
- **システムディレクトリ**や**ライブラリディレクトリ**から検索
- コンパイラやIDEが管理する標準的な場所を探す
- **外部ライブラリ**や**標準ヘッダー**に使用

**検索順序:**
1. システムインクルードパス（`/usr/include`など）
2. コンパイラの標準ライブラリパス
3. Arduino IDEなら`libraries/`フォルダ
4. PlatformIOなら`.pio/libdeps/`フォルダ

#### `#include "ファイル名"` - ローカルインクルード

```cpp
#include "my_app.h"
#include "config.h"
#include "../common/utils.h"
```

**特徴:**
- まず**カレントディレクトリ**（ソースファイルと同じ場所）から検索
- 見つからなければシステムディレクトリも探す
- **自分のプロジェクトのファイル**に使用

**検索順序:**
1. ソースファイルと同じディレクトリ
2. プロジェクトのインクルードパス
3. （見つからなければ）システムインクルードパス

---

### 実際の使い分け例

```cpp
// M5Stack Tab5のプロジェクト例

// ========================================
// システム/ライブラリヘッダー（山括弧）
// ========================================
#include <Arduino.h>
#include <M5Unified.h>
#include <M5GFX.h>
#include <lvgl.h>
#include <Wire.h>

// ========================================
// 自分のプロジェクトのヘッダー（ダブルクォート）
// ========================================
#include "my_app.h"
#include "ui_definitions.h"
#include "sensor_manager.h"
#include "lv_conf.h"
```

---

### ベストプラクティス

| 用途 | 使用する記法 | 例 |
|------|------------|-----|
| Arduino標準ライブラリ | `<>` | `#include <Arduino.h>` |
| 外部ライブラリ | `<>` | `#include <M5Unified.h>` |
| LVGL等のUIライブラリ | `<>` | `#include <lvgl.h>` |
| 自分のプロジェクトのヘッダー | `""` | `#include "my_header.h"` |
| 同じフォルダ内のヘッダー | `""` | `#include "config.h"` |
| 相対パスのヘッダー | `""` | `"../utils/helper.h"` |

---

## 3. C++基本データ型

### 整数型

| 型名 | バイト数 | 範囲 | 説明 |
|------|---------|------|------|
| `int8_t` | 1 | -128 ~ 127 | 符号付き8ビット整数 |
| `uint8_t` | 1 | 0 ~ 255 | 符号なし8ビット整数 |
| `int16_t` | 2 | -32,768 ~ 32,767 | 符号付き16ビット整数 |
| `uint16_t` | 2 | 0 ~ 65,535 | 符号なし16ビット整数 |
| `int32_t` | 4 | -2,147,483,648 ~ 2,147,483,647 | 符号付き32ビット整数 |
| `uint32_t` | 4 | 0 ~ 4,294,967,295 | 符号なし32ビット整数 |
| `int64_t` | 8 | ±9.2×10¹⁸ | 符号付き64ビット整数 |
| `uint64_t` | 8 | 0 ~ 1.8×10¹⁹ | 符号なし64ビット整数 |

### 従来のC/C++整数型

| 型名 | ESP32-P4でのサイズ | 推奨代替型 |
|------|------------------|-----------|
| `char` | 1バイト | `int8_t` または `uint8_t` |
| `short` | 2バイト | `int16_t` |
| `int` | 4バイト | `int32_t` |
| `long` | 4バイト | `int32_t` |
| `long long` | 8バイト | `int64_t` |

**⚠️ 注意**: 従来型（`int`, `long`など）はプラットフォーム依存のため、**サイズ明示型（`int32_t`など）を推奨**

### 浮動小数点型

| 型名 | バイト数 | 精度 | 範囲 |
|------|---------|------|------|
| `float` | 4 | 約7桁 | ±3.4×10³⁸ |
| `double` | 8 | 約15桁 | ±1.7×10³⁰⁸ |

**ESP32-P4での注意**: `double`も8バイトだが、ハードウェアFPUは32ビット浮動小数点のみサポート。`float`の使用を推奨。

### 論理型

```cpp
bool flag = true;   // または false
```

### 文字と文字列

```cpp
char c = 'A';                    // 単一文字
char str[] = "Hello";            // C言語スタイル文字列
String text = "Hello World";     // Arduino String型（動的）
```

---

## 4. Arduino固有のデータ型

### byte型

```cpp
byte data = 0xFF;  // 0～255（uint8_tと同じ）
```

### word型

```cpp
word value = 65535;  // 0～65535（uint16_tと同じ）
```

### String型

Arduino独自の動的文字列クラス：

```cpp
String text = "Hello";
text += " World";              // 文字列連結
int len = text.length();       // 長さ取得
char c = text.charAt(0);       // 文字取得
String sub = text.substring(0, 5);  // 部分文字列
```

**⚠️ メモリ注意**: `String`はヒープメモリを使うため、頻繁な操作はメモリ断片化を招く可能性あり。

---

## 5. 基本文法とプログラム構造

### Arduinoプログラムの基本構造

```cpp
#include <Arduino.h>
#include <M5Unified.h>

// グローバル変数
uint32_t counter = 0;

// 初期化関数（1回だけ実行）
void setup() {
    M5.begin();
    M5.Display.setRotation(3);
    Serial.begin(115200);
}

// メインループ（繰り返し実行）
void loop() {
    counter++;
    M5.Display.printf("Count: %d\n", counter);
    delay(1000);
}
```

### 変数宣言と初期化

```cpp
int x;              // 宣言のみ（値は不定）
int y = 10;         // 宣言と初期化
const int Z = 100;  // 定数（変更不可）

// 複数変数の宣言
int a, b, c;
int d = 1, e = 2, f = 3;
```

### const修飾子

```cpp
const int LED_PIN = 13;        // 定数
const float PI = 3.14159;

void func() {
    LED_PIN = 14;  // ❌ エラー: 定数は変更不可
}
```

---

## 6. 制御構文

### if文

```cpp
int value = 10;

if (value > 5) {
    Serial.println("Large");
} else if (value > 0) {
    Serial.println("Small");
} else {
    Serial.println("Zero or negative");
}

// 三項演算子
int max = (a > b) ? a : b;
```

### switch文

```cpp
int mode = 2;

switch (mode) {
    case 0:
        Serial.println("Mode 0");
        break;
    case 1:
        Serial.println("Mode 1");
        break;
    case 2:
        Serial.println("Mode 2");
        break;
    default:
        Serial.println("Unknown mode");
        break;
}
```

### forループ

```cpp
// 基本形
for (int i = 0; i < 10; i++) {
    Serial.println(i);
}

// 配列の走査
int arr[] = {1, 2, 3, 4, 5};
for (int i = 0; i < 5; i++) {
    Serial.println(arr[i]);
}

// 範囲ベースforループ（C++11以降）
int numbers[] = {10, 20, 30};
for (int num : numbers) {
    Serial.println(num);
}
```

### whileループ

```cpp
int i = 0;
while (i < 10) {
    Serial.println(i);
    i++;
}

// do-while（最低1回は実行）
int j = 0;
do {
    Serial.println(j);
    j++;
} while (j < 5);
```

---

## 7. 関数とポインタ

### 関数の定義

```cpp
// 戻り値なし
void printMessage() {
    Serial.println("Hello");
}

// 戻り値あり
int add(int a, int b) {
    return a + b;
}

// デフォルト引数
void setVolume(uint8_t vol = 50) {
    // volが指定されなければ50を使用
}
```

### ポインタ

```cpp
int value = 10;
int* ptr = &value;     // ポインタ宣言とアドレス取得

Serial.println(*ptr);  // 間接参照（値を取得）: 10
*ptr = 20;            // ポインタ経由で値を変更
Serial.println(value); // 20

// ポインタを使った関数呼び出し
void modifyValue(int* p) {
    *p = 100;
}

int x = 5;
modifyValue(&x);  // xのアドレスを渡す
Serial.println(x); // 100
```

### 参照（リファレンス）

```cpp
void modifyByReference(int& ref) {
    ref = 200;
}

int y = 5;
modifyByReference(y);  // アドレス演算子不要
Serial.println(y);     // 200

// constリファレンス（読み取り専用）
void printValue(const String& str) {
    Serial.println(str);
    // str = "New";  // ❌ エラー: 変更不可
}
```

---

## 8. M5Stack Tab5固有の注意点

### 8.1 ディスプレイ座標系

Tab5は**物理的に縦長パネル（720x1280）** ですが、通常は**回転させて横向き（1280x720）** で使用します。

```cpp
void setup() {
    M5.begin();
    
    // 回転設定
    M5.Display.setRotation(3);  // 270度回転（推奨）
    
    // 座標系の確認
    int w = M5.Display.width();   // 1280
    int h = M5.Display.height();  // 720
}
```

| setRotation値 | 画面向き | width | height |
|--------------|---------|-------|--------|
| 0 | 縦向き（デフォルト） | 720 | 1280 |
| 1 | 横向き（90度） | 1280 | 720 |
| 2 | 縦向き（180度） | 720 | 1280 |
| 3 | 横向き（270度）⭐️ | 1280 | 720 |

**⚠️ 重要**: LVGL使用時は`lv_conf.h`の解像度設定も回転に合わせる必要があります。

### 8.2 カラーフォーマット

Tab5はRGB565フォーマット（16ビット/ピクセル）を使用：

```cpp
// RGB565カラー定義
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0

// RGB888からRGB565への変換
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
```

**メモリ使用量計算**:
- フルスクリーン（1280×720）: 約1.8MB
- ダブルバッファ: 約3.6MB
- **推奨**: 部分バッファ使用（例: 高さ50行 = 180KB）

### 8.3 PSRAM使用

Tab5には8MB PSRAMが搭載されており、大きなバッファに使用できます：

```cpp
// PSRAMからメモリ確保
void* buffer = heap_caps_malloc(size, MALLOC_CAP_SPIRAM);

// LVGLでのPSRAM使用
lv_color_t* buf = (lv_color_t*)heap_caps_malloc(
    sizeof(lv_color_t) * BUFFER_SIZE, 
    MALLOC_CAP_SPIRAM
);
```

**⚠️ 注意**: PSRAMは通常RAMより遅いため、頻繁にアクセスするデータには不向き。

### 8.4 IMUデータ構造

```cpp
#include <M5Unified.h>

m5::imu_data_t imuData;

void loop() {
    M5.Imu.update();
    imuData = M5.Imu.getImuData();
    
    // 加速度データ（m/s²）
    float accelX = imuData.accel.x;
    float accelY = imuData.accel.y;
    float accelZ = imuData.accel.z;
    
    // ジャイロデータ（°/s）
    float gyroX = imuData.gyro.x;
    float gyroY = imuData.gyro.y;
    float gyroZ = imuData.gyro.z;
}
```

### 8.5 オーディオ

```cpp
// スピーカー音量設定（0～255）
M5.Speaker.setVolume(128);

// 音を鳴らす
M5.Speaker.tone(1000, 100);  // 1000Hz, 100ms

// オーディオバッファの再生（ES8388使用）
std::vector<int16_t> audioData;
// ... audioDataに音声データを格納 ...
hal::audioPlay(audioData, false);  // false: ループなし
```

### 8.6 カメラ（MIPI CSI）

```cpp
#include "hal/hal_camera.h"

// カメラプレビュー開始
lv_obj_t* screen = lv_scr_act();
hal::startCameraCapture(screen);

// カメラ停止
hal::stopCameraCapture();

// カメラ状態確認
bool capturing = hal::isCameraCapturing();
```

### 8.7 LVGLスレッドセーフ

Tab5でLVGLを使用する場合、**マルチスレッド環境でのロック**が必要です：

```cpp
// LVGL操作前にロック取得
void updateUI() {
    // ロックガード使用（自動ロック/アンロック）
    LvglLockGuard lock;
    
    lv_label_set_text(label, "Updated");
    lv_obj_set_x(obj, 100);
}
```

**⚠️ 重要**: LVGL APIを呼ぶ前に必ずロックを取得してください。デッドロックやクラッシュを防ぎます。

### 8.8 パーティション設定

Tab5は16MB Flashを搭載。大規模アプリには**カスタムパーティション**が必要：

```csv
# partitions.csv
nvs,      data, nvs,     0x9000,  0x5000,
otadata,  data, ota,     0xe000,  0x2000,
app0,     app,  factory, 0x10000, 0x7E0000,
coredump, data, coredump,0x7F0000,0x10000,
```

Arduino IDEでの設定:
```
ボード: "ESP32P4 Dev Module"
Partition Scheme: "Custom" (partitions.csvを使用)
```

---

## 9. よくある間違いとベストプラクティス

### ❌ よくある間違い

#### 1. インクルードの誤用

```cpp
// ❌ 悪い例
#include "Arduino.h"        // ライブラリなのに""を使用
#include <my_config.h>      // 自作ファイルなのに<>を使用

// ✅ 良い例
#include <Arduino.h>        // ライブラリは<>
#include "my_config.h"      // 自作ファイルは""
```

#### 2. 変数の未初期化

```cpp
// ❌ 悪い例
int value;
if (value > 10) { ... }  // 値が不定

// ✅ 良い例
int value = 0;
if (value > 10) { ... }
```

#### 3. String型の過度な使用

```cpp
// ❌ 悪い例（メモリ断片化リスク）
String msg = "Sensor: ";
for (int i = 0; i < 100; i++) {
    msg += String(i);  // 繰り返しメモリ確保
}

// ✅ 良い例
char buffer[64];
snprintf(buffer, sizeof(buffer), "Sensor: %d", value);
```

#### 4. delay()の多用

```cpp
// ❌ 悪い例（応答性が悪い）
void loop() {
    doTask1();
    delay(5000);  // 5秒間ブロック
    doTask2();
}

// ✅ 良い例（millis()を使用）
unsigned long lastTime = 0;
void loop() {
    doTask1();
    if (millis() - lastTime > 5000) {
        doTask2();
        lastTime = millis();
    }
}
```

#### 5. ポインタの誤用

```cpp
// ❌ 悪い例
int* ptr;
*ptr = 10;  // 初期化されていないポインタを使用

// ✅ 良い例
int value = 0;
int* ptr = &value;
*ptr = 10;
```

### ✅ ベストプラクティス

#### 1. const使用

```cpp
// 定数は必ずconstで宣言
const int LED_PIN = 13;
const float SENSOR_SCALE = 0.01;
```

#### 2. 適切なデータ型選択

```cpp
// サイズを明示
uint8_t brightness = 128;      // 0～255
int16_t temperature = -50;     // -32768～32767
uint32_t timestamp = millis(); // 0～4,294,967,295
```

#### 3. 関数の分割

```cpp
// 処理を小さな関数に分割
void updateDisplay() {
    clearScreen();
    drawHeader();
    drawSensorData();
    drawFooter();
}
```

#### 4. メモリ効率

```cpp
// グローバル変数は最小限に
// 大きな配列はPSRAMに配置
static uint8_t largeBuffer[100000] __attribute__((section(".psram")));
```

#### 5. エラーチェック

```cpp
// 戻り値を必ずチェック
bool success = M5.Imu.update();
if (!success) {
    Serial.println("IMU update failed");
}
```

---

## 関連ドキュメント

- [16_M5_Tab5_Basic_Arduino_Sketch_Guide_JA.md](./16_M5_Tab5_Basic_Arduino_Sketch_Guide_JA.md) - 基本的なスケッチの書き方
- [17_Tab5_Arduino_Basic_LVGL_Project_Guide_JA.md](./17_Tab5_Arduino_Basic_LVGL_Project_Guide_JA.md) - LVGLプロジェクトの構造
- [18_Tab5_IMU_Arduino_Guide_JA.md](./18_Tab5_IMU_Arduino_Guide_JA.md) - IMU実装例
- [14_Tab5_Hardware_Specification_Guide_JA.md](./14_Tab5_Hardware_Specification_Guide_JA.md) - ハードウェア仕様詳細

---

## まとめ

このガイドでは、M5Stack Tab5でのArduino/C++開発に必要な基礎知識をカバーしました：

✅ スコープ解決演算子とインクルード文法  
✅ C++基本データ型とArduino固有型  
✅ 制御構文と関数定義  
✅ Tab5固有の注意点（ディスプレイ、メモリ、LVGL）  
✅ ベストプラクティスとよくある間違い  

これらの知識を活用して、効率的で保守性の高いTab5アプリケーションを開発してください。

