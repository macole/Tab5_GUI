# Tab5 Flip Clock / Nixie Tube Clock

M5Stack Tab5を使用した、パタパタ時計（Flip Clock）とニキシー管時計（Nixie Tube Clock）のデジタル時計アプリケーションです。

## 概要

このプログラムは、M5Stack Tab5の大画面（1280x720）を活用して、レトロなデザインの時計を表示します。2つの表示モードを切り替え可能で、RTC（RX8130CE）から正確な時刻を取得して表示します。

- **パタパタ時計モード**: 数字がフリップするアニメーション効果付きの時計
- **ニキシー管時計モード**: ニキシー管の温かみのあるオレンジ色の表示

## 機能

- RTC（RX8130CE）からの時刻取得
- 日付表示（年/月/日と曜日）
- 時刻表示（時:分:秒）
- 2つの表示モード（パタパタ時計 / ニキシー管時計）
- 最適化された描画（変更があった数字のみ更新）
- コロンの点滅表示（1秒ごと）
- 曜日の自動計算（Zellerの公式を使用）
- 画面サイズに基づく自動スケーリング（パタパタ時計）
- 滑らかな画像拡大（黒い筋なし）

## ハードウェア要件

- **M5Stack Tab5** (1280x720 LCD)
- **RX8130CE RTCモジュール** (I2C接続)
- **GPIO 22**: バックライト制御
- **I2C**: SDA=GPIO 31, SCL=GPIO 32

## ライブラリ依存関係

```cpp
#include <M5Unified.h>                // M5Stack統一ライブラリ（M5GFXを含む）
#include <ArtronShop_RX8130CE.h>      // RX8130CE RTCライブラリ
#include <Wire.h>                     // I2C通信
#include <WiFi.h>                     // WiFi（使用していないが含まれている）
#include <time.h>                     // 時刻処理
```

### インストール方法

Arduino IDEのライブラリマネージャーから以下をインストール：
- **M5Unified** (M5GFXを含む)
- **ArtronShop_RX8130CE**

## プログラム構造

### 主要な定数

```cpp
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
#define JST_OFFSET_HOURS       9     // 日本標準時（JST）はUTC+9時間
```

### 主要な構造体

```cpp
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

// 画面サイズ情報
struct DisplayInfo {
  int32_t width;      // 画面の幅
  int32_t height;     // 画面の高さ
  int32_t centerX;    // 画面の中心X座標
  int32_t centerY;    // 画面の中心Y座標
  int32_t flipDigitWidth;   // パタパタ時計の数字の表示幅（自動計算）
  int32_t flipDigitHeight;  // パタパタ時計の数字の表示高さ（自動計算）
} displayInfo;
```

### 主要な変数

```cpp
int clockMode = CLOCK_MODE_FLIP;  // 表示モード: 1=パタパタ時計, 0=ニキシー管時計
uint16_t displayColor = GREEN;   // 表示色（コロンなど）
```

### 主要な関数

#### `setup()`
- シリアル通信の初期化
- ハードウェアの初期化（`initializeHardware()`）
- ディスプレイの初期化（`initializeDisplay()`）
- 起動メッセージの表示（`showStartupMessage()`）

#### `loop()`
- 1秒ごとにRTCから時刻を取得（`updateDateTimeFromRTC()`）
- 日付が変更された場合のみ日付表示を更新（`updateDateDisplay()`）
- 時刻表示の更新（`updateClockDisplay()`）
- コロンの点滅制御（`drawColon()`）

#### `initializeHardware()`
- M5Stackの初期化（`M5.begin()`）
- I2C通信の初期化（GPIO 31, 32）
- RX8130CE RTCの初期化
- バックライトのON

#### `initializeDisplay()`
- ディスプレイの回転設定
- 画面サイズ情報の取得・計算
- **パタパタ時計の表示サイズを自動計算**
  - 数字の幅：画面幅 ÷ 6
  - 数字の高さ：画面高さ × 70%

#### `updateDateTimeFromRTC()`
- RTCからUTC時刻を取得
- UTC時刻を日本時間（JST）に変換（`convertUTCtoJST()`）
- 曜日を計算（`calculateWeekday()`）

#### `convertUTCtoJST()`
- UTC時刻を日本時間（JST = UTC+9）に変換
- 時間が24を超えた場合の日付繰り上げ処理（`adjustDateAfterHourOverflow()`）

#### `updateClockDisplay()`
- 時・分・秒を各桁に分解
- 前回の値と比較して、変更があった数字のみ描画（`drawClockDigit()`）
- 最適化により描画負荷を軽減

#### `drawClockDigit(int position, int digit)`
- 表示モードに応じて適切な描画関数を呼び出し
- `position`: 桁位置（0=時10の位, 1=時の1の位, 2=分10の位, ...）
- `digit`: 表示する数字（0-9）

#### `drawFlipClockDigit(int position, int digit)`
- パタパタ時計の描画
- 数字のフリップアニメーションを実装
- 各数字には中間フレーム（H05, H10, H15...）が用意されている
- 画面サイズに基づいて自動計算されたサイズで表示

#### `drawNixieClockDigit(int position, int digit)`
- ニキシー管時計の描画
- アニメーションなしで直接数字を表示

#### `drawScaledBitmap(int16_t x, int16_t y, const uint16_t* bitmap, int16_t srcWidth, int16_t srcHeight, int16_t dstWidth, int16_t dstHeight)`
- ビットマップ画像のスケーリング表示（改善版）
- **目標サイズの各ピクセルに対して元画像の対応位置を参照**（隙間ができない）
- 滑らかな拡大表示を実現（黒い筋なし）
- `startWrite()` / `endWrite()`で描画処理を最適化

#### `drawColon(bool isVisible)`
- コロン（:）の表示/非表示
- `isVisible=true`: 点灯（displayColor）
- `isVisible=false`: 消灯（DARKGREY）
- 時と分の間、分と秒の間に配置

#### `calculateWeekday(int year, int month, int day)`
- Zellerの公式を使用して曜日を計算
- RTCの曜日情報が不具合のため、計算で算出
- 戻り値: 0=日曜, 1=月曜, ..., 6=土曜

## 技術的な詳細

### 画像データの構造

画像データはヘッダーファイルに配列として格納されています：

- **img_flip.h**: パタパタ時計用画像（60x140ピクセル、R5G6B5形式）
  - H00, H05, H10, ..., H95: 各数字のフリップアニメーションフレーム
- **img_nixie_tube.h**: ニキシー管用画像（70x134ピクセル、R5G6B5形式）
  - N00, N10, N20, ..., N90: 各数字の画像

### 描画最適化

1. **差分描画**: 前回の値と比較し、変更があった数字のみ描画
2. **バッファリング**: `startWrite()` / `endWrite()`で描画処理をバッファリング
3. **スケーリング改善**: 目標サイズの各ピクセルに対して元画像の対応位置を参照（隙間ができない）

### 画面レイアウト

```
┌─────────────────────────────────────┐
│ YYYY/MM/DD(Week)                     │ ← 日付表示エリア
│                                      │
│                                      │
│   HH : MM : SS                       │ ← 時刻表示エリア（中央）
│                                      │
│                                      │
└─────────────────────────────────────┘
```

- 画面サイズ: 1280x720（Tab5）
- **パタパタ時計**: 画面サイズに基づいて自動計算
  - 数字の幅：画面幅 ÷ 6（約213ピクセル）
  - 数字の高さ：画面高さ × 70%（約504ピクセル）
- **ニキシー管**: 数字の幅は画面幅 ÷ 6、高さは600ピクセル（固定）
- コロンの位置: 時と分の間、分と秒の間

### 時刻取得のタイミング

- `millis()`を使用して1秒ごとに時刻を取得
- RTCから`struct tm`形式でUTC時刻を取得
- 自動的に日本時間（JST = UTC+9）に変換
- 取得失敗時はシリアルにエラーメッセージを出力

## カスタマイズ方法

### 表示モードの変更

**パタパタ時計モードに設定**（デフォルト）:
```cpp
int clockMode = CLOCK_MODE_FLIP;  // 76行目付近
```

**ニキシー管モードに設定**:
```cpp
int clockMode = CLOCK_MODE_NIXIE;  // 76行目付近
```

### パタパタ時計のサイズ調整

パタパタ時計のサイズは画面サイズに基づいて自動計算されます。`initializeDisplay()`関数内で以下の計算式を変更できます：

```cpp
// 現在の設定（254-257行目付近）
displayInfo.flipDigitWidth = displayInfo.width / 6;  // 画面幅を6等分
displayInfo.flipDigitHeight = (int32_t)(displayInfo.height * 0.7);  // 画面高さの70%

// 例：高さを80%にする場合
displayInfo.flipDigitHeight = (int32_t)(displayInfo.height * 0.8);
```

### ニキシー管のサイズ調整

ニキシー管の表示サイズは`drawNixieClockDigit()`関数内で設定されています（547-548行目付近）：

```cpp
int digitSpacing = displayInfo.width / 6;  // 画面幅を6等分
int digitHeight = 600;  // 数字の高さ（ピクセル）
```

### アニメーション速度の調整

```cpp
#define FLIP_ANIMATION_DELAY   275   // パタパタ時計のアニメーション速度（ミリ秒）
#define NIXIE_DISPLAY_DELAY    300   // ニキシー管表示の遅延時間（ミリ秒）
```

### 色の変更

表示色は`setup()`関数内で自動設定されますが、手動で変更することもできます：

```cpp
// パタパタ時計モードの場合
displayColor = WHITE;

// ニキシー管モードの場合
displayColor = ORANGE;
```

### I2Cピンの変更

```cpp
#define PIN_I2C_SDA        31  // I2C SDAピン
#define PIN_I2C_SCL        32  // I2C SCLピン
```

### バックライト制御ピンの変更

```cpp
#define PIN_BACKLIGHT      22  // バックライト制御用GPIO番号
```

### タイムゾーンの設定

プログラムは、RTCから取得したUTC時刻を日本時間（JST = UTC+9）に自動変換します。

他のタイムゾーンを使用する場合、定数を変更してください：

```cpp
#define JST_OFFSET_HOURS    9    // 日本標準時（JST）はUTC+9時間
```

**タイムゾーンオフセット一覧**：
- JST（日本標準時）: +9
- EST（米国東部標準時）: -5
- PST（米国太平洋標準時）: -8
- CET（中央ヨーロッパ時間）: +1
- GMT（グリニッジ標準時）: 0

## RTCのバッテリーバックアップ機能について

### なぜ電源を抜いても時刻を保持できるのか？

RX8130CE RTCチップは、**バッテリーバックアップ機能**を備えています。これにより、メイン電源を切断しても時刻を保持し続けることができます。

#### 仕組み

1. **内蔵バッテリーまたは外部バッテリー**
   - RX8130CEは、内蔵バッテリーまたは外部バッテリー（ボタン電池など）で動作します
   - メイン電源（Tab5の電源）が切れても、バッテリーから電力を供給され続けます

2. **低消費電力設計**
   - RTCチップは非常に低消費電力で動作します（マイクロアンペア単位）
   - バッテリー1つで数年間動作可能です

3. **バッテリースイッチ機能**
   - プログラム内でバッテリースイッチ機能が有効化されています
   - これにより、メイン電源が切れた際に自動的にバッテリーに切り替わります

#### 動作の流れ

```
メイン電源ON
    ↓
RTCはメイン電源から動作
    ↓
メイン電源OFF（Tab5の電源を抜く）
    ↓
自動的にバッテリーに切り替わり
    ↓
RTCはバッテリーから動作し続ける（時刻をカウント）
    ↓
メイン電源ON（Tab5の電源を差し戻す）
    ↓
自動的にメイン電源に切り替わり
    ↓
プログラムがRTCから現在時刻を読み取る
```

#### 注意事項

- **バッテリーの寿命**: バッテリーが消耗すると、電源を抜いた際に時刻が保持されなくなります
- **初回設定**: 初めて使用する場合や、バッテリーが切れた後は、時刻を再設定する必要があります
- **バッテリーの確認**: バッテリーの電圧が低い場合、RTCのフラグレジスタで検出できます（VLF: Voltage Low Flag）

この仕組みにより、PCのBIOS時計と同様に、電源を切っても時刻が保持されます。

## ビルドと実行

### 前提条件

1. Arduino IDE または PlatformIO
2. M5Stack Tab5用のボード定義
3. 必要なライブラリのインストール

### ビルド手順

1. Arduino IDEを開く
2. ボードを「M5Stack Tab5」に設定
3. 必要なライブラリをインストール（M5Unified, ArtronShop_RX8130CE）
4. `tab5_flip_clock.ino`を開く
5. ビルド（Ctrl+R / Cmd+R）
6. アップロード（Ctrl+U / Cmd+U）

### 実行

1. M5Stack Tab5にアップロード
2. シリアルモニター（115200 baud）で動作確認
3. 画面に時計が表示される

## トラブルシューティング

### RTCが初期化できない

```
RX8130CE init error !
```

**対処法**:
- I2C接続を確認（SDA=GPIO 31, SCL=GPIO 32）
- RX8130CEの電源を確認
- I2Cプルアップ抵抗を確認

### 時刻が取得できない

```
RTC から時間を取得できません。
```

**対処法**:
- RTCの初期化を確認
- I2C通信を確認
- RTCの時刻設定を確認

### 表示時刻が9時間（または数時間）ずれている

**原因**: RTCはUTC（協定世界時）で時刻を保持しています。プログラムは自動的に日本時間（JST = UTC+9）に変換しますが、設定が正しくない場合にずれが発生します。

**対処法**:
1. シリアルモニターでデバッグ出力を確認：
   ```
   [DEBUG] UTC time from RTC: ...
   [DEBUG] JST converted: ...
   ```
2. UTC時刻が正しく取得できているか確認
3. JST変換後の時刻が正しいか確認
4. 他のタイムゾーンを使用する場合は、`JST_OFFSET_HOURS`定数を変更

### 表示が乱れる

**対処法**:
- 画面の回転設定を確認（`DISPLAY_ROTATION`）
- 画像データの整合性を確認
- メモリ不足の可能性（他の処理を削減）

### アニメーションが遅い/速い

**対処法**:
- `FLIP_ANIMATION_DELAY`の値を調整（275ミリ秒）
- 描画処理の最適化を確認

### 画像に黒い筋が表示される

**原因**: 古いバージョンのスケーリング関数を使用している場合に発生します。

**対処法**:
- 最新版のコードを使用していることを確認
- `drawScaledBitmap()`関数が改善版（目標サイズから元画像を参照する方式）になっているか確認

## パフォーマンス

- **更新間隔**: 約1秒
- **描画時間**: 通常100-300ms（変更があった数字の数による）
- **メモリ使用量**: 画像データが大きいため、メモリに注意

## コードの改善点（リファクタリング）

このプログラムは、初心者でも理解しやすいように以下の改善が行われています：

1. **構造体の使用**: 関連するデータを構造体でまとめて管理
2. **関数の分割**: 大きな処理を小さな関数に分割
3. **定数の明確化**: マジックナンバーを定数として定義
4. **変数名の改善**: 意味が明確な変数名を使用
5. **コメントの充実**: 各セクションに詳細な説明を追加
6. **画面サイズに基づく自動計算**: パタパタ時計のサイズを自動計算
7. **スケーリングの改善**: 黒い筋が表示されない滑らかな拡大

## ライセンス

オリジナルコード: 2025/05/23 By JK1VCK
Blog URL: https://gijin77.blog.jp/archives/44667829.html

## 参考情報

### 関連ファイル

- `tab5_flip_clock.ino`: メインプログラム
- `img_flip.h`: パタパタ時計用画像データ
- `img_nixie_tube.h`: ニキシー管時計用画像データ

### 同様のアプリを作成する場合のポイント

1. **画像データの準備**
   - 各数字の画像をR5G6B5形式の配列に変換
   - ヘッダーファイルとして保存

2. **描画最適化**
   - 変更があった部分のみ描画
   - `startWrite()` / `endWrite()`を使用

3. **アニメーション実装**
   - 中間フレームを用意
   - `delay()`でタイミングを調整

4. **RTCとの連携**
   - I2C通信の初期化
   - 定期的な時刻取得（1秒間隔推奨）

5. **画面レイアウト**
   - 画面サイズに応じたスケーリング
   - 中央揃えの計算

6. **スケーリングの実装**
   - 目標サイズの各ピクセルに対して元画像の対応位置を参照
   - 隙間ができないように実装

