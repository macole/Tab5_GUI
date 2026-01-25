# EEZ_Template2 - EEZ Studio開発用テンプレート

M5Stack Tab5向けのEEZ Studio + LVGLアプリケーション開発用テンプレートプロジェクトです。

## 📚 ドキュメント

- **[FOLDER_MIGRATION_GUIDE.md](./FOLDER_MIGRATION_GUIDE.md)** - フォルダ変更（ui/ → src/）の詳細ガイド
- **[CHANGES.md](./CHANGES.md)** - 変更履歴サマリー
- このファイル（README.md） - プロジェクト概要と使い方

## 📁 プロジェクト構造

```
EEZ_Template2/
├── EEZ_Template2.ino          # メインプログラム
├── lv_conf.h                  # LVGL設定ファイル（プロジェクトルート）
├── README.md                  # このファイル
├── EEZ/                       # EEZ Studioプロジェクトフォルダ
│   └── LV8wF_Template2.eez-project  # EEZ Studioプロジェクトファイル
└── src/                       # EEZ Studio生成ファイル（すべてここに配置）
    ├── ui.h, ui.c             # UI初期化
    ├── screens.h, screens.c   # 画面定義
    ├── eez-flow.h, eez-flow.cpp  # Flow言語エンジン
    ├── vars.h                 # 変数定義
    ├── actions.h              # アクション定義
    ├── styles.h, styles.c     # スタイル定義
    ├── images.h, images.c     # 画像データ
    ├── fonts.h                # フォント定義
    └── structs.h              # 構造体定義
```

## 🔧 ファイル構造の特徴

### srcフォルダ構成

EEZ Studioから生成されたすべてのファイルは`src/`フォルダに配置されています。これにより：

- **整理された構造**: EEZ Studio生成ファイルとメインコードが分離
- **自動コンパイル**: Arduinoのビルドシステムが`src/`フォルダを自動認識
- **管理しやすい**: EEZ Studioで再生成時に`src/`フォルダ内を更新するだけ
- **バージョン管理**: GitでEEZ生成ファイルを一括管理可能

## 📝 開発ワークフロー

### 1. EEZ StudioでUIを編集

```bash
# EEZ Studioでプロジェクトを開く
# ファイル → Open Project → EEZ/LV8wF_Template2.eez-project
```

### 2. UIをエクスポート

EEZ Studioで：
1. **Build → Build & Export** を実行
2. 生成されたファイルをすべて`src/`フォルダにコピー（上書き）

### 3. 必要な修正（自動化推奨）

`src/ui.c`の先頭行を確認し、以下のように修正されていることを確認：

```c
// 修正前（EEZ Studioが生成するコード）
#if defined(EEZ_FOR_LVGL)
#include <eez/core/vars.h>
#endif

// 修正後
// EEZ Studio生成ファイル - src/フォルダに配置済み
```

**注意**: この修正は、EEZ Studioで再エクスポートするたびに必要です。

### 4. コンパイル & アップロード

```bash
arduino-cli compile -b m5stack:esp32:m5stack_tab5
arduino-cli upload -b m5stack:esp32:m5stack_tab5 -p /dev/cu.usbserial-*
```

## 🎨 インクルードパスについて

### メインプログラム (EEZ_Template2.ino)

```cpp
#include <M5Unified.h>
#include <lvgl.h>
#include "src/ui.h"        // src/フォルダから
#include "src/screens.h"   // src/フォルダから
```

### src/フォルダ内のファイル

`src/`フォルダ内のファイル同士は、同じディレクトリ内で相対参照：

```cpp
// src/ui.h の例
#include "eez-flow.h"  // 同じディレクトリ
#include "screens.h"   // 同じディレクトリ
```

**重要**: Arduinoのビルドシステムは`src/`フォルダ内のファイルを自動的にコンパイルします。

## ⚙️ コンパイル設定

### 必要な設定

- **PSRAM**: Enabled（必須！）
- **Partition Scheme**: Default 4MB with spiffs
- **Flash Size**: 16MB
- **Upload Speed**: 921600

### Arduino CLI例

```bash
arduino-cli compile \
  --fqbn esp32:esp32:esp32p4:PSRAM=enabled,FlashSize=16M \
  EEZ_Template2/EEZ_Template2.ino
```

## 📚 必要なライブラリ

| ライブラリ | バージョン | 必須/オプション |
|------------|----------|----------------|
| M5Unified | >= 0.2.10 | 必須 |
| LVGL | 8.3.11 | 必須 |

### インストール方法

```bash
arduino-cli lib install "M5Unified@0.2.10"
arduino-cli lib install "lvgl@8.3.11"
```

## 🐛 トラブルシューティング

### コンパイルエラー: "eez/core/vars.h: No such file or directory"

**原因**: `src/ui.c`の修正が必要

**解決策**: 「3. 必要な修正」のセクションを参照

### コンパイルエラー: "src/ui.h: No such file or directory"

**原因**: `src/`フォルダにファイルが配置されていない

**解決策**: EEZ Studioでエクスポートしたファイルを`src/`フォルダにコピー

### リンクエラー: "undefined reference to ui_init"

**原因**: `src/`フォルダではなく`ui/`など別の名前のフォルダを使用している

**解決策**: フォルダ名を`src/`にリネーム（Arduinoは`src/`フォルダのみ自動認識）

### 画面が真っ暗

**原因**: PSRAM設定が無効

**解決策**: ボード設定で`PSRAM: Enabled`を確認

## 🚀 クイックスタート

1. **EEZ Studioでエクスポート**
   ```
   Build → Build & Export
   ```

2. **ファイルをsrc/フォルダにコピー**
   ```bash
   # 生成されたすべてのファイルを src/ にコピー
   ```

3. **src/ui.cを修正**（最初の1回のみ、または自動化）
   ```c
   // #include <eez/core/vars.h> を削除
   ```

4. **コンパイル**
   ```bash
   arduino-cli compile -b m5stack:esp32:m5stack_tab5
   ```

5. **アップロード**
   ```bash
   arduino-cli upload -b m5stack:esp32:m5stack_tab5 -p /dev/cu.usbserial-*
   ```

## 📖 参考リンク

- [EEZ Studio公式サイト](https://www.envox.eu/eez-studio/)
- [LVGL公式ドキュメント](https://docs.lvgl.io/8.3/)
- [M5Unified GitHub](https://github.com/m5stack/M5Unified)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)

## 📝 ライセンス

このプロジェクトはMITライセンスの下で公開されています。

---

**作成日**: 2026年1月19日  
**対象デバイス**: M5Stack Tab5 (ESP32-P4)  
**LVGLバージョン**: 8.3.11  
**M5Unifiedバージョン**: >= 0.2.10
