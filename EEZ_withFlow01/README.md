# EEZ-Studio UI with Flow for M5Stack Tab5

EEZ Studioで作成したFlow機能付きLVGL UIをM5Stack Tab5で実行するプロジェクトです。

## 📋 概要

このプロジェクトは、[EEZ Studio](https://www.envox.eu/eez-studio/)の視覚的なUIデザイナーとFlow言語を使用して作成したユーザーインターフェースをM5Stack Tab5で動作させます。

### 主な特徴

- ✅ **M5Unified.h対応** - M5Stack統合ライブラリ使用
- ✅ **LVGL 8.3.11** - 高機能GUIライブラリ
- ✅ **Flow言語サポート** - EEZ StudioのFlow言語による動作制御
- ✅ **全画面バッファ** - SPIRAM使用で高速描画
- ✅ **DMA転送** - pushImageDMAによる効率的な画面更新
- ✅ **1280x720解像度** - Tab5の大画面フル活用

## 🔧 ハードウェア要件

- **M5Stack Tab5** (ESP32-P4)
  - 10.1インチディスプレイ（1280x720）
  - 静電容量式タッチスクリーン
  - 8MB PSRAM

## 📚 必要なライブラリ

| ライブラリ | バージョン | インストール方法 |
|------------|----------|-----------------|
| M5Unified | >= 0.2.10 | Arduino Library Manager |
| LVGL | 8.3.11 | Arduino Library Manager |

### Arduino IDEでのインストール

```
スケッチ → ライブラリをインクルード → ライブラリを管理
```

1. "M5Unified"で検索してインストール
2. "lvgl"で検索して**バージョン8.3.11**をインストール

## 📁 プロジェクト構造

```
EEZ_withFlow/
├── EEZ_withFlow.ino          # メインプログラム
├── LVGLv8withFlow.eez-project # EEZ Studioプロジェクトファイル
├── README.md                  # このファイル
├── ui/                        # EEZ Studio生成ファイル
│   ├── lv_conf.h             # LVGL設定
│   ├── ui.c / ui.h           # UI初期化
│   ├── screens.c / screens.h # 画面定義
│   ├── eez-flow.cpp / .h     # Flow言語エンジン
│   ├── actions.h             # アクション定義
│   ├── styles.c / styles.h   # スタイル定義
│   ├── images.c / images.h   # 画像データ
│   ├── fonts.h               # フォント定義
│   ├── structs.h             # 構造体定義
│   └── vars.h                # 変数定義
└── libraries/                # ライブラリテンプレート（参考用）
```

## 🚀 使用方法

### 1. プロジェクトを開く

Arduino IDEで`EEZ_withFlow.ino`を開きます。

### 2. ボード設定

**ツール**メニューで以下を設定：

```
ボード: M5Tab5
Upload Speed: 921600
Flash Mode: QIO
Flash Frequency: 80MHz
Partition Scheme: Default 4MB with spiffs
PSRAM: Enabled
```

### 3. コンパイル・アップロード

1. **検証（✓）**ボタンでコンパイル
2. **アップロード（→）**ボタンでTab5に転送

### 4. シリアルモニタで確認

```
シリアルモニタを開く（115200 bps）
初期化プロセスとメモリ使用量が表示されます
```

## 🎨 EEZ Studioでの編集

### UIの編集方法

1. **EEZ Studioをインストール**
   - [公式サイト](https://www.envox.eu/eez-studio/)からダウンロード

2. **プロジェクトを開く**
   ```
   File → Open Project → LVGLv8withFlow.eez-project
   ```

3. **UIを編集**
   - Pages: 画面レイアウトを編集
   - Flow: Flow言語で動作を定義
   - Styles: スタイルをカスタマイズ

4. **エクスポート**
   ```
   Build → Build & Export
   ```
   
5. **生成ファイルをコピー**
   - `ui/`フォルダ内のファイルを更新

## 📊 現在のUI構成

### UIオブジェクト

このプロジェクトには以下のUIオブジェクトが含まれています：

```cpp
objects_t {
    lv_obj_t *main;      // メイン画面
    lv_obj_t *button1;   // トグルボタン（自動カウント制御）
    lv_obj_t *counter;   // カウンター表示ラベル
    lv_obj_t *slider;    // スライダー（画面の明るさ調整）
    lv_obj_t *obj0;      // 補助オブジェクト
    lv_obj_t *obj1;      // 補助オブジェクト
    lv_obj_t *obj2;      // 補助オブジェクト
}
```

### アプリケーション機能

#### 自動カウント機能
```cpp
void updateAutomation()
```
- ボタンの状態をチェック
- ONの場合、カウンターを0〜255まで自動カウント
- カウンター値をラベルに表示

#### 画面の明るさ調整
```cpp
void updateBrightness()
```
- スライダーの値を取得（0〜255）
- ディスプレイの明るさをリアルタイム調整

## ⚙️ 技術詳細

### メモリ管理

```cpp
// SPIRAMに全画面バッファを確保
g_color_buf = (lv_color_t *)heap_caps_malloc(
    sizeof(lv_color_t) * SCREEN_BUFFER_SIZE,  // 1.8MB
    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
);
```

**メモリ使用量**:
- 画面バッファ: 1,843,200バイト（1.8MB）
- SPIRAM使用率: 約23%（8MB中）

### 画面回転

```cpp
disp_drv.sw_rotate = 1;              // ソフトウェア回転有効
disp_drv.rotated = LV_DISP_ROT_90;   // 90度回転（横向き）
```

- 基本解像度: 720x1280（縦向き）
- 実際の表示: 1280x720（横向き）

### DMA転送

```cpp
M5.Display.pushImageDMA(area->x1, area->y1, width, height, 
                        (uint16_t *)&color_p->full);
```

- 非ブロッキング転送
- CPUリソースを節約
- スムーズな描画

## 🐛 トラブルシューティング

### コンパイルエラー: 'ui.h' not found

**原因**: uiフォルダのパスが正しくない

**解決策**:
```cpp
// EEZ_withFlow.inoの場所を確認
#include "ui/ui.h"       // ✅ 正しい
#include <ui.h>          // ❌ 間違い
```

### 色がおかしい

**原因**: LV_COLOR_16_SWAPの設定

**解決策**:
```cpp
// ui/lv_conf.h で確認
#define LV_COLOR_16_SWAP 1  // ✅ Tab5では1
```

### メモリ不足エラー

**原因**: SPIRAMが有効化されていない

**解決策**:
```
ツール → PSRAM → Enabled
```

### 画面が表示されない

**チェックリスト**:
1. ボード設定が"M5Tab5"か確認
2. ui_init()が呼ばれているか確認
3. lv_timer_handler()がループ内にあるか確認

## 🔄 開発ワークフロー

```
┌─────────────────┐
│  EEZ Studio     │
│  UIデザイン     │
│  Flow作成       │
└────────┬────────┘
         │
         ↓ Export
┌─────────────────┐
│  ui/*          │
│  生成ファイル   │
└────────┬────────┘
         │
         ↓ Include
┌─────────────────┐
│ EEZ_withFlow.ino│
│ M5Unified統合   │
└────────┬────────┘
         │
         ↓ Upload
┌─────────────────┐
│  M5Stack Tab5  │
│  実行          │
└─────────────────┘
```

## 📖 参考リンク

- [EEZ Studio公式サイト](https://www.envox.eu/eez-studio/)
- [EEZ Studio ドキュメント](https://github.com/eez-open/studio/wiki)
- [LVGL公式ドキュメント](https://docs.lvgl.io/8.3/)
- [M5Stack公式サイト](https://m5stack.com/)
- [M5Unified GitHubリポジトリ](https://github.com/m5stack/M5Unified)

## 📝 ライセンス

このプロジェクトはMITライセンスの下で公開されています。

## 🤝 コントリビューション

バグ報告や機能追加の提案は、Issueまたはプルリクエストでお願いします。

---

**最終更新**: 2026年1月10日
**対応バージョン**: M5Unified 0.2.10+, LVGL 8.3.11
