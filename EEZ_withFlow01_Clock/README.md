# EEZ_withFlow01_Clock - M5Stack Tab5 EEZ Studio プロジェクト

M5Stack Tab5向けのEEZ Studio + LVGL 8.3ベースのアプリケーションです。

## 📋 概要

このプロジェクトは、M5Stack Tab5でEEZ Studioを使用して設計したGUIアプリケーションです。EEZ StudioのFlowエンジンを活用してアプリケーションロジックを実装できます。

### 主な特徴

- ✅ **EEZ Studio設計**: 視覚的なUIデザインとFlow言語
- ✅ **LVGL 8.3.11**: 高機能GUIライブラリ
- ✅ **M5Unified統合**: M5Stack統合ライブラリ使用
- ✅ **高速描画**: DMA転送とSPIRAM使用
- ✅ **タッチ操作**: インタラクティブなUI

## 📁 プロジェクト構造

```
EEZ_withFlow01_Clock/
├── EEZ_withFlow01_Clock.ino  # メインプログラム
├── lv_conf.h                  # LVGL設定ファイル
├── README.md                  # このファイル
├── EEZ/                       # EEZ Studioプロジェクトフォルダ
│   └── LV8wF_Clock.eez-project  # EEZ Studioプロジェクトファイル
├── ui.h, ui.c                 # UI初期化（EEZ Studio生成）
├── screens.h, screens.c       # 画面定義（EEZ Studio生成）
├── eez-flow.h, eez-flow.cpp   # Flow言語エンジン（EEZ Studio生成）
├── vars.h                     # 変数定義（EEZ Studio生成）
├── actions.h                  # アクション定義（EEZ Studio生成）
├── styles.h, styles.c         # スタイル定義（EEZ Studio生成）
├── images.h, images.c         # 画像データ（EEZ Studio生成）
├── fonts.h                    # フォント定義（EEZ Studio生成）
└── structs.h                  # 構造体定義（EEZ Studio生成）
```

## 🔧 ハードウェア要件

- **M5Stack Tab5** (ESP32-P4)
  - 10.1インチディスプレイ（1280x720）
  - 静電容量式タッチスクリーン
  - 8MB PSRAM（必須）

## 📚 必要なライブラリ

| ライブラリ | バージョン | 必須 |
|------------|----------|------|
| M5Unified | >= 0.2.10 | ✅ |
| LVGL | 8.3.11 | ✅ |

### インストール方法

Arduino CLIを使用する場合：

```bash
arduino-cli lib install "M5Unified@0.2.10"
arduino-cli lib install "lvgl@8.3.11"
```

⚠️ **重要**: LVGL 9.xではなく、必ず8.3.11を使用してください。

## 🚀 セットアップ

### ボード設定

Arduino IDEで以下を設定：

```
ボード: M5Tab5 または ESP32P4 Dev Module
Upload Speed: 921600
Flash Mode: QIO
Flash Frequency: 80MHz
Partition Scheme: Default 4MB with spiffs
PSRAM: Enabled (必須!)
USB CDC On Boot: Enabled
USB Mode: Hardware CDC and JTAG
```

### コンパイル・アップロード

1. Arduino IDEで `EEZ_withFlow01_Clock.ino` を開く
2. **検証（✓）**ボタンでコンパイル
3. **アップロード（→）**ボタンでTab5に転送

## 🎨 UIのカスタマイズ

### EEZ Studioでの編集

1. **EEZ Studioをインストール**
   - [公式サイト](https://www.envox.eu/eez-studio/)からダウンロード
   - Windows、Mac、Linux対応

2. **プロジェクトを開く**
   ```
   File → Open Project → EEZ/LV8wF_Clock.eez-project
   ```

3. **UIを編集**
   - **Pages**: 画面レイアウトを視覚的に設計
   - **Flow**: Flow言語で動作を定義
   - **Styles**: 色、フォント、サイズをカスタマイズ
   - **Variables**: グローバル変数を定義

4. **エクスポート**
   ```
   Build → Build & Export
   ```

5. **生成ファイルを反映**
   - 生成されたファイルをプロジェクトフォルダにコピー
   - Arduino IDEで再コンパイル

## 🔍 プログラム構造

### メインプログラムの初期化順序

```cpp
void setup()
{
    M5.begin(cfg);          // M5Unified初期化
    Serial.begin(115200);   // シリアル通信初期化
    
    initLvglDisplay();      // LVGLディスプレイ初期化
    initLvglTouch();        // LVGLタッチ入力初期化
    
    // 🔴 重要: EEZ Flowフックの設定（ui_init()より前）
    setupEezFlowHooks();    
    
    ui_init();              // EEZ Studio UI初期化
    M5.Display.setBrightness(255);  // 明るさ設定
}

void loop()
{
    M5.update();            // M5Unified更新
    lv_timer_handler();     // LVGLタイマーハンドラー
    ui_tick();              // EEZ Flow Tickハンドラー
    delay(1);               // CPU負荷軽減
}
```

### 🔴 重要: EEZ Flowフックの設定

**必須**: `ui_init()`を呼び出す前に、EEZ Flowのフックを設定する必要があります。

これを行わないと、**リブートループ**が発生します（詳細は[TROUBLESHOOTING.md](TROUBLESHOOTING.md#6-eez-flowのアサーションエラーリブートループ)を参照）。

```cpp
// EEZ Flowフックの設定（setup()内、ui_init()の前に配置）
void setupEezFlowHooks() {
    // stopScriptフック: デフォルトのassert(false)を回避
    eez::flow::stopScriptHook = []() {
        Serial.println("⚠️  EEZ Flow: stopScript called");
    };
    
    // replacePageフック: 画面切り替え処理
    eez::flow::replacePageHook = [](int16_t pageId, uint32_t animType, 
                                     uint32_t speed, uint32_t delay) {
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
        if (target_screen) {
            lv_scr_load_anim(target_screen, (lv_scr_load_anim_t)animType, 
                            speed, delay, false);
        }
    };
    
    // scpiComponentInitフック
    eez::flow::scpiComponentInitHook = []() {
        Serial.println("🔌 EEZ Flow: scpiComponentInit called");
    };
}
```

**必要なインクルード**:
```cpp
#include "eez-flow.h"
#include "screens.h"
```

### UIオブジェクトへのアクセス

EEZ Studioで生成されたUIオブジェクトは、`objects`構造体を通してアクセスします：

```cpp
// screens.h で定義されている
extern objects_t objects;

// UIオブジェクトへのアクセス例
lv_label_set_text(objects.obj0, "Hello");
lv_slider_set_value(objects.obj1, 50, LV_ANIM_OFF);
```

**注意**: オブジェクトの操作は、EEZ StudioのFlowで行うことを推奨します。

## ⚙️ EEZ Studio Flowでの実装

### 推奨される実装方法

アプリケーションロジック（カウンター、明るさ調整など）は、Arduino側のC++コードではなく、**EEZ StudioのFlow**で実装することを推奨します。

### Flowの利点

- ✅ ビジュアルプログラミングで直感的
- ✅ 変数とUIウィジェットの連携が簡単
- ✅ イベント駆動型の処理が容易
- ✅ 再コンパイル不要でロジック変更可能

### 実装例

EEZ Studioで以下のようなFlowを作成：

```
[ボタンクリック] → [変数を増やす] → [ラベルを更新]
[スライダー変更] → [明るさ変数を更新] → [Native関数を呼ぶ]
```

## 📊 メモリ使用量

### 典型的なメモリ使用量

```
コンパイル後:
- プログラムストレージ: 約800KB / 16MB (5%)
- 動的メモリ: 約100KB / 512KB (20%)

実行時:
- 画面バッファ (SPIRAM): 1.8MB / 8MB (23%)
- 空きヒープ: 約400KB
- 空きPSRAM: 約6MB
```

## 🐛 トラブルシューティング

### コンパイルエラー: "LVGL_LCD_BUF_SIZE was not declared"

**原因**: 定数が定義されていない

**解決策**: 最新のコードでは定義済みです。以下を確認：

```cpp
#define EXAMPLE_LCD_H_RES 720
#define EXAMPLE_LCD_V_RES 1280
#define LVGL_LCD_BUF_SIZE (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)
```

### 画面が表示されない

**チェックリスト**:
1. ✅ ボード設定で`PSRAM: Enabled`になっているか
2. ✅ `ui_init()`が`setup()`で呼ばれているか
3. ✅ `ui_tick()`が`loop()`で呼ばれているか
4. ✅ シリアルモニターでエラーを確認

### タッチが反応しない

**解決策**:
1. `M5.update()`が`loop()`の最初で呼ばれているか確認
2. `initLvglTouch()`が`setup()`で呼ばれているか確認
3. タッチパネルの接続を確認

## 📖 参考資料

- [EEZ Studio公式サイト](https://www.envox.eu/eez-studio/)
- [EEZ Studio ドキュメント](https://github.com/eez-open/studio/wiki)
- [EEZ Studio GitHub](https://github.com/eez-open/studio)
- [LVGL公式ドキュメント](https://docs.lvgl.io/8.3/)
- [M5Unified GitHub](https://github.com/m5stack/M5Unified)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)

## 📝 ライセンス

このプロジェクトはMITライセンスで公開されています。

## 🤝 開発履歴

- **作成日**: 2025年1月
- **最終更新**: 2026年1月25日
- **対象デバイス**: M5Stack Tab5 (ESP32-P4)
- **LVGLバージョン**: 8.3.11
- **M5Unifiedバージョン**: >= 0.2.10
- **EEZ Studioバージョン**: 最新版

## 📖 関連ドキュメント

- **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** - トラブルシューティングガイド
  - 画面がチカチカする問題
  - EEZ Flowのリブートループ
  - メモリ不足の対処法
  - タッチパネルの反応が悪い問題
  - 段階的デバッグ手順

- **[CHANGES.md](CHANGES.md)** - 変更履歴
  - 2026年1月25日: EEZ Flow統合の修正
    - リブートループ問題の解決
    - フック実装の詳細
  - 2026年1月25日: タッチパネルの反応性改善
    - タッチ座標の回転変換
    - タッチ検出の改善
    - 読み取り間隔の最適化

---

**注意**: このプロジェクトはEEZ Studioで作成されています。Square Line Studioとは異なるため、UIオブジェクトへのアクセス方法やFlow言語の使い方が異なります。
